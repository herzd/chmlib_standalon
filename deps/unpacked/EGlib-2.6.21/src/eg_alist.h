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
/** @defgroup EGaList EGaList
 *
 * Here we define the basic interface for an array-based general list; we
 * assume that the structure is within a mayor structure; and all access are
 * from a base memory address. This implementation is based on the philosophy of
 * embeded structures.
 *
 * 
 * @version 0.0.1
 * @par History:
 * - 2013-04-01
 * 						- First version (adapted from eg_elist.h)
 *
 * @note Throughou the code, i is assumed that __abase[0] is of type EGaList_t;
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
 * @ingroup EGaList */
/** @addtogroup EGaList */
/** @{ */
/** @example eg_alist.ex.c 
 * This is a working (althought useless) example on @ref EGaList.
 * */
/* ========================================================================= */
#ifndef __EG_ALIST_H__
#define __EG_ALIST_H__
#include "eg_config.h"
#include "eg_macros.h"

/* ========================================================================= */
/** @brief debug level for lists */
#define EG_ALIST_DEBUG 100
/* ========================================================================= */
/** @brief List Node Structure.
 * @par Description:
 * This structure is to store a general node of the list. It is composed by
 * two members, that point to the next and previous structures in the list.*/
typedef struct EGaList_t
{
	int next;/**< position in the array of the next structure in the list */
	int prev;/**< position in the array of the previous structure in the list */
}
EGaList_t;

/* ========================================================================= */
/** @brief Null-initialized embeded list */
#define EGaListNull ((EGaList_t){0,0})

/* ========================================================================= */
/** @brief Initialize a given structure to point to itself.
 * @param __lpt position to the list to initialize. 
 * @param __abase address of the base array.
 * @param __stsz size of each basic structure (i.e. sizeof(main structure))
 * */
#define EGaListInit(__abase,__stsz,__lpt) do{\
	char*const __EGabase=(__abase);\
	size_t const __EGasz=(__stsz);\
	const int __EGacpos=(__lpt);\
	EGaList_t*const __EGalp=((EGaList_t*)(EGaGetElem(__EGabase,__EGasz,__EGacpos)));\
	__EGalp->next=__EGalp->prev=__EGacpos;\
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
#define __EGaListAdd(__abase,__stsz,__newpt,__prevpt,__nextpt) do{\
	char*const __EGabase=(__abase);\
	size_t const __EGasz=(__stsz);\
	const int __EGacpos=(__newpt), __EGappos=(__prevpt), __EGanpos=(__nextpt);\
	EGaList_t*const __EGeL_add_new = ((EGaList_t*)(EGaGetElem(__EGabase,__EGasz,__EGacpos))),\
	*const __EGeL_add_prev = ((EGaList_t*)(EGaGetElem(__EGabase,__EGasz,__EGappos))),\
	*const __EGeL_add_next = ((EGaList_t*)(EGaGetElem(__EGabase,__EGasz,__EGanpos)));\
	__EGeL_add_next->prev = __EGacpos,\
	__EGeL_add_prev->next = __EGacpos,\
	__EGeL_add_new->next = __EGanpos,\
	__EGeL_add_new->prev = __EGappos;\
	}while(0)

/* ========================================================================= */
/** @brief Insert a __newpt __entry after the given pointer.
 * @param __abase address of the base array.
 * @param __stsz size of each basic structure (i.e. sizeof(main structure))
 * @param __newpt position to the __newpt list node to insert.
 * @param __head position from where the __newpt __entry will follow.
 * */
#define EGaListAddAfter(__abase,__stsz,__newpt,__head) do{\
	char*const __EGabase2=(__abase);\
	size_t const __EGasz2=(__stsz);\
	const int __EGa1pos=(__head),__EGa2pos=(__newpt);\
	EGaList_t*const __EGa1t=((EGaList_t*)(EGaGetElem(__EGabase2,__EGasz2,__EGa1pos)));\
	const int __EGa3pos=__EGa1t->next;\
	__EGaListAdd(__EGabase2,__EGasz2,__EGa2pos,__EGa1pos,__EGa3pos);\
	}while(0)

/* ========================================================================= */
/** @brief Insert a __newpt __entry before the given pointer.
 * @param __abase address of the base array.
 * @param __stsz size of each basic structure (i.e. sizeof(main structure))
 * @param __newpt position to the __newpt list node to insert.
 * @param __tailpt position that will follow the __newpt __entry in the list.
 * */
