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
/** @defgroup EGaBKheap EGaBKheap
 *
 * Here we define the basic interface for binary heaps as an array-based 
 * structure.
 * In this implementation the heap does not grow on the fly, meaning that it 
 * may fill-up during an add call, to avoid that, the user must 
 * re-allocate when necesary. the heap start as a heap of size zero. 
 * This implementatioon is a minimum-heap implementatiton.
 *
 * @version 0.0.1
 * @par History:
 * - 2013-04-12
 * 						- First implementation, starting from base ldbl_EGeHeap_t 
 * @note 
 * This implementatiton is designed as a template using as base the types of
 * @ref EGlpNum
 * */
/** @file 
 * @ingroup EGaBKheap */
/** @addtogroup EGaBKheap */
/** @{ */
/** @example ldbl_eg_abkheap.ex.c
 * This is a simple example of the usage of heaps using @ref EGaBKheap */
/* ========================================================================= */
#ifndef ldbl___EG_ABKHEAP__
#define ldbl___EG_ABKHEAP__
#include "eg_config.h"
#include "eg_macros.h"
#include "eg_lpnum.h"
/* ========================================================================= */
/** @brief Debug level for the heap */
#ifndef ldbl_EG_ABKHEAP_DEBUG
#define ldbl_EG_ABKHEAP_DEBUG 1000
#endif

/* ========================================================================= */
/** @name Test macros, enabled only if debug level is high enough. */
/** @{ */
#if ldbl_EG_ABKHEAP_DEBUG <= DEBUG
#define ldbl_EGaBKheapCHECK_CN(__hp2,__hcn2) EXIT(__hcn2->pos>=__hp2->sz,"Heap Connector possition %d out of range [0,%d]",__hcn2->pos,__hp2->sz)
#define ldbl_EGaBKheapCHECK_NF(__hp2) EXIT(__hp2->sz >= __hp2->max_sz,"Heap "#__hp2" Is full, can't add an element")
#else
#define ldbl_EGaBKheapCHECK_CN(__hp,__hcn) 
#define ldbl_EGaBKheapCHECK_NF(__hp) 
#endif
/** @} */
/* ========================================================================= */
/** @brief number of maximum entries in the vector values */
#ifndef ldbl_EG_ABKHEAP_ENTRY
#define ldbl_EG_ABKHEAP_ENTRY 3
#endif
/* ========================================================================= */
/** @brief Structure to store the information relevant to an element in the
 * heap. */
typedef struct 
{
	long double val[ldbl_EG_ABKHEAP_ENTRY];		/**< Value of this node in the heap */
	uint32_t pos;					/**< Position in the heap-array for this node, if set to
												 #ldbl_EG_ABKHEAP_POISON, then the connector is not in any 
												 heap.*/
}
ldbl_EGaBKheapCn_t;
/* ========================================================================= */
/** @brief Poison position for heap connector not in a heap. */
#define ldbl_EG_ABKHEAP_POISON UINT32_MAX
/* ========================================================================= */
/** @brief given two heap connectors, return one if the first is less than the
 * second (in  lexicographic order).
 * @param __hcn1 first vector array.
 * @param __hcn2 second vector array.
 * @return one if __hcn1 <_LEX __hcn2 */
#define ldbl_EGaBKheapIsLess(__hcn1,__hcn2) ({\
	const long double*const __EGABKH1 = (__hcn1);\
	const long double*const __EGABKH2 = (__hcn2);\
	int __EGABKHj = 0, __EGABKHrval = 0;\
	for( ; __EGABKHj < ldbl_EG_ABKHEAP_ENTRY ; __EGABKHj++)\
	{\
		if(ldbl_EGlpNumIsLess(__EGABKH1[__EGABKHj], __EGABKH2[__EGABKHj])){\
			__EGABKHrval = 1; break;}\
		else if (ldbl_EGlpNumIsNeq(__EGABKH1[__EGABKHj], __EGABKH2[__EGABKHj],ldbl_epsLpNum)){\
			__EGABKHrval = 0; break;}\
	}\
	__EGABKHrval;})
