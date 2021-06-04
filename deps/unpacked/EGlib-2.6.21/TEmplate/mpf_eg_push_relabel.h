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
/** @defgroup EGalgPushRelabel EGalgPushRelabel
 *
 * Here we implement the push-relabel algorithm as defined in the book "Network 
 * Flows" by Magnanti et. all, in chapter 6,7 and 8. Using the variant
 * "Highest-label preflow-push algorithm" (described on page 230) wich choose
 * the active node from wich to push from as the one with highest distance
 * label. This variant has running time @f$ \mathcal{O}(n^2\sqrt{m}) @f$ where
 * @a n is the number of nodes in the graph, and @a m the number of edges in it.
 * Note that the call to #mpf_EGalgPRminSTcut produces a maximum @b pre_flow, to 
 * obtain a flow you should call the #mpf_EGalgPRmaxSTflow that takes the graph 
 * produced by #mpf_EGalgPRminSTcut and convert the preflow into a real flow. We
 * also choose to use to register the number of nodes with distance labels
 * @f$k,\quad\forall k=1,\ldots,n@f$ where @a n is the number of nodes in the
 * network. This is done because whenever the number of nodes with distance
 * labels @a k is zero, then all nodes with distance labels above @a k can be
 * set to @a n (and thus be added to the partially computed cut-set). This is an
 * (inportant) empirical speed-up, but does not affect the worst case complexity
 * analysis.
 * @version 1.0.0
 * @par History:
 * - 2010-05-07
 * 						- Add implementation using eg_adgraph.h
 * - 2005-06-01
 * 						- Add globla relabeling heuristic.
 * - 2005-05-30
 * 						- Final test results on the TSP x-files, all should be right now.
 * - 2005-05-26
 * 						- First Implementation.
 * @note This algorithm is implemented the embedded structures approach. I will
 * give further details on what this implies.
 *
 * @note It is important to note that this algorithm (as implemented here) 
 * @b WILL @b FAIL if an edge has infinite capacities. To handle that case
 * either we must re-program it, or you can put capacities suficiently large on
 * them (for example 2 times the sum of all bounded capacities) for this
 * algorithm to work.
 *
 * @note This implementation does use global relabeling, namelly, the
 * strategy when once in a while (for example every @a n or @a m relabeling
 * operations) we recompute
 * the exact distance labels. The use of this heuristic (together with the gap
 * heuristic) have been reported to be the most successfull in practice (see "On
 * Implementing Push-Relabel Method For The Maximum FLow Problem" from Boris V.
 * Cherkassy and Andrew V. Goldberg.) and also in the test that we have
 * performed on the fractional solutions of TSP's instances from the TSPLIB set
 * of problems using CONCORDE.
 * */
/** @file 
 * @ingroup EGalgPushRelabel */
/** @addtogroup EGalgPushRelabel */
/** @{ */
/** @example mpf_eg_push_relabel.ex.c
 * This is a complete example for the min-cut max-flow problem using the
 * push/relabel implementation offered in EGalgPR. */
#ifndef mpf___EG_PUSH_RELABEL_H__
#define mpf___EG_PUSH_RELABEL_H__
#include "eg_config.h"
#include "eg_elist.h"
#include "eg_edgraph.h"
#include "eg_lpnum.h"
#include "eg_mem.h"
/* ========================================================================= */

/* ========================================================================= */
/** @brief Level of debugging in the code. */
#define mpf___PR_DEBUGL__ 100

/* ========================================================================= */
/** @brief Level of debugging in the code. */
#define mpf___PR_TEST_VERBOSE__ 100

/* ========================================================================= */
/** @brief Level of debugging in the code. */
#define mpf___PR_VERBOSE__ 100

/* ========================================================================= */
/** @brief Level of profiling in the code. */
#define mpf___PR_PROFILE__ 100

/* ========================================================================= */
/** @brief If profiling is enable (i.e. #mpf___PR_PROFILE__ <= DEBUG), print 
 * some profiling information of the min s-t cut used up to now, and reset 
 * all internal counters to zero, if profiling is not enabled, nothing 
 * happen. */
