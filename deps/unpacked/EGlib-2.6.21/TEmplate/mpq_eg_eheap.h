#include "eg_config.h"
#ifdef HAVE_LIBGMP
#if HAVE_LIBGMP
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
/** @defgroup EGeHeap EGeHeap
 *
 * Here we define the basic interface for d-heaps as an embeded structure.
 * In this implementation the heap does not grow on the fly, meaning that it 
 * may fills-up during an add call, to avoid that, the user must call 
 * re-allocate when necesary. the heap start as a heap of size zero. 
 * This implementatioon is a minimum-heap implementatiton. Note also that the
 * internal connector array is shifted one position to the left. This is done 
 * so that the first element is in position 1, this also speed-up the 
 * computation of the parent and childrens of a given position.
 *
 * @version 0.0.1
 * @par History:
 * - 2013-04-03
 * 						- Change EGeheapSiftUp to push as far as possible he given
 * 						element; this helps the heap to behave like breath first search
 * 						when the values of the elements in the heap are equal
 * - 2010-09-05
 * 						- mpq_EGeHeapClear will free any internal memory (even if it was
 * 						asked for by the user).
 * - 2005-07-14
 * 						- Add mpq_EGeHeapEmpty to empty the heap (but keep its maximum
 * 							size)
 * 						- Add mpq_EGeHeapIsFull to test wether a heap is full or not.
 * - 2005-07-07
 * 						- First Implementation
 * @note 
 * This implementatiton is designed as a template using as base the types of
 * @ref EGlpNum
 * */
/** @file 
 * @ingroup EGeHeap */
/** @addtogroup EGeHeap */
/** @{ */
/** @example mpq_eg_eheap.ex.c
 * This is a simple example of the usage of heaps using @ref EGeHeap */
/* ========================================================================= */
#ifndef mpq___EG_EHEAP__
#define mpq___EG_EHEAP__
#include "eg_config.h"
#include "eg_macros.h"
#include "eg_lpnum.h"
/* ========================================================================= */
/** @brief Debug level for the heap */
#ifndef mpq_EG_EHEAP_DEBUG
#define mpq_EG_EHEAP_DEBUG 1000
#endif

/* ========================================================================= */
/** @name Test macros, enabled only if debug level is high enough. */
/** @{ */
#if mpq_EG_EHEAP_DEBUG <= DEBUG
#define mpq_EGeHeapCHECK_CN(__hp2,__hcn2) EXIT(__hcn2->pos>=__hp2->sz,"Heap Connector possition %d out of range [0,%d]",__hcn2->pos,__hp2->sz)
#define mpq_EGeHeapCHECK_NF(__hp2) EXIT(__hp2->sz >= __hp2->max_sz,"Heap "#__hp2" Is full, can't add an element")
#else
#define mpq_EGeHeapCHECK_CN(__hp,__hcn) 
#define mpq_EGeHeapCHECK_NF(__hp) 
#endif
/** @} */
/* ========================================================================= */
/** @brief Structure to store the information relevant to an element in the
 * heap. */
typedef struct 
{
	mpq_t val;		/**< Value of this node in the heap */
	unsigned int pos;	/**< Position in the heap array for this node, if set to
												 #mpq_EG_EHEAP_POISON, then the connector is not in any 
												 heap.*/
}
mpq_EGeHeapCn_t;

/* ========================================================================= */
/** @brief Poison position for heap connector not in a heap. */
#define mpq_EG_EHEAP_POISON UINT_MAX

/* ========================================================================= */
/** @brief Initialize a heap conector structure. This function will allocate any
 * interal memory not allocated by the user, it should be called only once, or
 * after a clear function call.
 * @param __hcn conector to initialize.
 * */
#define mpq_EGeHeapCnInit(__hcn) ({mpq_EGlpNumInitVar((__hcn)->val);(__hcn)->pos = mpq_EG_EHEAP_POISON;})

/* ========================================================================= */
/** @brief Reset a heap conector to the same state as after an init call, this
 * function is provided only for completness.
 * @param __hcn conector to reset
 * */
#define mpq_EGeHeapCnReset(__hcn) ((__hcn)->pos = mpq_EG_EHEAP_POISON)

