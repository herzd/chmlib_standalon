/* EGlib "Efficient General Library" provides some basic structures and
 * algorithms commons in many optimization algorithms.
 *
 * Copyright (C) 2005-2008 Daniel Espinoza
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
#include "eg_keytab.h"
#include "eg_ehash.h"
/** @file
 * @ingroup EGkeytab */
/** @addtogroup EGkeytab */
/** @{ */
/* ========================================================================= */
int EGkeytabLookUp( const EGkeytab_t*const keytab,
										const uint64_t key,
										int*const pos)
{
	int found=0;
	int next = keytab->hash ? keytab->hash[EGeHashFunc(key,keytab->hash_sz)] : 0;
	const EGkeytabInfo_t*cur=0;
	if(pos)*pos=INT_MAX;
	while(next && !found)
	{
		cur = &(keytab->key[next-1]);
		if(cur->key == key)
		{
			found = 1;
			if(pos) *pos = cur->pos;
		}
		next = cur->next;
	}
	return found;
}
/* ========================================================================= */
int EGkeytabAdd(EGkeytab_t*const keytab,
								const uint64_t key, 
								const int check_duplicate)
{
	register int i;
	int rval = 0, pos, next;
	if(check_duplicate)
	{
		rval = EGkeytabLookUp(keytab, key, 0);
		FTESTG(rval,CLEANUP,"key %"PRIu64" is already in table",key);
	}
	/* check if we must re-size */
	if(keytab->nkey + 1 > keytab->sz)
	{
		keytab->sz = ((size_t)(((keytab->sz>>12)+1)*3))<<11;
		keytab->key = EGrealloc(keytab->key, sizeof(EGkeytabInfo_t)*keytab->sz);
	}
	/* check if we must enlarge the hash table and rehash */
	if(keytab->nkey+1 > 2*keytab->hash_sz)
	{
		keytab->hash_sz = ((size_t)((keytab->hash_sz>>12)*4+1))<<12;
		EGfree(keytab->hash);
		/* this ensure that all positions in tables are 0, i.e. null */
		keytab->hash = EGsMalloc(int,keytab->hash_sz);
		for( i = (int)(keytab->nkey) ; i-- ; )
		{
			pos = EGeHashFunc(keytab->key[i].key, keytab->hash_sz);
			keytab->key[i].next = keytab->hash[pos];
			keytab->hash[pos] = i+1;
		}
	}
	/* now we add the symbol */
	next = (int)(keytab->nkey);
	pos = EGeHashFunc(key,keytab->hash_sz);
	keytab->key[next].key = key;
	keytab->key[next].pos = next;
	keytab->key[next].next = keytab->hash[pos];
	keytab->hash[pos] = next+1;
	keytab->nkey++;
	CLEANUP:
	return rval;
}
/* ========================================================================= */
/** @} */
