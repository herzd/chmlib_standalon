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
/** @defgroup EGeUgraph EGeUgraph
 * Here we define a basic directed graph structure, it holds the number of
 * nodes and edges in the graph, as well as the in and out degree of all
 * nodes, and allow to access the head and tail of any edge. The spirit of this
 * implementation is to use embeded sub-structures rather than pointers to
 * sub-structures, much in the spirit of the Linux Kernel List implementation.
 * Wether this will help in running time is hard to say, but at least now we
 * have two implementations (EGdGraph_t) one with embeded structures and one
 * with pointer to sub-structures.
 *
 * @version 0.0.1
 * @par History:
 * - 2005-06-15
 * 						- First Implementation.
 *
 * */
/** @file 
 * @ingroup EGeUgraph */
/** @addtogroup EGeUgraph */
/** @{ */
/** @example eg_eugraph.ex.c
 * This is a more detailed example on how to use this module */
/* ========================================================================= */

#ifndef _EG_E_UGRAPH
#define _EG_E_UGRAPH
#include "eg_config.h"
#include "eg_elist.h"

/* ========================================================================= */
/** @brief structure that hold all node related structures needed to define a
 * graph, and add/delete/modify it's structure */
typedef struct EGeUgraphNode_t
{
	EGeList_t edges;		/**< List of all edges with an endpoint in this node, if 
													 the edge is a loop, then it appears, two times in 
													 the adjacency list. */
	EGeList_t node_cn;	/**< List member of the list of all nodes in the graph, 
													 note that if this node is not (YET) in a graph, 
													 the contents are undefined */
	unsigned degree;		/**< Degree for the node, note that a loop edge is counted
													 twice, thus this is the number of members in the 
													 #EGeUgraphNode_t::edges list. */
}
EGeUgraphNode_t;

/* ========================================================================= */
/** @brief Structure for endpoints of edges. */
typedef struct EGeUgraphEP_t
{
	EGeList_t cn;					/**< List member of the edge list in the 
														 node for this edge. */
	EGeUgraphNode_t *node;/**< Link to the actual node of this end point */
	unsigned char type;		/**< either 0 or 1, zero for tail, one for head */
}
EGeUgraphEP_t;


/* ========================================================================= */
/** @brief structure that hold all edge related structures needed to define a
 * directed graph, and add/delete/modify it's structure */
typedef struct EGeUgraphEdge_t
{
	EGeUgraphEP_t ep[2];	/**< endpoints of the edge, by convention end point 
														 zero is the tail, and endpoint 1 is the head */
}
EGeUgraphEdge_t;

/* ========================================================================= */
/** @brief structure that holds all graph related structures needed to define
 * a directed graph, and to allow modifications over it. */
typedef struct EGeUgraph_t
{
	EGeList_t nodes;	/**< List head for all nodes in the graph */
	unsigned n_nodes;	/**< number of nodes in the graph */
	unsigned n_edges;	/**< number of edges in the graph */
}
EGeUgraph_t;

/* ========================================================================= */
/** @brief Initialize a graph structure as an empty graph with no members.
 * @param __Gpt pointer to the graph to be initialized */
#define EGeUgraphInit(__Gpt) ({\
	EGeUgraph_t*const __EGeDg_in_G = (__Gpt);\
	EGeListInit(&(__EGeDg_in_G->nodes));\
	__EGeDg_in_G->n_edges = __EGeDg_in_G->n_nodes = 0;})

/* ========================================================================= */
/** @brief Clear the structure so that we can free it (without memory leaks).
 * Note that this macro does nothing, because it is always safe to free this
 * structure */
#define EGeUgraphClear(__Gpt) ;

/* ========================================================================= */
/** @brief Initialize an edge as an empty edge, non attached to any graph.
 * @param __ept pointer to edge be initialized */
#define EGeUgraphEdgeInit(__ept) *(__ept) = (EGeUgraphEdge_t){{(EGeUgraphEP_t){(EGeList_t){0,0},0,0},(EGeUgraphEP_t){(EGeList_t){0,0},0,1}}};

