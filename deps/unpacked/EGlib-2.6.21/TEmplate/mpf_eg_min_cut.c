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
/** @file 
 * @ingroup EGalgMinCut */
/** @addtogroup EGalgMinCut */
/** @{ */
/* ========================================================================= */
#include "mpf_eg_min_cut.h"
#include "eg_random.h"
/* ========================================================================= */
/** @brief variables and macros to do the profiling for the algorithm, it
 * include counting the impact of all shrinking steps, the number of
 * improvements cuts found along the way, and how many times where the main
 * functions called. */
/** @{ */
static unsigned long long mpf___MC_profile_lvl[5] = { 0, 0, 0, 0, 0 };
																										 /**< shrinkings per level*/
static unsigned long long mpf___MC_profile_tn = 0;/**<Number of calls to #mpf_EGalgMCtestNode*/
static unsigned long long mpf___MC_profile_up = 0;/**< Number of improving cuts found */
void mpf_EGalgMCprofile(void)
{
	if(mpf___MC_PROFILE_ <= DEBUG)
	{
		fprintf(stderr,"PROFILE FOR EGalgMinCut:\nSrinking:\n\tLVL 1: %llu\n\tLVL"
				" 2: %llu\n\tLVL 3: %llu\n\tLVL 4: %llu\ns-t Cuts: %llu\nTestNode: "
				"%llu\nN Cuts: %llu\n", mpf___MC_profile_lvl[1], mpf___MC_profile_lvl[2], 
				mpf___MC_profile_lvl[3], mpf___MC_profile_lvl[4], mpf___MC_profile_lvl[0], 
				mpf___MC_profile_tn, mpf___MC_profile_up); mpf___MC_profile_lvl[0] = 
			mpf___MC_profile_lvl[1] = mpf___MC_profile_lvl[2] = mpf___MC_profile_lvl[3] = 
			mpf___MC_profile_lvl[4] = mpf___MC_profile_tn = mpf___MC_profile_up = 0;}
}
#if mpf___MC_PROFILE_ <= DEBUG
#define mpf_UPDATE(counter) (counter)++
#else
#define mpf_UPDATE(counter)
#endif
/** @} */
/* ========================================================================= */
/** @brief Expand all nodes shrinked within a node, and store their id-s
 * (#mpf_EGalgMCnode_t::id) in the given array (including the node itself ).
 * @param cut array where to store the cut (its size should be at least
 * #mpf_EGsrkNode_t::mmb_sz + 1).
 * @param N node to expand.
 * @return zero on success, non-zero otherwise.
 * */
static inline int mpf_EGalgMCexpandNode (unsigned int *const cut,
																		 mpf_EGalgMCnode_t * const N)
{
	register unsigned int i = N->node.mmb_sz;
	EGeList_t *const n_end = &(N->node.members);
	EGeList_t *n_it = n_end->next;
	mpf_EGalgMCnode_t *M;
	cut[i] = N->id;
	for (; n_it != n_end; n_it = n_it->next)
	{
		M = EGcontainerOf (n_it, mpf_EGalgMCnode_t, node.members);
		cut[--i] = M->id;
	}
	return 0;
}

/* ========================================================================= */
/** @brief Given a node, test if the node is a better cut (by itself) then the
 * currently found, and if so, update the current best cut, and call the
 * callback if it is non-NULL.
 * @param G current min-cut graph.
 * @param cb callback structure.
 * @param N node to test.
 * @return zero on success, non-zero otherwise. 
 * */
static inline int mpf_EGalgMCtestNode (mpf_EGalgMCgraph_t * const G,
																	 mpf_EGalgMCcbk_t * const cb,
																	 mpf_EGalgMCnode_t * const N)
{
	int rval = 0;
	mpf_UPDATE (mpf___MC_profile_tn);
	TESTL (mpf___MC_DEBUG_, mpf_EGlpNumIsLess (N->node.weight, mpf_zeroLpNum),
				 "node %u with " "negative weight %lf", N->id,
				 mpf_EGlpNumToLf (N->node.weight));
	if (mpf_EGlpNumIsLess (N->node.weight, G->cut_val))
	{
		mpf_UPDATE (mpf___MC_profile_up);
		MESSAGE (mpf___MC_VRBLVL_, "Update cut_val from %lf to %lf, n_nodes %u"
						 " n_edges %u", mpf_EGlpNumToLf (G->cut_val),
						 mpf_EGlpNumToLf (N->node.weight), G->G.G.n_nodes, G->G.G.n_edges);
		mpf_EGlpNumCopy (G->cut_val, N->node.weight);
		G->cut_sz = N->node.mmb_sz + 1;
		rval = mpf_EGalgMCexpandNode (G->cut, N);
		CHECKRVALG (rval, CLEANUP);
		if (cb && mpf_EGlpNumIsLess (N->node.weight, cb->cutoff))
		{
			rval = cb->do_fn (G->cut_val, G->cut_sz, G->cut, cb->param);
			CHECKRVALG (rval, CLEANUP);
		}
	}
	else if (cb && mpf_EGlpNumIsLess (N->node.weight, cb->cutoff))
	{
		unsigned int *cut = EGsMalloc (unsigned int, N->node.mmb_sz + 1);
		rval = mpf_EGalgMCexpandNode (cut, N);
		CHECKRVALG (rval, CLEANUP);
		rval = cb->do_fn (N->node.weight, N->node.mmb_sz + 1, cut, cb->param);
		CHECKRVALG (rval, CLEANUP);
		EGfree (cut);
	}
	/* ending */
CLEANUP:
	return rval;
}

