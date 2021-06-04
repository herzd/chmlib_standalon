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
/** @defgroup EGaHeap EGaHeap
 *
 * Here we define the basic interface for d-heaps as an array-based structure.
 * In this implementation the heap does not grow on the fly, meaning that it 
 * may fill-up during an add call, to avoid that, the user must 
 * re-allocate when necesary. the heap start as a heap of size zero. 
 * This implementatioon is a minimum-heap implementatiton.
 *
 * @version 0.0.1
 * @par History:
 * - 2013-04-04
 * 						- First implementation, starting from base fp20_EGeHeap_t 
 * @note 
 * This implementatiton is designed as a template using as base the types of
 * @ref EGlpNum
 * */
/** @file 
 * @ingroup EGaHeap */
/** @addtogroup EGaHeap */
/** @{ */
/** @example fp20_eg_aheap.ex.c
 * This is a simple example of the usage of heaps using @ref EGaHeap */
/* ========================================================================= */
#ifndef fp20___EG_AHEAP__
#define fp20___EG_AHEAP__
#include "eg_config.h"
#include "eg_macros.h"
#include "eg_lpnum.h"
/* ========================================================================= */
/** @brief Debug level for the heap */
#ifndef fp20_EG_AHEAP_DEBUG
#define fp20_EG_AHEAP_DEBUG 1000
#endif

/* ========================================================================= */
/** @name Test macros, enabled only if debug level is high enough. */
/** @{ */
#if fp20_EG_AHEAP_DEBUG <= DEBUG
#define fp20_EGaHeapCHECK_CN(__hp2,__hcn2) EXIT(__hcn2->pos>=__hp2->sz,"Heap Connector possition %d out of range [0,%d]",__hcn2->pos,__hp2->sz)
#define fp20_EGaHeapCHECK_NF(__hp2) EXIT(__hp2->sz >= __hp2->max_sz,"Heap "#__hp2" Is full, can't add an element")
#else
#define fp20_EGaHeapCHECK_CN(__hp,__hcn) 
#define fp20_EGaHeapCHECK_NF(__hp) 
#endif
/** @} */
/* ========================================================================= */
/** @brief Structure to store the information relevant to an element in the
 * heap. */
typedef struct 
{
	EGfp20_t val;		/**< Value of this node in the heap */
	unsigned pos;					/**< Position in the heap-array for this node, if set to
												 #fp20_EG_AHEAP_POISON, then the connector is not in any 
												 heap.*/
}
fp20_EGaHeapCn_t;

/* ========================================================================= */
/** @brief Poison position for heap connector not in a heap. */
#define fp20_EG_AHEAP_POISON UINT_MAX

/* ========================================================================= */
/** @brief Initialize a heap conector structure. This function will allocate any
 * interal memory not allocated by the user, it should be called only once, or
 * after a clear function call.
 * @param __hcn conector to initialize.
 * */
#define fp20_EGaHeapCnInit(__hcn) do{fp20_EGlpNumInitVar((__hcn)->val);(__hcn)->pos = fp20_EG_AHEAP_POISON;}while(0)

/* ========================================================================= */
/** @brief Reset a heap conector to the same state as after an init call, this
 * function is provided only for completness.
 * @param __hcn conector to reset
 * */
#define fp20_EGaHeapCnReset(__hcn) ((__hcn)->pos = fp20_EG_AHEAP_POISON)

/* ========================================================================= */
/** @brief Free all internal memory used by this structured not allocated by the
 * user. This function should be called after an init call, and only once.
 * @param __hcn conector to clear.
 * */
#define fp20_EGaHeapCnClear(__hcn) fp20_EGlpNumClearVar((__hcn)->val)

/* ========================================================================= */
/** @brief Structure to hold a whole heap structure, this structure is designed
 * so that it can grow on the fly with a low cost */
typedef struct 
{
	unsigned*cn;							/**<@brief position in the base array of the connector */
	unsigned d;
	unsigned sz;
	unsigned max_sz;
}
fp20_EGaHeap_t;

/* ========================================================================= */
/** @brief Return one if the heap is full, zero otherwise.
 * @param __hp heat to check */
