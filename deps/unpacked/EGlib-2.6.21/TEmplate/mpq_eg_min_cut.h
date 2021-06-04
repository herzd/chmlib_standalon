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
/** @defgroup EGalgMinCut EGalgMinCut
 *
 * Here we implement the min-cut algorithm based on the srinking
 * pre-processing of Padberg And Rinaldi in the paper "An Efficient 
 * Algorithm For The Minimum Capacity Cut Problem", Mathematical Programming
 * 47 (1990) pages 19-36. But using as minimum s-t cut code the Push-Relabel
 * max flow algorithm as implemented in the @ref EGalgPushRelabel module. This 
 * implies that we only support positive edge-weights.
 *
 * This implementation allows uses of diferent numbers as supported by
 * @ref EGlpNum module. And follows the philosophy of embeded structures as in
 * @ref EGalgPushRelabel module. Also, much of the approach used in this 
 * implementation come from CONCORDE's implementation.
 *
 * It is usually the case that the Minimum Cut Problem is just a sub-problem
 * of some larger problem, is for that reason that we implement (just as in
 * CONCORDE) a callback function that is called whenever an improving solution
 * is found, so that the user can do something with the given node-cutset and
 * value. for more details see the definition of #mpq_EGalgMCcbk_t .
 *
 * @note 
 * If run with types like EGfp20_t, if the arithmetic produces an overflow,
 * then we are in big trouble, note that the numbers involved in the algorithm
 * may range up to \f$\sum(w_e:e\in E(G))\f$.
 *
 * @version 0.0.1
 * @par History:
 * - 2005-08-19
 * 						- While computing a minimum S-T cut, choose S randomly. and T
 * 							as a node at maximum distance (number of edges) from S.
 * 						- Fix small problem with shrinking level 4
 * - 2005-06-20
 * 						- First Implementation.
 * */
/** @file
 * @ingroup EGalgMinCut */
/** @addtogroup EGalgMinCut */
/** @{ */
/** @example mpq_eg_min_cut.ex.c */
/* ========================================================================= */
#ifndef mpq__EG_MIN_CUT_H
#define mpq__EG_MIN_CUT_H
#include "eg_config.h"
#include "eg_elist.h"
#include "eg_eset.h"
#include "eg_edgraph.h"
#include "eg_eugraph.h"
#include "eg_lpnum.h"
#include "eg_mem.h"
#include "mpq_eg_push_relabel.h"
#include "mpq_eg_shrink_graph.h"
/* ========================================================================= */
/** @brief Verbosity Level */
#define mpq___MC_VRBLVL_ 100

/* ========================================================================= */
/** @brief Level of profiling in the code. */
#define mpq___MC_DEBUG_ 100

/* ========================================================================= */
/** @brief Level of profiling in the code. */
#define mpq___MC_PROFILE_ 0

/* ========================================================================= */
/** If profiling is enable (i.e. #mpq___MC_PROFILE_ <= DEBUG), print 
 * some profiling information of the min cut used up to now, and reset 
 * all internal counters to zero, if profiling is not enabled, nothing 
 * happen. */
/** @{ */
void mpq_EGalgMCprofile(void);
/** @} */

/* ========================================================================= */
/** @brief Call-back function, it receives as input the weight of the cut, the
 * size of the newly found cut, an array containing the cut (of length at
 * least the number of elements in the cut) as integers (as defined by the 
 * #mpq_EGalgMCnode_t::id field), and a pointer to some internal
 * data (as stored in #mpq_EGalgMCcbk_t::param). The function should return zero
 * on success, and non-zero if an error ocours, this error will be propagated
 * through the calling functions. */
typedef int (*mpq_EGalgMCdo_f) (mpq_t,
														const unsigned int,
														const unsigned int *const,
														void *);

/* ========================================================================= */
/** @brief Call-back structure for use when an improving minimum cut is found.
 * */
typedef struct mpq_EGalgMCcbk_t
{
	mpq_t cutoff;	/**< maximum value for the newly found minimum cut, for 
												 the function to be called. */
	void *param;			/**< external parameter needed by the function */
	mpq_EGalgMCdo_f do_fn;/**< actual function to be called if the cut-off condition 
												 holds */
}
mpq_EGalgMCcbk_t;

/* ========================================================================= */
/** @brief Initialize a call-back structure.
 * @param cb call-back to be initialized. */
