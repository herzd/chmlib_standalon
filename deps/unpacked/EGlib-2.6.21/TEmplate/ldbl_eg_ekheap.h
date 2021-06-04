#include "eg_config.h"
#ifdef HAVE_LONG_DOUBLE
#if HAVE_LONG_DOUBLE
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
/** @defgroup EGeKHeap EGeKHeap
 *
 * Here we define the basic interface for d-heaps with an array of values with
 * the lexicographic order for vectors as an embeded structure.
 * In this implementation the heap does not grow on the fly, meaning that it 
 * may fill-up during an add call, to avoid that, the user must call 
 * re-allocate when necesary. the heap start as a heap of size zero. 
 * This implementatioon is a minimum-heap implementatiton. Note also that the
 * internal connector array is shifted one position to the left. This is done 
 * so that the first element is in position 1, this also speed-up the 
 * computation of the parent and childrens of a given position.
 *
 * @version 0.0.1
 * @par History:
 * - 2010-09-05
 * 						- Change implementation of ldbl_EGeKHeapClear to free all internal
 * 						memory, including the one asked for the user during a
 * 						ldbl_EGeKHeapResize call.
 * - 2008-07-30
 * 						- First implementation
 * @note 
 * This implementatiton is designed as a template using as base the types of
 * @ref EGlpNum
 * */
/** @file 
 * @ingroup EGeKHeap */
/** @addtogroup EGeKHeap */
/** @{ */
/** @example ldbl_eg_ekheap.ex.c
 * This is a simple example of the usage of heaps using @ref EGeKHeap */
/* ========================================================================= */
#ifndef ldbl___EG_EKHEAP__
#define ldbl___EG_EKHEAP__
#include "eg_config.h"
#include "eg_macros.h"
#include "eg_lpnum.h"
/* ========================================================================= */
/** @brief Debug level for the heap */
#ifndef ldbl_EG_EKHEAP_DEBUG
#define ldbl_EG_EKHEAP_DEBUG 100
#endif

/* ========================================================================= */
/** @name Test macros, enabled only if debug level is high enough. */
/** @{ */
#if ldbl_EG_EKHEAP_DEBUG <= DEBUG
#define ldbl_EGeKHeapCHECK_CN(__hp2,__hcn2) EXIT(__hcn2->pos>=__hp2->sz,"Heap Connector possition %d out of range [0,%d]",__hcn2->pos,__hp2->sz)
#define ldbl_EGeKHeapCHECK_NF(__hp2) EXIT(__hp2->sz >= __hp2->max_sz,"Heap "#__hp2" Is full, can't add an element")
#else
#define ldbl_EGeKHeapCHECK_CN(__hp,__hcn) 
#define ldbl_EGeKHeapCHECK_NF(__hp) 
#endif
/** @} */
/* ========================================================================= */
/** @brief number of maximum entries in the vector values */
#ifndef ldbl_EG_EKHEAP_ENTRY
#define ldbl_EG_EKHEAP_ENTRY 3
#endif
/* ========================================================================= */
/** @brief Structure to store the information relevant to an element in the
 * heap. */
typedef struct
{
	long double val[ldbl_EG_EKHEAP_ENTRY];/**< Value of this node in the heap */
	unsigned int pos;	/**< Position in the heap array for this node, if set to
												 #ldbl_EG_EKHEAP_POISON, then the connector is not in any 
												 heap.*/
}
ldbl_EGeKHeapCn_t;

/* ========================================================================= */
/** @brief Poison position for heap connector not in a heap. */
#define ldbl_EG_EKHEAP_POISON UINT_MAX

/* ========================================================================= */
/** @brief Initialize a heap conector structure. This function will allocate any
 * interal memory not allocated by the user, it should be called only once, or
 * after a clear function call.
 * @param __hcn conector to initialize.
 * */
