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
/** @defgroup EGaUgraph EGaUgraph
 * Here we define a basic undirected graph structure, it holds the number of
 * nodes and edges in the graph, as well as the degree of all
 * nodes, assumes that no edge is multiple, and they have a given (undefined)
 * weight type.
 * The spirit of this
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
 * @ingroup EGaUgraph */
/** @addtogroup EGaUgraph */
/** @{ */
/** @example eg_augraph.ex.c
 * This is a more detailed example on how to use this module */
/* ========================================================================= */

#ifndef _EG_A_UGRAPH_H
#define _EG_A_UGRAPH_H
#include "eg_config.h"
#include "eg_macros.h"

/* ========================================================================= */
/** @brief structure that hold all node information */
typedef struct{
	uint32_t deg;	/**< @brief degree of the node */
	uint32_t beg;	/**< @brief where all neighbours begin, both for edges and for actual node neighbours */
	} EGaUN_t;
/* ========================================================================= */
/** @brief structure that holds all edge information */
typedef struct{
	uint32_t u,v;	/**< @brief end points of the edge */
	} EGaUE_t;
/* ========================================================================= */
/** @brief structure that holds the graph information (but the actual arrays
 * of nodes) */
typedef struct{
	uint32_t*all_neigh;	/**< @brief array storing all neighbours for every 
														node, length 2*nedges */
	uint32_t*all_edge;	/**< @brief array storing all edges touching every 
														node, length 2*nedges */
	uint32_t nnodes;
	uint32_t nedges;
	} EGaUG_t;
/* ========================================================================= */
/** @} */
/* end of eg_augraph.h */
#endif
