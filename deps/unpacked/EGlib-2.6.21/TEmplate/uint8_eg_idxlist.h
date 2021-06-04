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
/** @defgroup EGidxLst EGidxLst
 *
 * Here we define the basic interface for an array-based general list, but with
 * different width for the integer types; we assume that the structure is
 * within a mayor structure; and all access are from a base memory address.
 * This implementation is based on the philosophy of array-embeded structures.
 *
 * 
 * @version 0.9.0
 * @par History:
 * - 2013-05-27
 * 						- First version (adapted from eg_alist.h)
 *
 * @note Throughou the code, i is assumed that __abase[0] is of type uint8_EGidxLst_t;
 * this is not too bad, since we could use list in different positions wihin a
 * structure but using a diferent __abase pointer.
 *
 * @note In general, the functions described bellow don't perform consistency
 * checks. It is asumed that the user does know what is he doing.
 *
 * @note If you want to have some debugging control try changing the debug level
 * at compile time, and lowering the debug level asociated to the list function
 * as defined in eg_configure.h.
 *
 * */
/** @file 
 * @ingroup EGidxLst */
/** @addtogroup EGidxLst */
/** @{ */
/** @example uint8_eg_idxlist.ex.c 
 * This is a working (althought useless) example on @ref EGidxLst.
 * */
/* ========================================================================= */
#ifndef uint8___EG_IDXLIST_H__
#define uint8___EG_IDXLIST_H__
#include "eg_config.h"
#include "eg_macros.h"
#include "eg_idx.h"

/* ========================================================================= */
/** @brief debug level for lists */
#define uint8_EG_IDXLIST_DEBUG 100
/* ========================================================================= */
/** @brief List Node Structure.
 * @par Description:
 * This structure is to store a general node of the list. It is composed by
 * two members, that point to the next and previous structures in the list.*/
typedef struct uint8_EGidxLst_t
{
	uint8_EGidx_t next;/**< position in the array of the next structure in the list */
	uint8_EGidx_t prev;/**< position in the array of the previous structure in the list */
}
uint8_EGidxLst_t;

/* ========================================================================= */
/** @brief Null-initialized embeded list */
#define uint8_EGidxLstNull ((uint8_EGidxLst_t){.next=uint8_zeroEGidx,.prev=uint8_zeroEGidx})

/* ========================================================================= */
/** @brief Initialize a given structure to point to itself.
 * @param __lpt position to the list to initialize. 
 * @param __abase address of the base array.
 * @param __stsz size of each basic structure (i.e. sizeof(main structure))
 * */
#define uint8_EGidxLstInit(__abase,__stsz,__lpt) do{\
	char*const __EGil01=(__abase);\
	const size_t __EGil02=(__stsz);\
	uint8_EGidxc_t __EGil03=(__lpt);\
	(*((uint8_EGidxLst_t*)(EGaGetElem(__EGil01,__EGil02,__EGil03))))=(uint8_EGidxLst_t){.prev=__EGil03,.next=__EGil03};\
	}while(0)

/* ========================================================================= */
/** @brief Insert a __newpt __entry between two known consecutive entries.
 * @par Description:
 * This is only for internal list manipulation, where we know the prev/next
 * entries already.
 * @param __abase address of the base array.
 * @param __stsz size of each basic structure (i.e. sizeof(main structure))
 * @param __newpt position of the node to insert (in
 * (((char*)__abase)+__stsz*__newpt).
 * @param __prevpt position to the node to preceed the __newpt node.
 * @param __nextpt position to the node to follow the __newpt node. 
 * */
#define uint8___EGidxLstAdd(__abase,__stsz,__newpt,__prevpt,__nextpt) do{\
	char*const __EGil05=(__abase);\
	size_t const __EGil06=(__stsz);\
	uint8_EGidxc_t \
		__EGip06=(__prevpt),\
		__EGip07=(__newpt),\
		__EGip08=(__nextpt);\
	(*((uint8_EGidxLst_t*)(EGaGetElem(__EGil05,__EGil06,__EGip07))))=(uint8_EGidxLst_t){.prev=__EGip06,.next=__EGip08},\
((uint8_EGidxLst_t*)(EGaGetElem(__EGil05,__EGil06,__EGip06)))->next=__EGip07,\
((uint8_EGidxLst_t*)(EGaGetElem(__EGil05,__EGil06,__EGip08)))->prev=__EGip07;\
	}while(0)

