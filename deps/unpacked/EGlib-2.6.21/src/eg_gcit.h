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
/* ========================================================================= */
/** @defgroup EGgcIt Gray Code Iterator
 *
 * Here we define an implementation of knuth's looples gray code iterator for
 * binary strings as defined in ``The Art of Computer Programming, 
 * Chapter 7.2.1.1''.
 * */
/** @file 
 * @ingroup EGgcIt */
/** @addtogroup EGgcIt */
/** @{ */
/** @example eg_gcit.ex.c
 * This is a simple example of the usage of heaps using @ref EGgcIt */
/* ========================================================================= */
#ifndef __EG_GCIT__
#define __EG_GCIT__
#include "eg_config.h"
#include "eg_macros.h"
#include "eg_mem.h"
/* ========================================================================= */
/** @brief Debug level for the heap */
#define EG_GCIT_DBG 0

/* ========================================================================= */
/** @brief Structure to store the information relevant to the binary gray code
 * iterator. */
typedef struct EGgcIt_t
{
	int sz;					/**< @brief Number of bits in the code */
	int changed_pos;/**< @brief position of the last bit changed */
	int*tuple;			/**< @brief Current binary tuple being visited */
	int*focus;			/**< @brief Internal information */
}
EGgcIt_t;

/* ========================================================================= */
/** @brief Reset an initialized gray code iterator to the zero position.
 * @param __gc pointer to a gray-code iterator structure (#EGgcIt_t) 
 * */
#define EGgcItReset(__gc) do{\
	EGgcIt_t*const __EGgcit2 = (__gc);\
	register int __EGgci = __EGgcit2->sz;\
	__EGgcit2->focus[__EGgci]=__EGgci;\
	while(__EGgci--){__EGgcit2->tuple[__EGgci] = 0;\
	__EGgcit2->focus[__EGgci]=__EGgci;}\
	__EGgcit2->changed_pos=0;}while(0)

/* ========================================================================= */
/** @brief Initialize a binary gray code iterator, and set the iterator to the
 * zero position.
 * @param __gc pointer to a gray-code iterator structure (#EGgcIt_t) 
 * @param __sz number of bits to be used in the iterator.
 * */
#define EGgcItInit(__gc,__sz) do{\
	EGgcIt_t*const __EGgcit = (__gc);\
	const int __EGgcsz = __EGgcit->sz = ((int)(__sz));\
	__EGgcit->tuple = EGsMalloc(int,__EGgcsz);\
	__EGgcit->focus = EGsMalloc(int,__EGgcsz+1);\
	EGgcItReset(__EGgcit);}while(0)

/* ========================================================================= */
/** @brief free all internally allocated memory for the given #EGgcIt_t
 * structure.
 * @param __gc pointer to a gray-code iterator structure (#EGgcIt_t) 
 * */
#define EGgcItClear(__gc) do{\
	EGgcIt_t*const __EGgcit = (__gc);\
	EGfree(__EGgcit->tuple);\
	EGfree(__EGgcit->focus);\
	memset(__EGgcit,0,sizeof(EGgcIt_t));}while(0)

/* ========================================================================= */
/** @brief Return the number of bits for the given gray code iterator 
 * @param __gc pointer to a gray-code iterator structure (#EGgcIt_t) 
 * @return number of bits for the iterator
 * */
#define EGgcItGetSize(__gc) ((const int)((__gc)->sz))

/* ========================================================================= */
/** @brief Return a constant pointer to the current tuple 
 * @param __gc pointer to a gray-code iterator structure (#EGgcIt_t) 
 * @return pointer to an array containing the current tuple */
#define EGgcItGetTuple(__gc) ((const int*const)((__gc)->tuple))

/* ========================================================================= */
/** @brief return which position in the binary string changed in the last
 * iteration.
 * @param __gc pointer to a gray-code iterator structure (#EGgcIt_t) 
 * */
#define EGgcItGetChange(__gc) ((const int)((__gc)->changed_pos))

/* ========================================================================= */
/** @brief move to the next binary string, if no next string exists (i.e. we
 * finish the loop), return 0, otherwise return 1.
 * @param __gc pointer to a gray-code iterator structure (#EGgcIt_t) 
 * @return zero if no next string exists, otherwise 1.
 * */
#define EGgcItNext(__gc) ({\
	EGgcIt_t*const __EGgcit = (__gc);\
	const int __EGgccp = __EGgcit->changed_pos = __EGgcit->focus[0];\
	const int __EGgcrval=(__EGgccp!=__EGgcit->sz);\
	if(__EGgcrval){\
	__EGgcit->focus[0]=0;\
	__EGgcit->focus[__EGgccp]=__EGgcit->focus[__EGgccp+1];\
	__EGgcit->focus[__EGgccp+1]=__EGgccp+1;\
	__EGgcit->tuple[__EGgccp] = 1-__EGgcit->tuple[__EGgccp];}\
	__EGgcrval;})
	
/* ========================================================================= */
/** @} */
/* end of eg_gcit.h */
#endif
