/* EGlib "Efficient General Library" provides some basic structures and
 * algorithms commons in many optimization algorithms.
 *
 * Copyright (C) 2005 Daniel Espinoza and Marcos Goycoolea.
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
#include "eg_config.h"
#include "eg_macros.h"
#include "eg_alist.h"
#include "eg_mem.h"
#ifndef __EG_AHASH_H__
#define __EG_AHASH_H__
/* ========================================================================= */
/** @defgroup EGaHash Embeded-Hash Tables
 *
 * Here we defina a hash-table implementation using embeded structures and an
 * array of lists as the method to store elements.
 * @version 0.0.1
 * @par History:
 * - 2013-04-01
 * 						- First implementation (from ehash)
 * */
/** @file
 * @ingroup EGaHash */
/** @addtogroup EGaHash */
/** @{ */
/** @example eg_ahash.ex.c
 * This is a simple example of the usage of hash tables using @ref EGaHash */
/* ========================================================================= */

/* ========================================================================= */
/* to store the key, if the key are too big we can  change that definition 
 * to long long int or wathever */
#ifdef EGkey_t
#undef EGkey_t
#endif
#define EGkey_t uint64_t
/* prime number, this is one of the 1024 biggest prime for unsigned int */
#define EGaHashPrime 4294959751LLU
/* other prime number */
#define EGaHashB 109180541LLU
/* and yet other prime number */
#define EGaHashA 179422291LLU
/* ========================================================================= */
/** @brief this is the hashing function.
 * @param __key the key value we are looking for.
 * @param __maxsize size of the hash table
 * @return hash value. */
#define EGaHashFunc(__key,__maxsize) ((int)((((__key) * EGaHashB  + EGaHashA )% EGaHashPrime) % (__maxsize)))

/* ========================================================================= */
/** @brief structure to store in the hash table, for a key we store a key,
 * this should be general enough */
typedef struct EGaHashInfo_t
{
	EGaList_t cn;	/**< @brief connector in the list where this element lives. */
	EGkey_t key;	/**< @brief identifier of this element, note that we may 
										 have several elements with the same identifier in the 
										 hash table, all of them will live in the same list of 
										 the hash table, but may not be consecutive in the list. */
} EGaHashInfo_t;
/* ========================================================================= */
/* structure that holds the internal lists */
typedef struct EGaHashTable_t
{
	size_t max_sz;	/**< @brief Number of entries in the array of lists */
	size_t sz;			/**< @brief Number of elements in the hash table */
	int*table;			/**< @brief Array of size #EGaHashTable_t::max_sz */
}
EGaHashTable_t;
/* ========================================================================= */
/** @brief null initializer for hash tables */
#define EGaHashTableNull ((EGaHashTable_t){0,0,0})
/* ========================================================================= */
/** @brief Initialize a hash table as a table of the given max_size.
 * @param __maxsz size of the table.
 * @param __htable pointer to the Hash Table.
 * @note table-entry with INT_MAX are empty
 * @note This function will allocate some memory, and to free it,
 * #EGaHashTableClear should be called. */
#define EGaHashTableInit(__htable,__maxsz) do{\
	EGaHashTable_t*const __EGaht = (__htable);\
	size_t const __EGahtsz = (size_t)(__maxsz);\
	register int __EGahti = (int)__EGahtsz;\
	__EGaht->max_sz = __EGahtsz;\
	__EGaht->sz = 0;\
	__EGaht->table = EGsMalloc(int,__EGahtsz);\
	while(__EGahti--){__EGaht->table[__EGahti]=INT_MAX;}}while(0)
/* ========================================================================= */
/** @brief Clear a hash table (free internally allocated memory too).
 * @param __htable pointer to the Hash Table.
 * @note This function will not disconnect each list of #EGaHashInfo_t
 * structures, instead, it will just free the respective #EGaHashTable_t::table
 * array */
#define EGaHashTableClear(__htable) do{EGfree((__htable)->table);memset(__htable,0,sizeof(EGaHashTable_t));}while(0)