/* ========================================================================= */
/** @brief Free all internal memory used by this structured not allocated by the
 * user. This function should be called after an init call, and only once.
 * @param __hcn conector to clear.
 * */
#define mpq_EGeHeapCnClear(__hcn) mpq_EGlpNumClearVar((__hcn)->val)

/* ========================================================================= */
/** @brief Structure to hold a whole heap structure, this structure is designed
 * so that it can grow on the fly with a low cost */
typedef struct 
{
	mpq_EGeHeapCn_t **cn;
	unsigned int d;
	unsigned int sz;
	unsigned int max_sz;
}
mpq_EGeHeap_t;

/* ========================================================================= */
/** @brief Return one if the heap is full, zero otherwise.
 * @param __hp heat to check */
#define mpq_EGeHeapIsFull(__hp) ({mpq_EGeHeap_t*const __EGehp = (__hp); __EGehp->sz == __EGehp->max_sz;})

/* ========================================================================= */
/** @brief set the number of elements in hte heap to zero.
 * @param __hp heap to empty.
 * */
#define mpq_EGeHeapEmpty(__hp) ((__hp)->sz = 0)

/* ========================================================================= */
/** @brief Initialize a heap as an empty heap (with no space for conectors).
 * @param __hp heap to initialize.
 * */
#define mpq_EGeHeapInit(__hp) (*(__hp) = (mpq_EGeHeap_t){0,0,0,0})

/* ========================================================================= */
/** @brief Reset the given heap as an empty heap (just as returned by the init
 * call.
 * @param __hp heap to reset 
 * */
#define mpq_EGeHepReset(__hp) mpq_EGeHeapResize(__hp,0)

/* ========================================================================= */
/** @brief Clear a heap structure, and free any internal memory (not allocated
 * by the user).
 * @param __hp heap to clear.
 * */
#define mpq_EGeHeapClear(__hp) mpq_EGeHeapResize(__hp,0)

/* ========================================================================= */
/** @brief get the minimum value in the heap.
 * @param __hp heap where we are working.
 * @param number where to store the result
 * @return zero on success, non-zero otherwise.
 * */
#define mpq_EGeHeapGetMinVal(__hp,number) ({\
	mpq_EGeHeap_t*const __EGehp = (__hp);\
	__EGehp->sz ? (mpq_EGlpNumCopy(number,__EGehp->cn[0]->val),0):1;})

/* ========================================================================= */
/** @brief get the minimum conector in the heap, if the heap is empty, return
 * NULL.
 * @param __hp eap where we are working.
 * @return pointer to the minimum element in the heap.
 * */
#define mpq_EGeHeapGetMin(__hp) ({\
	mpq_EGeHeap_t*const __EGehp = (__hp);\
	__EGehp->sz ? __EGehp->cn[0] : 0;})

/* ========================================================================= */
/** @brief resize the heap cn array to the given size, if the new size is zero,
 * it is equivalent to free the internal memory, and left the heap as an empty
 * heap with zero space.
 * @param __hp heap where we are working.
 * @param new_sz new size for the  cn array .
 * */
#define mpq_EGeHeapResize(__hp,new_sz) ({\
	mpq_EGeHeap_t*const __EGehp = (__hp);\
	const size_t __EGehp_nsz = (size_t)(new_sz);\
	__EGehp->cn = EGrealloc((__EGehp->cn), __EGehp_nsz * sizeof(mpq_EGeHeapCn_t*));\
	__EGehp->max_sz = (unsigned int)(__EGehp_nsz);})

/* ========================================================================= */
/** @brief return the index of the father of the given index.
 * @param __d breadth of the heap.
 * @param __id position in the array to wich we want to compute it's father.
 * */
#define mpq_EGeHeapFatherId(__d,__id) ((__id)?(((__id)-1)/(__d)):0)

/* ========================================================================= */
/** @brief move an element in the heap up in the heap (position 0 is the top,
 * this kind of move is neded whenever we decrease the value in a heap element).
 * @param __hp heap where we are working.
 * @param __hcn element in the heap to move.
 * */