#define fp20_EGaHeapIsFull(__hp) ({fp20_EGaHeap_t*const __EGehp = (__hp); __EGehp->sz == __EGehp->max_sz;})

/* ========================================================================= */
/** @brief set the number of elements in hte heap to zero.
 * @param __hp heap to empty.
 * */
#define fp20_EGaHeapEmpty(__hp) ((__hp)->sz = 0)

/* ========================================================================= */
/** @brief Initialize a heap as an empty heap (with no space for conectors).
 * @param __hp heap to initialize.
 * @param __d width of the heap.
 * */
#define fp20_EGaHeapInit(__hp,__d) (*(__hp) = (fp20_EGaHeap_t){0,__d,0,0})

/* ========================================================================= */
/** @brief Reset the given heap as an empty heap (just as returned by the init
 * call.
 * @param __hp heap to reset 
 * */
#define fp20_EGaHeapReset(__hp) fp20_EGaHeapResize(__hp,0)

/* ========================================================================= */
/** @brief Clear a heap structure, and free any internal memory (not allocated
 * by the user).
 * @param __hp heap to clear.
 * */
#define fp20_EGaHeapClear(__hp) fp20_EGaHeapResize(__hp,0)

/* ========================================================================= */
/** @brief get the minimum value in the heap.
 * @param __hp heap where we are working.
 * @param __number where to store the result
 * @return zero on success, non-zero otherwise.
 * */
#define fp20_EGaHeapGetMinVal(__hp,__number) ({\
	fp20_EGaHeap_t*const __EGehp = (__hp);\
	__EGehp->sz ? (fp20_EGlpNumCopy(__number,__EGehp->cn[0]->val),0):1;})

/* ========================================================================= */
/** @brief get the minimum conector in the heap, if the heap is empty, return
 * fp20_EG_AHEAP_POISON.
 * @param __hp eap where we are working.
 * @return position (in the base array) of the minimum element in the heap.
 * */
#define fp20_EGaHeapGetMin(__hp) ({\
	fp20_EGaHeap_t*const __EGehp = (__hp);\
	__EGehp->sz ? __EGehp->cn[0] : fp20_EG_AHEAP_POISON;})

/* ========================================================================= */
/** @brief resize the heap cn array to the given size, if the new size is zero,
 * it is equivalent to free the internal memory, and left the heap as an empty
 * heap with zero space.
 * @param __hp heap where we are working.
 * @param __new_sz new size for the  cn array .
 * */
#define fp20_EGaHeapResize(__hp,__new_sz) ({\
	fp20_EGaHeap_t*const __EGehp = (__hp);\
	const unsigned __EGehp_nsz = (unsigned)(__new_sz);\
	__EGehp->cn = EGrealloc((__EGehp->cn),((size_t)(__EGehp_nsz*sizeof(unsigned))));\
	__EGehp->max_sz = __EGehp_nsz;})

/* ========================================================================= */
/** @brief return the index of the father of the given index.
 * @param __d breadth of the heap.
 * @param __id position in the heap-array to wich we want to compute it's father.
 * */
