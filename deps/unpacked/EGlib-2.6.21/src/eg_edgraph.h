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
/** @defgroup EGeDgraph EGeDgraph
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
 * - 2005-05-23
 * 						- First Implementation.
 *
 * */
/** @file 
 * @ingroup EGeDgraph */
/** @addtogroup EGeDgraph */
/** @{ */
/** @example eg_edgraph.ex.c
 * This is a more detailed example on how to use this module */
/* ========================================================================= */

#ifndef _EG_E_DGRAPH_H
#define _EG_E_DGRAPH_H
#include "eg_config.h"
#include "eg_elist.h"

/* ========================================================================= */
/** @brief structure that hold all node related structures needed to define a
 * graph, and add/delete/modify it's structure */
typedef struct EGeDgraphNode_t
{
	EGeList_t in_edge;	/**< List head for a list of incomming edges */
	EGeList_t out_edge;	/**< List head for a list of outgoing edges */
	EGeList_t node_cn;	/**< List member of the list of all nodes in the graph, 
													 note that if this node is not (YET) in a graph, 
													 the contents are undefined */
	unsigned in_size;		/**< Number of incomming edges */
	unsigned out_size;	/**< Number of outgoing edges */
}
EGeDgraphNode_t;

/* ========================================================================= */
/** @brief structure that hold all edge related structures needed to define a
 * directed graph, and add/delete/modify it's structure */
typedef struct EGeDgraphEdge_t
{
	EGeList_t head_cn;		/**< List member of the incomming edge list in the 
														 head node for this edge. */
	EGeList_t tail_cn;		/**< List member of the outgoing edge list in the 
														 tail node for this edge. */
	EGeDgraphNode_t *head;/**< pointer to the head node for this edge */
	EGeDgraphNode_t *tail;/**< pointer to the tail node for this edge */
}
EGeDgraphEdge_t;

/* ========================================================================= */
/** @brief structure that holds all graph related structures needed to define
 * a directed graph, and to allow modifications over it. */
typedef struct 
{
	EGeList_t nodes;	/**< List head for all nodes in the graph */
	unsigned n_nodes;	/**< number of nodes in the graph */
	unsigned n_edges;	/**< number of edges in the graph */
}
EGeDgraph_t;

/* ========================================================================= */
/** @brief Initialize a graph structure as an empty graph with no members.
 * @param __G pointer to the graph to be initialized */
#define EGeDgraphInit(__G) ({\
	EGeDgraph_t*const __EGeDg_in_G = (__G);\
	EGeListInit(&(__EGeDg_in_G->nodes));\
	__EGeDg_in_G->n_edges = __EGeDg_in_G->n_nodes = 0;})

/* ========================================================================= */
/** @brief Reset the given graph pointer as an empty graph.
 * @param __G pointer to the graph to reset */
#define EGeDgraphReset(__G) EGeDgraphInit(__G)

/* ========================================================================= */
/** @brief Clear the structure so that we can free it (without memory leaks).
 * Note that this macro does nothing, because it is always safe to free this
 * structure */
#define EGeDgraphClear(__G)

/* ========================================================================= */
/** @brief Initialize an edge as an empty edge, non attached to any graph.
 * @param __e pointer to edge be initialized */
#define EGeDgraphEdgeInit(__e) ({\
	EGeDgraphEdge_t*const __EGeDg_in_e = (__e);\
	__EGeDg_in_e->head_cn = (EGeList_t){0,0};\
	__EGeDg_in_e->tail_cn = (EGeList_t){0,0};\
	__EGeDg_in_e->head = __EGeDg_in_e->tail = 0;})

/* ========================================================================= */
/** @brief Reset the given edge pointer as an edge not linked to a graph.
 * @param __e pointer to the edge to reset */
#define EGeDgraphEdgeReset(__e) EGeDgraphEdgeInit(__e)

/* ========================================================================= */
/** @brief Clear the structure so that we can free it (without memory leaks).
 * Note that this macro does nothing, because it is always safe to free this
 * structure */
#define EGeDgraphEdgeClear(__G) ;

/* ========================================================================= */
/** @brief Initialize a node as an empty non-attached node.
 * @param __v pointer to node to be initialized */
#define EGeDgraphNodeInit(__v) ({\
	EGeDgraphNode_t*const __EGeDg_in_v = (__v);\
	EGeListInit(&(__EGeDg_in_v->in_edge));\
	EGeListInit(&(__EGeDg_in_v->out_edge));\
	__EGeDg_in_v->node_cn = (EGeList_t){0,0};\
	__EGeDg_in_v->in_size = __EGeDg_in_v->out_size = 0;})

/* ========================================================================= */
/** @brief Reset the given node pointer as a node not linked to a graph.
 * @param __v pointer to the node to reset */
#define EGeDgraphNodeReset(__v) EGeDgraphNodeInit(__v)

/* ========================================================================= */
/** @brief Clear the structure so that we can free it (without memory leaks).
 * Note that this macro does nothing, because it is always safe to free this
 * structure */
#define EGeDgraphNodeClear(__G)

/* ========================================================================= */
/** @brief Add a node to the graph.
 * @param __G pointer to the graph to where we will add a node, it should be an
 * initialized graph structure (see EGeDgraphInit).
 * @param __v pointer to the node to be added to the graph. Note that we don't
 * check wether the node has valid information inside (you should call
 * EGeDgraphNodeInit(__v) for all nodes before adding them.
 * @return the number of nodes in the graph (including the recently added
 * node).
 * */
#define EGeDgraphAddNode(__G,__v) ({\
	EGeDgraph_t*const __EGeDg_add_n_G = (__G);\
	EGeDgraphNode_t*const __EGeDg_add_n_v = (__v);\
	EGeListAddAfter(&(__EGeDg_add_n_v->node_cn),&(__EGeDg_add_n_G->nodes));\
	__EGeDg_add_n_G->n_nodes++;})

