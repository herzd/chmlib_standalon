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
/** @file 
 * @ingroup EGalgPushRelabel */
/** @addtogroup EGalgPushRelabel */
/** @{ */
/** @example eg_apr.ex.c
 * This is a complete example for the min-cut max-flow problem using the
 * push/relabel implementation offered in EGalgAPR. */
#ifndef __EG_APR_H__
#define __EG_APR_H__
#include "eg_config.h"
#include "eg_elist.h"
#include "eg_adgraph.h"
#include "eg_lpnum.h"
#include "eg_mem.h"
/* ========================================================================= */

/* ========================================================================= */
/** @brief Level of debugging in the code. */
#define __APR_DEBUGL__ 100

/* ========================================================================= */
/** @brief Level of debugging in the code. */
#define __APR_TEST_VERBOSE__ 100

/* ========================================================================= */
/** @brief Level of debugging in the code. */
#define __APR_VERBOSE__ 100

/* ========================================================================= */
/** @brief Level of profiling in the code. */
#define __APR_PROFILE__ 100

/* ========================================================================= */
/** @brief If profiling is enable (i.e. #__APR_PROFILE__ <= DEBUG), print 
 * some profiling information of the min s-t cut used up to now, and reset 
 * all internal counters to zero, if profiling is not enabled, nothing 
 * happen. */
/** @{ */
void EGalgAPRprofile(void);
/** @} */
/* ========================================================================= */
/** @brief If set to non-zero, use the global relabeling heuristic (to be called
 * every @a n number of relabel operations performed. if set to zero, it won't
 * use this heuristic. Note thought that it has been shown that this is a very
 * efficient heuristic to reduce the total running time, specially in the
 * #EGalgAPRminSTcut function call. */
#define EG_APR_RELABEL 1

/* ========================================================================= */
/** @brief If #EG_APR_RELABEL is set to one, then this integer controls how
 * often we perform the global relabeling heuristic (in multiples of number of
 * nodes), the default value is 1. */
#define EG_APR_RELABEL_FREC 1U

/* ========================================================================= */
/** @brief Node structure neede to run Push-Relabel algorithm on a network.
 * @note Notice that the directed graph part is assumed to exist
 * somewhere-else.
 * embeded in this structure as
 * well. Note  that we could define internally space for  LVL_list, 
 * but for the sake of speed we include them in the node structure. */
typedef struct
{
	uint32_t LVL_list;		/**< @brief Used to store the BFS list used for the first 
														 computations of the exact label distances, and 
														 then to store this node in it's current level list 
														 (this is used to implement the Highest-Label 
														 variant of the Preflow-Push algorithm) */
	uint32_t T_cut;				/**< @brief Used to speed-up the 'hole' heuristic, it is seted
														 once we enter the algorithm, so their value is 
														 non-important outside the function (but it's 
														 contents will be lost once we enter 
														 #EGalgAPRminSTcut). */
	uint32_t d;						/**< @brief Exact label distance for this node. Note that 
														 nodes with distance lables @f$ \geq n @f$ (where 
														 @a n is the number of nodes in the graph) define 
														 the minimum @f$ s-t@f$ cut that we are looking 
														 for. */
	EGlpNum_t e;					/**< @brief Exess flow in the node. Note that in particular 
														 the excess on node @a t (once #EGalgAPRminSTcut 
														 finish) correspond to the minimum cut value. */
}
EGalgAPRN_t;

/* ========================================================================= */
/** @brief Initialize a pointer to an EGalgAPRN_t structure */
#define EGalgAPRNInit(node_pt) ({\
	EGalgAPRN_t*const __EGalgAPR_in = (node_pt);\
	EGlpNumInitVar(__EGalgAPR_in->e);\
	EGeDgraphNodeInit(&(__EGalgAPR_in->v));})

/* ========================================================================= */
/** @brief Reset the given node pointer (as if it were new).
 * @param node_pt pointer to the node to reset.
 * 
 * This function set the node as an empty node not linked with any graph.
 * */
#define EGalgAPRnodeReset(node_pt) EGeDgraphNodeReset(&((node_pt)->v))

/* ========================================================================= */
/** @brief clear a pointer to an EGalgAPRN_t structure, and let it ready to be
 * freed if necesary. */
#define EGalgAPRnodeClear(node_pt) ({\
	EGlpNumClearVar((node_pt)->e);\
	EGeDgraphNodeClear(&((node_pt)->v));})

/* ========================================================================= */
/** @brief capacitated edge structure with forward/backward information. */
typedef struct EGalgAPRse_t
{
	EGeDgraphEdge_t e;	/**< actual edge information. */
	EGlpNum_t r;				/**< residual capacity of the edge. */
	EGlpNum_t u;				/**< maximum capacity on the edge. */
	unsigned char type;	/**< type of edge (0 for forward and 1 for backward */
}
EGalgAPRse_t;

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
typedef struct EGalgAPRedge_t
{
	EGalgAPRse_t fw;		/**< forward edge, we assum that fw.type = 0 */
	EGalgAPRse_t bw;		/**< backward edge, we assume that bw.type = 1 */
}
EGalgAPRedge_t;

/* ========================================================================= */
/** @brief Initialize a pointer to an EGalgAPRedge_t structure */
#define EGalgAPRedgeInit(edge_pt) ({\
	EGalgAPRedge_t*const __EGalgAPR_ie = (edge_pt);\
	EGlpNumInitVar(__EGalgAPR_ie->fw.r);\
	EGlpNumInitVar(__EGalgAPR_ie->fw.u);\
	EGlpNumInitVar(__EGalgAPR_ie->bw.r);\
	EGlpNumInitVar(__EGalgAPR_ie->bw.u);\
	EGeDgraphEdgeInit(&(__EGalgAPR_ie->fw.e));\
	EGeDgraphEdgeInit(&(__EGalgAPR_ie->bw.e));\
	__EGalgAPR_ie->bw.type = 1;\
	__EGalgAPR_ie->fw.type = 0;})

