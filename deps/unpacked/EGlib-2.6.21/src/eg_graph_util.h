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
/** @defgroup EGgraphUtil EGgraphUtil
 * Some utility functions to transform among different format of graphs 
 * */
/** @file
 * @ingroup EGgraphUtil */
/** @addtogroup EGgraphUtil */
/** @{ */
#ifndef __EG_GRAPH_UTIL__
#define __EG_GRAPH_UTIL__
#include "eg_config.h"
#include "eg_io.h"
/* ========================================================================= */
/** @brief given a graph, defined as an array of edges, compute in_degree,
 * out_degree, in_edge, out_edge, and in_perm and out_perm of edges if those
 * array are provided.
 * @param n number of nodes
 * @param m number of edges
 * @param edges array of length 2m, where \f$e_{i}=(edge[2i],edge[2i+1])\f$ for
 * \f$i=0,\ldots,m-1\f$.
 * @param in_d if non-null, array of length n where we compute in-degree of
 * each node.
 * @param in_beg if non-null (and in_d is not null)
 * \f$in_beg[i]=\sum\limits_{j=0}^{i-1}in_d[j]\f$.
 * @param in_e if non-null (and in_d, in_beg not null), satisfy
 * in_d[in_beg[i]+j] store the tail of the jth incomming edge for node i, for
 * \f$j=0,\ldots,in_d[i]-1\f$. Note that the relation is for each edge,
 * regardless of repetitions, no special sort criteria can be assumed on these
 * edges.
 * @param in_perm if non-null (and in_d, in_beg, in_e not null) satisfy
 * (edge[2*i],edge[2*i+1]) = (in_e[in_perm[i]],edge[2*i+1])
 * @param ou_d if non-null, array of length n where we compute out-degree of
 * each node.
 * @param ou_beg if non-null (and ou_d is not null)
 * \f$ou_beg[i]=\sum\limits_{j=0}^{i-1}ou_d[j]\f$.
 * @param ou_e if non-null (and ou_d, ou_beg not null), satisfy
 * ou_d[ou_beg[i]+j] store the head of the jth outgoing edge for node i, for
 * \f$j=0,\ldots,ou_d[i]-1\f$. Note that the relation is for each edge,
 * regardless of repetitions, no special sort criteria can be assumed on these
 * edges.
 * @param ou_perm if non-null (and ou_d, ou_beg, ou_e not null) satisfy
 * (edge[2*i],edge[2*i+1]) = (edge[2*i],ou_e[ou_perm[i]])
 * @return zero on success 
 * */
int EGguDegree(
	int32_t const n,
	int32_t const m,
	int32_t const*const edges,
	int32_t*const in_d,
	int32_t*const in_beg,
	int32_t*const in_e,
	int32_t*const in_perm,
	int32_t*const ou_d,
	int32_t*const ou_beg,
	int32_t*const ou_e,
	int32_t*const ou_perm);

/* ========================================================================= */
#endif
/** @} */