/** @{ */
void mpf_EGalgPRprofile(void);
/** @} */
/* ========================================================================= */
/** @brief If set to non-zero, use the global relabeling heuristic (to be called
 * every @a n number of relabel operations performed. if set to zero, it won't
 * use this heuristic. Note thought that it has been shown that this is a very
 * efficient heuristic to reduce the total running time, specially in the
 * #mpf_EGalgPRminSTcut function call. */
#define mpf_EG_PR_RELABEL 1

/* ========================================================================= */
/** @brief If #mpf_EG_PR_RELABEL is set to one, then this initeger controls how
 * often we perform the global relabeling heuristic (in multiples of number of
 * nodes), the default value is 1. */
#define mpf_EG_PR_RELABEL_FREC 1U

/* ========================================================================= */
/** @brief Node structure neede to run Push-Relabel algorithm on a network.
 * @note Notice that the directed graph part is embeded in this structure as
 * well. Note  that we could define internally space for  LVL_list, 
 * but for the sake of speed we include them in the node structure. */
typedef struct mpf_EGalgPRnode_t
{
	EGeDgraphNode_t v;		/**< Actual node structure to work with (EGeDgraph) */
	EGeList_t LVL_list;		/**< Used to store the BFS list used for the first 
														 computations of the exact label distances, and 
														 then to store this node in it's current level list 
														 (this is used to implement the Highest-Label 
														 variant of the Preflow-Push algorithm) */
	EGeList_t T_cut;			/**< Used to speed-up the 'hole' heuristic, it is seted
														 once we enter the algorithm, so their value is 
														 non-important outside the function (but it's 
														 contents will be lost once we enter 
														 #mpf_EGalgPRminSTcut). */
	unsigned int d;				/**< Exact label distance for this node. Note that 
														 nodes with distance lables @f$ \geq n @f$ (where 
														 @a n is the number of nodes in the graph) define 
														 the minimum @f$ s-t@f$ cut that we are looking 
														 for. */
	mpf_t e;					/**< Exess flow in the node. Note that in particular 
														 the excess on node @a t (once #mpf_EGalgPRminSTcut 
														 finish) correspond to the minimum cut value. */
}
mpf_EGalgPRnode_t;

/* ========================================================================= */
/** @brief Initialize a pointer to an mpf_EGalgPRnode_t structure */
#define mpf_EGalgPRnodeInit(node_pt) ({\
	mpf_EGalgPRnode_t*const __EGalgPR_in = (node_pt);\
	mpf_EGlpNumInitVar(__EGalgPR_in->e);\
	EGeDgraphNodeInit(&(__EGalgPR_in->v));})

/* ========================================================================= */
/** @brief Reset the given node pointer (as if it were new).
 * @param node_pt pointer to the node to reset.
 * 
 * This function set the node as an empty node not linked with any graph.
 * */
#define mpf_EGalgPRnodeReset(node_pt) EGeDgraphNodeReset(&((node_pt)->v))

/* ========================================================================= */
/** @brief clear a pointer to an mpf_EGalgPRnode_t structure, and let it ready to be
 * freed if necesary. */
#define mpf_EGalgPRnodeClear(node_pt) ({\
	mpf_EGlpNumClearVar((node_pt)->e);\
	EGeDgraphNodeClear(&((node_pt)->v));})

/* ========================================================================= */
/** @brief capacitated edge structure with forward/backward information. */
typedef struct mpf_EGalgPRse_t
{
	EGeDgraphEdge_t e;	/**< actual edge information. */
	mpf_t r;				/**< residual capacity of the edge. */
	mpf_t u;				/**< maximum capacity on the edge. */
	unsigned char type;	/**< type of edge (0 for forward and 1 for backward */
}
mpf_EGalgPRse_t;