/* ========================================================================= */
/** @brief Reset the given edge pointer (as if it were new).
 * @param edge_pt pointer to the node to reset.
 * 
 * This function set the edge as an empty edge not linked with any graph.
 * */
#define EGalgAPRedgeReset(edge_pt) ({\
	EGalgAPRedge_t*const __EGalgAPR_ie = (edge_pt);\
	EGeDgraphEdgeReset(&(__EGalgAPR_ie->fw.e));\
	EGeDgraphEdgeReset(&(__EGalgAPR_ie->bw.e));\
	__EGalgAPR_ie->bw.type = 1;\
	__EGalgAPR_ie->fw.type = 0;})


/* ========================================================================= */
/** @brief clear a pointer to an EGalgAPRedge_t structure, and let it ready to be
 * freed if necesary. */
#define EGalgAPRedgeClear(edge_pt) ({\
	EGlpNumClearVar((edge_pt)->fw.r);\
	EGlpNumClearVar((edge_pt)->fw.u);\
	EGlpNumClearVar((edge_pt)->bw.r);\
	EGlpNumClearVar((edge_pt)->bw.u);\
	EGeDgraphEdgeClear(&((edge_pt)->fw.e));\
	EGeDgraphEdgeClear(&((edge_pt)->bw.e));})

/* ========================================================================= */
/** @brief Graph structure needed to run Push-Relabel algorithm (with highest
 * label node selection rule). */
typedef struct EGalgAPRG_t
{
	EGaDG_t G;				/**< structure holding the graph information. */
	uint32_t*
}
EGalgAPRG_t;

/* ========================================================================= */
/** @brief Initialize a pointer to an EGalgAPRG_t structure */
#define EGalgAPRgraphInit(graph_pt) EGeDgraphInit(&((graph_pt)->G))

/* ========================================================================= */
/** @brief Reset the given graph pointer (as if it were new).
 * @param graph_pt pointer to the node to reset.
 * 
 * This function set the graph as an empty graph.
 * */
#define EGalgAPRgraphReset(graph_pt) EGeDgraphReset(&((graph_pt)->G))

/* ========================================================================= */
/** @brief clear a pointer to an EGalgAPRG_t structure, and let it ready 
 * to be freed if necesary. */
#define EGalgAPRgraphClear(graph_pt) EGeDgraphClear(&((graph_pt)->G))

/* ========================================================================= */
/** @brief Compute a minimum @f$s-t@f$ cut.
 * @param s pointer to the source node.
 * @param t pointer to the EGalgAPRN_t* sink node in the network.
 * @param G pointer to the EGalgAPRG_t* in wich we will work.
 * @return zero on success, non-zero otherwise.
 * @par Description:
 * When this funcion finish (successfully) all nodes with field
 * #EGalgAPRN_t::d bigger than or equal to @a n (the number of nodes in the
 * graph) bellong to the @a s cut, while nodes with value strictly less than @a
 * n will bellong to the @a t cut. The residual capacities imply a maximum
 * pre-flow in the network, to get an acutal maximum flow you should run 
 * #EGalgAPRmaxSTflow function with imput the output graph of this function 
 * (for an example look at the file eg_push_relabel.ex.c ).
 * @note This implementation uses the @a gap and @a global @a relabeling
 * heuristics to speed-up the computations.
 * */
int EGalgAPRminSTcut (EGalgAPRG_t * const G,
										 EGalgAPRN_t * const s,
										 EGalgAPRN_t * const t);

/* ========================================================================= */
/** @brief Compute a maximum @f$s-t@f$ flow from the ouput produced by
 * EGalgAPRminCur.
 * @param s pointer to the EGalgAPRN_t* source node in the network.
 * @param t pointer to the EGalgAPRN_t* sink node in the network.
 * @param G pointer to the EGalgAPRG_t* in wich we will work.
 * @return zero on success, non-zero otherwise.
 * @par Description:
 * We assume that our input graph is the (unaltered) result of a 
 * call to #EGalgAPRminSTcut. Also, note that while computing the actual 
 * max @a s-@a t flow, we don't need to look for @a gap in the array of 
 * distances.
 * Also note that once yoou call this function, the information in
 * #EGalgAPRN_t::d don't correspond any more to the cut as defined in
 * #EGalgAPRminSTcut.
 * */
int EGalgAPRmaxSTflow (EGalgAPRG_t * const G,
											EGalgAPRN_t * const s,
											EGalgAPRN_t * const t);

/* ========================================================================= */
/** @brief Check if the given input graph (with it's residual capacities)
 * represent an optimal solution to the maximum @f$ s-t @f$ flow / minimum
 * capacity @f$ s-t @f$ cut.
 * @param s pointer to the EGalgAPRN_t* source node in the network.
 * @param t pointer to the EGalgAPRN_t* sink node in the network.
 * @param G pointer to the EGalgAPRG_t* in wich we will work.
 * @param error worst error while checking for optimality conditions.
 * @return zero if all discrepancies are under the #epsLpNum threshold, 
 * otherwise, return the number of conditions that don't hold within that
 * threshold, and report in error the worst error found in any condition.
 * @note The input for this function should be the graph as returned by
 * #EGalgAPRmaxSTflow .
 * */
int EGalgAPRoptimalityTest (EGalgAPRG_t * const G,
													 EGalgAPRN_t * const s,
													 EGalgAPRN_t * const t,
													 EGlpNum_t * error);

/* ========================================================================= */
/* @} */
/* end of eg_push_relabel.h */
#endif
