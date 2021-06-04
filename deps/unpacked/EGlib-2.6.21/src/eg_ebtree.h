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
/** @defgroup EGeBTree EGeBTree
 *
 * Here we define a minimal implementation of binary trees, using the philosophy
 * of embeded structures. Operations as adding nodes (in any position of the
 * tree), deleting nodes, splicing trees, and separating sub-trees are performed
 * in O(1) time. To obtain this running time information such as depth is not
 * stored in the nodes of the tree.
 *
 * -2005-07-08
 * 					- First Implementation
 * */
/** @file
 * @ingroup EGeBTree */
/** @addtogroup EGeBTree */
/** @{ 
 * @example eg_ebtree.ex.c  
 * This is a simple example for the usage of binary trees as implemented in @ref
 * EGeBTree. */
/* ========================================================================= */
#ifndef __EG_EBTREE_H__
#define __EG_EBTREE_H__
#include "eg_macros.h"

/* ========================================================================= */
/** @brief Parent index */
#define EG_EBTREE_PARENT 0

/* ========================================================================= */
/** @brief Left Child Index */
#define EG_EBTREE_LEFT 1

/* ========================================================================= */
/** @brief Right Child Index */
#define EG_EBTREE_RIGHT 2

/* ========================================================================= */
/** @brief Basic structure of a tree node, note that a node in a tree is in
 * itself a (sub)tree. */
typedef struct EGeBTree_t
{
	struct EGeBTree_t *cn[3];	/**< This array contains three pointers, in 
																 position #EG_EBTREE_PARENT the link to the 
																 parent of this node, in position 
																 #EG_EBTREE_LEFT the link to the left child, and
																 in #EG_EBTREE_RIGHT the link to the right 
																 child for this node. If any of this pointers
																 is NULL, then the link does not exists. In 
																 particular a leaf is a node with both child
																 links set to NULL, and the root of a tree
																 is the node whose parent link is NULL. */
}
EGeBTree_t;

/* ========================================================================= */
/** @brief Initialize a tree structure. It let it as a tree containing only
 * itself. and allocate any internal memory (not allocated by the user).
 * @param __tn tree to initialize. 
 * @note Note that the definition of #EGeBTree_t does not need to initialize any
 * memory, and basically reset and init do exactly the same, while clear do
 * nothing. */
#define EGeBTreeInit(__tn) (*(__tn) = (EGeBTree_t){{0,0,0}})

/* ========================================================================= */
/** @brief Let a tree just as after an init call.
 * @param __tn tree to reset.
 * @note Note that this macro is provided onll for completeness, because there
 * is no need for it.
 * */
#define EGeBTreeReset(__tn) EGeBTreeInit(__tn)

/* ========================================================================= */
/** @brief Free any internal memory (not allocated by the user) related to this
 * structure.
 * @param __tn tree to be cleared.
 * @note Note that this function is not really needed, it is provided for
 * completness, but is defined as nothing. */
#define EGeBTreeClear(__tn)

/* ========================================================================= */
/** @brief Add a left __child to a node in a tree.
 * @param __parent node to wich we will add a left child
 * @param __child node to be a left child
 * @return zero on success, non zero otherwise.
 * */
