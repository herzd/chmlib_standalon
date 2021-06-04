#include "eg_config.h"
#ifdef HAVE_LIBGMP
#if HAVE_LIBGMP
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
/** @defgroup EGalgDijkstra EGalgDijkstra
 * Dijkstra implementation with 2-heaps
 * */
/** @file
 * @ingroup EGalgDijkstra */
/** @addtogroup EGalgDijkstra */
/** @{ */
/** @example mpf_eg_dijkstra.ex.c */
/* ========================================================================= */
#ifndef mpf___EG_DIJKSTRA_H
#define mpf___EG_DIJKSTRA_H
#include "eg_config.h"
#include "eg_edgraph.h"
#include "mpf_eg_eheap.h"
#include "eg_lpnum.h"
#include "eg_mem.h"
#include "eg_io.h"
/* ========================================================================= */
/** @brief Verbosity Level */
#define mpf_DJK_VRBLVL_ 100

/* ========================================================================= */
/** @brief Level of profiling in the code. */
#define mpf_DJK_DEBUG_ 100

/* ========================================================================= */
/** @brief implement dijkstra.
 * */
int mpf_EGalgDJK (
		int32_t const nnodes,
		int32_t const nedges,
		int32_t const*const ou_d,
		int32_t const*const ou_beg,
		int32_t const*const ou_e,
		mpf_t const*const weight,
		int32_t const s,
		int32_t const nt,
		int32_t const*const t,
		int32_t*const father,
		mpf_t*const dist
		);

/* ========================================================================= */
/** @brief read from a file in .x format (as in the concorde's .x files), the
 * format is as follows:
 * nnodes nedges
 * tail_1 head_1 weight_1
 * ...    ...    ....
 * tail_nnodes head_nnodes weight_nnodes
 * comments are allowed, if they start with % or #.
 * @param input file containing the data
 * @param n where to store the number of nodes
 * @param m where to store the number of edges
 * @param edges pointer to an array where to store the edges, note that memory
 * will be allocated in (*edges)
 * @param weight pointer to the array where we will store the weights, note
 * that this array will be allocated using mpf_EGlpNumAllocArray, and should be
 * deallocated using mpf_EGlpNumFreeArray.
 * @return zero on success, non-zero otherwise 
 * */
int mpf_EGguReadXgraph(
		EGioFile_t*const input,
		int32_t*const n,
		int32_t*const m,
		int32_t**const edges,
		mpf_t**const weight);
/* ========================================================================= */
/** @} 
 * end mpf_eg_dijkstra.h */
#endif

#endif
#endif
