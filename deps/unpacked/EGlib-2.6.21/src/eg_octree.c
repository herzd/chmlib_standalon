/* EGlib "Efficient General Library" provides some basic structures and
 * algorithms commons in many optimization algorithms.
 *
 * Copyright (C) 2009 Daniel Espinoza
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public 
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA 
 * */
#include "eg_octree.h"
#include "eg_mem.h"
/** @file
 * @ingroup EGeOctree */
/** @addtogroup EGeOctree */
/** @{ */
/* ========================================================================= */
EGeOctbranch_t* EGeOctalloc(void*adata) { return EGsMalloc(EGeOctbranch_t,1); }
/* ========================================================================= */
void EGeOctfree(void*adata,EGeOctbranch_t*branch) { EGfree(branch); }
/* ========================================================================= */
int EGeOctinit(
		EGeOctree_t*const tree,
		uint16_t max_x,
		uint16_t max_y,
		uint16_t max_z,
		void*adata,
		EGeOctalloc_f oalloc,
		EGeOctfree_f ofree)
{
	int rval = 0;
	unsigned mx = max_x|max_y|max_z;
	TESTG( (rval=(!oalloc || !ofree)), CLEANUP, "user allocation %p %p us null!",
					oalloc, ofree);
	tree->max_x = max_x;
	tree->max_y = max_y;
	tree->max_z = max_z;
	tree->adata = adata;
	tree->oalloc = oalloc;
	tree->ofree = ofree;
	tree->height = 0;
	tree->root = 0;
	if(mx&0xff00)
	{
		tree->height += 8;
		mx = mx >> 8;
	}
	if(mx&0xf0)
	{
		tree->height += 4;
		mx = mx >> 4;
	}
	if(mx&0xc)
	{
		tree->height += 2;
		mx = mx >> 2;
	}
	switch(mx)
	{
		case 3:
		case 2:
			tree->height += 2;
			break;
		case 1:
			tree->height += 1;
		case 0:
			;
	}
	CLEANUP:
	MESSAGE(EG_OCTR_DEBUG, "Octree with depth %u [%u,%u,%u]",
					tree->height, max_x, max_y, max_z);
	return rval;
}
/* ========================================================================= */
EGeOctdata_t* EGeOctbrother(const EGeOctdata_t*const branch)
{
	const EGeOctbranch_t*const parent = (const EGeOctbranch_t*const )branch->up;
	unsigned key = branch->key;
	EGeOctdata_t*res = 0;
	if(parent)
		for( ; ++key < 8U ; )
			if((res = parent->tree[key])) break;	
	return res;
}
/* ========================================================================= */
EGeOctdata_t*EGeOctson(const EGeOctdata_t*const data)
{
	const EGeOctbranch_t*const branch = (const EGeOctbranch_t*const)data;
	unsigned key = 0;
	EGeOctdata_t*res = 0;
	if(data->nson)
		for(;key<8U;)
			if((res=branch->tree[key++])) break;
	return res;
}
/* ========================================================================= */
EGeOctdata_t* EGeOctcousin(const EGeOctdata_t*const branch)
{
	const EGeOctbranch_t*parent = (const EGeOctbranch_t*)branch->up;
	unsigned key = branch->key;
	EGeOctdata_t*res = 0;
	while(parent && !res)
	{
		if (key == 7U)
		{
			key = parent->data.key;
			parent = (const EGeOctbranch_t*)(parent->data.up);
		}
		else
			res = parent->tree[++key];
	}
	return res;
}
/* ========================================================================= */
static const unsigned EGeOctBit[17] =
	{ 0x0000,0x0001,0x0002,0x0004,
		0x0008,0x0010,0x0020,0x0040,
		0x0080,0x0100,0x0200,0x0400,
		0x0800,0x1000,0x2000,0x4000,0x8000};
/* ========================================================================= */
#define EGeOctKey(__x,__y,__z,__cd) \
	((((__x&EGeOctBit[__cd+1])>>(__cd))<<2)| \
	 (((__y&EGeOctBit[__cd+1])>>(__cd))<<1)| \
	 (((__z&EGeOctBit[__cd+1])>>(__cd))))