#define EGeBTreeAddLeft(__parent,__child) ({\
	EGeBTree_t*const __EGeparent = (__parent);\
	EGeBTree_t*const __EGechild = (__child);\
	__EGeparent->cn[EG_EBTREE_LEFT] ? (fprintf(stderr,"Can't add left __child to "#__parent" because it already has a left __child, in %s:%s:%d\n",__func__,__FILE__,__LINE__),1): (__EGeparent->cn[EG_EBTREE_LEFT] = __EGechild, __EGechild->cn[EG_EBTREE_PARENT] = __EGeparent, 0);})

/* ========================================================================= */
/** @brief Add a right child to a node in a tree.
 * @param __parent node to wich we will add a right child
 * @param __child node to be a right child
 * @return zero on success, non zero otherwise.
 * */
#define EGeBTreeAddRight(__parent,__child) ({\
	EGeBTree_t*const __EGeparent = (__parent);\
	EGeBTree_t*const __EGechild = (__child);\
	__EGeparent->cn[EG_EBTREE_RIGHT] ? (fprintf(stderr,"Can't add right __child to "#__parent" because it already has a right __child, in %s:%s:%d\n",__func__,__FILE__,__LINE__),1): (__EGeparent->cn[EG_EBTREE_RIGHT] = __EGechild, __EGechild->cn[EG_EBTREE_PARENT] = __EGeparent, 0);})

/* ========================================================================= */
/** @return a pointer to the parent of the given node, if NULL, then this node
 * is the root node of the tree.
 * @param __member node for wich we are looking for the father.
 * @return pointer to the parent node in the tree.
 * */
#define EGeBTreeGetParent(__member) ((__member)->cn[EG_EBTREE_PARENT])

/* ========================================================================= */
/** @return a pointer to the left child of the given node, if NULL, then this
 * node don;t have a left child
 * @param __member node for wich we are looking for the left child.
 * @return pointer to the left child node in the tree.
 * */
#define EGeBTreeGetLeft(__member) ((__member)->cn[EG_EBTREE_LEFT])

/* ========================================================================= */
/** @return a pointer to the right child of the given node, if NULL, then this
 * node don;t have a right child
 * @param __member node for wich we are looking for the right child.
 * @return pointer to the right child node in the tree.
 * */
#define EGeBTreeGetRight(__member) ((__member)->cn[EG_EBTREE_RIGHT])

/* ========================================================================= */
/** @brief return the root of the tree containing the given tree node.
 * @param __member node for wich we are looking for the tree-root.
 * @return pointer to the root of the tree containing the given node.
 * */
#define EGeBTreeGetRoot(__member) ({\
	EGeBTree_t*__EGeroot = (__member);\
	while(__EGeroot->cn[EG_EBTREE_PARENT]) \
		__EGeroot = __EGeroot->cn[EG_EBTREE_PARENT];\
	__EGeroot;})

/* ========================================================================= */
/** @brief Replace a node in a tree.
 * @param __old node to be replaced.
 * @param __rep node to be used as replacement.
 * @return zero on success, non-zero otherwise.
 * */
#define EGeBTreeReplace(__old,__rep) ({\
	EGeBTree_t*const __EGeold = (__old);\
	EGeBTree_t*const __EGenew = (__rep);\
	EGeBTree_t*const __EGepar = __EGeold->cn[EG_EBTREE_PARENT];\
	EGeBTree_t*const __EGelft = __EGeold->cn[EG_EBTREE_LEFT];\
	EGeBTree_t*const __EGergt = __EGeold->cn[EG_EBTREE_RIGHT];\
	*__EGenew = *__EGeold;\
	if(__EGepar)\
	{\
		if(__EGepar->cn[EG_EBTREE_LEFT] == __EGeold)\
			__EGepar->cn[EG_EBTREE_LEFT] = __EGenew;\
		else __EGepar->cn[EG_EBTREE_RIGHT] = __EGenew;\
	}\
	if(__EGelft) __EGelft->cn[EG_EBTREE_PARENT] = __EGenew;\
	if(__EGergt) __EGergt->cn[EG_EBTREE_PARENT] = __EGenew;\
	0;})

/* ========================================================================= */
/** @brief return the successor of the given node in it's tree according to the
 * in-order ordering.
 * @param __node element for wich we want the successor.
 * @return pointer to the successor, if NULL is returned, then there is no
 * successor for the given node.
 * @note The in-order considered here is the one depicted in the next figure:
 * \f[
 \psset{unit=1cm,arrows=->}
 \begin{pspicture}(-1,-1)(7,3)
 \rput(0,0){\circlenode{a}{1}}
 \rput(1,1){\circlenode{b}{2}}
 \rput(2,0){\circlenode{c}{3}}
 \rput(3,2){\circlenode{d}{4}}
 \rput(4,0){\circlenode{e}{5}}
 \rput(5,1){\circlenode{f}{6}}
 \rput(6,0){\circlenode{g}{7}}
 \ncline{b}{a}
 \ncline{b}{c}
 \ncline{d}{b}
 \ncline{d}{f}
 \ncline{f}{e}
 \ncline{f}{g}
 \end{pspicture}
 \f] In this figure the numbers in the nodes represent the in-order order. */
#define EGeBTreeGetNext(__node) ({\
	EGeBTree_t* __EGexn = (__node);\
	EGeBTree_t* __EGecn = __EGexn->cn[EG_EBTREE_RIGHT];\
	if(__EGecn) while(__EGecn->cn[EG_EBTREE_LEFT])\
			__EGecn = __EGecn->cn[EG_EBTREE_LEFT];\
	else do __EGecn = __EGexn->cn[EG_EBTREE_PARENT]; while(__EGecn && \
			__EGecn->cn[EG_EBTREE_RIGHT] == __EGexn && (__EGexn = __EGecn));\
	__EGecn;})