/* ========================================================================= */
/** @brief Set all #mpf_EGalgMCnode_t::hit fields of all neighbours of the given
 * node.
 * @param NODE node whose neighbours we are going to hit.
 *
 * Note that we assume tha there exist variables (not in use within the scope
 * of this call) as e_it, e_end, lep, o_type, M and E.
 * */
#define mpf__EGalgMChitNeighbours(NODE)	{\
	e_end = &((NODE)->node.node.edges);\
	for( e_it = e_end->next; e_it != e_end; e_it = e_it->next)\
		{\
			lep = EGcontainerOf(e_it, EGeUgraphEP_t,cn);\
			o_type = lep->type ? 0U : 1U;\
			E = EGcontainerOf(lep, mpf_EGalgMCedge_t, edge.edge.ep[lep->type]);\
			M = EGcontainerOf(E->edge.edge.ep[o_type].node, mpf_EGalgMCnode_t, node.node);\
			M->hit = &(E->edge);\
		}}

/* ========================================================================= */
/** @brief Set all #mpf_EGalgMCnode_t::hit fields of all neighbours of the given
 * node.
 * @param NODE node whose neighbours we are going to hit.
 *
 * Note that we assume tha there exist variables (not in use within the scope
 * of this call) as e_it, e_end, lep, o_type, M and E.
 * */
#define mpf__EGalgMCunhitNeighbours(NODE)	{\
	e_end = &((NODE)->node.node.edges);\
	for( e_it = e_end->next; e_it != e_end; e_it = e_it->next)\
		{\
			lep = EGcontainerOf(e_it, EGeUgraphEP_t,cn);\
			o_type = lep->type ? 0U : 1U;\
			E = EGcontainerOf(lep, mpf_EGalgMCedge_t, edge.edge.ep[lep->type]);\
			M = EGcontainerOf(E->edge.edge.ep[o_type].node, mpf_EGalgMCnode_t, node.node);\
			M->hit = 0;\
		}}