/* ========================================================================= */
/** @brief Initialize a heap conector structure. This function will allocate any
 * interal memory not allocated by the user, it should be called only once, or
 * after a clear function call.
 * @param __hcn conector to initialize.
 * */
#define ldbl_EGaBKheapCnInit(__hcn) do{\
	ldbl_EGaBKheapCn_t*const __ABKHcn=(__hcn);\
	int __ABKHi = ldbl_EG_ABKHEAP_ENTRY;\
	memset(__ABKHcn,0,sizeof(ldbl_EGaBKheapCn_t));\
	for( ; __ABKHi-- ; ){ldbl_EGlpNumInitVar(__ABKHcn->val[__ABKHi]);}\
	__ABKHcn->pos = ldbl_EG_ABKHEAP_POISON;}while(0)
/* ========================================================================= */
/** @brief Reset a heap conector to the same state as after an init call, this
 * function is provided only for completness.
 * @param __hcn conector to reset
 * */
#define ldbl_EGaBKheapCnReset(__hcn) ((__hcn)->pos = ldbl_EG_ABKHEAP_POISON)
/* ========================================================================= */
/** @brief Free all internal memory used by this structured not allocated by the
 * user. This function should be called after an init call, and only once.
 * @param __hcn conector to clear.
 * */
#define ldbl_EGaBKheapCnClear(__hcn) do{\
	ldbl_EGaBKheapCn_t*const __ABKHcn = (__hcn);\
	int __ABKHi = ldbl_EG_ABKHEAP_ENTRY;\
	for( ; __ABKHi-- ; ){ldbl_EGlpNumClearVar(__ABKHcn->val[__ABKHi]);}}while(0)
/* ========================================================================= */
/** @brief Structure to hold a whole heap structure, this structure is designed
 * so that it can grow on the fly with a low cost */
typedef struct 
{
	uint32_t*cn;							/**<@brief position in the base array of the connector */
	uint32_t sz;
	uint32_t max_sz;
}
ldbl_EGaBKheap_t;
/* ========================================================================= */
/** @brief Return one if the heap is full, zero otherwise.
 * @param __hp heat to check */
#define ldbl_EGaBKheapIsFull(__hp) ({ldbl_EGaBKheap_t*const __EGehp = (__hp); __EGehp->sz == __EGehp->max_sz;})

/* ========================================================================= */
/** @brief set the number of elements in hte heap to zero.
 * @param __hp heap to empty.
 * */
#define ldbl_EGaBKheapEmpty(__hp) ((__hp)->sz = 0)

/* ========================================================================= */
/** @brief Initialize a heap as an empty heap (with no space for conectors).
 * @param __hp heap to initialize.
 * */
#define ldbl_EGaBKheapInit(__hp) (*(__hp) = (ldbl_EGaBKheap_t){0,0,0})

/* ========================================================================= */
/** @brief Reset the given heap as an empty heap (just as returned by the init
 * call.
 * @param __hp heap to reset 
 * */
#define ldbl_EGaBKheapReset(__hp) ldbl_EGaBKheapResize(__hp,0)

/* ========================================================================= */
/** @brief Clear a heap structure, and free any internal memory (not allocated
 * by the user).
 * @param __hp heap to clear.
 * */
#define ldbl_EGaBKheapClear(__hp) ldbl_EGaBKheapResize(__hp,0)

/* ========================================================================= */
/** @brief get the minimum conector in the heap, if the heap is empty, return
 * ldbl_EG_ABKHEAP_POISON.
 * @param __hp eap where we are working.
 * @return position (in the base array) of the minimum element in the heap.
 * */
#define ldbl_EGaBKheapGetMin(__hp) ({\
	ldbl_EGaBKheap_t*const __EGehp = (__hp);\
	__EGehp->sz ? __EGehp->cn[0] : ldbl_EG_ABKHEAP_POISON;})

/* ========================================================================= */
/** @brief resize the heap cn array to the given size, if the new size is zero,
 * it is equivalent to free the internal memory, and left the heap as an empty
 * heap with zero space.
 * @param __hp heap where we are working.
 * @param __new_sz new size for the  cn array .
 * */