/* ========================================================================= */
/** @brief Clear the structure so that we can free it (without memory leaks).
 * Note that this macro does nothing, because it is always safe to free this
 * structure */
#define EGeUgraphEdgeClear(__Gpt) ;

/* ========================================================================= */
/** @brief Initialize a node as an empty non-attached node.
 * @param __v pointer to node to be initialized */
#define EGeUgraphNodeInit(__v) ({\
	EGeUgraphNode_t*const __EGeDg_in_v = (__v);\
	EGeListInit(&(__EGeDg_in_v->edges));\
	__EGeDg_in_v->node_cn = (EGeList_t){0,0};\
	__EGeDg_in_v->degree = 0;})

/* ========================================================================= */
/** @brief Clear the structure so that we can free it (without memory leaks).
 * Note that this macro does nothing, because it is always safe to free this
 * structure */
#define EGeUgraphNodeClear(__Gpt) ;

/* ========================================================================= */
/** @brief Given the adjacency list connector of an edge, return the pointer
 * to the internal edge.
 * @param __edge_cn pointer to the edge connector as in the nodes adjacency
 * list.
 * @return pointer to the actual EGeUgraphEdge_t containing the connector. 
 * */
#define EGeUgraphGetAdjEdge(__edge_cn) ({\
	EGeUgraphEP_t*const __EGeUg_ep = EGcontainerOf(__edge_cn,EGeUgraphEP_t,cn);\
	EGcontainerOf(__EGeUg_ep,EGeUgraphEdge_t,ep[__EGeUg_ep->type]);})

/* ========================================================================= */
/** @brief Add a node to the graph.
 * @param __Gpt pointer to the graph to where we will add a node, it should be an
 * initialized graph structure (see EGeUgraphInit).
 * @param __v pointer to the node to be added to the graph. Note that we don't
 * check wether the node has valid information inside (you should call
 * EGeUgraphNodeInit(__v) for all nodes before adding them.
 * @return the number of nodes in the graph (including the recently added
 * node).
 * */
#define EGeUgraphAddNode(__Gpt,__v) ({\
	EGeUgraph_t*const __EGeDg_add_n_G = (__Gpt);\
	EGeUgraphNode_t*const __EGeDg_add_n_v = (__v);\
	EGeListAddAfter(&(__EGeDg_add_n_v->node_cn),&(__EGeDg_add_n_G->nodes));\
	__EGeDg_add_n_G->n_nodes++;})

/* ========================================================================= */
/** @brief Remove a node from a graph.
 * @param __v pointer to the node to be removed from the graph.
 * @param __Gpt pointer to the graph from where we will remove the node.
 * @return pointer to the removed node.
 * @note Note that the actual definition of removing a node from a graph is
 * not completelly well defined, since there might be some edges attached to
 * this node, and is not clear what to do in such a case. In this
 * implementation we chose to return an error if the degree of this node is
 * non-zero. */
#define EGeUgraphDelNode(__Gpt,__v) ({\
	EGeUgraph_t*const __EGeDg_del_n_G = (__Gpt);\
	EGeUgraphNode_t*const __EGeDg_del_n = (__v);\
	if(__EGeDg_del_n->degree) EXIT(1,"trying to remove node "#__v" with "\
		"incoming edges from graph "#__Gpt);\
	EGeListDel(&(__EGeDg_del_n->node_cn));\
	__EGeDg_del_n_G->n_nodes--;\
	__EGeDg_del_n;})

/* ========================================================================= */
/** @brief Add an edge to a graph.
 * @param __Gpt pointer to the graph.
 * @param __headpt pointer to the head node.
 * @param __tailpt pointer to the tail node.
 * @param __ept pointer to the edge.
 * */
