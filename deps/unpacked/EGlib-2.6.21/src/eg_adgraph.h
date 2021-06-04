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
/** @defgroup EGaDgraph EGaDgraph
 * Here we define a basic directed graph structure, it holds the number of
 * nodes and edges in the graph, as well as the in and out degree of all
 * nodes, and allow to access the head and tail of any edge. The spirit of this
 * implementation is to use array nested sub-structures rather than pointers
 * to sub-structures, more suited for static data structures, and also more
 * light in memory usage.
 *
 * @version 0.0.1
 * @par History:
 * - 2010-05-04
 * 						- First Implementation.
 *
 * */
/** @file 
 * @ingroup EGaDgraph */
/** @addtogroup EGaDgraph */
/** @{ */
/** @example eg_adgraph.ex.c
 * This is a more detailed example on how to use this module */
/* ========================================================================= */

#ifndef _EG_A_DGRAPH_H
#define _EG_A_DGRAPH_H
#include "eg_config.h"

/* ========================================================================= */
/** @brief structure that hold all node related structures needed to define a
 * graph structure */
typedef struct
{
	uint32_t in_sz;		/**< @brief number of incomming edges */
	uint32_t in_beg;	/**< @brief beginning of index array of incomming edges */
	uint32_t out_sz;	/**< @brief number of outgoing edges */
	uint32_t out_beg;	/**< @brief beginning of index array of outgoing edges */
}
EGaDN_t;

/* ========================================================================= */
/** @brief structure that hold all edge related structures needed to define a
 * directed graph structure */
typedef struct 
{
	uint32_t head;
	uint32_t tail;
}
EGaDE_t;

/* ========================================================================= */
/** @brief structure that holds all graph related structures needed to define
 * a directed graph. */
typedef struct 
{
	uint32_t n_nodes;	/**< @brief number of nodes in the graph */
	uint32_t n_edges;	/**< @brief number of edges in the graph */
	char*all_nodes;		/**< @brief memory containing all nodes in the graph */
	char*all_edges;		/**< @brief memory containing all edges in the graph */
	uint32_t*all_out;	/**< @brief array of length n_edges containing the index
																of all outgoing edges for every node */
	uint32_t*all_in;	/**< @brief array of length n_edges containing the index
																of all outgoing edges for every node */
	size_t szof_node;	/**< @brief the idea is that all nodes are allocated 
																together in an array once, and all references
																are done by index number in this array. In this
																case szof_node = sizeof(node_structure) */
	size_t szof_edge;	/**< @brief the idea is that all edges are allocated
																together in an array once, and all references
																are done by index number in this array. In this
																case szof_node = sizeof(node_structure) */
}
EGaDG_t;

/* ========================================================================= */
/** @brief macro helpers to acces nodes and edges by reference, note that all
 * structures built on top of the directed graph must have the base
 * data-structure at the beginning
 * @param __nodeid number of the node 
 * @param __graph_st graph structure in use 
 * @return pointer to the corresponding node */
#define EGaDGgetNode(__nodeid,__graph_st) ({\
	const EGaDG_t*const __EGaDG = (const EGaDG_t*const)(__graph_st);\
	((EGaDN_t*)(__EGaDG->all_nodes+(__EGaDG->szof_node*((size_t)(__nodeid)))));})

/* ========================================================================= */
/** @brief macro helpers to acces nodes and edges by reference, note that all
 * structures built on top of the directed graph must have the base
 * data-structure at the beginning 
 * @param __edgeid number of the edge 
 * @param __graph_st graph structure in use 
 * @return pointer to the corresponding edge */
#define EGaDGgetEdge(__edgeid,__graph_st) ({\
	const EGaDG_t*const __EGaDG = (const EGaDG_t*const)(__graph_st);\
	((EGaDE_t*)(__EGaDG->all_edges+(__EGaDG->szof_edge*((size_t)(__edgeid)))));})

/* ========================================================================= */
/** @brief macro to get ID of i-th out edge, we assume that underlying
 * structure starts with the node/graph structure */
#define EGaDGgetOutE(__id,__node_st,__graph_st) (((const EGaDG_t*const)(__graph_st))->all_out[((const EGaDN_t*const)(__node_st))->out_beg+(__id)])

/* ========================================================================= */
/** @brief macro to get ID of i-th in edge, we assume that underlying
 * structure starts with the node/graph structure */
#define EGaDGgetInE(__id,__node_st,__graph_st) (((const EGaDG_t*const)(__graph_st))->all_in[((const EGaDN_t*const)(__node_st))->in_beg+(__id)])

/* ========================================================================= */
/** @brief number of incoming edges */
#define EGaDGgetNin(__node_st) (((const EGaDN_t*const)(__node_st))->in_sz)

/* ========================================================================= */
/** @brief number of outgoing edges */
#define EGaDGgetNout(__node_st) (((const EGaDN_t*const)(__node_st))->out_sz)

/* ========================================================================= */
/** @brief number of nodes */
#define EGaDGgetNnodes(__graph_st) (((const EGaDG_t*const)(__graph_st))->n_nodes)

/* ========================================================================= */
/** @brief number of edges */
#define EGaDGgetNedges(__graph_st) (((const EGaDG_t*const)(__graph_st))->n_edges)

/* ========================================================================= */
/** @brief Initialize a graph structure as an empty graph with no members.
 * @param G pointer to the graph to be initialized.
 * @param n number of nodes
 * @param m number of edges
 * @param szof_node sizeof node structure
 * @param szof_edge sizeof edge structure
 * @param all_nodes array of nodes
 * @param all_edges array of edges
 * @note we assume that the array of edges has the correct head/tails in the
 * structure, and from there, the initalization function computes in/out edges
 * and sub-structures for all nodes, and any internal allocation is performed
 * here.
 * */
void EGaDGInit(
		EGaDG_t*const G, 
		const uint32_t n,
		const uint32_t m,
		const size_t szof_node,
		const size_t szof_edge,
		char*const all_nodes,
		char*const all_edges);
/* ========================================================================= */
/** @brief clear any internal memory allocated in Init */
#define EGaDGClear(__G) {\
	EGaDG_t*const __ADG = (EGaDG_t*const)(__G);\
	EGfree(__ADG->all_out);\
	EGfree(__ADG->all_in);\
	memset(__ADG,0,sizeof(EGaDG_t));}
/* ========================================================================= */
/** @} */
/* end of eg_adgraph.h */
#endif