/* ========================================================================= */
/** @brief Insert a __newpt __entry after the given pointer.
 * @param __abase address of the base array.
 * @param __stsz size of each basic structure (i.e. sizeof(main structure))
 * @param __newpt position to the __newpt list node to insert.
 * @param __head position from where the __newpt __entry will follow.
 * */
#define uint8_EGidxLstAddAfter(__abase,__stsz,__newpt,__head) do{\
	char*const __EGil11=(__abase);\
	size_t const __EGil12=(__stsz);\
	uint8_EGidxc_t __EGip10=(__head),\
		__EGip11=(__newpt),\
		__EGip12=((uint8_EGidxLst_t*)(EGaGetElem(__EGil11,__EGil12,__EGip10)))->next;\
	uint8___EGidxLstAdd(__EGil11,__EGil12,__EGip11,__EGip10,__EGip12);\
	}while(0)

/* ========================================================================= */
/** @brief Insert a __newpt __entry before the given pointer.
 * @param __abase address of the base array.
 * @param __stsz size of each basic structure (i.e. sizeof(main structure))
 * @param __newpt position to the __newpt list node to insert.
 * @param __tailpt position that will follow the __newpt __entry in the list.
 * */
#define uint8_EGidxLstAddBefore(__abase,__stsz,__newpt,__tailpt) do{\
	char*const __EGil20=(__abase);\
	size_t const __EGil21=(__stsz);\
	uint8_EGidxc_t __EGil23=(__newpt),\
		__EGil24=(__tailpt),\
		__EGil22=((uint8_EGidxLst_t*)(EGaGetElem(__EGil20,__EGil21,__EGil24)))->prev;\
	uint8___EGidxLstAdd(__EGil20,__EGil21,__EGil23,__EGil22,__EGil24);\
	}while(0)

/* ========================================================================= */
/** @brief Given two nodes, link them as if they would follow one another in the
 * list (used to delete points from a list).
 * @param __abase address of the base array.
 * @param __stsz size of each basic structure (i.e. sizeof(main structure))
 * @param __prevpt position to the guy to be in first in the list.
 * @param __nextpt position to the guy to follow in the list.
 * @par Description:
 * This function is intended to be used only internally, where we know what is
 * what, if you use it is because you also know what is going on.
 * */
#define uint8___EGidxLstLink(__abase,__stsz,__prevpt,__nextpt) do{\
	char*const __EGil25=(__abase);\
	size_t const __EGil26=(__stsz);\
	uint8_EGidxc_t __EGil27=(__prevpt),\
		__EGil28=(__nextpt);\
	((uint8_EGidxLst_t*)(EGaGetElem(__EGil25,__EGil26,__EGil27)))->next=__EGil28,\
	((uint8_EGidxLst_t*)(EGaGetElem(__EGil25,__EGil26,__EGil28)))->prev=__EGil27;\
	}while(0)

/* ========================================================================= */
/** @brief Given a node, eliminate it from the list it bellongs. but don't
 * change the internal data in the eliminated list (be carefull, if you will 
 * use it afterwards, then you MUST initialize it). If debugging is enabled,
 * then whenever you delete, the connector is reseted to 0xffffffff. What you
 * can count on is that the connector won't be NULL after deleting it from the
 * list, but it's values may be lost if we are debugging.
 * @param __abase address of the base array.
 * @param __stsz size of each basic structure (i.e. sizeof(main structure))
 * @param __entry entry to eliminate from the list.
 * */
#define uint8_EGidxLstDel(__abase,__stsz,__entry) do{\
	char*const __EGil29=(__abase);\
	size_t const __EGil30=(__stsz);\
	uint8_EGidxc_t __EGil31=(__entry);\
	uint8_EGidxLst_t*const __EGil32 = (uint8_EGidxLst_t*)(EGaGetElem(__EGil29,__EGil30,__EGil31));\
	uint8___EGidxLstLink(__EGil29,__EGil30,__EGil32->prev,__EGil32->next);\
	if(uint8_EG_IDXLIST_DEBUG <= DEBUG) (*__EGil32) = (uint8_EGidxLst_t){.next=uint8_MaxEGidx,.prev=uint8_MaxEGidx};\
	} while(0)

/* ========================================================================= */
/** @brief Replace one entry with another in a list.
 * @param __oldpt entry to be replaced, note that the pointers stored in next/prev
 * won't be changed, this may possibly lead to errors if the entry is used
 * afterwards without initialization.
 * @param __newpt newpt entry in the list.
 * @param __abase address of the base array.
 * @param __stsz size of each basic structure (i.e. sizeof(main structure))
 * */