#define EGeUgraphAddEdge(__Gpt,__headpt,__tailpt,__ept) ({\
	EGeUgraph_t*const __EGeDg_add_e_G = (__Gpt);\
	EGeUgraphNode_t*const __EGeDg_add_e_head = (__headpt);\
	EGeUgraphNode_t*const __EGeDg_add_e_tail = (__tailpt);\
	EGeUgraphEdge_t*const __EGeDg_add_e = (__ept);\
	EGeListAddAfter(&(__EGeDg_add_e->ep[1].cn),&(__EGeDg_add_e_head->edges));\
	EGeListAddAfter(&(__EGeDg_add_e->ep[0].cn),&(__EGeDg_add_e_tail->edges));\
	__EGeDg_add_e->ep[1].node = __EGeDg_add_e_head;\
	__EGeDg_add_e->ep[0].node = __EGeDg_add_e_tail;\
	__EGeDg_add_e_head->degree++;\
	__EGeDg_add_e_tail->degree++;\
	__EGeDg_add_e_G->n_edges++;})

/* ========================================================================= */
/** @brief Delete an edge from a graph.
 * @param __Gpt pointer to the graph.
 * @param __ept pointer to the edge.
 * @return pointer to the deleted edge.
 * @note Take notice that this function won't change the values stored in the
 * given edge '__ept', so if you access the internal information it may or may not
 * be still valid, (depending on what else has happen with the graph in the
 * meantime).
 * */
#define EGeUgraphDelEdge(__Gpt,__ept) ({\
	EGeUgraph_t*const __EGeDg_del_e_G = (__Gpt);\
	EGeUgraphEdge_t*const __EGeDg_del_e = (__ept);\
	EGeListDel(&(__EGeDg_del_e->ep[1].cn));\
	EGeListDel(&(__EGeDg_del_e->ep[0].cn));\
	__EGeDg_del_e->ep[1].node->degree--;\
	__EGeDg_del_e->ep[0].node->degree--;\
	__EGeDg_del_e_G->n_edges--;\
	__EGeDg_del_e;})

/* ========================================================================= */
/** @brief Change an endpoint of an edge.
 * @param __ept pointer to the edge whose tail will be changed.
 * @param __Gpt pointer to the graph.
 * @param __tailpt pointer to the new edge's tail.
 * @param __eptype number of the endpoint (either 0 or 1).
 * @return pointer to the new tail of the given edge.
 * @par Description:
 * Note that this function assumes that  that
 * both (__ept) and (__tailpt) bellong to an initialized graph. */
#define EGeUgraphChangeEP(__Gpt,__ept,__tailpt,__eptype) ({\
	EGeUgraphNode_t*const __EGeDg_chg_hd_tail = (__tailpt);\
	EGeUgraphEdge_t*const __EGeDg_chg_hd_e = (__ept);\
	EGeUgraphEP_t*const __EGeDg_ep = &(__EGeDg_chg_hd_e->ep[__eptype]);\
	__EGeDg_ep->node->degree--;\
	EGeListDel(&(__EGeDg_ep->cn));\
	EGeListAddAfter(&(__EGeDg_ep->cn),&(__EGeDg_chg_hd_tail->edges));\
	__EGeDg_chg_hd_tail->degree++;\
	__EGeDg_ep->node = __EGeDg_chg_hd_tail;})

/* ========================================================================= */
/** @brief Change the tail of an edge.
 * @param __ept pointer to the edge whose tail will be changed.
 * @param __Gpt pointer to the graph.
 * @param __tailpt pointer to the new edge's tail.
 * @return pointer to the new tail of the given edge.
 * @par Description:
 * Note that this function assumes that  that
 * both (__ept) and (__tailpt) bellong to an initialized graph. */
#define EGeUgraphChangeTail(__Gpt,__ept,__tailpt) EGeUgraphChangeEP(__Gpt,__ept,__tailpt,0)

/* ========================================================================= */
/** @brief Change the head of an edge.
 * @param __ept pointer to the edge whose head will be changed.
 * @param __headpt pointer to the new edge's head.
 * @param __Gpt pointer to the graph.
 * @return pointer to the new head of the given edge.
 * @par Description:
 * Note that this function assumes that  that
 * both (__ept) and (__headpt) bellong to an initialized graph. */
#define EGeUgraphChangeHead(__Gpt,__ept,__headpt) EGeUgraphChangeEP(__Gpt,__ept,__headpt,1)

/* ========================================================================= */
/** @} */
/* end of eg_edgraph.h */
#endif