#define ldbl_EGaBKheapResize(__hp,__new_sz) ({\
	ldbl_EGaBKheap_t*const __EGehp = (__hp);\
	const uint32_t __EGehp_nsz = (uint32_t)(__new_sz);\
	__EGehp->cn = EGrealloc((__EGehp->cn),((size_t)(__EGehp_nsz*sizeof(uint32_t))));\
	__EGehp->max_sz = __EGehp_nsz;})

/* ========================================================================= */
/** @brief return the index of the father of the given index.
 * @param __id position in the heap-array to wich we want to compute it's father.
 * */
#define ldbl_EGaBKheapFatherId(__id) ((uint32_t)((__id)?(((uint32_t)((__id)-1))>>1U):0))

/* ========================================================================= */
/** @brief move an element in the heap up in the heap (position 0 is the top,
 * this kind of move is neded whenever we decrease the value in a heap element).
 * @note this is an INTERNAL function; and MUST NOT be called directly by
 * users.
 * @param __pbase3 pointer to he base array of connectors.
 * @param __psz3 size of each structure holding the heap connector sub-structure
 * @param __hp3 heap where we are working.
 * @param __hcn3 element in the heap to move.
 * */
#define ldbl___EGaBKheapSiftUp(__pbase3,__psz3,__hp3,__hcn3) do{\
	char*const __EGbase3=((char*)(__pbase3));\
	const uint32_t __EGstsz3=((uint32_t)(__psz3));\
	ldbl_EGaBKheap_t*const __EGehp3 = (__hp3);\
	const uint32_t __EGapos3 = __EGehp3->cn[__hcn3];\
	ldbl_EGaBKheapCn_t*const __EGecn3 = ((ldbl_EGaBKheapCn_t*)(EGaGetElem(__EGbase3,__EGstsz3,__EGapos3)));\
	uint32_t __EGcpos3 = __EGecn3->pos;\
	uint32_t __EGfpos3 = ldbl_EGaBKheapFatherId(__EGcpos3);\
	ldbl_EGaBKheapCn_t*__EGfcn3 = ((ldbl_EGaBKheapCn_t*)(EGaGetElem(__EGbase3,__EGstsz3,__EGehp3->cn[__EGfpos3])));\
	ldbl_EGaBKheapCHECK_CN(__EGehp3,__EGecn3);\
	while(__EGcpos3 && \
				ldbl_EGaBKheapIsLess(__EGecn3->val,__EGfcn3->val))\
	{\
		__EGfcn3->pos = __EGcpos3;\
		__EGehp3->cn[__EGcpos3] = ((uint32_t)(EGaGetPos(__EGbase3,__EGstsz3,__EGfcn3)));\
		__EGcpos3 = __EGfpos3;\
		__EGfpos3 = ldbl_EGaBKheapFatherId(__EGcpos3);\
		__EGfcn3 = ((ldbl_EGaBKheapCn_t*)(EGaGetElem(__EGbase3,__EGstsz3,__EGehp3->cn[__EGfpos3])));\
	}\
	__EGecn3->pos = __EGcpos3;\
	__EGehp3->cn[__EGcpos3] = __EGapos3;\
	}while(0)
/* ========================================================================= */
/** @brief Add an element to the heap
 * @param __pbase pointer to he base array of connectors.
 * @param __psz size of each structure holding the heap connector sub-structure
 * @param __hp heap where to add the element.
 * @param __hcn element to be added.
 * */