/* ========================================================================= */
/** @brief Remove a node from a graph.
 * @param __v pointer to the node to be removed from the graph.
 * @param __G pointer to the graph from where we will remove the node.
 * @return pointer to the removed node.
 * @note Note that the actual definition of removing a node from a graph is
 * not completelly well defined, since there might be some edges attached to
 * this node, and is not clear what to do in such a case. In this
 * implementation we chose to return an error if the degree of this node is
 * non-zero. */
#define EGeDgraphDelNode(__G,__v) ({\
	EGeDgraph_t*const __EGeDg_del_n_G = (__G);\
	EGeDgraphNode_t*const __EGeDg_del_n = (__v);\
	if(__EGeDg_del_n->in_size) EXIT(1,"trying to remove node "#__v" with "\
		"incoming edges from graph "#__G);\
	if(__EGeDg_del_n->out_size) EXIT(1,"trying to remove node "#__v" with "\
		"outgoing edges from graph "#__G);\
	EGeListDel(&(__EGeDg_del_n->node_cn));\
	__EGeDg_del_n_G->n_nodes--;\
	__EGeDg_del_n;})

/* ========================================================================= */
/** @brief Add an edge to a graph.
 * @param __G pointer to the graph.
 * @param __hpt pointer to the head node.
 * @param __tpt pointer to the tail node.
 * @param __e pointer to the edge.
 * */
#define EGeDgraphAddEdge(__G,__hpt,__tpt,__e) ({\
	EGeDgraph_t*const __EGeDg_add_e_G = (__G);\
	EGeDgraphNode_t*const __EGeDg_add_e_head = (__hpt);\
	EGeDgraphNode_t*const __EGeDg_add_e_tail = (__tpt);\
	EGeDgraphEdge_t*const __EGeDg_add_e = (__e);\
	EGeListAddAfter(&(__EGeDg_add_e->head_cn),&(__EGeDg_add_e_head->in_edge));\
	EGeListAddAfter(&(__EGeDg_add_e->tail_cn),&(__EGeDg_add_e_tail->out_edge));\
	__EGeDg_add_e->head = __EGeDg_add_e_head;\
	__EGeDg_add_e->tail = __EGeDg_add_e_tail;\
	__EGeDg_add_e_head->in_size++;\
	__EGeDg_add_e_tail->out_size++;\
	__EGeDg_add_e_G->n_edges++;})

/* ========================================================================= */
/** @brief Delete an edge from a graph.
 * @param __G pointer to the graph.
 * @param __e pointer to the edge.
 * @return pointer to the deleted edge.
 * @note Take notice that this function won't change the values stored in the
 * given edge '__e', so if you access the internal information it may or may not
 * be still valid, (depending on what else has happen with the graph in the
 * meantime).
 * */
#define EGeDgraphDelEdge(__G,__e) ({\
	EGeDgraph_t*const __EGeDg_del_e_G = (__G);\
	EGeDgraphEdge_t*const __EGeDg_del_e = (__e);\
	EGeListDel(&(__EGeDg_del_e->head_cn));\
	EGeListDel(&(__EGeDg_del_e->tail_cn));\
	__EGeDg_del_e->head->in_size--;\
	__EGeDg_del_e->tail->out_size--;\
	__EGeDg_del_e_G->n_edges--;\
	__EGeDg_del_e;})

/* ========================================================================= */
/** @brief Change the tail of an edge.
 * @param __e pointer to the edge whose tail will be changed.
 * @param __G pointer to the graph.
 * @param __tpt pointer to the new edge's tail.
 * @return pointer to the new tail of the given edge.
 * @par Description:
 * Note that this function assumes that  that
 * both (__e) and (__tpt) bellong to an initialized graph. */
#define EGeDgraphChangeTail(__G,__e,__tpt) ({\
	EGeDgraphNode_t*const __EGeDg_chg_hd_tail = (__tpt);\
	EGeDgraphEdge_t*const __EGeDg_chg_hd_e = (__e);\
	__EGeDg_chg_hd_e->tail->out_size--;\
	EGeListDel(&(__EGeDg_chg_hd_e->tail_cn));\
	EGeListAddAfter(&(__EGeDg_chg_hd_e->tail_cn),&(__EGeDg_chg_hd_tail->out_edge));\
	__EGeDg_chg_hd_tail->out_size++;\
	__EGeDg_chg_hd_e->tail = __EGeDg_chg_hd_tail;})

/* ========================================================================= */
/** @brief Change the head of an edge.
 * @param __e pointer to the edge whose head will be changed.
 * @param __hpt pointer to the new edge's head.
 * @param __G pointer to the graph.
 * @return pointer to the new head of the given edge.
 * @par Description:
 * Note that this function assumes that  that
 * both (__e) and (__hpt) bellong to an initialized graph. */
#define EGeDgraphChangeHead(__G,__e,__hpt) ({\
	EGeDgraphNode_t*const __EGeDg_chg_hd_head = (__hpt);\
	EGeDgraphEdge_t*const __EGeDg_chg_hd_e = (__e);\
	__EGeDg_chg_hd_e->head->in_size--;\
	EGeListDel(&(__EGeDg_chg_hd_e->head_cn));\
	EGeListAddAfter(&(__EGeDg_chg_hd_e->head_cn),&(__EGeDg_chg_hd_head->in_edge));\
	__EGeDg_chg_hd_head->in_size++;\
	__EGeDg_chg_hd_e->head = __EGeDg_chg_hd_head;})

/* ========================================================================= */
/** @} */
/* end of eg_edgraph.h */
#endif
