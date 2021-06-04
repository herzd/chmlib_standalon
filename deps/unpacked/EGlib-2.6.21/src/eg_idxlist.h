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
 * @note Throughou the code, i is assumed that __abase[0] is of type EGidxLst_t;
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
/** @example eg_idxlist.ex.c 
 * This is a working (althought useless) example on @ref EGidxLst.
 * */
/* ========================================================================= */
#ifndef __EG_IDXLIST_H__
#define __EG_IDXLIST_H__
#include "eg_config.h"
#include "eg_macros.h"
#include "eg_idx.h"

/* ========================================================================= */
/** @brief debug level for lists */
#define EG_IDXLIST_DEBUG 100
/* ========================================================================= */
/** @brief List Node Structure.
 * @par Description:
 * This structure is to store a general node of the list. It is composed by
 * two members, that point to the next and previous structures in the list.*/
typedef struct EGidxLst_t
{
	EGidx_t next;/**< position in the array of the next structure in the list */
	EGidx_t prev;/**< position in the array of the previous structure in the list */
}
EGidxLst_t;

/* ========================================================================= */
/** @brief Null-initialized embeded list */
#define EGidxLstNull ((EGidxLst_t){.next=zeroEGidx,.prev=zeroEGidx})

/* ========================================================================= */
/** @brief Initialize a given structure to point to itself.
 * @param __lpt position to the list to initialize. 
 * @param __abase address of the base array.
 * @param __stsz size of each basic structure (i.e. sizeof(main structure))
 * */
#define EGidxLstInit(__abase,__stsz,__lpt) do{\
	char*const __EGil01=(__abase);\
	const size_t __EGil02=(__stsz);\
	EGidxc_t __EGil03=(__lpt);\
	(*((EGidxLst_t*)(EGaGetElem(__EGil01,__EGil02,__EGil03))))=(EGidxLst_t){.prev=__EGil03,.next=__EGil03};\
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
#define __EGidxLstAdd(__abase,__stsz,__newpt,__prevpt,__nextpt) do{\
	char*const __EGil05=(__abase);\
	size_t const __EGil06=(__stsz);\
	EGidxc_t \
		__EGip06=(__prevpt),\
		__EGip07=(__newpt),\
		__EGip08=(__nextpt);\
	(*((EGidxLst_t*)(EGaGetElem(__EGil05,__EGil06,__EGip07))))=(EGidxLst_t){.prev=__EGip06,.next=__EGip08},\
((EGidxLst_t*)(EGaGetElem(__EGil05,__EGil06,__EGip06)))->next=__EGip07,\
((EGidxLst_t*)(EGaGetElem(__EGil05,__EGil06,__EGip08)))->prev=__EGip07;\
	}while(0)

/* ========================================================================= */
/** @brief Insert a __newpt __entry after the given pointer.
 * @param __abase address of the base array.
 * @param __stsz size of each basic structure (i.e. sizeof(main structure))
 * @param __newpt position to the __newpt list node to insert.
 * @param __head position from where the __newpt __entry will follow.
 * */
#define EGidxLstAddAfter(__abase,__stsz,__newpt,__head) do{\
	char*const __EGil11=(__abase);\
	size_t const __EGil12=(__stsz);\
	EGidxc_t __EGip10=(__head),\
		__EGip11=(__newpt),\
		__EGip12=((EGidxLst_t*)(EGaGetElem(__EGil11,__EGil12,__EGip10)))->next;\
	__EGidxLstAdd(__EGil11,__EGil12,__EGip11,__EGip10,__EGip12);\
	}while(0)

/* ========================================================================= */
/** @brief Insert a __newpt __entry before the given pointer.
 * @param __abase address of the base array.
 * @param __stsz size of each basic structure (i.e. sizeof(main structure))
 * @param __newpt position to the __newpt list node to insert.
 * @param __tailpt position that will follow the __newpt __entry in the list.
 * */
