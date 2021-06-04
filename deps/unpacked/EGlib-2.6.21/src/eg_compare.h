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
#ifndef __EG_COMPARE_H__
#define __EG_COMPARE_H__
#include "eg_config.h"
#include "eg_macros.h"
#include "eg_mem.h"
/* ========================================================================= */
/** @defgroup EGcompare EGcompare
 *
 * Here we define an interface for comparison functions, and some default
 * comparators.
 *
 * -2004-08-16
 * 					- First Implementation
 * */
/** @file
 * @ingroup EGcompare */
/* @{ */
/* ========================================================================= */

/* ========================================================================= */
/** @brief comparison function type.
 * @par Description:
 * This type of function is intended to represent general comparison functions,
 * the comparison function must return an integer less than, equal to, or 
 * greater than zero if the first argument is considered to be respectively 
 * less than, equal to, or greater than the second. Note that the standard
 * function memcompare operate in the same sense.
 * */
typedef int (*EGcompare_f) (const void *,
														const void *);

/* ========================================================================= */
/** @brief Lexicographical order of strings.
 * @par Description:
 * This function compare two strings of (\0 terminated) chars in
 * lexicographical order. */
extern int EGstringCompare (const void *str1,
														const void *str2);

/* ========================================================================= */
/** @brief Normal order of doubles.
 * @par Description:
 * This function compare two doubles. */
extern int EGlfCompare (const void *str1,
												const void *str2);

/* ========================================================================= */
/** @brief Normal order of integers.
 * @par Description:
 * This function compare two integers. */
extern int EGdCompare (const void *str1,
											 const void *str2);

/* ========================================================================= */
/** @brief Normal order of unsigned integers.
 * @par Description:
 * This function compare two unsigned integers. */
extern int EGudCompare (const void *str1,
												const void *str2);

/* ========================================================================= */
/** @brief Normal order of pointers.
 * @par Description:
 * This function compare pointers in 'memory' order. */
extern int EGptCompare (const void *str1,
												const void *str2);

/* ========================================================================= */
/* } */
/* end of eg_compare.h */
#endif