/* ========================================================================= */
/** @brief this is to find an element in the hash table, if the key is not 
 * found, it return INT_MAX, otherwise, it will return the position in the 
 * array to the first 
 * #EGaHashInfo_t structure that has the sought after key.
 * @param __hbase address of the base array.
 * @param __hsz size of each basic structure (i.e. sizeof(main structure))
 * @param __hkey key that we are looking for.
 * @param __htable pointer to the hash table where we are looking the key.
 * @return INT_MAX if the key was not found, otherwise, the position to the first
 * #EGaHashInfo_t structure with the right key
 * */
#define EGaHashTableFind(__hbase,__hsz,__htable,__hkey) ({\
	char*const __EGhbase=(char*)(__hbase);\
	const size_t __EGhsz=(size_t)(__hsz);\
	EGaHashTable_t*const __EGahtp = (__htable);\
	const EGkey_t __EGahk = (__hkey);\
	const int __EGahsp = __EGahtp->table[EGaHashFunc(__EGahk,__EGahtp->max_sz)];\
	int __EGahp=__EGahsp,__EGahtfn = INT_MAX;\
	EGaHashInfo_t*__EGaht;\
	if(__EGahp<INT_MAX){\
		do{\
			__EGaht=((EGaHashInfo_t*)(EGaGetElem(__EGhbase,__EGhsz,__EGahp)));\
			if(__EGaht->key==__EGahk){__EGahtfn=__EGahp;break;}\
			__EGahp=__EGaht->cn.next;}while(__EGahp!=__EGahsp);}\
		 __EGahtfn;})

/* ========================================================================= */
/** @brief Given a hash info position in a table, find the
 * next #EGaHashInfo_t structure in the table with the same key. If no more
 * structures with the same key are found, return INT_MAX.
 * @param __hbase address of the base array.
 * @param __hsz size of each basic structure (i.e. sizeof(main structure))
 * @param __hinfo position to the hash info structure for which we want its
 * successor.
 * @param __htable pointer to the hash table where we are looking the key.
 * @return INT_MAX if no further key was found, otherwise, a position to the next
 * #EGaHashInfo_t structure with the right key
 * */
#define EGaHashTableFindNext(__hbase,__hsz,__htable,__hinfo) ({\
	char*const __EGhbase=(char*)(__hbase);\
	const size_t __EGhsz=(size_t)(__hsz);\
	EGaHashTable_t*const __EGahtp = (__htable);\
	int __EGahp=(__hinfo),__EGahtfn=INT_MAX;\
	EGaHashInfo_t*__EGaht=((EGaHashInfo_t*)(EGaGetElem(__EGhbase,__EGhsz,__EGahp)));\
	const EGkey_t __EGahk = __EGaht->key;\
	const int __EGahsp = __EGahtp->table[EGaHashFunc(__EGahk,__EGahtp->max_sz)];\
	if(__EGahp<INT_MAX){\
		__EGahp=__EGaht->cn.next;\
		while(__EGahp!=__EGahsp){\
			__EGaht=((EGaHashInfo_t*)(EGaGetElem(__EGhbase,__EGhsz,__EGahp)));\
			if(__EGaht->key==__EGahk){__EGahtfn=__EGahp;break;}\
			__EGahp=__EGaht->cn.next;}}\
		 __EGahtfn;})

/* ========================================================================= */
/** @brief insert an element to the hash table. 
 * @param __hbase address of the base array.
 * @param __hsz size of each basic structure (i.e. sizeof(main structure))
 * @param __htable pointer to the hash table where we are looking the key.
 * @param __hinfo position to the hash info structure that we want to add.
 * */