/* ========================================================================= */
/** @brief return the predecesor of the given node in it's tree according to 
 * the in-order ordering.
 * @param __node element for wich we want the predecesor.
 * @return pointer to the predecesor, if NULL is returned, then there is no
 * predecesor for the given node.
 * @note The in-order considered here is the one depicted in the next figure:
 * \f[
 \psset{unit=1cm,arrows=->}
 \begin{pspicture}(-1,-1)(7,3)
 \rput(0,0){\circlenode{a}{1}}
 \rput(1,1){\circlenode{b}{2}}
 \rput(2,0){\circlenode{c}{3}}
 \rput(3,2){\circlenode{d}{4}}
 \rput(4,0){\circlenode{e}{5}}
 \rput(5,1){\circlenode{f}{6}}
 \rput(6,0){\circlenode{g}{7}}
 \ncline{b}{a}
 \ncline{b}{c}
 \ncline{d}{b}
 \ncline{d}{f}
 \ncline{f}{e}
 \ncline{f}{g}
 \end{pspicture}
 \f] In this figure the numbers in the nodes represent the in-order order. */
#define EGeBTreeGetPrev(__node) ({\
	EGeBTree_t* __EGexn = (__node);\
	EGeBTree_t* __EGecn = __EGexn->cn[EG_EBTREE_LEFT];\
	if(__EGecn) while(__EGecn->cn[EG_EBTREE_RIGHT])\
			__EGecn = __EGecn->cn[EG_EBTREE_RIGHT];\
	else do __EGecn = __EGexn->cn[EG_EBTREE_PARENT]; while(__EGecn && \
			__EGecn->cn[EG_EBTREE_LEFT] == __EGexn && (__EGexn = __EGecn));\
	__EGecn;})

/* ========================================================================= */
/** @brief return the first node in the (sub-)tree (according to the in-order).
 * @param __node pointer to a node of the tree.
 * @return first node in the tree.
 * */
#define EGeBTreeGetFirst(__node) ({\
	EGeBTree_t* __EGecn = (__node);\
	while(__EGecn->cn[EG_EBTREE_LEFT]) __EGecn = __EGecn->cn[EG_EBTREE_LEFT];\
	__EGecn;})

/* ========================================================================= */
/** @brief return the last node in the (sub-)tree (according to the in-order).
 * @param __node pointer to a node of the tree.
 * @return last node in the tree.
 * */
#define EGeBTreeGetLast(__node) ({\
	EGeBTree_t* __EGecn = (__node);\
	while(__EGecn->cn[EG_EBTREE_RIGHT]) __EGecn = __EGecn->cn[EG_EBTREE_RIGHT];\
	__EGecn;})

/* ========================================================================= */
/** @brief Erase a leaf node from a tree.
 * @param __leaf node to be erased from the tree.
 * @return zero on success, non-zero otherwise, a failure means that the given
 * node wasn't a leaf in the tree.
 * */
#define EGeBTreeDel(__leaf) ({\
	EGeBTree_t*const __EGelf = (__leaf);\
	EGeBTree_t*const __EGepar = __EGelf->cn[EG_EBTREE_PARENT];\
	int __rval = 0;\
	if(__EGelf->cn[EG_EBTREE_LEFT] || __EGelf->cn[EG_EBTREE_RIGHT]) __rval = 1;\
	else if(__EGepar)\
	{\
		if(__EGepar->cn[EG_EBTREE_LEFT] == __EGelf) \
			__EGepar->cn[EG_EBTREE_LEFT] = 0;\
		else __EGepar->cn[EG_EBTREE_RIGHT] = 0;\
	}\
	__rval;})

/* ========================================================================= */
/** @brief Cut the given node from the tree containing it (and so create a
 * sub-tree).
 * @param __member
 * @return zero on success, non-zero otherwise.
 * */
#define EGeBTreeCut(__member) ({\
	EGeBTree_t*const __EGeroot = (__member);\
	EGeBTree_t*const __EGepar = __EGeroot->cn[EG_EBTREE_PARENT];\
	if(__EGepar) \
	{\
		__EGeroot->cn[EG_EBTREE_PARENT] = 0; \
		if(__EGepar->cn[EG_EBTREE_LEFT] == __EGeroot)\
			__EGepar->cn[EG_EBTREE_LEFT] = 0;\
		else __EGepar->cn[EG_EBTREE_RIGHT] = 0;\
	}\
	0;})

/* ========================================================================= */
/** @} */
#endif