#define EGidxLstAddBefore(__abase,__stsz,__newpt,__tailpt) do{\
	char*const __EGil20=(__abase);\
	size_t const __EGil21=(__stsz);\
	EGidxc_t __EGil23=(__newpt),\
		__EGil24=(__tailpt),\
		__EGil22=((EGidxLst_t*)(EGaGetElem(__EGil20,__EGil21,__EGil24)))->prev;\
	__EGidxLstAdd(__EGil20,__EGil21,__EGil23,__EGil22,__EGil24);\
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
#define __EGidxLstLink(__abase,__stsz,__prevpt,__nextpt) do{\
	char*const __EGil25=(__abase);\
	size_t const __EGil26=(__stsz);\
	EGidxc_t __EGil27=(__prevpt),\
		__EGil28=(__nextpt);\
	((EGidxLst_t*)(EGaGetElem(__EGil25,__EGil26,__EGil27)))->next=__EGil28,\
	((EGidxLst_t*)(EGaGetElem(__EGil25,__EGil26,__EGil28)))->prev=__EGil27;\
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
#define EGidxLstDel(__abase,__stsz,__entry) do{\
	char*const __EGil29=(__abase);\
	size_t const __EGil30=(__stsz);\
	EGidxc_t __EGil31=(__entry);\
	EGidxLst_t*const __EGil32 = (EGidxLst_t*)(EGaGetElem(__EGil29,__EGil30,__EGil31));\
	__EGidxLstLink(__EGil29,__EGil30,__EGil32->prev,__EGil32->next);\
	if(EG_IDXLIST_DEBUG <= DEBUG) (*__EGil32) = (EGidxLst_t){.next=MaxEGidx,.prev=MaxEGidx};\
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
#define EGidxLstReplace(__abase,__stsz,__oldpt,__newpt) do{\
	char*const __EGil33=(__abase);\
	size_t const __EGil34=(__stsz);\
	EGidxc_t __EGil36=(__newpt);\
	EGidxLst_t*const __EGil37=((EGidxLst_t*)(EGaGetElem(__EGil33,__EGil34,(__oldpt))));\
	/* copy contents in old member to replaciong member */\
	(*((EGidxLst_t*)(EGaGetElem(__EGil33,__EGil34,__EGil36))))=*__EGil37,\
	/* modify next/prev of neighbours */\
	((EGidxLst_t*)(EGaGetElem(__EGil33,__EGil34,__EGil37->next)))->prev=__EGil36,\
	((EGidxLst_t*)(EGaGetElem(__EGil33,__EGil34,__EGil37->prev)))->next=__EGil36;\
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
#define EGidxLstMoveAfter(__abase,__stsz,__entry,__head) do{\
	char*const __EGil38=(__abase);\
	size_t const __EGil39=(__stsz);\
	EGidxc_t __EGil40=(__entry);\
	EGidxLst_t*const __EGil41=((EGidxLst_t*)(EGaGetElem(__EGil38,__EGil39,__EGil40)));\
	__EGidxLstLink(__EGil38,__EGil39,__EGil41->prev,__EGil41->next);\
	EGidxLstAddAfter(__EGil38,__EGil39,__EGil40,(__head));\
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
#define EGidxLstMoveBefore(__abase,__stsz,__entry,__tailpt) do{\
	char*const __EGil42=(__abase);\
	size_t const __EGil43=(__stsz);\
	EGidxc_t __EGil44=(__entry);\
	EGidxLst_t*const __EGil45=((EGidxLst_t*)(EGaGetElem(__EGil42,__EGil43,__EGil44)));\
	__EGidxLstLink(__EGil42,__EGil43,__EGil45->prev,__EGil45->next);\
	EGidxLstAddBefore(__EGil42,__EGil43,__EGil44,(__tailpt));\
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
#define EGidxLstSplice(__abase,__stsz,__list,__head) do{\
	char*const __EGil46=(__abase);\
	size_t const __EGil47=(__stsz);\
	EGidxc_t __EGil48=(__list),__EGil49=(__head);\
	EGidxc_t __EGil52=((EGidxLst_t*)(EGaGetElem(__EGil46,__EGil47,__EGil48)))->prev,\
					 __EGil53=((EGidxLst_t*)(EGaGetElem(__EGil46,__EGil47,__EGil49)))->prev;\
	__EGidxLstLink(__EGil46,__EGil47,__EGil52,__EGil49);\
	__EGidxLstLink(__EGil46,__EGil47,__EGil53,__EGil48);\
	}while(0)

/* ========================================================================= */
/** @}*/
/* end of eg_idxlist.h */
#endif