#define uint8_EGidxLstReplace(__abase,__stsz,__oldpt,__newpt) do{\
	char*const __EGil33=(__abase);\
	size_t const __EGil34=(__stsz);\
	uint8_EGidxc_t __EGil36=(__newpt);\
	uint8_EGidxLst_t*const __EGil37=((uint8_EGidxLst_t*)(EGaGetElem(__EGil33,__EGil34,(__oldpt))));\
	/* copy contents in old member to replaciong member */\
	(*((uint8_EGidxLst_t*)(EGaGetElem(__EGil33,__EGil34,__EGil36))))=*__EGil37,\
	/* modify next/prev of neighbours */\
	((uint8_EGidxLst_t*)(EGaGetElem(__EGil33,__EGil34,__EGil37->next)))->prev=__EGil36,\
	((uint8_EGidxLst_t*)(EGaGetElem(__EGil33,__EGil34,__EGil37->prev)))->next=__EGil36;\
	}while(0)

/* ========================================================================= */
/** @brief Move an element from one list to another (deleting it from the
 * original one, and assuming both are part of the base array).
 * @param __entry element to be removed from it's current list to a position 
 * after the given __head.
 * @param __head element to be before the moved element.
 * @param __abase address of the base array.
 * @param __stsz size of each basic structure (i.e. sizeof(main structure))
 * */
#define uint8_EGidxLstMoveAfter(__abase,__stsz,__entry,__head) do{\
	char*const __EGil38=(__abase);\
	size_t const __EGil39=(__stsz);\
	uint8_EGidxc_t __EGil40=(__entry);\
	uint8_EGidxLst_t*const __EGil41=((uint8_EGidxLst_t*)(EGaGetElem(__EGil38,__EGil39,__EGil40)));\
	uint8___EGidxLstLink(__EGil38,__EGil39,__EGil41->prev,__EGil41->next);\
	uint8_EGidxLstAddAfter(__EGil38,__EGil39,__EGil40,(__head));\
	}while(0)

/* ========================================================================= */
/** @brief Move an element from one list to another (deleting it from the
 * original one).
 * @param __abase address of the base array.
 * @param __stsz size of each basic structure (i.e. sizeof(main structure))
 * @param __entry element to be removed from it's current list to a position
 * before the given __tailpt.
 * @param __tailpt element to be after the moved element.
 * */
#define uint8_EGidxLstMoveBefore(__abase,__stsz,__entry,__tailpt) do{\
	char*const __EGil42=(__abase);\
	size_t const __EGil43=(__stsz);\
	uint8_EGidxc_t __EGil44=(__entry);\
	uint8_EGidxLst_t*const __EGil45=((uint8_EGidxLst_t*)(EGaGetElem(__EGil42,__EGil43,__EGil44)));\
	uint8___EGidxLstLink(__EGil42,__EGil43,__EGil45->prev,__EGil45->next);\
	uint8_EGidxLstAddBefore(__EGil42,__EGil43,__EGil44,(__tailpt));\
	}while(0)

/* ========================================================================= */
/** @brief move all elements in one list to the given location in a second list.
 * @note we assume that both list are circular and that the given elemens ARE
 * part of the list.
 * @param __abase address of the base array.
 * @param __stsz size of each basic structure (i.e. sizeof(main structure))
 * @param __list position to the list to be joined with the second.
 * @param __head position from where the list will be spliced in.
 * */
#define uint8_EGidxLstSplice(__abase,__stsz,__list,__head) do{\
	char*const __EGil46=(__abase);\
	size_t const __EGil47=(__stsz);\
	uint8_EGidxc_t __EGil48=(__list),__EGil49=(__head);\
	uint8_EGidxc_t __EGil52=((uint8_EGidxLst_t*)(EGaGetElem(__EGil46,__EGil47,__EGil48)))->prev,\
					 __EGil53=((uint8_EGidxLst_t*)(EGaGetElem(__EGil46,__EGil47,__EGil49)))->prev;\
	uint8___EGidxLstLink(__EGil46,__EGil47,__EGil52,__EGil49);\
	uint8___EGidxLstLink(__EGil46,__EGil47,__EGil53,__EGil48);\
	}while(0)

/* ========================================================================= */
/** @}*/
/* end of uint8_eg_idxlist.h */
#endif