#define ldbl_EGeKHeapCnInit(__hcn) do{\
	ldbl_EGeKHeapCn_t*const __EKHcn = (__hcn);\
	int __EKHi = ldbl_EG_EKHEAP_ENTRY;\
	memset(__EKHcn,0,sizeof(ldbl_EGeKHeapCn_t));\
	for( ; __EKHi-- ; ){ldbl_EGlpNumInitVar(__EKHcn->val[__EKHi]);}\
	__EKHcn->pos = ldbl_EG_EKHEAP_POISON;}while(0)

/* ========================================================================= */
/** @brief Reset a heap conector to the same state as after an init call, this
 * function is provided only for completness.
 * @param __hcn conector to reset
 * */
#define ldbl_EGeKHeapCnReset(__hcn) ((__hcn)->pos = ldbl_EG_EKHEAP_POISON)

/* ========================================================================= */
/** @brief Free all internal memory used by this structured not allocated by the
 * user. This function should be called after an init call, and only once.
 * @param __hcn conector to clear.
 * */
#define ldbl_EGeKHeapCnClear(__hcn) do{\
	ldbl_EGeKHeapCn_t*const __EKHcn = (__hcn);\
	int __EKHi = ldbl_EG_EKHEAP_ENTRY;\
	for( ; __EKHi-- ; ){ldbl_EGlpNumClearVar(__EKHcn->val[__EKHi]);}}while(0)

/* ========================================================================= */
/** @brief Structure to hold a whole heap structure, this structure is designed
 * so that it can grow on the fly with a low cost */
typedef struct
{
	ldbl_EGeKHeapCn_t **cn;
	unsigned int d;
	unsigned int sz;
	unsigned int max_sz;
}
ldbl_EGeKHeap_t;

/* ========================================================================= */
/** @brief Return one if the heap is full, zero otherwise.
 * @param __hp heat to check */
#define ldbl_EGeKHeapIsFull(__hp) ({ldbl_EGeKHeap_t*const __EGekhp = (__hp); __EGekhp->sz == __EGekhp->max_sz;})

/* ========================================================================= */
/** @brief set the number of elements in hte heap to zero.
 * @param __hp heap to empty.
 * */
#define ldbl_EGeKHeapEmpty(__hp) ((__hp)->sz = 0)

/* ========================================================================= */
/** @brief Initialize a heap as an empty heap (with no space for conectors).
 * @param __hp heap to initialize.
 * */
#define ldbl_EGeKHeapInit(__hp) (*(__hp) = (ldbl_EGeKHeap_t){0,0,0,0})

/* ========================================================================= */
/** @brief Reset the given heap as an empty heap (just as returned by the init
 * call.
 * @param __hp heap to reset 
 * */
#define ldbl_EGeKHepReset(__hp) ldbl_EGeKHeapResize(__hp,0)

/* ========================================================================= */
/** @brief Clear a heap structure, and free any internal memory (not allocated
 * by the user).
 * @param __hp heap to clear.
 * */
#define ldbl_EGeKHeapClear(__hp) ldbl_EGeKHeapResize(__hp,0)

/* ========================================================================= */
/** @brief get the minimum value in the heap, note that since we are dealing
 * with a vector of values sorted lexicographically, the value is the value in
 * the first coordinate, other values can be accesses through
 * ldbl_EGeKHeapGetMinKVal function.
 * @param __hp heap where we are working.
 * @param __number where to store the result
 * @return zero on success, non-zero otherwise.
 * */
#define ldbl_EGeKHeapGetMinVal(__hp,__number) ({\
	ldbl_EGeKHeap_t*const __EGehp = (__hp);\
	__EGehp->sz ? (ldbl_EGlpNumCopy(__number,__EGehp->cn[0]->val[0]),0):1;})

/* ========================================================================= */
/** @brief get the k-th value of the first element in the heap.
 * @param __hp heap where we are working.
 * @param __number where to store the result.
 * @param __k which value to get (between 0 and ldbl_EG_EKHEAP_ENTRY.
 * @return zero on success, non-zero otherwise.
 * */
