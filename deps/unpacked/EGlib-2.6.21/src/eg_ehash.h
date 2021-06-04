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
#include "eg_elist.h"
#include "eg_mem.h"
#ifndef __EG_EHASH_H__
#define __EG_EHASH_H__
/* ========================================================================= */
/** @defgroup EGeHash Embeded-Hash Tables
 *
 * Here we defina a hash-table implementation using embeded structures and an
 * array of lists as the method to store elements.
 * @version 0.0.1
 * @par History:
 * - 2006-10-04
 * 						- First implementation
 * */
/** @file
 * @ingroup EGeHash */
/** @addtogroup EGeHash */
/** @{ */
/** @example eg_ehash.ex.c
 * This is a simple example of the usage of hash tables using @ref EGeHash */
/* ========================================================================= */

/* ========================================================================= */
/* to store the key, if the key are too big we can  change that definition 
 * to long long int or wathever */
typedef uint64_t EGkey_t;
/* ========================================================================= */
/** @brief zero key value */
#define EGkeyZero ((EGkey_t)0)

/* ========================================================================= */
/* this is for the hash function */
/* prime number, this is one of the 1024 biggest prime for unsigned int */
extern const EGkey_t EGhashPrime;
/* other prime number */
extern const EGkey_t EGhashB;
/* and yet other prime number */
extern const EGkey_t EGhashA;


/* ========================================================================= */
/** @brief this is the hashing function.
 * @param __key the key value we are looking for.
 * @param __maxsize size of the hash table
 * @return hash value. */
#define EGeHashFunc(__key,__maxsize)	({\
	(int)(((((EGkey_t)(__key)) * EGhashB  + EGhashA )% EGhashPrime) % (__maxsize));})

/* ========================================================================= */
/** @brief structure to store in the hash table, for a key we store a key,
 * this should be general enough */
typedef struct EGeHashInfo_t
{
	EGeList_t cn;	/**< @brief connector in the list where this element lives. */
	EGkey_t key;	/**< @brief identifier of this element, note that we may 
										 have several elements with the same identifier in the 
										 hash table, all of them will live in the same list of 
										 the hash table, but may not be consecutive in the list. */
} EGeHashInfo_t;
/* ========================================================================= */
/* structure that holds the internal lists */
typedef struct EGeHashTable_t
{
	size_t max_sz;	/**< @brief Number of entries in the array of lists */
	size_t sz;			/**< @brief Number of elements in the hash table */
	EGeList_t*table;/**< @brief Array of size #EGeHashTable_t::max_sz */
}
EGeHashTable_t;
/* ========================================================================= */
/** @brief null initializer for hash tables */
#define EGeHashTableNull ((EGeHashTable_t){(size_t)0,(size_t)0,(EGeList_t*)0})
/* ========================================================================= */
/** @brief Initialize a hash table as a table of the given max_size.
 * @param __maxsz size of the table.
 * @param __htable pointer to the Hash Table.
 * @note This functyion will allocate some memory, and to free it,
 * #EGeHashTableClear should be called. */
#define EGeHashTableInit(__htable,__maxsz) do{\
	EGeHashTable_t*const __EGeht = (__htable);\
	size_t const __EGehtsz = (size_t)(__maxsz);\
	register int __EGehti = (int)__EGehtsz;\
	__EGeht->max_sz = __EGehtsz;\
	__EGeht->sz = 0;\
	__EGeht->table = EGsMalloc(EGeList_t,__EGehtsz);\
	while(__EGehti--){EGeListInit(__EGeht->table+__EGehti);}}while(0)

/* ========================================================================= */
/** @brief Clear a hash table (free internally allocated memory too).
 * @param __htable pointer to the Hash Table.
 * @note This function will not disconnect each list of #EGeHashInfo_t
 * structures, instead, it will just free the respective #EGeHashTable_t::table
 * array */
#define EGeHashTableClear(__htable) EGfree((__htable)->table)

/* ========================================================================= */
/** @brief this is to find an element in the hash table, if the key is not 
 * found, it return 0, otherwise, it will return the pointer to the first 
 * #EGeHashInfo_t structure that has the sought after key.
 * @param __hkey key that we are looking for.
 * @param __htable pointer to the hash table where we are looking the key.
 * @return NULL if the key was not found, otherwise, a pointer to the first
 * #EGeHashInfo_t structure with the right key
 * */