/* ========================================================================= */
/** @brief Edge Structure needed to run Push-Relabel algorithm on a network.
 * @note Notice that the this edge actually has actually two capacited edge
  substructures, one for forward edges and one for backward edge, it is assumed
 * that fw.type == 0 and bw.type == 1. This is needed because the algorithm 
 * asumes that both
 * edges exists (althought one may have zero capacity). Moreover, while
 * computing the residual capacities we need to access both edges e_ij and e_ji
 * at the same time, thus our choice to represent both edges in just one
 * structure. We also assume that the lower bound on the flow of all edges is
 * zero. Note that we don't need to keep explicitly the flow on the edges,
 * because given the residual capacity and the capacity on the edge we have that
 * @f$ x_{ij} - x_{ji} = u_{ij} - r_{ij} @f$ and thus we can set @f$ x_{ij} =
 * (u_{ij}-r_{ij})_+ @f$ and @f$ x_{ji} = (r_{ij}-u_{ij})_+ @f$.
 * if we have computed the maximal flow. */
typedef struct mpf_EGalgPRedge_t
{
	mpf_EGalgPRse_t fw;		/**< forward edge, we assum that fw.type = 0 */
	mpf_EGalgPRse_t bw;		/**< backward edge, we assume that bw.type = 1 */
}
mpf_EGalgPRedge_t;

/* ========================================================================= */
/** @brief Initialize a pointer to an mpf_EGalgPRedge_t structure */
#define mpf_EGalgPRedgeInit(edge_pt) ({\
	mpf_EGalgPRedge_t*const __EGalgPR_ie = (edge_pt);\
	mpf_EGlpNumInitVar(__EGalgPR_ie->fw.r);\
	mpf_EGlpNumInitVar(__EGalgPR_ie->fw.u);\
	mpf_EGlpNumInitVar(__EGalgPR_ie->bw.r);\
	mpf_EGlpNumInitVar(__EGalgPR_ie->bw.u);\
	EGeDgraphEdgeInit(&(__EGalgPR_ie->fw.e));\
	EGeDgraphEdgeInit(&(__EGalgPR_ie->bw.e));\
	__EGalgPR_ie->bw.type = 1;\
	__EGalgPR_ie->fw.type = 0;})

/* ========================================================================= */
/** @brief Reset the given edge pointer (as if it were new).
 * @param edge_pt pointer to the node to reset.
 * 
 * This function set the edge as an empty edge not linked with any graph.
 * */
#define mpf_EGalgPRedgeReset(edge_pt) ({\
	mpf_EGalgPRedge_t*const __EGalgPR_ie = (edge_pt);\
	EGeDgraphEdgeReset(&(__EGalgPR_ie->fw.e));\
	EGeDgraphEdgeReset(&(__EGalgPR_ie->bw.e));\
	__EGalgPR_ie->bw.type = 1;\
	__EGalgPR_ie->fw.type = 0;})


/* ========================================================================= */
/** @brief clear a pointer to an mpf_EGalgPRedge_t structure, and let it ready to be
 * freed if necesary. */
#define mpf_EGalgPRedgeClear(edge_pt) ({\
	mpf_EGlpNumClearVar((edge_pt)->fw.r);\
	mpf_EGlpNumClearVar((edge_pt)->fw.u);\
	mpf_EGlpNumClearVar((edge_pt)->bw.r);\
	mpf_EGlpNumClearVar((edge_pt)->bw.u);\
	EGeDgraphEdgeClear(&((edge_pt)->fw.e));\
	EGeDgraphEdgeClear(&((edge_pt)->bw.e));})

/* ========================================================================= */
/** @brief Graph structure needed to run Push-Relabel algorithm (with highest
 * label node selection rule). */
typedef struct mpf_EGalgPRgraph_t
{
	EGeDgraph_t G;				/**< EGeDgraph structure holding the graph 
														 information. */
}
mpf_EGalgPRgraph_t;

/* ========================================================================= */
/** @brief Initialize a pointer to an mpf_EGalgPRgraph_t structure */
#define mpf_EGalgPRgraphInit(graph_pt) EGeDgraphInit(&((graph_pt)->G))

