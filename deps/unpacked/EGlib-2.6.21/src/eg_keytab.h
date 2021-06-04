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
#include "eg_macros.h"
#include "eg_mem.h"
#ifndef __EG_KEYTAB_H__
#define __EG_KEYTAB_H__
/* ========================================================================= */
/** @defgroup EGkeytab key table structure
 * This version suport addition and finding keys, as well as getting its
 * associated number, which correspond to the order in which it was added. For
 * now it does not support deletion of elements.
 * */
/** @file
 * @ingroup EGkeytab */
/** @addtogroup EGkeytab */
/** @{ */
/** @example eg_keytab.ex.c
 * This is a simple example of the usage of symbol tables using @ref EGkeytab */
/* ========================================================================= */
/** @brief minimal information to store in the hash table */
typedef struct 
{
	uint64_t key;				/**< @brief key being stored */
	int next;						/**< @brief next element in the hash list 
														(offset of one)*/
	int pos;						/**< @brief position for the asociated key */
} EGkeytabInfo_t;
/* ========================================================================= */
/** @brief structure to store a symbol table */
typedef struct 
{
	size_t nkey;				/**< @brief number of keys in the table */
	size_t sz;					/**< @brief actual size of #EGkeytab_t::key */
	size_t hash_sz;			/**< @brief actual size of #EGkeytab_t::hash */
	EGkeytabInfo_t*key;	/**< @brief array of all key in table */ 
	int* hash;					/**< @brief hash table for inverse mapping */
} EGkeytab_t;
/* ========================================================================= */
/** @brief initialize an EGkeytab_t structure
 * @param __keytab structure to initialize 
 * */
#define EGkeytabInit(__keytab) do{\
	EGkeytab_t*const __EGkeytab = (__keytab);\
	memset(__EGkeytab,0,sizeof(EGkeytab_t));}while(0)
/* ========================================================================= */
/** @brief clear any internally allocated memory 
 * @param __keytab structure to clear
 * */
#define EGkeytabClear(__keytab) do{\
	EGkeytab_t*const __EGkeytab = (__keytab);\
	EGfree(__EGkeytab->key);\
	EGfree(__EGkeytab->hash);\
	__EGkeytab->nkey = __EGkeytab->sz = __EGkeytab->hash_sz = 0;}while(0)
/* ========================================================================= */
/** @brief return the i-th symbol in the table
 * @note we don't check for limits, it should be >=0 < #EGkeytab_t::nkey
 * @param __i symbol we look for
 * @param __keytab table to use
 * @return pointer to the corresponding string 
 * */
#define EGkeytabKey(__keytab,__i) ((const uint64_t)((__keytab)->key[__i].key))
/* ========================================================================= */
/** @brief Add a new element to the key table, if the element already
 * exists, then return non zero, otherwise return zero
 * @param keytab table where we will add the key
 * @param key to add to the table
 * @param check_duplicate if set to one, check that the added key was not added
 * before.
 * @return zero on success, non-zero otherwise
 * */
int EGkeytabAdd(EGkeytab_t*const keytab,
								const uint64_t key,
								const int check_duplicate);
/* ========================================================================= */
/** @brief Look for a key in the table, and if found, report the position of
 * the key in the table.
 * @param keytab table where we will look for the key
 * @param key to look up in the table
 * @param pos if not null, return there the position of the key in the
 * table.
 * @return one if the key is in the table, zero otherwise.
 * */
int EGkeytabLookUp( const EGkeytab_t*const keytab,
										const uint64_t key,
										int*const pos);
/* ========================================================================= */
/** @} */
/* end eg_keytab.h */
#endif