#define EGeHashTableFind(__htable,__hkey) ({\
	EGeHashTable_t*const __EGehtf = (__htable);\
	const EGkey_t __EGehtfk = (__hkey);\
	EGeList_t*const __EGehtfr = __EGehtf->table + EGeHashFunc(__EGehtfk,__EGehtf->max_sz);\
	EGeList_t*__EGehtfi = __EGehtfr->next;\
	EGeHashInfo_t*__EGehtfn = 0;\
	while(__EGehtfi != __EGehtfr){\
		__EGehtfn = EGcontainerOf(__EGehtfi,EGeHashInfo_t,cn);\
		if(__EGehtfn->key == __EGehtfk) break;\
		__EGehtfi = __EGehtfi->next;}\
	__EGehtfi == __EGehtfr ? (EGeHashInfo_t*)0 : __EGehtfn;})

/* ========================================================================= */
/** @brief Given a hash info pointer in a table, find the
 * next #EGeHashInfo_t structure in the table with the same key. If no more
 * structures with the same key are found, return 0.
 * @param __hinfo pointer to the hash info structure for which we want it
 * successor.
 * @param __htable pointer to the hash table where we are looking the key.
 * @return NULL if no further key was found, otherwise, a pointer to the next
 * #EGeHashInfo_t structure with the right key
 * */
#define EGeHashTableFindNext(__htable,__hinfo) ({\
	EGeHashTable_t*const __EGehtf = (__htable);\
	const EGkey_t __EGehtfk = (__hinfo)->key;\
	EGeList_t*const __EGehtfr = __EGehtf->table + EGeHashFunc(__EGehtfk,__EGehtf->max_sz);\
	EGeHashInfo_t*__EGehtfn = (__hinfo);\
	EGeList_t*__EGehtfi = __EGehtfn->cn.next;\
	while(__EGehtfi != __EGehtfr){\
		__EGehtfn = EGcontainerOf(__EGehtfi,EGeHashInfo_t,cn);\
		if(__EGehtfn->key == __EGehtfk) break;\
		__EGehtfi = __EGehtfi->next;}\
	__EGehtfi == __EGehtfr ? (EGeHashInfo_t*)0 : __EGehtfn;})

/* ========================================================================= */
/** @brief insert an element to the hash table. 
 * @param __htable pointer to the hash table where we are looking the key.
 * @param __hinfo pointer to the hash info structure that we want to add.
 * */
#define EGeHashTableAdd(__htable,__hinfo) do{\
	EGeHashTable_t*const __EGehta = (__htable);\
	EGeHashInfo_t*const __EGehtai = (__hinfo);\
	EGeList_t*__EGehtar = __EGehta->table + EGeHashFunc(__EGehtai->key,__EGehta->max_sz);\
	EGeListAddAfter(&(__EGehtai->cn),__EGehtar);\
	__EGehta->sz++;}while(0)
	

/* ========================================================================= */
/** @brief this is to eliminate an element from the hash table. 
 * @param __htable pointer to the hash table.
 * @param __hinfo pointer to the hash info structure that we want to remove.
 * */
#define EGeHashTableDel(__htable,__hinfo) do{\
	EGeHashTable_t*const __EGehtd = (__htable);\
	EGeHashInfo_t*const __EGehtdi = (__hinfo);\
	__EGehtd->sz--;\
	EGeListDel(&(__EGehtdi->cn));}while(0)

/* ========================================================================= */
/** @brief Given an initialized hash table, resize the table array and move all
 * members from the old array to the new one.
 * @param __htable pointer to the hash table.
 * @param __maxsz new size of the table (it should be positive).
 * */
#define EGeHashTableResize(__htable,__maxsz) do{\
	EGeHashTable_t*const __EGehtr = (__htable);\
	size_t __EGehtosz = __EGehtr->max_sz;\
	size_t __EGehtnsz = (__maxsz);\
	EGeList_t*__EGehtot = __EGehtr->table;\
	EGeHashInfo_t*__EGehtri = 0;\
	__EGehtr->max_sz = __EGehtnsz;\
	__EGehtr->sz = 0;\
	__EGehtr->table = EGsMalloc(EGeList_t,__EGehtnsz);\
	while(__EGehtnsz--){EGeListInit(__EGehtr->table+__EGehtnsz);}\
	for(;__EGehtosz--;){\
		while(__EGehtot[__EGehtosz].next != __EGehtot+__EGehtosz){\
			__EGehtri = EGcontainerOf(__EGehtot[__EGehtosz].next,EGeHashInfo_t,cn);\
			EGeListDel(&(__EGehtri->cn));\
			EGeHashTableAdd(__EGehtr,__EGehtri);}}\
	EGfree(__EGehtot);}while(0)		
	
/* ========================================================================= */
/** @} */
/* end eg_ehash.h */
#endif