#define ldbl_EGeKHeapGetMinKVal(__hp,__k,__number) ({\
	ldbl_EGeKHeap_t*const __EGehp = (__hp);\
	const int __EGki = (__k);\
	EXITL(ldbl_EG_EKHEAP_DEBUG,(__EGki >= ldbl_EG_EKHEAP_ENTRY) || (__EGki <0),\
				"K=%d out of range in ldbl_EGeKHeapGetMinKVal", __EGki);\
	__EGehp->sz ? (ldbl_EGlpNumCopy(__number,__EGehp->cn[0]->val[__EGki]),0):1;})

/* ========================================================================= */
/** @brief get the minimum conector in the heap, if the heap is empty, return
 * NULL.
 * @param __hp eap where we are working.
 * @return pointer to the minimum element in the heap.
 * */
#define ldbl_EGeKHeapGetMin(__hp) ({\
	ldbl_EGeKHeap_t*const __EGehp = (__hp);\
	__EGehp->sz ? __EGehp->cn[0] : 0;})

/* ========================================================================= */
/** @brief resize the heap cn array to the given size, if the new size is zero,
 * it is equivalent to free the internal memory, and left the heap as an empty
 * heap with zero space.
 * @param __hp heap where we are working.
 * @param __new_sz new size for the  cn array .
 * */
#define ldbl_EGeKHeapResize(__hp,__new_sz) ({\
	ldbl_EGeKHeap_t*const __EGehp = (__hp);\
	const size_t __EGehp_nsz = (size_t)(__new_sz);\
	__EGehp->cn = EGrealloc((__EGehp->cn), __EGehp_nsz * sizeof(ldbl_EGeKHeapCn_t*));\
	__EGehp->max_sz = (unsigned int)(__EGehp_nsz);})

/* ========================================================================= */
/** @brief return the index of the father of the given index.
 * @param __d breadth of the heap.
 * @param __id position in the array to wich we want to compute it's father.
 * */
#define ldbl_EGeKHeapFatherId(__d,__id) ((__id)?(((__id)-1)/(__d)):0)

/* ========================================================================= */
/** @brief given two heap connectors, return one if the first is less than the
 * second (in  lexicographic order).
 * @param __hcn1 first vector array.
 * @param __hcn2 second vector array.
 * @return one if __hcn1 <_LEX __hcn2 */
#define ldbl_EGeKHeapIsLess(__hcn1,__hcn2) ({\
	const long double*const __EGEKH1 = (__hcn1);\
	const long double*const __EGEKH2 = (__hcn2);\
	int __EGEKHj = 0, __EGEKHrval = 0;\
	for( ; __EGEKHj < ldbl_EG_EKHEAP_ENTRY ; __EGEKHj++)\
	{\
		if(ldbl_EGlpNumIsLess(__EGEKH1[__EGEKHj], __EGEKH2[__EGEKHj])){\
			__EGEKHrval = 1; break;}\
		else if (ldbl_EGlpNumIsNeq(__EGEKH1[__EGEKHj], __EGEKH2[__EGEKHj],ldbl_epsLpNum)){\
			__EGEKHrval = 0; break;}\
	}\
	__EGEKHrval;})

/* ========================================================================= */
/** @brief copy two vector of values (only  ldbl_EG_EKHEAP_ENTRY positions) from the
 * rource to the destination.
 * @param src ldbl_source array.
 * @param dst destination array.
 * */
void ldbl_EGeKHeapCopyVal(long double*const dst, const long double*const src);

/* ========================================================================= */
/** @brief move an element in the heap up in the heap (position 0 is the top,
 * this kind of move is neded whenever we decrease the value in a heap element).
 * @param __hp heap where we are working.
 * @param __hcn element in the heap to move.
 * @return zero on success, non-zero otherwise.
 * */
