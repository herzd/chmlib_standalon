/* algorithms commons in many optimization algorithms.
 *
 * Copyright (C) 2010 Daniel Espinoza
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
/** @defgroup EGalgMaxClosure Maximum Closure
 *
 * Here we provide a simple interface to use different methods to compute
 * maximum closure in a given graph. The (acyclic) graph is given by the edges,
 * a number of nodes, and values in the nodes (positive and negative).
 * @version 1.0.0
 * @par History:
 * - First implementation using push-relabel algorithm
 * */
/** @file 
 * @ingroup EGalgMaxClosure */
/** @addtogroup EGalgMaxClosure */
/** @{ */
/** @example eg_maxclosure.ex.c */
/* ========================================================================= */
#ifndef __EG_MAXCLOSURE_H__
#define __EG_MAXCLOSURE_H__
#include "eg_adgraph.h"
/* ========================================================================= */
/** @name Available algorithms to compute maximum closure */
/* @{ */
/** @brief default algorithm */
#define EG_ALG_MAXCLOSURE_DEFAULT 0
/** @brief Push-relabel implementation */
#define EG_ALG_MAXCLOSURE_PR 1
/* @} */
/* ========================================================================= */
/** @brief main interface function.
 * @param algo ID of the algorithm to use.
 * @param n number of nodes in graph
 * @param m number of edges in graph
 * @param edges array of directed edges to use, its length is @f$2m@f$ and
 * position (2k,2k+1) stores the head and tail of the k-th edge.
 * @param weight array of length n with the benefit of each node in the graph
 * @param closure_sz pointer to an integer where we store size of the resulting
 * closure.
 * @param closure array of length (at least) n, where we store the elements in the
 * maximum closure.
 * @param display display level, zero for none
 * @return zero on success, non-zero otherwise */
int EGalgMaxClosure(
		const int algo,
		const int n,
		const int m,
		const int*const edges,
		const double*const weight,
		int*const closure_sz,
		int*const closure,
		const int display);
/* ========================================================================= */
/** @brief version using array-based static graphs 
 * @param algo ID of the algorithm to use.
 * @param weight array of length n with the benefit of each node in the graph
 * @param closure_sz pointer to an integer where we store size of the resulting
 * closure.
 * @param closure array of length (at least) n, where we store the elements in the
 * maximum closure.
 * @param display display level, zero for none
 * @return zero on success, non-zero otherwise */
int EGalgAMaxClosure(
		const int algo,
		const EGaDG_t*const G,
		const double*const weight,
		int*const closure_sz,
		int*const closure,
		const int display);
/* ========================================================================= */
/** @} */
#endif