#define fp20_EGaHeapFatherId(__d,__id) ((unsigned)((__id)?(((__id)-1)/(__d)):0))

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
#define fp20___EGaHeapSiftUp(__pbase3,__psz3,__hp3,__hcn3) do{\
	char*const __EGbase3=((char*)(__pbase3));\
	const size_t __EGstsz3=((size_t)(__psz3));\
	fp20_EGaHeap_t*const __EGehp3 = (__hp3);\
	const unsigned __EGapos3 = __EGehp3->cn[__hcn3];\
	fp20_EGaHeapCn_t*const __EGecn3 = ((fp20_EGaHeapCn_t*)(EGaGetElem(__EGbase3,__EGstsz3,__EGapos3)));\
	unsigned __EGcpos3 = __EGecn3->pos;\
	unsigned __EGfpos3 = fp20_EGaHeapFatherId(__EGehp3->d,__EGcpos3);\
	fp20_EGaHeapCn_t*__EGfcn3 = ((fp20_EGaHeapCn_t*)(EGaGetElem(__EGbase3,__EGstsz3,__EGehp3->cn[__EGfpos3])));\
	fp20_EGaHeapCHECK_CN(__EGehp3,__EGecn3);\
	while(__EGcpos3 && \
				fp20_EGlpNumIsLess(__EGecn3->val,__EGfcn3->val))\
	{\
		__EGfcn3->pos = __EGcpos3;\
		__EGehp3->cn[__EGcpos3] = ((unsigned)(EGaGetPos(__EGbase3,__EGstsz3,__EGfcn3)));\
		__EGcpos3 = __EGfpos3;\
		__EGfpos3 = fp20_EGaHeapFatherId(__EGehp3->d,__EGcpos3);\
		__EGfcn3 = ((fp20_EGaHeapCn_t*)(EGaGetElem(__EGbase3,__EGstsz3,__EGehp3->cn[__EGfpos3])));\
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
#define fp20_EGaHeapAdd(__pbase,__psz,__hp,__hcn) do{\
	char*const __EGbase2=((char*)(__pbase));\
	const size_t __EGstsz2=((size_t)(__psz));\
	fp20_EGaHeap_t*const __EGlhp = (__hp);\
	const unsigned __EGapos2=(unsigned)(__hcn);\
	fp20_EGaHeapCn_t*const __EGlcn = ((fp20_EGaHeapCn_t*)(EGaGetElem(__EGbase2,__EGstsz2,__EGapos2)));\
	fp20_EGaHeapCHECK_NF(__EGlhp);\
	__EGlcn->pos=__EGlhp->sz,__EGlhp->cn[__EGlhp->sz]=__EGapos2;\
	__EGlhp->sz +=1;\
	fp20___EGaHeapSiftUp(__EGbase2,__EGstsz2,__EGlhp,__EGlhp->sz-1);}while(0)
/* ========================================================================= */
/** @brief Give the first child for a given position.
 * @param __id position that we want to get the first child.
 * @param __d breath of the heap. */
#define fp20_EGaHeapFirstChildId(__d,__id) ((unsigned)((__d)*(__id)+1U))
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
#define fp20___EGaHeapSiftDown(__pbase,__psz,__hp,__hcn) do{\
	char*const __EGbase=((char*)(__pbase));\
	const size_t __EGstsz=((size_t)(__psz));\
	fp20_EGaHeap_t*const __EGehp = (__hp);\
	const unsigned __EGapos = __EGehp->cn[__hcn];\
	fp20_EGaHeapCn_t*const __EGecn = ((fp20_EGaHeapCn_t*)(EGaGetElem(__EGbase,__EGstsz,__EGapos)));\
	const unsigned __EGhsz = __EGehp->sz;\
	unsigned __EGcpos = __EGecn->pos;\
	unsigned __EGfchd = fp20_EGaHeapFirstChildId(__EGehp->d,__EGcpos);\
	unsigned __EGlchd = __EGfchd + __EGehp->d;\
	fp20_EGaHeapCn_t*__EGcchd=0,*__EGnchd=0;\
	register unsigned __EGehi = 0;\
	fp20_EGaHeapCHECK_CN(__EGehp,__EGecn);\
	while(__EGfchd < __EGhsz)\
	{\
		/* detect the minimum child */\
		__EGcchd = ((fp20_EGaHeapCn_t*)(EGaGetElem(__EGbase,__EGstsz,__EGehp->cn[__EGfchd])));\
		for(__EGehi = (unsigned)((__EGlchd > __EGhsz) ? __EGhsz-1U:__EGlchd-1U); __EGehi > __EGfchd ; __EGehi--){\
			if(fp20_EGlpNumIsLeq((__EGnchd=((fp20_EGaHeapCn_t*)(EGaGetElem(__EGbase,__EGstsz,__EGehp->cn[__EGehi]))))->val,__EGcchd->val))\
				__EGcchd = __EGnchd;}\
		/* if the minimum child is less than the current position, move the minimum\
		 * child to the position of the current element */\
		if(fp20_EGlpNumIsLess(__EGcchd->val,__EGecn->val))\
		{\
			__EGfchd = __EGcchd->pos;\
			__EGcchd->pos = __EGcpos, __EGehp->cn[__EGcpos] = ((unsigned)(EGaGetPos(__EGbase,__EGstsz,__EGcchd)));\
			__EGecn->pos = __EGcpos = __EGfchd, __EGehp->cn[__EGfchd] = ((unsigned)(EGaGetPos(__EGbase,__EGstsz,__EGecn)));\
			__EGfchd = fp20_EGaHeapFirstChildId(__EGehp->d,__EGcpos);\
			__EGlchd = __EGfchd + __EGehp->d;\
		}\
		/* else we exit the main loop */\
		else __EGfchd = fp20_EG_AHEAP_POISON;\
	}\
}while(0)