#define mpq_EGeHeapSiftUp(__hp,__hcn) ({\
	mpq_EGeHeap_t*const __EGehp = (__hp);\
	mpq_EGeHeapCn_t*const __EGecn = (__hcn);\
	unsigned int __EGcpos = __EGecn->pos;\
	unsigned int __EGfpos = mpq_EGeHeapFatherId(__EGehp->d,__EGcpos);\
	mpq_EGeHeapCn_t*__EGfcn = __EGehp->cn[__EGfpos];\
	mpq_EGeHeapCHECK_CN(__EHehp,__EGecn);\
	while(__EGcpos && \
				mpq_EGlpNumIsLeq(__EGecn->val,__EGfcn->val))\
	{\
		__EGfcn->pos = __EGcpos;\
		__EGehp->cn[__EGcpos] = __EGfcn;\
		__EGcpos = __EGfpos;\
		__EGfpos = mpq_EGeHeapFatherId(__EGehp->d,__EGcpos);\
		__EGfcn = __EGehp->cn[__EGfpos];\
	}\
	__EGecn->pos = __EGcpos;\
	__EGehp->cn[__EGcpos] = __EGecn;\
	0;})

/* ========================================================================= */
/** @brief Add an element to the heap
 * @param __hp heap where to add the element.
 * @param __hcn element to be added.
 * @return zero on success, non-zero otherwise.
 * */
#define mpq_EGeHeapAdd(__hp,__hcn) ({\
	mpq_EGeHeap_t*const __EGlhp = (__hp);\
	mpq_EGeHeapCn_t*const __EGlcn = (__hcn);\
	mpq_EGeHeapCHECK_NF(__EGlhp);\
	__EGlcn->pos = __EGlhp->sz, \
	__EGlhp->cn[__EGlhp->sz] = __EGlcn;\
	__EGlhp->sz +=1, \
	mpq_EGeHeapSiftUp(__EGlhp,__EGlcn), 0;})

/* ========================================================================= */
/** @brief Give the first child for a given position.
 * @param __id position that we want to get the first child.
 * @param __d breath of the heap. */
#define mpq_EGeHeapFirstChildId(__d,__id) ((__d)*(__id)+1)

/* ========================================================================= */
/** @brief Move an element down in the heap (position 0 is the
 * top), this kind of operation is needed whenever we increase the value in a
 * heap element.
 * @param __hp heap where we are working.
 * @param __hcn element in the heap to move.
 * @return zero on success, non-zero otherwise.
 * */
#define mpq_EGeHeapSiftDown(__hp,__hcn) ({\
	mpq_EGeHeap_t*const __EGehp = (__hp);\
	mpq_EGeHeapCn_t*const __EGecn = (__hcn);\
	const unsigned int __EGhsz = __EGehp->sz;\
	unsigned int __EGcpos = __EGecn->pos;\
	unsigned int __EGfchd = mpq_EGeHeapFirstChildId(__EGehp->d,__EGcpos);\
	unsigned int __EGlchd = __EGfchd + __EGehp->d;\
	mpq_EGeHeapCn_t*__EGcchd = 0;\
	register unsigned int __EGehi = 0;\
	mpq_EGeHeapCHECK_CN(__EGehp,__EGecn);\
	while(__EGfchd < __EGhsz)\
	{\
		/* detect the minimum child */\
		__EGcchd = __EGehp->cn[__EGfchd];\
		for(__EGehi = __EGlchd > __EGhsz ? __EGhsz-1 : __EGlchd-1 ;\
			__EGehi > __EGfchd ; __EGehi--)\
			if(mpq_EGlpNumIsLess(__EGehp->cn[__EGehi]->val,__EGcchd->val))\
				__EGcchd = __EGehp->cn[__EGehi];\
		/* if the minimum child is less than the current position, move the minimum\
		 * child to the position of the current element */\
		if(mpq_EGlpNumIsLess(__EGcchd->val,__EGecn->val))\
		{\
			__EGfchd = __EGcchd->pos;\
			__EGcchd->pos = __EGcpos;\
			__EGehp->cn[__EGcpos] = __EGcchd;\
			__EGecn->pos = __EGcpos = __EGfchd;\
			__EGehp->cn[__EGcpos] = __EGecn;\
			__EGfchd = mpq_EGeHeapFirstChildId(__EGehp->d,__EGcpos);\
			__EGlchd = __EGfchd + __EGehp->d;\
		}\
		/* else we exit the main loop */\
		else __EGfchd = UINT_MAX;\
	}\
	0;})