#define ldbl_EGeKHeapSiftUp(__hp,__hcn) ({\
	ldbl_EGeKHeap_t*const __EGehp = (__hp);\
	ldbl_EGeKHeapCn_t*const __EGecn = (__hcn);\
	unsigned int __EGcpos = __EGecn->pos;\
	unsigned int __EGfpos = ldbl_EGeKHeapFatherId(__EGehp->d,__EGcpos);\
	ldbl_EGeKHeapCn_t*__EGfcn = __EGehp->cn[__EGfpos];\
	ldbl_EGeKHeapCHECK_CN(__EGehp,__EGecn);\
	while(__EGcpos && \
				ldbl_EGeKHeapIsLess(__EGecn->val,__EGfcn->val))\
	{\
		__EGfcn->pos = __EGcpos;\
		__EGehp->cn[__EGcpos] = __EGfcn;\
		__EGcpos = __EGfpos;\
		__EGfpos = ldbl_EGeKHeapFatherId(__EGehp->d,__EGcpos);\
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
#define ldbl_EGeKHeapAdd(__hp,__hcn) ({\
	ldbl_EGeKHeap_t*const __EGlhp = (__hp);\
	ldbl_EGeKHeapCn_t*const __EGlcn = (__hcn);\
	ldbl_EGeKHeapCHECK_NF(__EGlhp);\
	__EGlcn->pos = __EGlhp->sz, \
	__EGlhp->cn[__EGlhp->sz] = __EGlcn, \
	__EGlhp->sz +=1, \
	ldbl_EGeKHeapSiftUp(__EGlhp,__EGlcn), 0;})

/* ========================================================================= */
/** @brief Give the first child for a given position.
 * @param __id position that we want to get the first child.
 * @param __d breath of the heap. */
#define ldbl_EGeKHeapFirstChildId(__d,__id) ((__d)*(__id)+1)

/* ========================================================================= */
/** @brief Move an element down in the heap (position 0 is the
 * top), this kind of operation is needed whenever we increase the value in a
 * heap element.
 * @param __hp heap where we are working.
 * @param __hcn element in the heap to move.
 * @return zero on success, non-zero otherwise.
 * */
#define ldbl_EGeKHeapSiftDown(__hp,__hcn) ({\
	ldbl_EGeKHeap_t*const __EGehp = (__hp);\
	ldbl_EGeKHeapCn_t*const __EGecn = (__hcn);\
	const unsigned int __EGhsz = __EGehp->sz;\
	unsigned int __EGcpos = __EGecn->pos;\
	unsigned int __EGfchd = ldbl_EGeKHeapFirstChildId(__EGehp->d,__EGcpos);\
	unsigned int __EGlchd = __EGfchd + __EGehp->d;\
	ldbl_EGeKHeapCn_t*__EGcchd = 0;\
	register unsigned int __EGehi = 0;\
	ldbl_EGeKHeapCHECK_CN(__EGehp,__EGecn);\
	while(__EGfchd < __EGhsz)\
	{\
		/* detect the minimum child */\
		__EGcchd = __EGehp->cn[__EGfchd];\
		for(__EGehi = __EGlchd > __EGhsz ? __EGhsz-1 : __EGlchd-1 ;\
			__EGehi > __EGfchd ; __EGehi--)\
			if(ldbl_EGeKHeapIsLess(__EGehp->cn[__EGehi]->val,__EGcchd->val))\
				__EGcchd = __EGehp->cn[__EGehi];\
		/* if the minimum child is less than the current position, move the minimum\
		 * child to the position of the current element */\
		if(ldbl_EGeKHeapIsLess(__EGcchd->val,__EGecn->val))\
		{\
			__EGfchd = __EGcchd->pos;\
			__EGcchd->pos = __EGcpos;\
			__EGehp->cn[__EGcpos] = __EGcchd;\
			__EGecn->pos = __EGcpos = __EGfchd;\
			__EGehp->cn[__EGcpos] = __EGecn;\
			__EGfchd = ldbl_EGeKHeapFirstChildId(__EGehp->d,__EGcpos);\
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
 * @param __new_val new value for the element (note this is an array of size at
 * least ldbl_EG_EKHEAP_ENTRY.
 * @return zero on success, non-zero otherwise.
 * */
#define ldbl_EGeKHeapChangeVal(__hp,__hcn,__new_val) ({\
	ldbl_EGeKHeapCn_t*const __EGEKHcn = (__hcn);\
	(ldbl_EGeKHeapIsLess(__new_val,(__EGEKHcn)->val)) ? (ldbl_EGeKHeapCopyVal((__EGEKHcn)->val,__new_val),ldbl_EGeKHeapSiftUp(__hp,__EGEKHcn)) : (ldbl_EGeKHeapCopyVal((__EGEKHcn)->val,__new_val),ldbl_EGeKHeapSiftDown(__hp,__EGEKHcn));})

/* ========================================================================= */
/** @brief Eliminate an element from the heap, note that the position stored in
 * the eliminated element is reset to zero.
 * @param __hp heap where we are working.
 * @param __hcn element to eliminate from the heap.
 * @return zero on success, non-zero otherwise.
 * */
#define ldbl_EGeKHeapDel(__hp,__hcn) ({\
	ldbl_EGeKHeap_t*const __EGlhp = (__hp);\
	ldbl_EGeKHeapCn_t*const __EGlhpcn = (__hcn);\
	unsigned int const __EGlcn = __EGlhpcn->pos;\
	unsigned int const __EGlhsz = __EGlhp->sz - 1;\
	__EGlhpcn->pos = ldbl_EG_EKHEAP_POISON;\
	__EGlhp->sz = __EGlhsz;\
	if(__EGlhsz && __EGlhsz != __EGlcn){\
		__EGlhp->cn[__EGlcn] = __EGlhp->cn[__EGlhp->sz];\
		__EGlhp->cn[__EGlcn]->pos = __EGlcn;\
		ldbl_EGeKHeapSiftDown(__EGlhp,__EGlhp->cn[__EGlcn]);}\
	__EGlhp->cn[__EGlhp->sz] = 0;})