/* ========================================================================= */
int mpf_EGalgMCidentifyPRedges (mpf_EGalgMCgraph_t * const G,
														mpf_EGalgMCcbk_t * const cb,
														const unsigned int max_lvl)
{
	/* local variables */
	int rval = 0;
	EGeList_t *e_it,
	 *e_end,
	 *e2_it,
	 *e3_it,
	 *e2_end,
	 *c_lvl;
	unsigned int o_type,
	  o_type2,
	  o_type3;
	mpf_EGalgMCnode_t *N,
	 *M,
	 *X,
	 *Y;
	mpf_EGalgMCedge_t *E,
	 *F,
	 *H;
	EGeUgraphEP_t *lep,
	 *lep2,
	 *lep3;
	/* N_dv store the delta of N over two minus G->epsilon, and the same is true 
	 * for M_dv, but for node M, the N_dv2 and M_dv2 are used to speed-up the
	 * second PR test. */
	mpf_t N_dv2,
	  M_dv2,
	  N_dv,
	  M_dv;
	mpf_EGlpNumInitVar (N_dv2);
	mpf_EGlpNumInitVar (M_dv2);
	mpf_EGlpNumInitVar (N_dv);
	mpf_EGlpNumInitVar (M_dv);
REDO:
	/* ======================================================================= */
	/* we start doing test on list level 0 */
	MESSAGE (mpf___MC_VRBLVL_, "PR shrinking level 0, nodes %u edges %u",
					 G->G.G.n_nodes, G->G.G.n_edges);
	c_lvl = G->lvl_list;
	while (!EGeListIsEmpty (c_lvl) && mpf_EGlpNumIsLess (mpf_zeroLpNum, G->cut_val))
	{
		N = EGcontainerOf (c_lvl->next, mpf_EGalgMCnode_t, lvl_cn);
		rval = mpf_EGalgMCtestNode (G, cb, N);
		CHECKRVALG (rval, CLEANUP);
		N->mpf_lvl = 1;
		EGeListMoveAfter (&(N->lvl_cn), c_lvl + 1);
	}
	/* ======================================================================= */
	/* now we start doing the first level test */
	if (max_lvl < 1 || G->G.G.n_nodes < 3 ||
			mpf_EGlpNumIsEqual (mpf_zeroLpNum, G->cut_val, G->epsilon))
		goto CLEANUP;
	MESSAGE (mpf___MC_VRBLVL_, "PR shrinking level 1, nodes %u edges %u",
					 G->G.G.n_nodes, G->G.G.n_edges);
	c_lvl++;
	while (!EGeListIsEmpty (c_lvl) && G->G.G.n_nodes > 2)
	{
		N = EGcontainerOf (c_lvl->next, mpf_EGalgMCnode_t, lvl_cn);
		TESTL (mpf___MC_DEBUG_, mpf_EGlpNumIsLess (N->node.weight, mpf_zeroLpNum),
					 "node %u with " "negative weight %lf", N->id,
					 mpf_EGlpNumToLf (N->node.weight));
		mpf_EGlpNumCopy (N_dv, N->node.weight);
		mpf_EGlpNumDivUiTo (N_dv, 2);
		mpf_EGlpNumSubTo (N_dv, G->epsilon);
		e_end = &(N->node.node.edges);
		for (e_it = e_end->next; e_it != e_end; e_it = e_it->next)
		{
			lep = EGcontainerOf (e_it, EGeUgraphEP_t, cn);
			o_type = lep->type ? 0U : 1U;
			E = EGcontainerOf (lep, mpf_EGalgMCedge_t, edge.edge.ep[lep->type]);
			M =
				EGcontainerOf (E->edge.edge.ep[o_type].node, mpf_EGalgMCnode_t, node.node);
			/* if this node is done processing (first condition) or if it is waiting
			 * a higher value test level, we should skip it */
			if (M->mpf_lvl > 1)
				continue;
			/* otherwise we should make the test */
			TESTL (mpf___MC_DEBUG_, mpf_EGlpNumIsLess (M->node.weight, mpf_zeroLpNum),
						 "node %u with " "negative weight %lf", M->id,
						 mpf_EGlpNumToLf (M->node.weight));
			mpf_EGlpNumCopy (M_dv, M->node.weight);
			mpf_EGlpNumDivUiTo (M_dv, 2);
			mpf_EGlpNumSubTo (M_dv, G->epsilon);
			/* check if edge E is shrinkable, if so, shrink these two nodes, and
			 * update the new node level, and restart the testing phase */
			TESTL (mpf___MC_DEBUG_, mpf_EGlpNumIsLess (E->edge.weight, mpf_zeroLpNum),
						 "edge %u with " "negative weight %lf", E->id,
						 mpf_EGlpNumToLf (E->edge.weight));
			if (mpf_EGlpNumIsLeq (N_dv, E->edge.weight)
					|| mpf_EGlpNumIsLeq (M_dv, E->edge.weight))
			{
				mpf_EGalgMCidentifyNodes (G, N, M);
				mpf_UPDATE (mpf___MC_profile_lvl[1]);
				TESTL (mpf___MC_DEBUG_, mpf_EGlpNumIsLess (N->node.weight, mpf_zeroLpNum),
							 "node %u" " with negative weight %lf", N->id,
							 mpf_EGlpNumToLf (N->node.weight));
				goto REDO;
			}
		}
		/* move node to next level */
		N->mpf_lvl = 2;
		EGeListMoveAfter (&(N->lvl_cn), c_lvl + 1);
	}
	/* ======================================================================= */
	/* now we start performing test level two */
	if (max_lvl < 2 || G->G.G.n_nodes < 3)
		goto CLEANUP;
	MESSAGE (mpf___MC_VRBLVL_, "PR shrinking level 2, nodes %u edges %u",
					 G->G.G.n_nodes, G->G.G.n_edges);
	c_lvl++;
	while (!EGeListIsEmpty (c_lvl) && G->G.G.n_nodes > 2)
	{
		N = EGcontainerOf (c_lvl->next, mpf_EGalgMCnode_t, lvl_cn);
		TESTL (mpf___MC_DEBUG_, mpf_EGlpNumIsLess (N->node.weight, mpf_zeroLpNum),
					 "node %u with " "negative weight %lf", N->id,
					 mpf_EGlpNumToLf (N->node.weight));
		mpf_EGlpNumCopy (N_dv, N->node.weight);
		mpf_EGlpNumDivUiTo (N_dv, 2);
		mpf_EGlpNumSubTo (N_dv, G->epsilon);
		e_end = &(N->node.node.edges);
		/* first hit all relevant neighbours of N */
		mpf__EGalgMChitNeighbours (N);
		/* and now, for all neighbour M of N, find a common nwighbour X */
		for (e_it = e_end->next; e_it != e_end; e_it = e_it->next)
		{
			lep = EGcontainerOf (e_it, EGeUgraphEP_t, cn);
			o_type = lep->type ? 0U : 1U;
			E = EGcontainerOf (lep, mpf_EGalgMCedge_t, edge.edge.ep[lep->type]);
			M =
				EGcontainerOf (E->edge.edge.ep[o_type].node, mpf_EGalgMCnode_t, node.node);
			TESTL (mpf___MC_DEBUG_, mpf_EGlpNumIsLess (M->node.weight, mpf_zeroLpNum),
						 "node %u with " "negative weight %lf", M->id,
						 mpf_EGlpNumToLf (M->node.weight));
			mpf_EGlpNumCopyDiff (N_dv2, N_dv, E->edge.weight);
			/* if this node is done processing (first condition) or if it is waiting
			 * a higher value test level, we should skip it */
			if (M->mpf_lvl > 2)
				continue;
			TESTL (mpf___MC_DEBUG_, mpf_EGlpNumIsLess (E->edge.weight, mpf_zeroLpNum),
						 "edge %u with " "negative weight %lf", E->id,
						 mpf_EGlpNumToLf (E->edge.weight));
			mpf_EGlpNumCopy (M_dv, M->node.weight);
			mpf_EGlpNumDivUiTo (M_dv, 2);
			mpf_EGlpNumSubTo (M_dv, G->epsilon);
			mpf_EGlpNumSubTo (M_dv, E->edge.weight);
			e2_end = &(M->node.node.edges);
			/* now for all common neighbours (i.e. triangles) */
			for (e2_it = e2_end->next; e2_it != e2_end; e2_it = e2_it->next)
			{
				lep2 = EGcontainerOf (e2_it, EGeUgraphEP_t, cn);
				o_type2 = lep2->type ? 0U : 1U;
				F = EGcontainerOf (lep2, mpf_EGalgMCedge_t, edge.edge.ep[lep2->type]);
				X =
					EGcontainerOf (F->edge.edge.ep[o_type2].node, mpf_EGalgMCnode_t,
												 node.node);
				/* check the condition, first if X is a common neighbour, and if so,
				 * if the weight conditions for shrinking hold, if so, do the
				 * apropiate update. */
				if (X->hit && mpf_EGlpNumIsLeq (N_dv2, X->hit->weight) &&
						mpf_EGlpNumIsLeq (M_dv, F->edge.weight))
				{
					mpf_EGalgMCidentifyNodes (G, N, M);
					mpf_UPDATE (mpf___MC_profile_lvl[2]);
					TESTL (mpf___MC_DEBUG_, mpf_EGlpNumIsLess (N->node.weight, mpf_zeroLpNum),
								 "node %u" " with negative weight %lf", N->id,
								 mpf_EGlpNumToLf (N->node.weight));
					goto REDO;
				}
			}
		}
		/* ending, for that, we un-hit all neighbours of N */
		mpf__EGalgMCunhitNeighbours (N);
		/* move this node to the next level */
		N->mpf_lvl = 3;
		EGeListMoveAfter (&(N->lvl_cn), c_lvl + 1);
	}
	/* ======================================================================= */
	/* now we start performing test level three */
	if (max_lvl < 3 || G->G.G.n_nodes < 3)
		goto CLEANUP;
	MESSAGE (mpf___MC_VRBLVL_, "PR shrinking level 3, nodes %u edges %u",
					 G->G.G.n_nodes, G->G.G.n_edges);
	c_lvl++;
	while (!EGeListIsEmpty (c_lvl) && G->G.G.n_nodes > 2)
	{
		N = EGcontainerOf (c_lvl->next, mpf_EGalgMCnode_t, lvl_cn);
		e_end = &(N->node.node.edges);
		/* first hit all relevant neighbours of N */
		mpf__EGalgMChitNeighbours (N);
		/* and now, for all neighbour M of N, compute lb_{NM} stored in N_dv */
		for (e_it = e_end->next; e_it != e_end; e_it = e_it->next)
		{
			lep = EGcontainerOf (e_it, EGeUgraphEP_t, cn);
			o_type = lep->type ? 0U : 1U;
			E = EGcontainerOf (lep, mpf_EGalgMCedge_t, edge.edge.ep[lep->type]);
			M =
				EGcontainerOf (E->edge.edge.ep[o_type].node, mpf_EGalgMCnode_t, node.node);
			mpf_EGlpNumCopyDiff (N_dv, E->edge.weight, G->epsilon);
			/* if this node is done processing (first condition) or if it is waiting
			 * a higher value test level, we should skip it */
			if (M->mpf_lvl > 3)
				continue;
			e2_end = &(M->node.node.edges);
			/* now for all common neighbours (i.e. triangles) */
			for (e2_it = e2_end->next; e2_it != e2_end; e2_it = e2_it->next)
			{
				lep2 = EGcontainerOf (e2_it, EGeUgraphEP_t, cn);
				o_type2 = lep2->type ? 0U : 1U;
				F = EGcontainerOf (lep2, mpf_EGalgMCedge_t, edge.edge.ep[lep2->type]);
				X =
					EGcontainerOf (F->edge.edge.ep[o_type2].node, mpf_EGalgMCnode_t,
												 node.node);
				/* check the condition, first if X is a common neighbour, and if so,
				 * if the weight conditions for shrinking hold, if so, do the
				 * apropiate update. */
				if (X->hit)
				{
					if (mpf_EGlpNumIsLess (X->hit->weight, F->edge.weight))
						mpf_EGlpNumAddTo (N_dv, X->hit->weight);
					else
						mpf_EGlpNumAddTo (N_dv, F->edge.weight);
				}
			}													/* end e2_it */
			/* check condition, and do the shrinking if necesary */
			if (mpf_EGlpNumIsLeq (G->cut_val, N_dv))
			{
				mpf_EGalgMCidentifyNodes (G, N, M);
				mpf_UPDATE (mpf___MC_profile_lvl[3]);
				TESTL (mpf___MC_DEBUG_, mpf_EGlpNumIsLess (N->node.weight, mpf_zeroLpNum),
							 "node %u" " with negative weight %lf", N->id,
							 mpf_EGlpNumToLf (N->node.weight));
				goto REDO;
			}
		}
		/* ending this level test for N, for that, we un-hit all neighbours of N */
		mpf__EGalgMCunhitNeighbours (N);
		/* move this node to the next level */
		N->mpf_lvl = 4;
		EGeListMoveAfter (&(N->lvl_cn), c_lvl + 1);
	}
	/* ======================================================================= */
	/* now we start performing test level four */
	if (max_lvl < 4 || G->G.G.n_nodes < 3)
		goto CLEANUP;
	MESSAGE (mpf___MC_VRBLVL_, "PR shrinking level 4, nodes %u edges %u",
					 G->G.G.n_nodes, G->G.G.n_edges);
	c_lvl++;
	while (!EGeListIsEmpty (c_lvl) && G->G.G.n_nodes > 2)
	{
		N = EGcontainerOf (c_lvl->next, mpf_EGalgMCnode_t, lvl_cn);
		MESSAGE (mpf___MC_VRBLVL_, "Looking at node %u, level %u", N->id, N->mpf_lvl);
		e_end = &(N->node.node.edges);
		/* first hit all relevant neighbours of N */
		mpf__EGalgMChitNeighbours (N);
		mpf_EGlpNumCopy (N_dv, N->node.weight);
		mpf_EGlpNumDivUiTo (N_dv, 2);
		mpf_EGlpNumSubTo (N_dv, G->epsilon);
		/* and now, for all neighbour M of N, find two common neighbours X Y */
		for (e_it = e_end->next; e_it != e_end; e_it = e_it->next)
		{
			lep = EGcontainerOf (e_it, EGeUgraphEP_t, cn);
			o_type = lep->type ? 0U : 1U;
			E = EGcontainerOf (lep, mpf_EGalgMCedge_t, edge.edge.ep[lep->type]);
			M =
				EGcontainerOf (E->edge.edge.ep[o_type].node, mpf_EGalgMCnode_t, node.node);
			mpf_EGlpNumCopyDiff (N_dv2, N_dv, E->edge.weight);
			mpf_EGlpNumCopy (M_dv, M->node.weight);
			mpf_EGlpNumDivUiTo (M_dv, 2);
			mpf_EGlpNumSubTo (M_dv, G->epsilon);
			mpf_EGlpNumSubTo (M_dv, E->edge.weight);
			/* if this node is done processing (first condition) or if it is waiting
			 * a higher value test level, we should skip it */
			if (M->mpf_lvl > 4)
				continue;
			e2_end = &(M->node.node.edges);
			/* now for all common neighbours (i.e. triangles) */
			for (e2_it = e2_end->next; e2_it != e2_end; e2_it = e2_it->next)
			{
				lep2 = EGcontainerOf (e2_it, EGeUgraphEP_t, cn);
				o_type2 = lep2->type ? 0U : 1U;
				F = EGcontainerOf (lep2, mpf_EGalgMCedge_t, edge.edge.ep[lep2->type]);
				X =
					EGcontainerOf (F->edge.edge.ep[o_type2].node, mpf_EGalgMCnode_t,
												 node.node);
				/* check the condition, first if X is a common neighbour, and if so,
				 * find a second common neighbour */
				if (X->hit)
					for (e3_it = e2_it->next; e3_it != e2_end; e3_it = e3_it->next)
					{
						lep3 = EGcontainerOf (e3_it, EGeUgraphEP_t, cn);
						o_type3 = lep3->type ? 0U : 1U;
						H = EGcontainerOf (lep3, mpf_EGalgMCedge_t, edge.edge.ep[lep3->type]);
						Y = EGcontainerOf (H->edge.edge.ep[o_type3].node, mpf_EGalgMCnode_t,
															 node.node);
						/* check the condition, first if Y is a common neighbour, and if so,
						 * test the condition */
						if (Y->hit)
						{
							mpf_EGlpNumCopySum (M_dv2, X->hit->weight, Y->hit->weight);
							if (mpf_EGlpNumIsLess (M_dv2, N_dv2))
								continue;
							mpf_EGlpNumCopySum (M_dv2, F->edge.weight, H->edge.weight);
							if (mpf_EGlpNumIsLess (M_dv2, M_dv))
								continue;
							if (mpf_EGlpNumIsLess (Y->hit->weight, N_dv2) &&
									mpf_EGlpNumIsLess (F->edge.weight, M_dv))
								continue;
							if (mpf_EGlpNumIsLess (X->hit->weight, N_dv2) &&
									mpf_EGlpNumIsLess (H->edge.weight, M_dv))
								continue;
							/* if we reach this point, we can shrink N and M */
							mpf_EGalgMCidentifyNodes (G, N, M);
							mpf_UPDATE (mpf___MC_profile_lvl[4]);
							TESTL (mpf___MC_DEBUG_, mpf_EGlpNumIsLess (N->node.weight, mpf_zeroLpNum),
										 "node %u with negative weight %lf", N->id,
										 mpf_EGlpNumToLf (N->node.weight));
							goto REDO;
						}										/* end Y is neighbour */
					}											/* end e3_it */
			}													/* end e2_it */
		}														/* end e_it */
		/* ending this level test for N, for that, we un-hit all neighbours of N */
		mpf__EGalgMCunhitNeighbours (N);
		/* take out his node from the lists */
		N->mpf_lvl = 5;
		MESSAGE (mpf___MC_VRBLVL_, "Erasing at node %u, level %u", N->id, N->mpf_lvl);
		EGeListDel (&(N->lvl_cn));
	}
	/* ending */
CLEANUP:
	MESSAGE (mpf___MC_VRBLVL_, "PR shrinking level %u, nodes %u edges %u", max_lvl,
					 G->G.G.n_nodes, G->G.G.n_edges);
	mpf_EGlpNumClearVar (N_dv2);
	mpf_EGlpNumClearVar (M_dv2);
	mpf_EGlpNumClearVar (N_dv);
	mpf_EGlpNumClearVar (M_dv);
	return rval;
}

