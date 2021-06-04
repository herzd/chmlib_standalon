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
/** @defgroup EGpermIt All Permutations Iterator
 *
 * Here we define an implementation of knuth'__EGs plain changes iterator  for
 * permutations of \f$n\f$ different elements as defined in 
 * ``The Art of Computer Programming, Chapter 7.2.1.2''.
 * We use an algorithms that perform single swaps in each step, the last
 * position of the swap is stored.
 * */
/** @file 
 * @ingroup EGpermIt */
/** @addtogroup EGpermIt */
/** @{ */
/** @example eg_perm_it.ex.c
 * This is a simple example of the usage of heaps using @ref EGpermIt */
/* ========================================================================= */
#ifndef __EG_PERMIT__
#define __EG_PERMIT__
#include "eg_config.h"
#include "eg_macros.h"
#include "eg_nummacros.h"
#include "eg_mem.h"
/* ========================================================================= */
/** @brief Debug level for the heap */
#define EG_PERMIT_DBG 0

/* ========================================================================= */
/** @brief Structure to store the information relevant to the permutation 
 * iterator. */
typedef struct EGpermIt_t
{
	int sz;					/**< @brief Number of elements to permute */
	int changed_pos;/**< @brief position of the last bit changed */
	int*tuple;			/**< @brief Current binary tuple being visited */
	int*c_focus;		/**< @brief Internal information */
	int*o_focus;		/**< @brief Internal information */
}
EGpermIt_t;

/* ========================================================================= */
/** @brief Reset an initialized permutation iterator to the zero position.
 * @param __gc pointer to a permutation iterator structure (#EGpermIt_t) 
 * */
#define EGpermItReset(__gc) do{\
	EGpermIt_t*const __EGgcit2 = (__gc);\
	register int __EGgci = __EGgcit2->sz;\
	while(__EGgci--){__EGgcit2->tuple[__EGgci] = __EGgci;\
	__EGgcit2->o_focus[__EGgci]=1;\
	__EGgcit2->c_focus[__EGgci]=0;}\
	__EGgcit2->changed_pos=0;}while(0)

/* ========================================================================= */
/** @brief Initialize a binary permutation iterator, and set the iterator to the
 * zero position.
 * @param __gc pointer to a permutation iterator structure (#EGpermIt_t) 
 * @param __sz number of bits to be used in the iterator.
 * */
#define EGpermItInit(__gc,__sz) do{\
	EGpermIt_t*const __EGgcit = (__gc);\
	const int __EGgcsz = __EGgcit->sz = ((int)(__sz));\
	__EGgcit->tuple = EGsMalloc(int,__EGgcsz);\
	__EGgcit->o_focus = EGsMalloc(int,__EGgcsz);\
	__EGgcit->c_focus = EGsMalloc(int,__EGgcsz);\
	EGpermItReset(__EGgcit);}while(0)

/* ========================================================================= */
/** @brief free all internally allocated memory for the given #EGpermIt_t
 * structure.
 * @param __gc pointer to a permutation iterator structure (#EGpermIt_t) 
 * */
#define EGpermItClear(__gc) do{\
	EGpermIt_t*const __EGgcit = (__gc);\
	EGfree(__EGgcit->tuple);\
	EGfree(__EGgcit->o_focus);\
	EGfree(__EGgcit->c_focus);\
	memset(__EGgcit,0,sizeof(EGpermIt_t));}while(0)

/* ========================================================================= */
/** @brief Return the number of bits for the given permutation iterator 
 * @param __gc pointer to a permutation iterator structure (#EGpermIt_t) 
 * @return number of bits for the iterator
 * */
#define EGpermItGetSize(__gc) ((const int)((__gc)->sz))

/* ========================================================================= */
/** @brief Return a constant pointer to the current tuple 
 * @param __gc pointer to a permutation iterator structure (#EGpermIt_t) 
 * @return pointer to an array containing the current tuple */
#define EGpermItGetTuple(__gc) ((const int*const)((__gc)->tuple))

/* ========================================================================= */
/** @brief return which position in the permutation changed in the last
 * iteration.
 * @param __gc pointer to a permutation iterator structure (#EGpermIt_t) 
 * */
#define EGpermItGetChange(__gc) ((const int)((__gc)->changed_pos))

/* ========================================================================= */
/** @brief choose a macro or function definition of #EGpermItNext */
#define __PERM_IT_NO_INLINE__ 0
/* ========================================================================= */
/** @brief move to the next permutation, if no next string exists (i.e. we
 * finish the loop), return 0, otherwise return 1.
 * @param __EGprit pointer to a permutation iterator structure (#EGpermIt_t) 
 * @return zero if no next string exists, otherwise 1.
 * */
#if __PERM_IT_NO_INLINE__
int EGpermItNext(EGpermIt_t*const __EGprit);
#else
#define EGpermItNext(__prit) ({\
	EGpermIt_t*const __EGprit = (__prit);\
	int*const __EGc = __EGprit->c_focus;\
	int*const __EGo = __EGprit->o_focus;\
	int*const __EGtuple = __EGprit->tuple;\
	int __EGitmp, __EGj=__EGprit->sz-1, __EGs=0, __EGpos=0;\
	int  __EGq=__EGc[__EGj]+__EGo[__EGj];\
	int __EGrval = 1;\
	/* find where we should make the following switch */\
	while(__EGq<0 || __EGq>__EGj)\
	{\
		if(__EGq>__EGj)\
		{\
			if (!__EGj){\
				__EGrval= 0;\
				break;}\
			__EGs+=1;\
		}\
		__EGo[__EGj]=-__EGo[__EGj];\
		__EGj-=1;\
		__EGq=__EGc[__EGj]+__EGo[__EGj];\
	}\
	/* now we just switch positions __EGj-c_j+__EGs and __EGj-__EGq+__EGs */\
	if(__EGrval){\
		__EGprit->changed_pos = __EGpos = __EGc[__EGj] > __EGq ? __EGj-__EGc[__EGj]+__EGs:__EGj-__EGq+__EGs;\
		__EGc[__EGj] = __EGq;\
		__EGitmp = __EGtuple[__EGpos];\
		__EGtuple[__EGpos] = __EGtuple[__EGpos+1];\
		__EGtuple[__EGpos+1] = __EGitmp;\
	}\
	/* and we are done */\
	__EGrval;})
#endif
	
/* ========================================================================= */
/** @} */
/* end of eg_gcit.h */
#endif