/* ========================================================================= */
/** @brief Check the integrity of the given heap.
 * @param __hp heap to check.
 * @return zero on success, non-zero otherwise.
 * */
#if ldbl_EG_EKHEAP_DEBUG <= DEBUG
#define ldbl_EGeKHeapCheck(__hp) ({\
	ldbl_EGeKHeap_t*const __EGehp = (__hp);\
	register unsigned int __EGehi = __EGehp->sz;\
	if(__EGehi)\
		while(--__EGehi)\
			if(__EGehp->cn[__EGehi]->pos != __EGehi || \
				ldbl_EGeKHeapIsLess( __EGehp->cn[__EGehi]->val,\
												__EGehp->cn[ldbl_EGeKHeapFatherId(__EGehp->d,__EGehi)]->val))\
			{\
				MESSAGE(ldbl_EG_EKHEAP_DEBUG,"Element %u is wrong, pos %u val [%lf,%lf]"\
							 ,__EGehi, __EGehp->cn[__EGehi]->pos, \
							 ldbl_EGlpNumToLf(__EGehp->cn[__EGehi]->val[0]), \
							 ldbl_EGlpNumToLf(__EGehp->cn[ldbl_EGeKHeapFatherId(__EGehp->d,__EGehi)]->val[0]));\
				break;\
			}\
	__EGehi;})
#else
#define ldbl_EGeKHeapCheck(__hp) 0
#endif

/* ========================================================================= */
/** @brief set the breath of the heap, this function must be called only when
 * the heap is empty.
 * @param __hp heap to set breath.
 * @param __width new with for the heap.
 * @return zero on success, non-zero otherwise.
 * */
#define ldbl_EGeKHeapChangeD(__hp,__width) ({\
	ldbl_EGeKHeap_t*const __EGehp = (__hp);\
	EXIT((__width)<2,"Width should be at least 2 for heaps");\
	__EGehp->sz ? 1 : (__EGehp->d = (__width), 0);})

/* ========================================================================= */
/** @} */
/* end of ldbl_eg_ekheap.h */
#endif
#endif
#endif