#define mpq_EGalgMCcbkInit(cb) ({\
	mpq_EGalgMCcbk_t*const _EGalgMCcb = (cb);\
	mpq_EGlpNumInitVar(_EGalgMCcb->cutoff);\
	_EGalgMCcb->param = 0;\
	_EGalgMCcb->do_fn = 0;})

/* ========================================================================= */
/** @brief Free all internal memory asociated with this structure (not
 * allocated by the user).
 * @param cb call-back strucure to be cleared */
#define mpq_EGalgMCcbkClear(cb) mpq_EGlpNumClearVar((cb)->cutoff)

/* ========================================================================= */
/** @brief Node structure for Minimum Cut */
typedef struct mpq_EGalgMCnode_t
{
	mpq_EGsrkNode_t node;		/**< Actual shrinkable node */
	unsigned int id;		/**< External Identifier for the node */
	EGeList_t lvl_cn;		/**< Connector for the level list */
	unsigned int mpq_lvl;		/**< Current node level test to be performed */
	unsigned int new_id;/**< internal data, it's values can be discarded */
	mpq_EGsrkEdge_t *hit;		/**< Used to speed-up the Padberg-Rinaldi tests. */
}
mpq_EGalgMCnode_t;

/* ========================================================================= */
/** @brief Initialize a node structure for use.
 * @param N node to be initialized */
#define mpq_EGalgMCnodeInit(N) ({\
	mpq_EGalgMCnode_t*const _EGalgMCn = (N);\
	mpq_EGsrkNodeInit(&(_EGalgMCn->node));\
	_EGalgMCn->lvl_cn = (EGeList_t){0,0};\
	_EGalgMCn->mpq_lvl = 0;\
	_EGalgMCn->id = UINT_MAX;\
	_EGalgMCn->new_id = UINT_MAX;\
	_EGalgMCn->hit = 0;})

/* ========================================================================= */
/** @brief Clear any internal memory (not allocated by the user) used by this
 * structure.
 * @param N node to be cleared */
#define mpq_EGalgMCnodeClear(N) mpq_EGsrkNodeClear(&((N)->node))

/* ========================================================================= */
/** @brief Edge structure for the Minimum Cut */
typedef struct mpq_EGalgMCedge_t
{
	mpq_EGsrkEdge_t edge;	/**< Actual shrinkable edge */
	unsigned int id;	/**< External Identifier for the edge */
}
mpq_EGalgMCedge_t;

/* ========================================================================= */
/** @brief Initialize an edge structure for use.
 * @param E edge to be initialized */
#define mpq_EGalgMCedgeInit(E) ({\
	mpq_EGalgMCedge_t*const _EGalgMCe = (E);\
	mpq_EGsrkEdgeInit(&(_EGalgMCe->edge));\
	_EGalgMCe->id = UINT_MAX;})

/* ========================================================================= */
/** @brief Clear any internal memory (not allocated by the user) used by this
 * structure.
 * @param E node to be cleared */
#define mpq_EGalgMCedgeClear(E) mpq_EGsrkEdgeClear(&((E)->edge))

/* ========================================================================= */
/** @brief Graph Structure for Minimum Cut.
 *
 * Note that this structure also holds some parameters as the epsilon to use
  in the comparisons, the current best cut found (or bound), and the current
 * cut found so-far. As well as an array containing all edges and nodes in
 * thee graph (remember that when we Identify two nodes, we loose any
 * reference to the shrinked node in the graph structure as discussed in
 * #mpq_EGsrkIdentifyNodes ) 
 * */
typedef struct mpq_EGalgMCgraph_t
{
	mpq_EGsrkGraph_t G;						/**< Actual shrinking graph used */
	mpq_t epsilon;				/**< error tolerance used for equality testing */
	mpq_t cut_val;				/**< if #mpq_EGalgMCgraph_t::cut_sz is not zero, then 
																 this is the value of the (currenlty) best 
																 minimum cut found so far. otherwise is a bound 
																 on the value of the minimum cut (note that this
																 value should be set before actually computing 
																 the minimum cut, and can be set to the value 
																 of @f$\delta(v)@f$ for some node @a v in the 
																 graph. */
	unsigned int cut_sz;			/**< number of nodes in the current best cut, if 
																 set to zero, then no cut has been found 
																 (so far) */
	EGeList_t lvl_list[5];		/**< List of nodes in different levels of tests */
	unsigned int *cut;				/**< Array  storing the current cut, the size of 
																 this array should be at least 
																 #mpq_EGsrkGraph_t::n_onodes */
	mpq_EGalgMCnode_t *all_nodes;	/**< Array containing all nodes of the graph. */
	mpq_EGalgMCedge_t *all_edges;	/**< Array containing all edges of the graph. */
}
mpq_EGalgMCgraph_t;