/* ========================================================================= */
/** @brief Change the value of an element in the heap.
 * @param __pbase pointer to he base array of connectors.
 * @param __psz size of each structure holding the heap connector sub-structure
 * @param __hp heap where we are working.
 * @param __hcn element in the heap that we are going to change it's value.
 * @param __new_val new value for the element.
 * */
#define fp20_EGaHeapChangeVal(__pbase,__psz,__hp,__hcn,__new_val) do{\
	char*const __EGbase2=((char*)(__pbase));\
	const size_t __EGstsz2=((size_t)(__psz));\
	fp20_EGaHeap_t*const __EGlhp=(__hp);\
	const unsigned __EGapos2=(unsigned)(__hcn);\
	fp20_EGaHeapCn_t*__EGlcn2=((fp20_EGaHeapCn_t*)(EGaGetElem(__EGbase2,__EGstsz2,__EGapos2)));\
	EGfp20_t __EGnum;\
	fp20_EGlpNumCopy(__EGnum,(__new_val));\
	if(fp20_EGlpNumIsLess(__EGnum,__EGlcn2->val)){\
		fp20_EGlpNumCopy(__EGlcn2->val,__EGnum);\
		fp20___EGaHeapSiftUp(__EGbase2,__EGstsz2,__EGlhp,__EGlcn2->pos);}\
	else{\
		fp20_EGlpNumCopy(__EGlcn2->val,__EGnum);\
		fp20___EGaHeapSiftDown(__EGbase2,__EGstsz2,__EGlhp,__EGlcn2->pos);}}while(0)

/* ========================================================================= */
/** @brief Eliminate an element from the heap, note that the position stored in
 * the eliminated element is reset to zero.
 * @param __pbase pointer to he base array of connectors.
 * @param __psz size of each structure holding the heap connector sub-structure
 * @param __hp heap where we are working.
 * @param __hcn element to eliminate from the heap.
 * */
#define fp20_EGaHeapDel(__pbase,__psz,__hp,__hcn) do{\
	char*const __EGbase2=((char*)(__pbase));\
	const size_t __EGstsz2=((size_t)(__psz));\
	fp20_EGaHeap_t*const __EGlhp = (__hp);\
	const unsigned __EGapos2=(unsigned)(__hcn);\
	fp20_EGaHeapCn_t*const __EGlhpcn = ((fp20_EGaHeapCn_t*)(EGaGetElem(__EGbase2,__EGstsz2,__EGapos2)));\
	const unsigned __EGlcn = __EGlhpcn->pos;\
	const unsigned __EGlhsz = __EGlhp->sz - 1;\
	fp20_EGaHeapCn_t*const __EGllcn = ((fp20_EGaHeapCn_t*)(EGaGetElem(__EGbase2,__EGstsz2,__EGlhp->cn[__EGlhsz])));\
	__EGlhpcn->pos = fp20_EG_AHEAP_POISON, __EGlhp->sz = __EGlhsz;\
	if(__EGlhsz && __EGlhsz != __EGlcn){\
		__EGlhp->cn[__EGlcn] = __EGlhp->cn[__EGlhsz],__EGllcn->pos = __EGlcn;\
		fp20___EGaHeapSiftDown(__EGbase2,__EGstsz2,__EGlhp,__EGlcn);}\
	__EGlhp->cn[__EGlhp->sz] = fp20_EG_AHEAP_POISON;}while(0)

/* ========================================================================= */
/** @} */
/* end of fp20_eg_aheap.h */
#endif