#define EGaListAddBefore(__abase,__stsz,__newpt,__tailpt) do{\
	char*const __EGabase2=(__abase);\
	size_t const __EGasz2=(__stsz);\
	const int __EGa2pos=(__newpt),__EGa3pos=(__tailpt);\
	EGaList_t*const __EGa3t=((EGaList_t*)(EGaGetElem(__EGabase2,__EGasz2,__EGa3pos)));\
	const int __EGa1pos=__EGa3t->prev;\
	__EGaListAdd(__EGabase2,__EGasz2,__EGa2pos,__EGa1pos,__EGa3pos);\
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
#define __EGaListLink(__abase,__stsz,__prevpt,__nextpt) do{\
	char*const __EGabase=(__abase);\
	size_t const __EGasz=(__stsz);\
	const int __EGappos=(__prevpt),__EGanpos=(__nextpt);\
	EGaList_t*const __EGeL_lnk_prev = (EGaList_t*)(EGaGetElem(__EGabase,__EGasz,__EGappos)),\
	*const __EGeL_lnk_next = (EGaList_t*)(EGaGetElem(__EGabase,__EGasz,__EGanpos));\
	__EGeL_lnk_prev->next = __EGanpos,\
	__EGeL_lnk_next->prev = __EGappos;\
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
#define EGaListDel(__abase,__stsz,__entry) do{\
	char*const __EGabase2=(__abase);\
	size_t const __EGasz2=(__stsz);\
	const int __EGacpos=(__entry);\
	EGaList_t *const __EGeL_del_entr = (EGaList_t*)(EGaGetElem(__EGabase2,__EGasz2,__EGacpos));\
	__EGaListLink(__EGabase2,__EGasz2,__EGeL_del_entr->prev,__EGeL_del_entr->next);\
	if(EG_ALIST_DEBUG <= DEBUG) (*__EGeL_del_entr) = (EGaList_t){INT_MAX,INT_MAX};\
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
#define EGaListReplace(__abase,__stsz,__oldpt,__newpt) do{\
	char*const __EGabase=(__abase);\
	size_t const __EGasz=(__stsz);\
	const int __EGaopos=(__oldpt), __EGacpos=(__newpt);\
	EGaList_t*const __EGaopt=((EGaList_t*)(EGaGetElem(__EGabase,__EGasz,__EGaopos))),\
	*const __EGacpt=((EGaList_t*)(EGaGetElem(__EGabase,__EGasz,__EGacpos)));\
	EGaList_t*const __EGanpt=((EGaList_t*)(EGaGetElem(__EGabase,__EGasz,__EGaopt->next))),\
	*const __EGappt=((EGaList_t*)(EGaGetElem(__EGabase,__EGasz,__EGaopt->prev)));\
	__EGacpt->next=__EGaopt->next,__EGacpt->prev=__EGaopt->prev,__EGanpt->prev=__EGappt->next=__EGacpos;\
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
#define EGaListMoveAfter(__abase,__stsz,__entry,__head) do{\
	char*const __EGabase3=(__abase);\
	size_t const __EGasz3=(__stsz);\
	const int __EGaepos=(__entry),__EGahpos=(__head);\
	EGaList_t*const __EGaept=((EGaList_t*)(EGaGetElem(__EGabase3,__EGasz3,__EGaepos)));\
	__EGaListLink(__EGabase3,__EGasz3,__EGaept->prev,__EGaept->next);\
	EGaListAddAfter(__EGabase3,__EGasz3,__EGaepos,__EGahpos);\
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
#define EGaListMoveBefore(__abase,__stsz,__entry,__tailpt) do{\
	char*const __EGabase3=(__abase);\
	size_t const __EGasz3=(__stsz);\
	const int __EGaepos=(__entry),__EGatpos=(__tailpt);\
	EGaList_t*const __EGaept=((EGaList_t*)(EGaGetElem(__EGabase3,__EGasz3,__EGaepos)));\
	__EGaListLink(__EGabase3,__EGasz3,__EGaept->prev,__EGaept->next);\
	EGaListAddBefore(__EGabase3,__EGasz3,__EGaepos,__EGatpos);\
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
#define EGaListSplice(__abase,__stsz,__list,__head) do{\
	char*const __EGabase2=(__abase);\
	size_t const __EGasz2=(__stsz);\
	const int __EGap10=(__list),__EGap20=(__head);\
	EGaList_t*const __EGat10=((EGaList_t*)(EGaGetElem(__EGabase2,__EGasz2,__EGap10))),\
	*const __EGat20=((EGaList_t*)(EGaGetElem(__EGabase2,__EGasz2,__EGap20)));\
	const int __EGap11=__EGat10->prev,__EGap21=__EGat20->prev;\
	__EGaListLink(__EGabase2,__EGasz2,__EGap11,__EGap20);\
	__EGaListLink(__EGabase2,__EGasz2,__EGap21,__EGap10);\
	}while(0)

/* ========================================================================= */
/** @}*/
/* end of eg_alist.h */
#endif
