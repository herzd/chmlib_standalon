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
#include "eg_a32list.h"
#include "eg_mem.h"
#ifndef __EG_A32HASH_H__
#define __EG_A32HASH_H__
/* ========================================================================= */
/** @defgroup EGa32Hash Embeded-Hash Tables
 *
 * Here we defina a hash-table implementation using embeded structures and an
 * array of lists as the method to store elements.
 * @version 0.0.1
 * @par History:
 * - 2013-04-01
 * 						- First implementation (from ehash)
 * */
/** @file
 * @ingroup EGa32Hash */
/** @addtogroup EGa32Hash */
/** @{ */
/** @example eg_a32hash.ex.c
 * This is a simple example of the usage of hash tables using @ref EGa32Hash */
/* ========================================================================= */

/* ========================================================================= */
/* to store the key, if the key are too big we can  change that definition 
 * to long long int or wathever */
#ifdef EGkey_t
#undef EGkey_t
#endif
#define EGkey_t uint64_t
/* prime number, this is one of the 1024 biggest prime for unsigned int */
#define EGa32HashPrime 4294959751LLU
/* other prime number */
#define EGa32HashB 109180541LLU
/* and yet other prime number */
#define EGa32HashA 179422291LLU
/* ========================================================================= */
/** @brief this is the hashing function.
 * @param __key the key value we are looking for.
 * @param __maxsize size of the hash table
 * @return hash value. */
#define EGa32HashFunc(__key,__maxsize) ((int)((((__key) * EGa32HashB  + EGa32HashA )% EGa32HashPrime) % (__maxsize)))

/* ========================================================================= */
/** @brief structure to store in the hash table, for a key we store a key,
 * this should be general enough */
typedef struct EGa32HashInfo_t
{
	EGa32List_t cn;	/**< @brief connector in the list where this element lives. */
	EGkey_t key;	/**< @brief identifier of this element, note that we may 
										 have several elements with the same identifier in the 
										 hash table, all of them will live in the same list of 
										 the hash table, but may not be consecutive in the list. */
} EGa32HashInfo_t;
/* ========================================================================= */
/* structure that holds the internal lists */
typedef struct EGa32HashTable_t
{
	size_t max_sz;	/**< @brief Number of entries in the array of lists */
	size_t sz;			/**< @brief Number of elements in the hash table */
	uint32_t*table;			/**< @brief Array of size #EGa32HashTable_t::max_sz */
}
EGa32HashTable_t;
/* ========================================================================= */
/** @brief null initializer for hash tables */
#define EGa32HashTableNull ((EGa32HashTable_t){0,0,0})
/* ========================================================================= */
/** @brief Initialize a hash table as a table of the given max_size.
 * @param __maxsz size of the table.
 * @param __htable pointer to the Hash Table.
 * @note table-entry with UINT32_MAX are empty
 * @note This function will allocate some memory, and to free it,
 * #EGa32HashTableClear should be called. */
#define EGa32HashTableInit(__htable,__maxsz) do{\
	EGa32HashTable_t*const __EGaht = (__htable);\
	size_t const __EGahtsz = (size_t)(__maxsz);\
	register uint32_t __EGahti = (uint32_t)__EGahtsz;\
	__EGaht->max_sz = __EGahtsz;\
	__EGaht->sz = 0;\
	__EGaht->table = EGsMalloc(uint32_t,__EGahtsz);\
	while(__EGahti--){__EGaht->table[__EGahti]=UINT32_MAX;}}while(0)
/* ========================================================================= */
/** @brief Clear a hash table (free internally allocated memory too).
 * @param __htable pointer to the Hash Table.
 * @note This function will not disconnect each list of #EGa32HashInfo_t
 * structures, instead, it will just free the respective #EGa32HashTable_t::table
 * array */
#define EGa32HashTableClear(__htable) do{EGfree((__htable)->table);memset(__htable,0,sizeof(EGa32HashTable_t));}while(0)

/* ========================================================================= */
/** @brief this is to find an element in the hash table, if the key is not 
 * found, it return UINT32_MAX, otherwise, it will return the position in the 
 * array to the first 
 * #EGa32HashInfo_t structure that has the sought after key.
 * @param __hbase address of the base array.
 * @param __hsz size of each basic structure (i.e. sizeof(main structure))
 * @param __hkey key that we are looking for.
 * @param __htable pointer to the hash table where we are looking the key.
 * @return UINT32_MAX if the key was not found, otherwise, the position to the first
 * #EGa32HashInfo_t structure with the right key
 * */
#define EGa32HashTableFind(__hbase,__hsz,__htable,__hkey) ({\
	char*const __EGhbase=(char*)(__hbase);\
	const size_t __EGhsz=(size_t)(__hsz);\
	EGa32HashTable_t*const __EGahtp = (__htable);\
	const EGkey_t __EGahk = (__hkey);\
	const uint32_t __EGahsp = __EGahtp->table[EGa32HashFunc(__EGahk,__EGahtp->max_sz)];\
	uint32_t __EGahp=__EGahsp,__EGahtfn = UINT32_MAX;\
	EGa32HashInfo_t*__EGaht;\
	if(__EGahp<UINT32_MAX){\
		do{\
			__EGaht=((EGa32HashInfo_t*)(EGaGetElem(__EGhbase,__EGhsz,__EGahp)));\
			if(__EGaht->key==__EGahk){__EGahtfn=__EGahp;break;}\
			__EGahp=__EGaht->cn.next;}while(__EGahp!=__EGahsp);}\
		 __EGahtfn;})

