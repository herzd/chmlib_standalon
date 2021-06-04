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
/** @defgroup EGsrkGraph EGsrkGraph
 * This is a group of functions, macros and types designed to work with
 * graphs that are shrinkable, meaning that we can take two nodes in the
 * (current) graph, and shrink them into a single node, and at the same time
 * collapse all edges that become loops and if two edges are parallel, keep
 * just one (but keep a reference to the collapsed edge). At the same time the
 * shrunken nodes keep a list to the nodes 'embeded' or 'shrunken' into the
 * given node. More details in the structure definition and in the example.
 * Note that this implementation only support undirected graphs with actual
 * weights on the edges, the weights must be of type EGlpNum_t, and their
 * values are updated during the shrinking procedure, so if anyone want to
 * have the original values omewere else, they will have to keep an extra copy
 * outside. Most of the ideas used in this implementation come from CONCORDE.
 * 
 * @version 0.0.1
 * @par History:
 * - 2005-06-01
 * 						- First Implementation.
 * */
/** @file
 * @ingroup EGsrkGraph */
/** @addtogroup EGsrkGraph */
/** @{ */
/** @example eg_shrink_graph.ex.c */
/* ========================================================================= */

#ifndef _EGshrinkGraph_h__
#define _EGshrinkGraph_h__
#include "eg_config.h"
#include "eg_elist.h"
#include "eg_eset.h"
#include "eg_eugraph.h"
#include "eg_lpnum.h"
#ifndef EG_SRK_DEBUG
/* ========================================================================= */
/** @brief debuigging level, the lower the more debugging is carried out */
#define EG_SRK_DEBUG 100
#endif

/* ========================================================================= */
/** @brief Edge structure for shrinkable graphs */
typedef struct EGsrkEdge_t
{
	EGeUgraphEdge_t edge;	/**< Actual edge structure for the graph */
	EGeList_t members;		/**< list of other edges shrunken within this edge */
	unsigned int mmb_sz;	/**< length of the members list (without including the 
														 edge itsself */
	EGlpNum_t weight;			/**< Weight for the edge */
}
EGsrkEdge_t;

/* ========================================================================= */
/** @brief Node structure for shrinkable graphs */
typedef struct EGsrkNode_t
{
	EGeUgraphNode_t node;	/**< actual node structure for the graph */
	EGeList_t members;		/**< list of other nodes shrunken with this node */
	unsigned int mmb_sz;	/**< length of the members list (without including the
														 node itself */
	EGes_t parent;				/**< If this node is the representant for its class, 
														 then this is a 'parent' node, otherwise, is a 
														 shrunken node */
	EGlpNum_t weight;			/**< Weight of the @f$\delta(n)@f$ edges for this node
														 (in the shrunken graph), this should be 
														 initialized by the user. */
	EGsrkEdge_t *hit;			/**< used for internal purposes, in particular, while 
														 merging two adjacency lists, this field is used 
														 to store the first edge touching this node, and 
														 then used to retrieve that information. When we 
														 call #EGsrkIdentifyNodes this field is assumed 
														 to be NULL */
}
EGsrkNode_t;

/* ========================================================================= */
/** @brief Graph structure for shrinkable graphs */
typedef struct EGsrkGraph_t
{
	EGeUgraph_t G;					/**< Actual graph structure. */
	unsigned n_onodes;			/**< Number of original nodes */
	unsigned n_oedges;			/**< Number of original edges */
}
EGsrkGraph_t;

/* ========================================================================= */
/** @brief Initialize an edge structure.
 * @param e_edge */
#define EGsrkEdgeInit(e_edge) ({\
	EGsrkEdge_t*const _EGsrkE = (e_edge);\
	EGeUgraphEdgeInit(&(_EGsrkE->edge));\
	EGeListInit(&(_EGsrkE->members));\
	_EGsrkE->mmb_sz = 0;\
	EGlpNumInitVar(_EGsrkE->weight);\
	EGlpNumZero(_EGsrkE->weight);})

/* ========================================================================= */
/** @brief Clear internal memory (not allocated by the user) of an edge
 * structure.
 * @param e_edge */