/* ========================================================================= */
/** @brief Given a current min cut graph, build a Push-Relabel graph.
 * @param mcG pointer to the min cut graph.
 * @param prG pointer to the push-relabel graph, it should be previously
 * initialized.
 * @param node_map array of mappings for nodes in the push-relabel graph, to
 * nodes in the min cut graph. It's size should be at least n_nodes.
 * @param all_pr_nodes array containing nodes to use in the push-relabel
 * graph, all elements should be already initialized, and the array should be
 * of size at least n_nodes.
 * @param all_pr_edges array contaaining edges to use in the push-relabel
 * graph, all elements should be initialized, and the array should be of size
 * at least n_edges.
 * @return zero on success, non-zero otherwise.
 * */
static inline int mpf_EGalgMCbuildPRgraph (mpf_EGalgMCgraph_t * const mcG,
																			 mpf_EGalgPRgraph_t * const prG,
																			 mpf_EGalgMCnode_t ** const node_map,
																			 mpf_EGalgPRnode_t * const all_pr_nodes,
																			 mpf_EGalgPRedge_t * const all_pr_edges)
{
	int rval = 0;
	EGeList_t *e_it,
	 *e_end,
	 *n_it,
	 *n_end;
#if DEBUG
	unsigned int const n_pr_nodes = mcG->G.G.n_nodes;
	unsigned int const n_pr_edges = mcG->G.G.n_edges;
#endif
	mpf_EGalgMCnode_t *N,
	 *M;
	mpf_EGalgMCedge_t *E;
	EGeUgraphEP_t *lep;
	register unsigned int i;
	MESSAGE (mpf___MC_VRBLVL_,
					 "Building push-relabel graph on %u nodes and %u edges", n_pr_nodes,
					 n_pr_edges);
	/* building graph */
	mpf_EGalgPRgraphReset (prG);
	n_end = &(mcG->G.G.nodes);
	/* add all nodes */
	for (i = 0, n_it = n_end->next; n_it != n_end; n_it = n_it->next, i++)
	{
		N = EGcontainerOf (n_it, mpf_EGalgMCnode_t, node.node.node_cn);
		mpf_EGalgPRnodeReset (all_pr_nodes + i);
		EGeDgraphAddNode (&(prG->G), &(all_pr_nodes[i].v));
		node_map[i] = N;
		N->new_id = i;
	}
	/* test if we added the right number of nodes */
	TESTG ((rval = (i != n_pr_nodes)), CLEANUP, "number of added nodes %u is not"
				 "the number of nodes in the MinCut graph %u", i, n_pr_nodes);
	/* and now add all edges */
	for (i = 0, n_it = n_end->next; n_it != n_end; n_it = n_it->next)
	{
		N = EGcontainerOf (n_it, mpf_EGalgMCnode_t, node.node.node_cn);
		e_end = &(N->node.node.edges);
		for (e_it = e_end->next; e_it != e_end; e_it = e_it->next)
		{
			lep = EGcontainerOf (e_it, EGeUgraphEP_t, cn);
			if (lep->type)
				continue;
			E = EGcontainerOf (lep, mpf_EGalgMCedge_t, edge.edge.ep[0]);
			M = EGcontainerOf (E->edge.edge.ep[1].node, mpf_EGalgMCnode_t, node.node);
			/* we need to add the edge only once */
			/* now we add the edge */
			mpf_EGalgPRedgeReset (all_pr_edges + i);
			mpf_EGlpNumCopy (all_pr_edges[i].fw.u, E->edge.weight);
			mpf_EGlpNumCopy (all_pr_edges[i].bw.u, E->edge.weight);
			EGeDgraphAddEdge (&(prG->G),
												&(all_pr_nodes[N->new_id].v),
												&(all_pr_nodes[M->new_id].v), &(all_pr_edges[i].fw.e));
			EGeDgraphAddEdge (&(prG->G),
												&(all_pr_nodes[M->new_id].v),
												&(all_pr_nodes[N->new_id].v), &(all_pr_edges[i].bw.e));
			i++;
		}
	}
	/* test if we added the right number of edges */
	TESTG ((rval =
					(i != n_pr_edges)), CLEANUP,
				 "number of added edges %u is not "
				 "the number of edges in the MinCut graph %u", i, n_pr_edges);

	/* ending */
#if DEBUG
CLEANUP:
#endif
	return rval;
}