/* ========================================================================= */
/** @brief Given a hash info position in a table, find the
 * next #EGa32HashInfo_t structure in the table with the same key. If no more
 * structures with the same key are found, return UINT32_MAX.
 * @param __hbase address of the base array.
 * @param __hsz size of each basic structure (i.e. sizeof(main structure))
 * @param __hinfo position to the hash info structure for which we want its
 * successor.
 * @param __htable pointer to the hash table where we are looking the key.
 * @return UINT32_MAX if no further key was found, otherwise, a position to the next
 * #EGa32HashInfo_t structure with the right key
 * */
#define EGa32HashTableFindNext(__hbase,__hsz,__htable,__hinfo) ({\
	char*const __EGhbase=(char*)(__hbase);\
	const size_t __EGhsz=(size_t)(__hsz);\
	EGa32HashTable_t*const __EGahtp = (__htable);\
	uint32_t __EGahp=(__hinfo),__EGahtfn=UINT32_MAX;\
	EGa32HashInfo_t*__EGaht=((EGa32HashInfo_t*)(EGaGetElem(__EGhbase,__EGhsz,__EGahp)));\
	const EGkey_t __EGahk = __EGaht->key;\
	const uint32_t __EGahsp = __EGahtp->table[EGa32HashFunc(__EGahk,__EGahtp->max_sz)];\
	if(__EGahp<UINT32_MAX){\
		__EGahp=__EGaht->cn.next;\
		while(__EGahp!=__EGahsp){\
			__EGaht=((EGa32HashInfo_t*)(EGaGetElem(__EGhbase,__EGhsz,__EGahp)));\
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
#define EGa32HashTableAdd(__hbase,__hsz,__htable,__hinfo) do{\
	char*const __EGhbase=(char*)(__hbase);\
	const size_t __EGhsz=(size_t)(__hsz);\
	EGa32HashTable_t*const __EGahta = (__htable);\
	const uint32_t __EGahp=(__hinfo);\
	EGa32HashInfo_t*const __EGahtai = ((EGa32HashInfo_t*)(EGaGetElem(__EGhbase,__EGhsz,__EGahp)));\
	const uint32_t __EGahtp=EGa32HashFunc(__EGahtai->key,__EGahta->max_sz);\
	const uint32_t __EGahlp=__EGahta->table[__EGahtp];\
	if(__EGahlp==UINT32_MAX){__EGahta->table[__EGahtp]=__EGahp;EGa32ListInit(__EGhbase,__EGhsz,__EGahp);}\
	else{EGa32ListAddBefore(__EGhbase,__EGhsz,__EGahp,__EGahlp);}\
	__EGahta->sz++;}while(0)

/* ========================================================================= */
/** @brief this is to eliminate an element from the hash table. 
 * @param __hbase address of the base array.
 * @param __hsz size of each basic structure (i.e. sizeof(main structure))
 * @param __htable pointer to the hash table.
 * @param __hinfo position to the hash info structure that we want to remove.
 * */
#define EGa32HashTableDel(__hbase,__hsz,__htable,__hinfo) do{\
	char*const __EGhbase=(char*)(__hbase);\
	const size_t __EGhsz=(size_t)(__hsz);\
	EGa32HashTable_t*const __EGahtd = (__htable);\
	const uint32_t __EGahp=(__hinfo);\
	EGa32HashInfo_t*const __EGahtai = ((EGa32HashInfo_t*)(EGaGetElem(__EGhbase,__EGhsz,__EGahp)));\
	const uint32_t __EGahlp=EGa32HashFunc(__EGahtai->key,__EGahta->max_sz);\
	if(__EGahtai->cn.next==__EGahp) __EGahtd->table[__EGahlp]=UINT32_MAX;\
	else{\
		if(__EGahtd->table[__EGahlp]==__EGahp) __EGahd->table[__EGahlp]=__EGahtai->cn.next;\
	__EGa32ListDel(__EGhbase,__EGhsz,__EGahp);}\
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
#define EGa32HashTableResize(__hbase,__hsz,__htable,__maxsz) do{\
	char*const __EGhbase2=(char*)(__hbase);\
	const size_t __EGhsz2=(size_t)(__hsz);\
	EGa32HashTable_t*const __EGahtr = (__htable);\
	size_t __EGahtosz = __EGahtr->max_sz;\
	size_t __EGahtnsz = (__maxsz);\
	uint32_t*__EGahtot = __EGahtr->table,__EGahnpos,__EGahcpos;\
	EGa32HashInfo_t*__EGahtri = 0;\
	__EGahtr->max_sz = __EGahtnsz;\
	__EGahtr->sz = 0;\
	__EGahtr->table = EGsMalloc(uint32_t,__EGahtnsz);\
	while(__EGahtnsz--){__EGahtr->table[__EGahtnsz]=UINT32_MAX;}\
	for(;__EGahtosz--;){\
		if((__EGahcpos=__EGahtot[__EGahtosz])==UINT32_MAX) continue;\
		__EGahtri=((EGa32HashInfo_t*)(EGaGetElem(__EGhbase2,__EGhsz2,__EGahcpos)));\
		while((__EGahnpos=__EGahtri->cn.next)!=__EGahcpos){/* list with more than one element*/\
			EGa32ListDel(__EGhbase2,__EGhsz2,__EGahcpos);\
			EGa32HashTableAdd(__EGhbase2,__EGhsz2,__EGahtr,__EGahcpos);\
			__EGahcpos=__EGahnpos;\
			__EGahtri=((EGa32HashInfo_t*)(EGaGetElem(__EGhbase2,__EGhsz2,__EGahcpos)));\
			}/* deal with last element */\
		EGa32HashTableAdd(__EGhbase2,__EGhsz2,__EGahtr,__EGahcpos);}\
	EGfree(__EGahtot);\
	}while(0)
/* ========================================================================= */
/** @} */
/* end eg_a32hash.h */
#endif