#define ldbl_EGaBKheapAdd(__pbase,__psz,__hp,__hcn) do{\
	char*const __EGbase2=((char*)(__pbase));\
	const uint32_t __EGstsz2=((uint32_t)(__psz));\
	ldbl_EGaBKheap_t*const __EGlhp = (__hp);\
	const uint32_t __EGapos2=(uint32_t)(__hcn);\
	ldbl_EGaBKheapCn_t*const __EGlcn = ((ldbl_EGaBKheapCn_t*)(EGaGetElem(__EGbase2,__EGstsz2,__EGapos2)));\
	ldbl_EGaBKheapCHECK_NF(__EGlhp);\
	__EGlcn->pos=__EGlhp->sz,__EGlhp->cn[__EGlhp->sz]=__EGapos2;\
	__EGlhp->sz +=1;\
	ldbl___EGaBKheapSiftUp(__EGbase2,__EGstsz2,__EGlhp,__EGlhp->sz-1);}while(0)
/* ========================================================================= */
/** @brief Give the first child for a given position.
 * @param __id position that we want to get the first child.
 * */
#define ldbl_EGaBKheapFirstChildId(__id) ((uint32_t)(((uint32_t)(__id))<<1U)+1U)
/* ========================================================================= */
/** @brief Move an element down in the heap (position 0 is the
 * top), this kind of operation is needed whenever we increase the value in a
 * heap element.
 * @note this is an INTERNAL function; and MUST NOT be called directly by
 * users.
 * @param __pbase pointer to he base array of connectors.
 * @param __psz size of each structure holding the heap connector sub-structure
 * @param __hp heap where we are working.
 * @param __hcn element in the heap to move.
 * */
#define ldbl___EGaBKheapSiftDown(__pbase,__psz,__hp,__hcn) do{\
	char*const __EGbase=((char*)(__pbase));\
	const uint32_t __EGstsz=((uint32_t)(__psz));\
	ldbl_EGaBKheap_t*const __EGehp = (__hp);\
	const uint32_t __EGapos = __EGehp->cn[__hcn];\
	ldbl_EGaBKheapCn_t*const __EGecn = ((ldbl_EGaBKheapCn_t*)(EGaGetElem(__EGbase,__EGstsz,__EGapos)));\
	const uint32_t __EGhsz = __EGehp->sz;\
	uint32_t __EGcpos = __EGecn->pos;\
	uint32_t __EGfchd = ldbl_EGaBKheapFirstChildId(__EGcpos);\
	uint32_t __EGlchd = ((uint32_t)(__EGfchd + 2U));\
	ldbl_EGaBKheapCn_t*__EGcchd=0,*__EGnchd=0;\
	ldbl_EGaBKheapCHECK_CN(__EGehp,__EGecn);\
	while(__EGfchd < __EGhsz)\
	{\
		/* detect the minimum child */\
		__EGcchd = ((ldbl_EGaBKheapCn_t*)(EGaGetElem(__EGbase,__EGstsz,__EGehp->cn[__EGfchd])));\
		if(__EGlchd<=__EGhsz){\
			__EGnchd = ((ldbl_EGaBKheapCn_t*)(EGaGetElem(__EGbase,__EGstsz,__EGehp->cn[__EGfchd+1])));\
			if(ldbl_EGaBKheapIsLess(__EGnchd->val,__EGcchd->val)) __EGcchd=__EGnchd;}\
		/* if the minimum child is less than the current position, move the minimum\
		 * child to the position of the current element */\
		if(ldbl_EGaBKheapIsLess(__EGcchd->val,__EGecn->val))\
		{\
			__EGfchd = __EGcchd->pos;\
			__EGcchd->pos = __EGcpos, __EGehp->cn[__EGcpos] = ((uint32_t)(EGaGetPos(__EGbase,__EGstsz,__EGcchd)));\
			__EGecn->pos = __EGcpos = __EGfchd, __EGehp->cn[__EGfchd] = ((uint32_t)(EGaGetPos(__EGbase,__EGstsz,__EGecn)));\
			__EGfchd = ldbl_EGaBKheapFirstChildId(__EGcpos);\
			__EGlchd = ((uint32_t)(__EGfchd + 2U));\
		}\
		/* else we exit the main loop */\
		else __EGfchd = ldbl_EG_ABKHEAP_POISON;\
	}\
}while(0)
/* ========================================================================= */
/** @brief copy two vector of values (only  ldbl_EG_EKHEAP_ENTRY positions) from the
 * rource to the destination.
 * @param src ldbl_source array.
 * @param dst destination array.
 * */