#define EGsrkEdgeClear(e_edge) ({\
	EGeUgraphEdgeClear(&((e_edge)->edge));\
	EGlpNumClearVar((e_edge)->weight);})

/* ========================================================================= */
/** @brief Initialize a graph structure 
 * @param graph graph to be initialized */
#define EGsrkGraphInit(graph) ({\
	EGsrkGraph_t*const _EGsrkG = (graph);\
	EGeUgraphInit(&(_EGsrkG->G));\
	_EGsrkG->n_onodes = _EGsrkG->n_oedges = 0;})

/* ========================================================================= */
/** @brief Clear internal memory (not allocated by the user) of a graph
 * structure.
 * @param graph */
#define EGsrkGraphClear(graph) EGeUgraphClear(&((graph)->G))

/* ========================================================================= */
/** @brief Initialize a node structure.
 * @param e_node node to be initialized */
#define EGsrkNodeInit(e_node) ({\
	EGsrkNode_t*const _EGsrkN = (e_node);\
	EGeUgraphNodeInit(&(_EGsrkN->node));\
	EGeListInit(&(_EGsrkN->members));\
	_EGsrkN->mmb_sz = 0;\
	_EGsrkN->hit = 0;\
	EGesInit(&(_EGsrkN->parent));\
	EGlpNumInitVar(_EGsrkN->weight);\
	EGlpNumZero(_EGsrkN->weight);})

/* ========================================================================= */
/** @brief Clear internal memory (not allocated by the user) of a node
 * structure.
 * @param e_node */
#define EGsrkNodeClear(e_node) ({\
	EGeUgraphNodeClear(&((e_node)->node));\
	EGlpNumClearVar((e_node)->weight);})

/* ========================================================================= */
/** @brief Add a #EGsrkNode_t node to a #EGsrkGraph_t graph.
 * @param graph graph were to add the node.
 * @param N node to add to the graph.
 * @return zero on success, non-zero otherwise.
 * */
#define EGsrkAddNode(graph,N) EGeUgraphAddNode(&((graph)->G),&((N)->node))

/* ========================================================================= */
/** @brief Add a #EGsrkEdge_t edge to a #EGsrkGraph_t graph.
 * @param lG graph were to add the edge.
 * @param head_pt head node of the edge.
 * @param tail_pt tail node of the edge.
 * @param E edge to be added with end-points head_pt and tail_pt.
 * Note that this function will update the accumulated weight of both
 * endpoints of the newly added edge according to the value stored in the
 * #EGsrkEdge_t::weight field.
 * */
#define EGsrkAddEdge(lG,head_pt,tail_pt,E) ({\
	EGsrkNode_t*const _EGsrkH = (head_pt);\
	EGsrkNode_t*const _EGsrkT = (tail_pt);\
	EGsrkEdge_t*const _EGsrkE = (E);\
	EGlpNumAddTo(_EGsrkH->weight,_EGsrkE->weight);\
	EGlpNumAddTo(_EGsrkT->weight,_EGsrkE->weight);\
	EGeUgraphAddEdge(&((lG)->G),&(_EGsrkH->node),&(_EGsrkT->node),&(_EGsrkE->edge));})

/* ========================================================================= */
/** @brief Given two nodes in the current shrunken graph, shrunk them into one 
 * node.
 * @param G pointer to the graph where we are working
 * @param base first node.
 * @param srkN second node.
 * @return pointer to the new representing node.
 * @note We assume that the field EGsrkNode_t::hit is identically NULL for all
 * nodes currently in the shrunken graph (including base and srkN). 
 * @note We allways assume that N1 will be the representing node.
 * @note Take note that this structure can't get back the pointer to the srkN
 * node, the user should take care of that if needed.
 * */
EGsrkNode_t *EGsrkIdentifyNodes (EGsrkGraph_t * const G,
																 EGsrkNode_t * const base,
																 EGsrkNode_t * const srkN);

/* ========================================================================= */
/** @} 
 * end of eg_shrink_graph.h */
#endif
