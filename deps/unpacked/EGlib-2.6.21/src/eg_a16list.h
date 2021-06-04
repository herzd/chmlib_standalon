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
/** @defgroup EGa16List EGa16List
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
 * @note Throughou the code, i is assumed that __abase[0] is of type EGa16List_t;
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
 * @ingroup EGa16List */
/** @addtogroup EGa16List */
/** @{ */
/** @example eg_a16lis.ex.c 
 * This is a working (althought useless) example on @ref EGa16List.
 * */
/* ========================================================================= */
#ifndef __EG_A16LIST_H__
#define __EG_A16LIST_H__
#include "eg_config.h"
#include "eg_macros.h"

/* ========================================================================= */
/** @brief debug level for lists */
#define EG_A16LIST_DEBUG 100
/* ========================================================================= */
/** @brief List Node Structure.
 * @par Description:
 * This structure is to store a general node of the list. It is composed by
 * two members, that point to the next and previous structures in the list.*/
typedef struct EGa16List_t
{
	uint16_t next;/**< position in the array of the next structure in the list */
	uint16_t prev;/**< position in the array of the previous structure in the list */
}
EGa16List_t;

/* ========================================================================= */
/** @brief Null-initialized embeded list */
#define EGa16ListNull ((EGa16List_t){0,0})

/* ========================================================================= */
/** @brief Initialize a given structure to point to itself.
 * @param __lpt position to the list to initialize. 
 * @param __abase address of the base array.
 * @param __stsz size of each basic structure (i.e. sizeof(main structure))
 * */
#define EGa16ListInit(__abase,__stsz,__lpt) do{\
	char*const __EGabase=(__abase);\
	size_t const __EGasz=(__stsz);\
	const uint16_t __EGacpos=(__lpt);\
	EGa16List_t*const __EGalp=((EGa16List_t*)(EGaGetElem(__EGabase,__EGasz,__EGacpos)));\
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
#define __EGa16ListAdd(__abase,__stsz,__newpt,__prevpt,__nextpt) do{\
	char*const __EGabase=(__abase);\
	size_t const __EGasz=(__stsz);\
	const uint16_t __EGacpos=(__newpt), __EGappos=(__prevpt), __EGanpos=(__nextpt);\
	EGa16List_t*const __EGeL_add_new = ((EGa16List_t*)(EGaGetElem(__EGabase,__EGasz,__EGacpos))),\
	*const __EGeL_add_prev = ((EGa16List_t*)(EGaGetElem(__EGabase,__EGasz,__EGappos))),\
	*const __EGeL_add_next = ((EGa16List_t*)(EGaGetElem(__EGabase,__EGasz,__EGanpos)));\
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
#define EGa16ListAddAfter(__abase,__stsz,__newpt,__head) do{\
	char*const __EGabase2=(__abase);\
	size_t const __EGasz2=(__stsz);\
	const uint16_t __EGa1pos=(__head),__EGa2pos=(__newpt);\
	EGa16List_t*const __EGa1t=((EGa16List_t*)(EGaGetElem(__EGabase2,__EGasz2,__EGa1pos)));\
	const uint16_t __EGa3pos=__EGa1t->next;\
	__EGa16ListAdd(__EGabase2,__EGasz2,__EGa2pos,__EGa1pos,__EGa3pos);\
	}while(0)

/* ========================================================================= */
/** @brief Insert a __newpt __entry before the given pointer.
 * @param __abase address of the base array.
 * @param __stsz size of each basic structure (i.e. sizeof(main structure))
 * @param __newpt position to the __newpt list node to insert.
 * @param __tailpt position that will follow the __newpt __entry in the list.
 * */