/* ========================================================================= */
/** @brief Change the value of an element in the heap.
 * @param __hp heap where we are working.
 * @param __hcn element in the heap that we are going to change it's value.
 * @param __new_val new value for the element.
 * @return zero on success, non-zero otherwise.
 * */
#define mpq_EGeHeapChangeVal(__hp,__hcn,__new_val) ({\
	(mpq_EGlpNumIsLess(__new_val,(__hcn)->val)) ? (mpq_EGlpNumCopy((__hcn)->val,__new_val),mpq_EGeHeapSiftUp(__hp,__hcn)) : (mpq_EGlpNumCopy((__hcn)->val,__new_val),mpq_EGeHeapSiftDown(__hp,__hcn));})

/* ========================================================================= */
/** @brief Eliminate an element from the heap, note that the position stored in
 * the eliminated element is reset to zero.
 * @param __hp heap where we are working.
 * @param __hcn element to eliminate from the heap.
 * @return zero on success, non-zero otherwise.
 * */
#define mpq_EGeHeapDel(__hp,__hcn) ({\
	mpq_EGeHeap_t*const __EGlhp = (__hp);\
	mpq_EGeHeapCn_t*const __EGlhpcn = (__hcn);\
	unsigned int const __EGlcn = __EGlhpcn->pos;\
	unsigned int const __EGlhsz = __EGlhp->sz - 1;\
	__EGlhpcn->pos = mpq_EG_EHEAP_POISON;\
	__EGlhp->sz = __EGlhsz;\
	if(__EGlhsz && __EGlhsz != __EGlcn){\
		__EGlhp->cn[__EGlcn] = __EGlhp->cn[__EGlhp->sz];\
		__EGlhp->cn[__EGlcn]->pos = __EGlcn;\
		mpq_EGeHeapSiftDown(__EGlhp,__EGlhp->cn[__EGlcn]);}\
	__EGlhp->cn[__EGlhp->sz] = 0;})

/* ========================================================================= */
/** @brief Check the integrity of the given heap.
 * @param __hp heap to check.
 * @return zero on success, non-zero otherwise.
 * */
#if mpq_EG_EHEAP_DEBUG <= DEBUG
#define mpq_EGeHeapCheck(__hp) ({\
	mpq_EGeHeap_t*const __EGehp = (__hp);\
	register unsigned int __EGehi = __EGehp->sz;\
	if(__EGehi)\
		while(--__EGehi)\
			if(__EGehp->cn[__EGehi]->pos != __EGehi || mpq_EGlpNumIsLess( __EGehp->cn[\
				 __EGehi]->val,__EGehp->cn[mpq_EGeHeapFatherId(__EGehp->d,__EGehi)]->val))\
			{\
				MESSAGE(mpq_EG_EHEAP_DEBUG,"Element %u is wrong, pos %u val [%lf,%lf]"\
							 ,__EGehi, __EGehp->cn[__EGehi]->pos, \
							 mpq_EGlpNumToLf(__EGehp->cn[__EGehi]->val), \
							 mpq_EGlpNumToLf(__EGehp->cn[mpq_EGeHeapFatherId(__EGehp->d,__EGehi)]->val));\
				break;\
			}\
	__EGehi;})
#else
#define mpq_EGeHeapCheck(__hp) 0
#endif

/* ========================================================================= */
/** @brief set the breath of the heap, this function must be called only when
 * the heap is empty.
 * @param __hp heap to set breath.
 * @param __width new with for the heap.
 * @return zero on success, non-zero otherwise.
 * */
#define mpq_EGeHeapChangeD(__hp,__width) ({\
	mpq_EGeHeap_t*const __EGehp = (__hp);\
	EXIT((__width)<2,"Width should be at least 2 for heaps");\
	__EGehp->sz ? 1 : (__EGehp->d = (__width), 0);})

/* ========================================================================= */
/** @} */
/* end of mpq_eg_eheap.h */
#endif
#endif
#endif