/* ========================================================================= */
/** @brief Compute the farthest node in the push-relabel graph from the given
 * starting point.
 * @param prG push relabel graph.
 * @param S mpf_source node.
 * @param T where to store the farthest node.
 * @return zero on success, non-zero otherwise */
static inline int mpf_EGalgMCcomputeT (mpf_EGalgPRgraph_t * const prG,
																	 mpf_EGalgPRnode_t * const S,
																	 mpf_EGalgPRnode_t ** const T)
{
	EGeList_t *n_it,
	 *n_end,
	 *queue;
	mpf_EGalgPRnode_t *N = 0,
	 *P = 0;
	mpf_EGalgPRse_t *E = 0;
	n_end = &(prG->G.nodes);
	/* initialize distance, and queue */
	for (n_it = n_end->next; n_it != n_end; n_it = n_it->next)
	{
		N = EGcontainerOf (n_it, mpf_EGalgPRnode_t, v.node_cn);
		N->LVL_list = (EGeList_t)
		{
		0, 0};
		N->d = UINT_MAX;
	}
	S->d = 0;
	*T = P = S;
	queue = &(S->LVL_list);
	EGeListInit (queue);
	n_end = &(P->v.out_edge);
	for (n_it = n_end->next; n_it != n_end; n_it = n_it->next)
	{
		E = EGcontainerOf (n_it, mpf_EGalgPRse_t, e.tail_cn);
		N = EGcontainerOf (E->e.head, mpf_EGalgPRnode_t, v);
		if (N->LVL_list.next)
			continue;
		EGeListAddAfter (&(N->LVL_list), queue);
		N->d = P->d + 1;
	}
	while (!EGeListIsEmpty (queue))
	{
		P = EGcontainerOf (queue->prev, mpf_EGalgPRnode_t, LVL_list);
		EGeListDel (queue->prev);
		n_end = &(P->v.out_edge);
		for (n_it = n_end->next; n_it != n_end; n_it = n_it->next)
		{
			E = EGcontainerOf (n_it, mpf_EGalgPRse_t, e.tail_cn);
			N = EGcontainerOf (E->e.head, mpf_EGalgPRnode_t, v);
			if (N->LVL_list.next)
				continue;
			EGeListAddAfter (&(N->LVL_list), queue);
			N->d = P->d + 1;
		}
	}
	n_end = &(prG->G.nodes);
	for (n_it = n_end->next; n_it != n_end; n_it = n_it->next)
	{
		N = EGcontainerOf (n_it, mpf_EGalgPRnode_t, v.node_cn);
		if ((*T)->d < N->d)
			*T = N;
	}
	MESSAGE (mpf___MC_VRBLVL_, "worst distance is %u", (*T)->d);
	return 0;
}