/* ========================================================================= */
/** @brief Reset the given graph pointer (as if it were new).
 * @param graph_pt pointer to the node to reset.
 * 
 * This function set the graph as an empty graph.
 * */
#define mpf_EGalgPRgraphReset(graph_pt) EGeDgraphReset(&((graph_pt)->G))

/* ========================================================================= */
/** @brief clear a pointer to an mpf_EGalgPRgraph_t structure, and let it ready 
 * to be freed if necesary. */
#define mpf_EGalgPRgraphClear(graph_pt) EGeDgraphClear(&((graph_pt)->G))

/* ========================================================================= */
/** @brief Compute a minimum @f$s-t@f$ cut.
 * @param s pointer to the mpf_source node.
 * @param t pointer to the mpf_EGalgPRnode_t* sink node in the network.
 * @param G pointer to the mpf_EGalgPRgraph_t* in wich we will work.
 * @return zero on success, non-zero otherwise.
 * @par Description:
 * When this funcion finish (successfully) all nodes with field
 * #mpf_EGalgPRnode_t::d bigger than or equal to @a n (the number of nodes in the
 * graph) bellong to the @a s cut, while nodes with value strictly less than @a
 * n will bellong to the @a t cut. The residual capacities imply a maximum
 * pre-flow in the network, to get an acutal maximum flow you should run 
 * #mpf_EGalgPRmaxSTflow function with imput the output graph of this function 
 * (for an example look at the file mpf_eg_push_relabel.ex.c ).
 * @note This implementation uses the @a gap and @a global @a relabeling
 * heuristics to speed-up the computations.
 * */
int mpf_EGalgPRminSTcut (mpf_EGalgPRgraph_t * const G,
										 mpf_EGalgPRnode_t * const s,
										 mpf_EGalgPRnode_t * const t);

/* ========================================================================= */
/** @brief Compute a maximum @f$s-t@f$ flow from the ouput produced by
 * EGalgPRminCur.
 * @param s pointer to the mpf_EGalgPRnode_t* mpf_source node in the network.
 * @param t pointer to the mpf_EGalgPRnode_t* sink node in the network.
 * @param G pointer to the mpf_EGalgPRgraph_t* in wich we will work.
 * @return zero on success, non-zero otherwise.
 * @par Description:
 * We assume that our input graph is the (unaltered) result of a 
 * call to #mpf_EGalgPRminSTcut. Also, note that while computing the actual 
 * max @a s-@a t flow, we don't need to look for @a gap in the array of 
 * distances.
 * Also note that once you call this function, the information in
 * #mpf_EGalgPRnode_t::d don't correspond any more to the cut as defined in
 * #mpf_EGalgPRminSTcut.
 * */
int mpf_EGalgPRmaxSTflow (mpf_EGalgPRgraph_t * const G,
											mpf_EGalgPRnode_t * const s,
											mpf_EGalgPRnode_t * const t);

/* ========================================================================= */
/** @brief Check if the given input graph (with it's residual capacities)
 * represent an optimal solution to the maximum @f$ s-t @f$ flow / minimum
 * capacity @f$ s-t @f$ cut.
 * @param s pointer to the mpf_EGalgPRnode_t* mpf_source node in the network.
 * @param t pointer to the mpf_EGalgPRnode_t* sink node in the network.
 * @param G pointer to the mpf_EGalgPRgraph_t* in wich we will work.
 * @param error worst error while checking for optimality conditions.
 * @return zero if all discrepancies are under the #mpf_epsLpNum threshold, 
 * otherwise, return the number of conditions that don't hold within that
 * threshold, and report in error the worst error found in any condition.
 * @note The input for this function should be the graph as returned by
 * #mpf_EGalgPRmaxSTflow .
 * */
int mpf_EGalgPRoptimalityTest (mpf_EGalgPRgraph_t * const G,
													 mpf_EGalgPRnode_t * const s,
													 mpf_EGalgPRnode_t * const t,
													 mpf_t * error);

/* ========================================================================= */
/* @} */
/* end of mpf_eg_push_relabel.h */
#endif
#endif
#endif
