/* EGlib "Efficient General Library" provides some basic structures and
 * algorithms commons in many optimization algorithms.
 *
 * Copyright (C) 2005-2008 Daniel Espinoza.
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
#include "eg_mem.h"
#ifndef __EG_SYMTAB_H__
#define __EG_SYMTAB_H__
/* ========================================================================= */
/** @defgroup EGsymtab Symbol table structure
 * This version suport addition and finding symbols, as well as getting its
 * associated number, which correspond to the order in which it was added. For
 * now it does not support deletion of elements.
 * */
/** @file
 * @ingroup EGsymtab */
/** @addtogroup EGsymtab */
/** @{ */
/** @example eg_symtab.ex.c
 * This is a simple example of the usage of symbol tables using @ref EGsymtab */
/* ========================================================================= */
/** @brief structure to store a symbol table */
typedef struct EGsymtab_t
{
	size_t nsym;				/**< @brief symbols in table */
	size_t sz;					/**< @brief actual size of some internal arrays (>=nsym) */
	int*symbols;				/**< @brief array of offsets of all symbols in table */ 
	int*sort_perm;			/**< @brief permutation of sorted symbols (used to find 
														by binary search) */
	int must_sort;			/**< @brief flag to indicate whether or not we should 
														re-sort sort_perm */ 
	char*all_sym;				/**< @brief array of all strings */
	size_t all_sz;			/**< @brief length of all_sym */
	size_t use_sz;			/**< @brief actual used space in all_sym */
} EGsymtab_t;
/* ========================================================================= */
/** @brief initialize an EGsymtab_t structure
 * @param __symtab structure to initialize 
 * */
#define EGsymtabInit(__symtab) memset(__symtab,0,sizeof(EGsymtab_t))
/* ========================================================================= */
/** @brief clear any internally allocated memory 
 * @param __symtab structure to clear
 * */
#define EGsymtabClear(__symtab) do{\
	EGsymtab_t*const __EGsymtab = (__symtab);\
	EGfree(__EGsymtab->all_sym);\
	EGfree(__EGsymtab->symbols);\
	EGfree(__EGsymtab->sort_perm);\
	__EGsymtab->nsym = __EGsymtab->sz = __EGsymtab->all_sz = \
	__EGsymtab->use_sz = 0;}while(0)
/* ========================================================================= */
/** @brief return the i-th symbol in the table
 * @note we don't check for limits, i should be >=0 < #EGsymtab_t::nsym
 * @param __i symbol we look for
 * @param __symtab table to use
 * @return pointer to the corresponding string 
 * */
#define EGsymtabSymbol(__symtab,__i) ({\
	const EGsymtab_t*const __EGsymtab = (__symtab);\
	((const char*const)((__EGsymtab)->all_sym+__EGsymtab->symbols[__i]));})
/* ========================================================================= */
/** @brief Add a new element to the symbol table, if the element already
 * exists, then return non zero, otherwise return zero
 * @param symtab table where we will add the symbol
 * @param symbol string to add to the table
 * @return zero on success, non-zero otherwise
 * */
int EGsymtabAdd(EGsymtab_t*const symtab,
								const char*const symbol);
/* ========================================================================= */
/** @brief Look for a symbol in the table, and if found, report the position of
 * the symbol in the table.
 * @param symtab table where we will add the symbol
 * @param symbol string to look up in the table
 * @param pos if not null, return there the position of the symbol in the
 * table.
 * @return one if the symbol is in the table, zero otherwise.
 * */
int EGsymtabLookUp( EGsymtab_t*const symtab,
										const char*const symbol,
										int*const pos);
/* ========================================================================= */
/** @} */
/* end eg_symtab.h */
#endif
