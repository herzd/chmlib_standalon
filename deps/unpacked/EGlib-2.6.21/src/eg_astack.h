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
/** @defgroup EGaStack EGaStack
 *
 * Basic stack structure based on arrays; no attach data supported.
 * 
 * @version 0.0.1
 * @par History:
 * - 2012-10-04
 * 						- First Implementation.
 *
 * */
/** @file 
 * @ingroup EGaStack */
/** @addtogroup EGaStack */
/** @{ */
/** @example eg_astack.ex.c 
 * This is a working (althought useless) example on @ref EGaStack.
 * */
/* ========================================================================= */
#ifndef __EG_ASTACK_H__
#define __EG_ASTACK_H__
#include "eg_config.h"
#include "eg_macros.h"
#include "eg_mem.h"

/* ========================================================================= */
/** @brief debug level for lists */
#define __ASTACK_DEBUG__ 100
/* ========================================================================= */
/** @brief Null-initialized embeded list */
#define EGaStackNull ((EGaStack_t){0,-1,0})
/* ========================================================================= */
/** @brief array-based stack structure */
typedef struct 
{
	int*stack;	/**< @brief array of length @ref EGaStack_t::sz storing 
										indices in stack */
	int n;			/**< @brief top of the stack, -1 is an empty stack */
	int sz;			/**< @brief actual size of the stack array */
}
EGaStack_t;
/* ========================================================================= */
/** @brief Initialize te stack as an empty stack.
 * @param __stpt pointer to the stack to initialize. 
 * @return the pointer to the stack. */
#define EGaStackInit(__stpt) do{\
	EGaStack_t*const __EGast2 = (__stpt);\
	__EGast2->stack=0,__EGast2->n=-1,__EGast2->sz=0;}while(0)
/* ========================================================================= */
/** @brief if the stack is full, increase the size of the stack 
 * @param __stpt pointer to the stack structure
 * */
#define __EGaStackResize(__stpt) do{\
	EGaStack_t*const __EGast2 = (__stpt);\
	int const __EGnsz = 2*(__EGast2->sz+100);\
	if(__EGast2->n+1>=__EGast2->sz){\
		__EGast2->stack=EGrealloc(__EGast2->stack,sizeof(int)*__EGnsz);\
		__EGast2->sz=__EGnsz;}}while(0)
/* ========================================================================= */
/** @brief free any internal memory allocated by the stack and leave it as an
 * initialized stack.
 * @param __stpt pointer to the stack structure
 * */
#define EGaStackClear(__stpt) do{\
	EGaStack_t*const __EGast = (__stpt);\
	EGfree(__EGast->stack);\
	EGaStackInit(__stpt);}while(0)
/* ========================================================================= */
/** @brief push a new element into the stack 
 * @param __stpt pointer to the stack structure
 * @param __id index to add to the stack.
 * @return zero on success, non-zero otherwise.
 * */
#define EGaStackPush(__stpt,__id) ({\
	EGaStack_t*const __EGast = (__stpt);\
	__EGaStackResize(__EGast);\
	__EGast->stack[++(__EGast->n)]=(__id);0;})
/* ========================================================================= */
/** @brief test if the stack is empty
 * @param __stpt pointer to the stack structure
 * @return one if stack is empty, zero otherwise */
#define EGaStackIsEmpty(__stpt) ((((const EGaStack_t*const)(__stpt))->n)<0?1:0)
/* ========================================================================= */
/** @brief peek the element in the top of the heap, if the heap is empty,
 * return error.
 * @param __stpt pointer to the stack structure
 * @param __id pointer to where store the index in the top of the heap.
 * @return zero on success, non-zero otherwise.
 * */
#define EGaStackPeek(__stpt,__id) ({\
	EGaStack_t*const __EGast = (__stpt);\
	int const __EGrv = __EGast->n<0?1:0;\
	if(!__EGrv) (__id)=__EGast->stack[__EGast->n];\
	__EGrv;})
/* ========================================================================= */
/** @brief pop the element in the top of the heap, if the heap is empty,
 * return error.
 * @param __stpt pointer to the stack structure
 * @param __id pointer to where store the index in the top of the heap.
 * @return zero on success, non-zero otherwise.
 * */
#define EGaStackPop(__stpt,__id) ({\
	EGaStack_t*const __EGast = (__stpt);\
	int const __EGrv = __EGast->n<0?1:0;\
	if(!__EGrv) (*(__id))=__EGast->stack[(__EGast->n)--];\
	__EGrv;})
/* ========================================================================= */
/** @}*/
/* end of eg_astack.h */
#endif