/* ========================================================================= */
/** @brief Initialize a graph structure for use.
 * @param Graph graph to be initialized */
#define mpq_EGalgMCgraphInit(Graph) ({\
	mpq_EGalgMCgraph_t*const _EGalgMCg = (Graph);\
	mpq_EGsrkGraphInit(&(_EGalgMCg->G));\
	mpq_EGlpNumInitVar(_EGalgMCg->epsilon);\
	mpq_EGlpNumZero(_EGalgMCg->epsilon);\
	mpq_EGlpNumInitVar(_EGalgMCg->cut_val);\
	mpq_EGlpNumZero(_EGalgMCg->cut_val);\
	_EGalgMCg->cut_sz = 0;\
	EGeListInit(_EGalgMCg->lvl_list);\
	EGeListInit(_EGalgMCg->lvl_list+1);\
	EGeListInit(_EGalgMCg->lvl_list+2);\
	EGeListInit(_EGalgMCg->lvl_list+3);\
	EGeListInit(_EGalgMCg->lvl_list+4);\
	_EGalgMCg->cut = 0;\
	_EGalgMCg->all_nodes = 0;\
	_EGalgMCg->all_edges = 0;})

/* ========================================================================= */
/** @brief Clear internal memory (not allocated by the user) of a graph
 * structure.
 * @param Graph graph to be cleared. */
#define mpq_EGalgMCgraphClear(Graph) ({\
	mpq_EGsrkGraphClear(&((Graph)->G));\
	mpq_EGlpNumClearVar((Graph)->epsilon);\
	mpq_EGlpNumClearVar((Graph)->cut_val);})

/* ========================================================================= */
/** @brief Shrink two nodes in the graph, and update internal structures.
 * @param Graph current graph.
 * @param N node to keep in graph.
 * @param M node to shrink within N. */
#define mpq_EGalgMCidentifyNodes(Graph,N,M) ({\
	mpq_EGalgMCgraph_t*const _EGalgMCg = (Graph);\
	mpq_EGalgMCnode_t*const _EGalgMCn = (N), *const _EGalgMCm = (M);\
	MESSAGE(mpq___MC_DEBUG_,"Shrinking nodes with weight %lf %lf", \
					mpq_EGlpNumToLf(_EGalgMCn->node.weight), \
					mpq_EGlpNumToLf(_EGalgMCm->node.weight));\
	mpq_EGsrkIdentifyNodes(&(_EGalgMCg->G), &(_EGalgMCn->node), &(_EGalgMCm->node));\
	if(_EGalgMCn->mpq_lvl < 5)\
	{\
		EGeListDel(&(_EGalgMCm->lvl_cn));\
		EGeListMoveAfter(&(_EGalgMCn->lvl_cn), _EGalgMCg->lvl_list);\
	}\
	else EGeListAddAfter(&(_EGalgMCn->lvl_cn), _EGalgMCg->lvl_list);\
	_EGalgMCn->mpq_lvl = 0;})