#define EGa16ListAddBefore(__abase,__stsz,__newpt,__tailpt) do{\
	char*const __EGabase2=(__abase);\
	size_t const __EGasz2=(__stsz);\
	const uint16_t __EGa2pos=(__newpt),__EGa3pos=(__tailpt);\
	EGa16List_t*const __EGa3t=((EGa16List_t*)(EGaGetElem(__EGabase2,__EGasz2,__EGa3pos)));\
	const uint16_t __EGa1pos=__EGa3t->prev;\
	__EGa16ListAdd(__EGabase2,__EGasz2,__EGa2pos,__EGa1pos,__EGa3pos);\
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
#define __EGa16ListLink(__abase,__stsz,__prevpt,__nextpt) do{\
	char*const __EGabase=(__abase);\
	size_t const __EGasz=(__stsz);\
	const uint16_t __EGappos=(__prevpt),__EGanpos=(__nextpt);\
	EGa16List_t*const __EGeL_lnk_prev = (EGa16List_t*)(EGaGetElem(__EGabase,__EGasz,__EGappos)),\
	*const __EGeL_lnk_next = (EGa16List_t*)(EGaGetElem(__EGabase,__EGasz,__EGanpos));\
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
#define EGa16ListDel(__abase,__stsz,__entry) do{\
	char*const __EGabase2=(__abase);\
	size_t const __EGasz2=(__stsz);\
	const uint16_t __EGacpos=(__entry);\
	EGa16List_t *const __EGeL_del_entr = (EGa16List_t*)(EGaGetElem(__EGabase2,__EGasz2,__EGacpos));\
	__EGa16ListLink(__EGabase2,__EGasz2,__EGeL_del_entr->prev,__EGeL_del_entr->next);\
	if(EG_A16LIST_DEBUG <= DEBUG) (*__EGeL_del_entr) = (EGa16List_t){UINT16_MAX,UINT16_MAX};\
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
#define EGa16ListReplace(__abase,__stsz,__oldpt,__newpt) do{\
	char*const __EGabase=(__abase);\
	size_t const __EGasz=(__stsz);\
	const uint16_t __EGaopos=(__oldpt), __EGacpos=(__newpt);\
	EGa16List_t*const __EGaopt=((EGa16List_t*)(EGaGetElem(__EGabase,__EGasz,__EGaopos))),\
	*const __EGacpt=((EGa16List_t*)(EGaGetElem(__EGabase,__EGasz,__EGacpos)));\
	EGa16List_t*const __EGanpt=((EGa16List_t*)(EGaGetElem(__EGabase,__EGasz,__EGaopt->next))),\
	*const __EGappt=((EGa16List_t*)(EGaGetElem(__EGabase,__EGasz,__EGaopt->prev)));\
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
#define EGa16ListMoveAfter(__abase,__stsz,__entry,__head) do{\
	char*const __EGabase3=(__abase);\
	size_t const __EGasz3=(__stsz);\
	const uint16_t __EGaepos=(__entry),__EGahpos=(__head);\
	EGa16List_t*const __EGaept=((EGa16List_t*)(EGaGetElem(__EGabase3,__EGasz3,__EGaepos)));\
	__EGa16ListLink(__EGabase3,__EGasz3,__EGaept->prev,__EGaept->next);\
	EGa16ListAddAfter(__EGabase3,__EGasz3,__EGaepos,__EGahpos);\
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
#define EGa16ListMoveBefore(__abase,__stsz,__entry,__tailpt) do{\
	char*const __EGabase3=(__abase);\
	size_t const __EGasz3=(__stsz);\
	const uint16_t __EGaepos=(__entry),__EGatpos=(__tailpt);\
	EGa16List_t*const __EGaept=((EGa16List_t*)(EGaGetElem(__EGabase3,__EGasz3,__EGaepos)));\
	__EGa16ListLink(__EGabase3,__EGasz3,__EGaept->prev,__EGaept->next);\
	EGa16ListAddBefore(__EGabase3,__EGasz3,__EGaepos,__EGatpos);\
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
#define EGa16ListSplice(__abase,__stsz,__list,__head) do{\
	char*const __EGabase2=(__abase);\
	size_t const __EGasz2=(__stsz);\
	const uint16_t __EGap10=(__list),__EGap20=(__head);\
	EGa16List_t*const __EGat10=((EGa16List_t*)(EGaGetElem(__EGabase2,__EGasz2,__EGap10))),\
	*const __EGat20=((EGa16List_t*)(EGaGetElem(__EGabase2,__EGasz2,__EGap20)));\
	const uint16_t __EGap11=__EGat10->prev,__EGap21=__EGat20->prev;\
	__EGa16ListLink(__EGabase2,__EGasz2,__EGap11,__EGap20);\
	__EGa16ListLink(__EGabase2,__EGasz2,__EGap21,__EGap10);\
	}while(0)

/* ========================================================================= */
/** @}*/
/* end of eg_a16lis.h */
#endif