#define ldbl_EGaBKheapCopyVal(__dst,__src) do{\
	long double*const __EGABKHdst=(__dst);\
	const long double*const __EGABKHsrc=(__src);\
	int __EGABKHi=ldbl_EG_ABKHEAP_ENTRY;\
	for(;__EGABKHi--;){ldbl_EGlpNumCopy(__EGABKHdst[__EGABKHi],__EGABKHsrc[__EGABKHi]);}}while(0)
/* ========================================================================= */
/** @brief Change the value of an element in the heap.
 * @param __pbase pointer to he base array of connectors.
 * @param __psz size of each structure holding the heap connector sub-structure
 * @param __hp heap where we are working.
 * @param __hcn element in the heap that we are going to change it's value.
 * @param __new_val new value for the element.
 * */
#define ldbl_EGaBKheapChangeVal(__pbase,__psz,__hp,__hcn,__new_val) do{\
	char*const __EGbase2=((char*)(__pbase));\
	const uint32_t __EGstsz2=((uint32_t)(__psz));\
	ldbl_EGaBKheap_t*const __EGlhp=(__hp);\
	const uint32_t __EGapos2=(uint32_t)(__hcn);\
	ldbl_EGaBKheapCn_t*__EGlcn2=((ldbl_EGaBKheapCn_t*)(EGaGetElem(__EGbase2,__EGstsz2,__EGapos2)));\
	if(ldbl_EGaBKheapIsLess(__new_val,__EGlcn2->val)){\
		ldbl_EGaBKheapCopyVal(__EGlcn2->val,__new_val);\
		ldbl___EGaBKheapSiftUp(__EGbase2,__EGstsz2,__EGlhp,__EGlcn2->pos);}\
	else{\
		ldbl_EGaBKheapCopyVal(__EGlcn2->val,__new_val);\
		ldbl___EGaBKheapSiftDown(__EGbase2,__EGstsz2,__EGlhp,__EGlcn2->pos);}}while(0)

/* ========================================================================= */
/** @brief Eliminate an element from the heap, note that the position stored in
 * the eliminated element is reset to zero.
 * @param __pbase pointer to he base array of connectors.
 * @param __psz size of each structure holding the heap connector sub-structure
 * @param __hp heap where we are working.
 * @param __hcn element to eliminate from the heap.
 * */
#define ldbl_EGaBKheapDel(__pbase,__psz,__hp,__hcn) do{\
	char*const __EGbase2=((char*)(__pbase));\
	const uint32_t __EGstsz2=((uint32_t)(__psz));\
	ldbl_EGaBKheap_t*const __EGlhp = (__hp);\
	const uint32_t __EGapos2=(uint32_t)(__hcn);\
	ldbl_EGaBKheapCn_t*const __EGlhpcn = ((ldbl_EGaBKheapCn_t*)(EGaGetElem(__EGbase2,__EGstsz2,__EGapos2)));\
	const uint32_t __EGlcn = __EGlhpcn->pos;\
	const uint32_t __EGlhsz = __EGlhp->sz - 1;\
	ldbl_EGaBKheapCn_t*const __EGllcn = ((ldbl_EGaBKheapCn_t*)(EGaGetElem(__EGbase2,__EGstsz2,__EGlhp->cn[__EGlhsz])));\
	__EGlhpcn->pos = ldbl_EG_ABKHEAP_POISON, __EGlhp->sz = __EGlhsz;\
	if(__EGlhsz && __EGlhsz != __EGlcn){\
		__EGlhp->cn[__EGlcn] = __EGlhp->cn[__EGlhsz],__EGllcn->pos = __EGlcn;\
		ldbl___EGaBKheapSiftDown(__EGbase2,__EGstsz2,__EGlhp,__EGlcn);}\
	__EGlhp->cn[__EGlhp->sz] = ldbl_EG_ABKHEAP_POISON;}while(0)

/* ========================================================================= */
/** @} */
/* end of ldbl_eg_abkheap.h */
#endif
#endif
#endif
