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
/* ========================================================================= */
/** @defgroup EGeOctree EGeOctree
 *
 * Here we define basic interface for octrees as an embeded structure.
 *
 * @version 0.0.1
 * @par History:
 * - 2009-10-26
 * 						- first implementation
 * */
/** @file 
 * @ingroup EGeOctree */
/** @addtogroup EGeOctree */
/** @{ */
/** @example eg_octree.ex.c
 * This is a simple example of the usage of octree using @ref EGeOctree */
/* ========================================================================= */
#ifndef __EG_OCTREE_H__
#define __EG_OCTREE_H__
#include "eg_config.h"
#include "eg_macros.h"
/* ========================================================================= */
/** @brief Debug level for the octree */
#ifndef EG_OCTR_DEBUG
#define EG_OCTR_DEBUG 10
#endif
/* ========================================================================= */
/** @brief basic data structure */
typedef struct EGeOctdata_st
{
	uint16_t x; /** @brief X position in the grid (up to 65536) */
	uint16_t y; /** @brief Y position in the grid (up to 65536) */
	uint16_t z; /** @brief Z position in the grid (up to 65536) */
	uint16_t depth:4; /** @brief depth of the node (0-15) 0 means a leaf */
	uint16_t key:3;		/** @brief position in the parent for this node (0-7) */
	uint16_t nson:4;	/** @brief number of childrens for this node */
	uint16_t unused:5;/** @brief unused bits (for now) */
	struct EGeOctdata_st*up;/** @brief pointer to up-level */
} EGeOctdata_t;

/* ========================================================================= */
/** @brief data structure for intermediate nodes */
typedef struct 
{
	EGeOctdata_t data;	/** @brief Note that both leafs and branches have the 
																 same basic data, but in principle we have 
																 many more leaves than branches */
	EGeOctdata_t*tree[8];/** @brief branches, they are numbered according to 
																 binary encoding of (>=x?)|(>=y?)|(>=z?), from 
																 zero to seven. If the pointer is NULL, then
																 no children in that sub-region. If current
																 level is one, pointer is of type EGeOctdata_t
																 otherwise, is of type EGeOctbranch_t */
} EGeOctbranch_t;
/* ========================================================================= */
/** @brief user alloc function for branches */
typedef EGeOctbranch_t* (*EGeOctalloc_f)(void*adata);
/* ========================================================================= */
/** @brief user free function for branches */
typedef void (*EGeOctfree_f)(void*adata,EGeOctbranch_t*branch);
/* ========================================================================= */
/** @brief data structure for complete tree.
 * @note that branch 0,0,0 should
 * always be a perfect cube of size 2^n */
typedef struct
{
	EGeOctbranch_t* root; /**<@brief root of the octree */
	uint16_t max_x;				/**<@brief X-range of data [0..max_x-1] */
	uint16_t max_y;				/**<@brief Y-range of data [0..max_y-1] */
	uint16_t max_z;				/**<@brief Z-range of data [0..max_z-1] */
	uint16_t height;			/**<@brief ceil( max( log_2(max_x), log_2(max_y),
																							log_2(max_z) ) ) */
	void*adata;						/**<@brief user allocator data */
	EGeOctalloc_f oalloc;	/**<@brief user allocator function for branches */
	EGeOctfree_f ofree;		/**<@brief user free function for branches */
} EGeOctree_t;
/* ========================================================================= */
/** @brief default branch allocator */
EGeOctbranch_t* EGeOctalloc(void*adata);
/* ========================================================================= */
/** @brief default branch free function */
void EGeOctfree(void*adata,EGeOctbranch_t*branch);
/* ========================================================================= */
/** @brief Add a new leaf to the tree */
int EGeOctaddleaf(EGeOctree_t*const tree,EGeOctdata_t*const leaf);
/* ========================================================================= */
/** @brief delete the given leaf from the tree (and erase empty branches), the
 * given leaf will have a NULL parent after the call. If the leaf is not
 * attached to a tree, nothing will hapen */
int EGeOctdelleaf(EGeOctree_t*const tree,EGeOctdata_t*const leaf);
/* ========================================================================= */
/** @brief Find exact node, if no node, return NULL */
EGeOctdata_t*EGeOctfind(
		const EGeOctree_t*const tree,
		const unsigned depth,
		const unsigned x,
		const unsigned y,
		const unsigned z);
/* ========================================================================= */
/** @brief initializator for a tree */
int EGeOctinit(
		EGeOctree_t*const tree,
		uint16_t max_x,
		uint16_t max_y,
		uint16_t max_z,
		void*adata,
		EGeOctalloc_f oalloc,
		EGeOctfree_f ofree);
/* ========================================================================= */
/** @brief clear a tree and leave it as an empty tree */
int EGeOctclear( EGeOctree_t*const tree);
/* ========================================================================= */
/** @brief Return (if any) next brother of the given node in the tree */
EGeOctdata_t* EGeOctbrother(const EGeOctdata_t*const branch);
/* ========================================================================= */
/** @brief Return (if any) first son of the given node in the tree */
EGeOctdata_t* EGeOctson(const EGeOctdata_t*const branch);
/* ========================================================================= */
/** @brief Return (if any) next cousin (i.e. next relative which is not an
 * ancestor of this node, this implies that if there is such a node, it will be
 * higher up in the tree.) */
EGeOctdata_t* EGeOctcousin(const EGeOctdata_t*const branch);
/* ========================================================================= */
/** @} */
/* end of eg_octree.h */
#endif