/* ========================================================================= */
int EGeOctaddleaf(EGeOctree_t*const tree,EGeOctdata_t*const leaf)
{
	const unsigned x = leaf->x;
	const unsigned y = leaf->y;
	const unsigned z = leaf->z;
	int rval = 0;
	unsigned cd = tree->height;
	unsigned key = EGeOctKey(x,y,z,cd);
	EGeOctbranch_t*cur = tree->root, *parent = 0;
	TESTGL( EG_OCTR_DEBUG, (rval=(x>tree->max_x) || (y>tree->max_y) ||
					(z>tree->max_z)), CLEANUP, "(%u,%u,%u) outside range (%u,%u,%u)",
					x, y, z, tree->max_x, tree->max_y, tree->max_z);
	while(cd)
	{
		/* add branch node if needed */
		if(!cur)
		{
			cur = tree->oalloc(tree->adata);
			cur->data.depth =cd;
			cur->data.key = key;
			cur->data.nson = 0;
			cur->data.up = (EGeOctdata_t*)parent;
			memset(cur->tree,0,sizeof(void*)*8);
			if(parent)
			{
				parent->tree[key] = (EGeOctdata_t*)cur;
				parent->data.nson++;
				if(key&4U) cur->data.x = parent->data.x + EGeOctBit[cd];
				else cur->data.x = parent->data.x - EGeOctBit[cd];
				if(key&2U) cur->data.y = parent->data.y + EGeOctBit[cd];
				else cur->data.y = parent->data.y - EGeOctBit[cd];
				if(key&1U) cur->data.z = parent->data.z + EGeOctBit[cd];
				else cur->data.z = parent->data.z - EGeOctBit[cd];
			}
			else
			{
				tree->root = cur;
				cur->data.x = cur->data.y = cur->data.z = EGeOctBit[cd];
			}
		}
		#if EG_OCTR_DEBUG <= DEBUG
		else
		{
			TESTG((rval=(parent && (parent->tree[key] != (EGeOctdata_t*)cur))),
						CLEANUP, "Internal corruption");
			TESTG((rval=((EGeOctdata_t*)parent!= cur->data.up)), CLEANUP, "Internal corruption");
			TESTG((rval=(key!= cur->data.key)), CLEANUP, "Internal corruption");
			TESTG((rval=(cd!= cur->data.depth)), CLEANUP, "Internal corruption");
		}
		MESSAGE(0,"(%u,%u,%u) cur (%p) = {%u,%u,%u,%2u,%1u,%1u,%p}",x, y, z, 
						cur, cur->data.x, cur->data.y, cur->data.z, cur->data.depth,
						cur->data.key, cur->data.nson, cur->data.up);
		#endif
		/* move down the tree */
		cd--;
		key = EGeOctKey(x,y,z,cd);
		parent = cur;
		cur = (EGeOctbranch_t*)(cur->tree[key]);
	}
	parent->tree[key] = leaf;
	parent->data.nson++;
	leaf->depth = 0;
	leaf->key = key;
	leaf->nson = 0;
	leaf->up = (EGeOctdata_t*)parent;
	MESSAGE(EG_OCTR_DEBUG,"(%u,%u,%u) cur (%p) = {%u,%u,%u,%2u,%1u,%1u,%p}",
					x, y, z, leaf, leaf->x, leaf->y, leaf->z, leaf->depth, leaf->key,
					leaf->nson, leaf->up);
	CLEANUP:
	return rval;
}
/* ========================================================================= */
int EGeOctdelleaf(EGeOctree_t*const tree,EGeOctdata_t*const leaf)
{
	unsigned key = leaf->key;
	int rval = 0;
	EGeOctbranch_t*cur = (EGeOctbranch_t*)leaf,
								*parent = (EGeOctbranch_t*)(leaf->up);
	/* discard dandling leaves */
	if(!parent) goto CLEANUP;
	TESTG((rval=(cur->data.depth)), CLEANUP, "Error, delete a non-leaf");
	/* unatach the leaf */
	cur->data.up = 0;
	parent->data.nson--;
	parent->tree[key]=0;
	/* now, we work our way up from parent deleting empty branches */
	while(parent)
	{
		if(parent->data.nson) break;
		#if EG_OCTR_DEBUG <= DEBUG
		MESSAGE(0,"(%u,%u,%u) cur (%p) = {%u,%u,%u,%2u,%1u,%1u,"
						"%p}",leaf->x, leaf->y, leaf->z, cur, cur->data.x, cur->data.y,
						cur->data.z, cur->data.depth, cur->data.key, cur->data.nson,
						cur->data.up);
		#endif
		cur = parent;
		key = cur->data.key;
		parent = (EGeOctbranch_t*)(cur->data.up);
		if(parent)
		{
			parent->data.nson--;
			parent->tree[key] = 0;
		}
		else
		{
			tree->root = 0;
		}
		tree->ofree(tree->adata,cur);
	}
	CLEANUP:
	return rval;
}
/* ========================================================================= */
EGeOctdata_t*EGeOctfind(
		const EGeOctree_t*const tree,
		const unsigned depth,
		const unsigned x,
		const unsigned y,
		const unsigned z)
{
	unsigned cd = tree->height;
	unsigned key = EGeOctKey(x,y,z,cd);
	EGeOctbranch_t*cur = tree->root, *parent = 0;
	while(cd>depth && cur)
	{
		#if EG_OCTR_DEBUG <= DEBUG
		MESSAGE(0,"(%u,%u,%u,%2u) cur (%p) = {%u,%u,%u,%2u,%1u,%1u,%p}",x, y,
						z, depth, cur, cur->data.x, cur->data.y, cur->data.z, 
						cur->data.depth, cur->data.key, cur->data.nson, cur->data.up);
		#endif
		cd--;
		key = EGeOctKey(x,y,z,cd);
		parent = cur;
		cur = (EGeOctbranch_t*)(cur->tree[key]);
	}
	return (EGeOctdata_t*)cur;
}
/* ========================================================================= */
int EGeOctclear( EGeOctree_t*const tree)
{
	int rval = 0;
	EGeOctdata_t*cur = (EGeOctdata_t*)(tree->root),*next = 0;
	EGeOctbranch_t*parent=0;
	while(cur)
	{
		#if EG_OCTR_DEBUG <= DEBUG
		MESSAGE(0,"cur (%p) = {%u,%u,%u,%2u,%1u,%1u,%p}",
						cur, cur->x, cur->y, cur->z, cur->depth,
						cur->key, cur->nson, cur->up);
		#endif
		next = EGeOctson(cur);
		if(next)
		{
			cur = next;
		}
		else
		{
			next = EGeOctbrother(cur);
			if(!next) next = cur->up;
			parent = (EGeOctbranch_t*)(cur->up);
			if(parent)
			{
				parent->data.nson--;
				parent->tree[cur->key] = 0;
			}
			if(cur->depth) tree->ofree(tree->adata,(EGeOctbranch_t*)cur);
		}
		cur = next;
	}
	return rval;
}
/* ========================================================================= */
/** @} */