/* ========================================================================= */
/** @brief Identify all Padberg and Rinaldy edges. i.e. shrink all edges that
 * satisfy the conditions in their paper. we choose to make tests over pair of
 * nodes linked by an edge. 
 * @param max_lvl set a limit on wich tests to perform. for example, if set to
 * 1, only the first and second tests will be carried out.
 * @param G graph over wich we are working. 
 * @param cb call back structure to use (if set to NULL it is not used).
 * @return zero on success, non-zero otherwise. 
 *
 * Note that while doing this identification process, we update the values of
 * #mpq_EGalgMCgraph_t::cut, #mpq_EGalgMCgraph_t::cut_sz and #mpq_EGalgMCgraph_t::cut_val,
 * as well as performing the actual shrinking procedure.
 *
 * The original theorem (for local conditions on shrinking) is the following:
 * Let @f$ Z @f$ be a proper subset of @f$ V @f$ (the set of all nodes in the
 * graph), @f$ |Z|\geq2 @f$, and let 
 * @f[ P(Z) = \bigcup\left\{ N(u)\cap N(v):u\neq v, u,v\in Z \right\} @f]
 * where @f$ N(u) @f$ if the set of neighbours of @f$ u @f$. If there exists
 * @f$ Y\subseteq P(Z) @f$ such that for every nonempty proper subset @f$ W
 * @f$ of @f$ Z @f$ and for every @f$ T\subseteq Y @f$ either:
 * -# @f$ w(\delta(W))/2 \leq w(W:(Y-T)+(Z-W)) @f$ or
 * -# @f$ w(\delta(Z-W))/2 \leq w(Z-W:T+W) @f$.
 * Then there exists a minimum cut @f$(X:V-X)@f$ such that either @f$
 * Z\subseteq X @f$ or @f$ X\subseteq Z @f$.
 *
 * And the original theorem (in fact is the corollary 3.5 in the paper) 
 * regarding global conditions for shrinking is the following:
 * Let @f$ u\neq v\in V @f$, and let @f$ q @f$ be an upper bound on the
 * minimum cut value, and @f$ lb_{uv} @f$ be a lower bound in the value of a
 * minimum @f$ u-v @f$ cut, then if @f$ lb_{uv}\geq q @f$ the set 
 * @f$ \{u,v\} @f$ is shrinkable.
 * 
 * The actual tests that we perform (for every edge) are the following:
 * -# If @f$ w(\delta(u)) < @f$ #mpq_EGalgMCgraph_t::cut_val, update the minimum
 * cut value and set.
 * -# If @f$ w_{uv} \geq \min\{w(\delta(u)),w(\delta(v))\}/2 @f$ then we can 
 * safely shrink edge @f$ uv @f$.
 * -# If we have a triangle @f$ uv,\quad vw,\quad wu  @f$, with 
 * @f$ w_{uv} + w_{vw} \geq w(\delta(v))/2 @f$ and  
 * @f$ w_{uw} + w_{vw} \geq w(\delta(w))/2 @f$ then we can safely shrink edge
 * @f$ wv @f$.
 * -# Compute lower bound on the cut that separates the endpoints of the
 * current edge as :
 * @f[ lb_{uv}=w_{uv}+\sum\limits_{w\in N(u)\cap N(v)}\min\{w_{uw},w_{vw}\} @f]
 * If  @f$ lb_{uv} \geq  @f$ #mpq_EGalgMCgraph_t::cut_val , then we can shrink the edge  @f$ uv @f$.
 * -# Consider the edge @f$ uv @f$ and two common neighbours @f$ x,y @f$. If  
 * @f$ w_{ux} + w_{uy} + w_{uv} \geq w(\delta(u))/2 @f$ and 
 * @f$ w_{vx} + w_{vy} + w_{vu} \geq w(\delta(v))/2 @f$ and at least one of 
 * @f$ w_{uv} + w_{uy} \geq w(\delta(u))/2 @f$ and 
 * @f$ w_{uv} + w_{vx} \geq w(\delta(v))/2 @f$ and at least one of 
 * @f$ w_{uv} + w_{ux} \geq w(\delta(u))/2 @f$ and 
 * @f$ w_{uv} + w_{vy} \geq w(\delta(v))/2 @f$ then we can safely shrink edge
 * @f$ uv @f$.
 *
 * We make thiese tests in order, i.e. first we perform all level 1 tests,
 * then level2, and so on, and whenever two nodes are Identify (shrinked) we
 * set the level of the node to 1 (i.e. in the next test we will test the
 * first condition). This is done using an array of (5) lists, where all nodes
 * are distributed. Originally all nodes should be in the first lists (i.e.
 * all nodes should be tested to improve the current best cut by themselves).
 * */
int mpq_EGalgMCidentifyPRedges (mpq_EGalgMCgraph_t * const G,
														mpq_EGalgMCcbk_t * const cb,
														const unsigned int max_lvl);

/* ========================================================================= */
/** @brief Compute a minimum cut on the given graph. 
 * @param max_lvl set a limit on wich tests to perform during the
 * Padberg-Rinaldy shrinking step. for example, if set to
 * 1, only the first and second tests will be carried out.
 * @param G graph over wich we are working. 
 * @param cb call back structure to use (if set to NULL it is not used).
 * @return zero on success, non-zero otherwise. 
 *
 * This function takes as input a graph, and perform the minimum cut algorithm
 * as described in the paper "An Efficient 
 * Algorithm For The Minimum Capacity Cut Problem", Mathematical Programming
 * 47 (1990) pages 19-36. 
 *
 * Note that the graph should have all fields properly initialized.
 * */
int mpq_EGalgMC (mpq_EGalgMCgraph_t * const G,
						 mpq_EGalgMCcbk_t * const cb,
						 const unsigned int max_lvl);

/* ========================================================================= */
/** @} 
 * end mpq_eg_min_cut.h */
#endif
#endif
#endif