#define EGaHashTableAdd(__hbase,__hsz,__htable,__hinfo) do{\
	char*const __EGhbase=(char*)(__hbase);\
	const size_t __EGhsz=(size_t)(__hsz);\
	EGaHashTable_t*const __EGahta = (__htable);\
	const int __EGahp=(__hinfo);\
	EGaHashInfo_t*const __EGahtai = ((EGaHashInfo_t*)(EGaGetElem(__EGhbase,__EGhsz,__EGahp)));\
	const int __EGahtp=EGaHashFunc(__EGahtai->key,__EGahta->max_sz);\
	const int __EGahlp=__EGahta->table[__EGahtp];\
	if(__EGahlp==INT_MAX){__EGahta->table[__EGahtp]=__EGahp;EGaListInit(__EGhbase,__EGhsz,__EGahp);}\
	else{EGaListAddBefore(__EGhbase,__EGhsz,__EGahp,__EGahlp);}\
	__EGahta->sz++;}while(0)

/* ========================================================================= */
/** @brief this is to eliminate an element from the hash table. 
 * @param __hbase address of the base array.
 * @param __hsz size of each basic structure (i.e. sizeof(main structure))
 * @param __htable pointer to the hash table.
 * @param __hinfo position to the hash info structure that we want to remove.
 * */
#define EGaHashTableDel(__hbase,__hsz,__htable,__hinfo) do{\
	char*const __EGhbase=(char*)(__hbase);\
	const size_t __EGhsz=(size_t)(__hsz);\
	EGaHashTable_t*const __EGahtd = (__htable);\
	const int __EGahp=(__hinfo);\
	EGaHashInfo_t*const __EGahtai = ((EGaHashInfo_t*)(EGaGetElem(__EGhbase,__EGhsz,__EGahp)));\
	const int __EGahlp=EGaHashFunc(__EGahtai->key,__EGahta->max_sz);\
	if(__EGahtai->cn.next==__EGahp) __EGahtd->table[__EGahlp]=INT_MAX;\
	else{\
		if(__EGahtd->table[__EGahlp]==__EGahp) __EGahd->table[__EGahlp]=__EGahtai->cn.next;\
	__EGaListDel(__EGhbase,__EGhsz,__EGahp);}\
	__EGahtd->sz--;\
	}while(0)

/* ========================================================================= */
/** @brief Given an initialized hash table, resize the table array and move all
 * members from the old array to the new one.
 * @param __hbase address of the base array.
 * @param __hsz size of each basic structure (i.e. sizeof(main structure))
 * @param __htable pointer to the hash table.
 * @param __maxsz new size of the table (it should be positive).
 * */
#define EGaHashTableResize(__hbase,__hsz,__htable,__maxsz) do{\
	char*const __EGhbase2=(char*)(__hbase);\
	const size_t __EGhsz2=(size_t)(__hsz);\
	EGaHashTable_t*const __EGahtr = (__htable);\
	size_t __EGahtosz = __EGahtr->max_sz;\
	size_t __EGahtnsz = (__maxsz);\
	int*__EGahtot = __EGahtr->table,__EGahnpos,__EGahcpos;\
	EGaHashInfo_t*__EGahtri = 0;\
	__EGahtr->max_sz = __EGahtnsz;\
	__EGahtr->sz = 0;\
	__EGahtr->table = EGsMalloc(int,__EGahtnsz);\
	while(__EGahtnsz--){__EGahtr->table[__EGahtnsz]=INT_MAX;}\
	for(;__EGahtosz--;){\
		if((__EGahcpos=__EGahtot[__EGahtosz])==INT_MAX) continue;\
		__EGahtri=((EGaHashInfo_t*)(EGaGetElem(__EGhbase2,__EGhsz2,__EGahcpos)));\
		while((__EGahnpos=__EGahtri->cn.next)!=__EGahcpos){/* list with more than one element*/\
			EGaListDel(__EGhbase2,__EGhsz2,__EGahcpos);\
			EGaHashTableAdd(__EGhbase2,__EGhsz2,__EGahtr,__EGahcpos);\
			__EGahcpos=__EGahnpos;\
			__EGahtri=((EGaHashInfo_t*)(EGaGetElem(__EGhbase2,__EGhsz2,__EGahcpos)));\
			}/* deal with last element */\
		EGaHashTableAdd(__EGhbase2,__EGhsz2,__EGahtr,__EGahcpos);}\
	EGfree(__EGahtot);\
	}while(0)
/* ========================================================================= */
/** @} */
/* end eg_ahash.h */
#endif