/* ========================================================================= */
int mpf_EGalgMC (mpf_EGalgMCgraph_t * const G,
						 mpf_EGalgMCcbk_t * const cb,
						 const unsigned int max_lvl)
{
	int rval = 0;
	EGrandState_t g;
	mpf_EGalgPRgraph_t prG;
	mpf_EGalgPRnode_t *all_pr_nodes = 0,
	 *pr_node,
	 *pr_node2;
	mpf_EGalgPRedge_t *all_pr_edges = 0;
	mpf_EGalgMCnode_t **node_map = 0;
	mpf_EGalgMCnode_t *N;
	unsigned int *const cut = G->cut;
	EGeList_t *n_it,
	 *n_end,
	 *c_it,
	 *c_end;
	unsigned int n_pr_nodes,
	  n_pr_edges,
	  cur_s = 0;
	register unsigned int i;
	/* start random with default seed */
	EGrandInit(&g);
	/* we star by calling the Padberg-Rinaldi shrinking */
	rval = mpf_EGalgMCidentifyPRedges (G, cb, max_lvl);
	CHECKRVALG (rval, CLEANUP);
	/* intialize memory to be used by the push-relabel graph */
	n_pr_nodes = G->G.G.n_nodes;
	n_pr_edges = G->G.G.n_edges;
	/* if the reduced graph is empty, we have win */
	if (n_pr_nodes < 3 || !n_pr_edges)
		goto CLEANUP;
	/* otherwise, we have to work */
	all_pr_nodes = EGsMalloc (mpf_EGalgPRnode_t, n_pr_nodes);
	for (i = n_pr_nodes; i--;)
		mpf_EGalgPRnodeInit (all_pr_nodes + i);
	all_pr_edges = EGsMalloc (mpf_EGalgPRedge_t, n_pr_edges);
	for (i = n_pr_edges; i--;)
		mpf_EGalgPRedgeInit (all_pr_edges + i);
	node_map = EGsMalloc (mpf_EGalgMCnode_t *, n_pr_nodes);
	mpf_EGalgPRgraphInit (&prG);
	/* main loop */
	while (G->G.G.n_nodes > 2 && G->G.G.n_edges &&
				 mpf_EGlpNumIsLess (mpf_zeroLpNum, G->cut_val))
	{
		/* now we build our first push-relabel graph */
		rval = mpf_EGalgMCbuildPRgraph (G, &prG, node_map, all_pr_nodes, all_pr_edges);
		CHECKRVALG (rval, CLEANUP);
		/* do the actual min-st-cut. Note that an heuristic that might help
		 * speeding up this part is to always peform the minimum s-t cut by
		 * slecting t to be one at maximum distance from s. thus next time we run
		 * this code, the distance would be half the previous time. */
		/* we first select s randomly from among all nodes in the graph */
		cur_s = EGrand(&g) % prG.G.n_nodes;
		MESSAGE (mpf___MC_VRBLVL_, "computing t, maximum distance node from s");
		rval = mpf_EGalgMCcomputeT (&prG, all_pr_nodes + cur_s, &pr_node);
		CHECKRVALG (rval, CLEANUP);
		MESSAGE (mpf___MC_VRBLVL_, "computing s-t cut");
		rval = mpf_EGalgPRminSTcut (&prG, all_pr_nodes + cur_s, pr_node);
		CHECKRVALG (rval, CLEANUP);
		/* check if the cut found improve the minimum cut */
		if (mpf_EGlpNumIsLess (pr_node->e, G->cut_val))
		{
			/* if so, we update the best cut */
			mpf_UPDATE (mpf___MC_profile_up);
			MESSAGE (mpf___MC_VRBLVL_, "Update cut_val from %lf to %lf",
							 mpf_EGlpNumToLf (G->cut_val), mpf_EGlpNumToLf (pr_node->e));
			mpf_EGlpNumCopy (G->cut_val, pr_node->e);
			c_end = &(prG.G.nodes);
			for (c_it = c_end->next, i = 0; c_it != c_end; c_it = c_it->next)
			{
				pr_node2 = EGcontainerOf (c_it, mpf_EGalgPRnode_t, v.node_cn);
				if (pr_node2->d >= prG.G.n_nodes)
					continue;
				N = node_map[pr_node2 - all_pr_nodes];
				cut[i++] = N->id;
				n_end = &(N->node.members);
				for (n_it = n_end->next; n_it != n_end; n_it = n_it->next)
				{
					N = EGcontainerOf (n_it, mpf_EGalgMCnode_t, node.members);
					cut[i++] = N->id;
				}
			}													/* end computing the cut */
			G->cut_sz = i;
			/* call the call-back if necesary */
			if (cb && mpf_EGlpNumIsLess (G->cut_val, cb->cutoff))
			{
				rval = cb->do_fn (G->cut_val, G->cut_sz, G->cut, cb->param);
				CHECKRVALG (rval, CLEANUP);
			}													/* end call-back */
		}														/* end improving test */
		else if (cb && mpf_EGlpNumIsLess (pr_node->e, cb->cutoff))
		{
			unsigned int *lcut = EGsMalloc (unsigned int, G->G.n_onodes);
			c_end = &(prG.G.nodes);
			for (c_it = c_end->next, i = 0; c_it != c_end; c_it = c_it->next)
			{
				pr_node2 = EGcontainerOf (c_it, mpf_EGalgPRnode_t, v.node_cn);
				if (pr_node2->d >= prG.G.n_nodes)
					continue;
				N = node_map[pr_node2 - all_pr_nodes];
				lcut[i++] = N->id;
				n_end = &(N->node.members);
				for (n_it = n_end->next; n_it != n_end; n_it = n_it->next)
				{
					N = EGcontainerOf (n_it, mpf_EGalgMCnode_t, node.members);
					lcut[i++] = N->id;
				}
			}													/* end computing the cut */
			rval = cb->do_fn (pr_node->e, i, lcut, cb->param);
			CHECKRVALG (rval, CLEANUP);
			EGfree (lcut);

		}
		/* now we just shrink s and t, and call the padberg-rinaldy shrink */
		mpf_UPDATE (mpf___MC_profile_lvl[0]);
		TESTL (mpf___MC_DEBUG_,
					 mpf_EGlpNumIsLess (node_map[cur_s]->node.weight, mpf_zeroLpNum),
					 "node %u with negative weight %lf", node_map[cur_s]->id,
					 mpf_EGlpNumToLf (node_map[0]->node.weight));
		mpf_EGalgMCidentifyNodes (G, node_map[cur_s], node_map[pr_node - all_pr_nodes]);
		TESTL (mpf___MC_DEBUG_,
					 mpf_EGlpNumIsLess (node_map[cur_s]->node.weight, mpf_zeroLpNum),
					 "node %u with negative weight %lf", node_map[cur_s]->id,
					 mpf_EGlpNumToLf (node_map[cur_s]->node.weight));
		rval = mpf_EGalgMCidentifyPRedges (G, cb, max_lvl);
		CHECKRVALG (rval, CLEANUP);
	}															/* end main loop */

	/* ending */
CLEANUP:
	if (all_pr_nodes)
	{
		for (i = n_pr_nodes; i--;)
			mpf_EGalgPRnodeClear (all_pr_nodes + i);
		EGfree (all_pr_nodes);
	}
	if (all_pr_edges)
	{
		for (i = n_pr_edges; i--;)
			mpf_EGalgPRedgeClear (all_pr_edges + i);
		EGfree (all_pr_edges);
	}
	if (node_map)
		EGfree (node_map);
	return rval;
}

/* ========================================================================= */
/** @}
 * end of mpf_eg_min_cut.c */
