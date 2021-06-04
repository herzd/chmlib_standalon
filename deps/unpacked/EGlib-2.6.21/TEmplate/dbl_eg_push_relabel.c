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
 * @ingroup EGalgPushRelabel */
/** @addtogroup EGalgPushRelabel */
/** @{ */
/* ========================================================================= */
#include "dbl_eg_push_relabel.h"

/* ========================================================================= */
/** @brief If using profiling, we count the number of push, relabels, nodes
 * moved to S because we have a hole in the numb array, the number of levels
 * that we we looked at, and the number of nodes that we move to the S cut
 * because their label become bigger than number of nodes */
/** @{ */
static unsigned long long dbl___PR_profile_push = 0;
static unsigned long long dbl___PR_profile_numb = 0;
static unsigned long long dbl___PR_profile_lvel = 0;
static unsigned long long dbl___PR_profile_move = 0;
static unsigned long long dbl___PR_profile_rela = 0;
void dbl_EGalgPRprofile(void)
{
	if(dbl___PR_PROFILE__ <= DEBUG)
	{
		fprintf(stderr,"PROFILE FOR EGalgPushRelabel:\nnpush %llu\n"
				"nrela %llu\nnlevl %llu\nnmove %llu\nnnumb %llu\n",
				dbl___PR_profile_push, dbl___PR_profile_rela, dbl___PR_profile_lvel,
				dbl___PR_profile_move,dbl___PR_profile_numb); dbl___PR_profile_push = 
			dbl___PR_profile_numb = dbl___PR_profile_lvel = dbl___PR_profile_move = 
			dbl___PR_profile_rela = 0;
	}
}
#if dbl___PR_PROFILE__ <= DEBUG
#define dbl_UPDATE(counter) (counter)++
#else
#define dbl_UPDATE(counter)
#endif
/** @} */

/* ========================================================================= */
/** @brief counter to keep track of the relabel operations performed, this is
 * needed to implement the global relabeling heuristic */
static unsigned long long dbl___last_global = 0;

/* ========================================================================= */
/** @brief Compute the exact label distance in the graph to the given node.
 * @param t pointer to the dbl_EGalgPRnode_t node to wich we are computing the
 * exact label distances.
 * @param G pointer to the dbl_EGalgPRgraph_t graph where we are working.
 * @param DIST array of EGeList_t lists, DIST[i] is a list of all nodes
 * with distance label i.
 * @param numb array of integers where numb[k] contain the number of nodes
 * with distance labels equal to k. This array should be of size at least
 * n_nodes.
 * @return zero on success, non-zero otherwise.
 * @par Description:
 * We use a BFS implementation to solve this problem. We assume that the field
 * #dbl_EGalgPRnode_t::LVL_list is set to {0,0}. We also assume that the field
 * #dbl_EGalgPRnode_t::d is set to a suficiently large value that denotes that they
 * are disconnected from @a t, this value should be at least the number of nodes
 * in the graph that we are working on. */
static inline int dbl_EGalgPRcomputeLabels (dbl_EGalgPRgraph_t * const G,
																				dbl_EGalgPRnode_t * const t,
																				unsigned int *const numb,
																				EGeList_t * DIST)
{
	int rval = 0;
	EGeList_t *node_it = G->G.nodes.next;
	EGeList_t *const BFS_head = &(t->LVL_list);
	const EGeList_t *node_head = &(G->G.nodes);
	const unsigned n_nodes = G->G.n_nodes;
	unsigned int d;
	dbl_EGalgPRnode_t *node_pt,
	 *parent_pt;
	dbl_EGalgPRse_t *edge;
	MESSAGE (dbl___PR_VERBOSE__, "entering ");
	/* initialize the BFS list and set the distance to t as zero */
	numb[0]++;
	EGeListAddAfter (&(t->T_cut), DIST);
	t->d = 0;
	MESSAGE (dbl___PR_VERBOSE__, "label %u", 0);
	EGeListInit (BFS_head);
	/* add all tail of all incomming edges that have non-zero residual capacity to
	 * the BFS list */
	node_head = &(t->v.in_edge);
	node_it = t->v.in_edge.next;
	for (; node_it != node_head; node_it = node_it->next)
	{
		edge = EGcontainerOf (node_it, dbl_EGalgPRse_t, e.head_cn);
		/* discard edges without residual capacities */
		if (dbl_EGlpNumIsLeq (edge->r, dbl_epsLpNum))
			continue;
		node_pt = EGcontainerOf (edge->e.tail, dbl_EGalgPRnode_t, v);
		/* discard nodes that have already been added to the BFS list */
		if (node_pt->LVL_list.next)
			continue;
		EGeListAddAfter (&(node_pt->LVL_list), BFS_head);
		node_pt->d = 1;
		numb[1]++;
		EGeListAddAfter (&(node_pt->T_cut), DIST + 1);
		MESSAGE (dbl___PR_VERBOSE__, "label %u", 1);
	}

	/* now we traverse the graph in BFS mode */
	while (!EGeListIsEmpty (BFS_head))
	{
		parent_pt = EGcontainerOf (BFS_head->prev, dbl_EGalgPRnode_t, LVL_list);
		EGeListDel (BFS_head->prev);
		/* now we loop through all incomming edges in parent_pt */
		node_head = &(parent_pt->v.in_edge);
		node_it = parent_pt->v.in_edge.next;
		d = parent_pt->d + 1;
		for (; node_it != node_head; node_it = node_it->next)
		{
			edge = EGcontainerOf (node_it, dbl_EGalgPRse_t, e.head_cn);
			/* discard edges without residual capacities */
			if (dbl_EGlpNumIsLeq (edge->r, dbl_epsLpNum))
				continue;
			node_pt = EGcontainerOf (edge->e.tail, dbl_EGalgPRnode_t, v);
			/* discard nodes that have already been added to the BFS list */
			if (node_pt->LVL_list.next)
				continue;
			/* and for those nodes not already in the BFS tree, put them on it, and
			 * set their exact distaance labels. */
			EGeListAddAfter (&(node_pt->LVL_list), BFS_head);
			node_pt->d = d;
			if (d < n_nodes)
			{
				numb[d]++;
				EGeListAddAfter (&(node_pt->T_cut), DIST + d);
				MESSAGE (dbl___PR_VERBOSE__, "label %u", d);
			}
		}
	}
	/* now we reset the LVL_list entry to NULL */
	node_head = &(G->G.nodes);
	node_it = node_head->next;
	for (; node_it != node_head; node_it = node_it->next)
	{
		node_pt = EGcontainerOf (node_it, dbl_EGalgPRnode_t, v.node_cn);
		node_pt->LVL_list = (EGeList_t)
		{
		0, 0};
	}

	/* ending */
	MESSAGE (dbl___PR_VERBOSE__, "done ");
	return rval;
}

/* ========================================================================= */
/** @brief Perform a push operation in an edge.
 * @param edge_pt pointer to dbl_EGalgPRse_t edge where we will perform the push.
 * @param flow amount of (extra) flow to send in this edge.
 * @param LEVEL array of EGeList_t lists, LEVEL[i] is a list of all ACTIVE nodes
 * with distance label i.
 * @param n_nodes number of nodes in the graph.
 * @return zero on success, non-zero otherwise.
 * @par Description:
 * This function will perform a push operation on an edge, update the exess of
 * its endpoints, and update the residual capacities in this edge and it's
 * reverse edge. Furthermore, if debugging is enabled, this function will test
 * that the push is feasible (i.e. the residual capacity (before the push) is
 * larger than or equal the amount of pushed flow. */
static inline int dbl_EGalgPRpush (dbl_EGalgPRse_t * const edge_pt,
															 double flow,
															 EGeList_t * const LEVEL,
															 const unsigned int n_nodes)
{
	/* local variables */
	int rval = 0;
	dbl_EGalgPRse_t *const inv_edge = edge_pt->type ?
		&(EGcontainerOf (edge_pt, dbl_EGalgPRedge_t, bw)->fw) :
		&(EGcontainerOf (edge_pt, dbl_EGalgPRedge_t, fw)->bw);
	dbl_EGalgPRnode_t *const head = EGcontainerOf (edge_pt->e.head, dbl_EGalgPRnode_t, v);
	dbl_EGalgPRnode_t *const tail = EGcontainerOf (edge_pt->e.tail, dbl_EGalgPRnode_t, v);
	MESSAGE (dbl___PR_VERBOSE__, "pushing %lg r %lg u %lg", dbl_EGlpNumToLf (flow),
					 dbl_EGlpNumToLf (edge_pt->r), dbl_EGlpNumToLf (edge_pt->u));
	dbl_UPDATE (dbl___PR_profile_push);

	/* test feasibility of the change */
	EXITL (dbl___PR_DEBUGL__, dbl_EGlpNumIsLess (edge_pt->r, flow),
				 "Pushing more flow (%lg) "
				 "than the residual capacity (%lg) on the edge", dbl_EGlpNumToLf (flow),
				 dbl_EGlpNumToLf (edge_pt->r));

	/* make the changes where necesary */
	dbl_EGlpNumSubTo (edge_pt->r, flow);
	dbl_EGlpNumAddTo (inv_edge->r, flow);
	dbl_EGlpNumSubTo (tail->e, flow);
	dbl_EGlpNumAddTo (head->e, flow);
	/* if the head node wasn't active, we activate it now */
	if (!(head->LVL_list.next) && head->d < n_nodes)
	{
		EGeListAddAfter (&(head->LVL_list), LEVEL + head->d);
		MESSAGE (dbl___PR_VERBOSE__, "activating label %u", head->d);
	}

	/* ending */
	MESSAGE (dbl___PR_VERBOSE__, "done ");
	return rval;
}

/* ========================================================================= */
/** @brief Initialize flow to zero, and saturate forward edges in the dbl_source.
 * @param s pointer to the dbl_EGalgPRnode_t node dbl_source for the flow.
 * @param t pointer to the dbl_EGalgPRnode_t node sink for the flow
 * @param G pointer to the dbl_EGalgPRgraph_t graph where we are working.
 * @param numb array of integers where numb[k] contain the number of nodes
 * with distance labels equal to k. This array should be of size at least
 * n_nodes.
 * @param DIST array of EGeList_t lists, DIST[i] is a list of all nodes
 * with distance label i.
 * @param LEVEL array of EGeList_t lists, LEVEL[i] is a list of all ACTIVE nodes
 * with distance label i.
 * @return zero on success, non-zero otherwise.
 * @par Description:
 * This function implement the procedure @c preprocess as defined in page 225 in
 * the "Network Flows" book of Magnanti et. all. */
static inline int dbl_EGalgPRpreprocess (dbl_EGalgPRgraph_t * const G,
																		 dbl_EGalgPRnode_t * const s,
																		 dbl_EGalgPRnode_t * const t,
																		 unsigned int *const numb,
																		 EGeList_t * const LEVEL,
																		 EGeList_t * const DIST)
{
	int rval = 0;
	EGeList_t *node_it = G->G.nodes.next,
	 *edge_it,
	 *edge_head;
	EGeList_t *const node_head = &(G->G.nodes);
	dbl_EGalgPRnode_t *node_pt;
	dbl_EGalgPRse_t *edge_pt;
	const unsigned int n_nodes = G->G.n_nodes;
	MESSAGE (dbl___PR_VERBOSE__, "entering ");
	/* we first set all residual capacities to the actual capacities on the edges
	 * (this is equivalent to set all flow to zero) */
	for (; node_it != node_head; node_it = node_it->next)
	{
		node_pt = EGcontainerOf (node_it, dbl_EGalgPRnode_t, v.node_cn);
		node_pt->d = n_nodes;
		node_pt->LVL_list = (EGeList_t)
		{
		0, 0};
		dbl_EGlpNumZero (node_pt->e);
		edge_head = &(node_pt->v.in_edge);
		edge_it = edge_head->next;
		for (; edge_it != edge_head; edge_it = edge_it->next)
		{
			edge_pt = EGcontainerOf (edge_it, dbl_EGalgPRse_t, e.head_cn);
			dbl_EGlpNumCopy (edge_pt->r, edge_pt->u);
		}
	}

	/* now we compute the exact label distances */
	rval = dbl_EGalgPRcomputeLabels (G, t, numb, DIST);
	CHECKRVAL (rval);

	/* now we saturate alll outgoing edges in s */
	edge_head = &(s->v.out_edge);
	edge_it = edge_head->next;
	for (; edge_it != edge_head; edge_it = edge_it->next)
	{
		edge_pt = EGcontainerOf (edge_it, dbl_EGalgPRse_t, e.tail_cn);
		rval = dbl_EGalgPRpush (edge_pt, edge_pt->u, LEVEL, n_nodes);
		CHECKRVAL (rval);
	}
	/* set the distance from s to t to the number of nodes in the graph */
	MESSAGE (dbl___PR_VERBOSE__, "reset label from %u to %u", s->d, n_nodes);
	if (s->d < n_nodes)
	{
		numb[s->d]--;
		EGeListDel (&(s->T_cut));
	}
	s->d = n_nodes;

	/* ending */
	MESSAGE (dbl___PR_VERBOSE__, "done ");
	return rval;
}

/* ========================================================================= */
/** @brief Push/Relabel the given node. 
 * @param node_pt pointer to the dbl_EGalgPRnode_t node structure that we will
 * relabel.
 * @param numb array of integers where numb[k] contain the number of nodes
 * with distance labels equal to k. This array should be of size at least
 * n_nodes.
 * @param n_nodes number of nodes in the graph.
 * @param DIST array of EGeList_t lists, LEVEL[i] is a list of all nodes
 * with distance label i.
 * @param LEVEL array of EGeList_t lists, LEVEL[i] is a list of all ACTIVE nodes
 * with distance label i.
 * @return zero on success, non-zero otherwise.
 * @par Description:
 * This function implement the procedure @c push_relabel as defined in page 
 * 225 in the "Network Flows" book of Magnanti et. all. */
static int dbl_EGalgPRpushRelabel (dbl_EGalgPRnode_t * const node_pt,
															 unsigned int *const numb,
															 const unsigned int n_nodes,
															 EGeList_t * const LEVEL,
															 EGeList_t * const DIST)
{
	/* local variables */
	int rval = 0;
	unsigned inf_flow = 1;
	double flow,
	 *excess = &(node_pt->e);
	unsigned min_d = UINT_MAX - 1;
	const unsigned int d = node_pt->d - 1;
	EGeList_t const *const edge_head = &(node_pt->v.out_edge);
	EGeList_t *edge_it = edge_head->next;
	dbl_EGalgPRnode_t *head_pt;
	dbl_EGalgPRse_t *edge_pt;
	dbl_EGlpNumInitVar (flow);
	MESSAGE (dbl___PR_VERBOSE__, "entering ");
	EXITL (dbl___PR_DEBUGL__, !node_pt->LVL_list.next, "This is inconcivable!");
	/* loop through all residual outgoing edges */
	for (; edge_it != edge_head; edge_it = edge_it->next)
	{
		edge_pt = EGcontainerOf (edge_it, dbl_EGalgPRse_t, e.tail_cn);
		head_pt = EGcontainerOf (edge_pt->e.head, dbl_EGalgPRnode_t, v);
		/* if the edge is not in the residual network, discard it */
		if (dbl_EGlpNumIsLeq (edge_pt->r, dbl_epsLpNum))
		{
			continue;
		}
		/* compute the minimum label distance on all residual neighbours */
		if (min_d > head_pt->d)
		{
			MESSAGE (dbl___PR_VERBOSE__, "set min_d from %u to %u", min_d, head_pt->d);
			min_d = head_pt->d;
		}
		/* if the edge is not active, discard it */
		if (d != head_pt->d)
		{
			continue;
		}
		/* if we reach this line, then we will perform at least one push */
		inf_flow = 0;
		dbl_EGlpNumCopy (flow, *excess);
		if (dbl_EGlpNumIsLess (edge_pt->r, flow))
			dbl_EGlpNumCopy (flow, edge_pt->r);
		dbl_EGalgPRpush (edge_pt, flow, LEVEL, n_nodes);
		/* if after the push the excess is exausted, we exit the loop */
		if (dbl_EGlpNumIsLeq (*excess, dbl_epsLpNum))
			break;
	}
	/* if we didn't perform any push, then we must do a relabel */
	min_d++;
	/* deal with the case that we can't send flow in an active node */
	if (!inf_flow)
	{
		/* on the other hand, if we did send flow we might still have some excess 
		 * to send */
		/* if we don't have excess flow, we de-activate this node, and return */
		if (dbl_EGlpNumIsLeq (*excess, dbl_epsLpNum))
		{
			MESSAGE (dbl___PR_VERBOSE__, "saturated push, de-activating level %u",
							 node_pt->d);
			EGeListDel (&(node_pt->LVL_list));
			node_pt->LVL_list = (EGeList_t)
			{
			0, 0};
		}
		/* if not, then we still have some flow to send, but we have to re-label */
		else
		{
			MESSAGE (dbl___PR_VERBOSE__, "non-saturated push cur %u min_d %d",
							 node_pt->d, min_d);
		}
	}
	else
	{
		/* then we were unable to push, and we are forced to do a re-label */
		EXITL (dbl___PR_DEBUGL__, min_d <= node_pt->d, "relabeling to lower or equal "
					 "value %u %u", min_d, node_pt->d);
		numb[node_pt->d]--;
		EGeListDel (&(node_pt->T_cut));
		dbl_UPDATE (dbl___PR_profile_rela);
		dbl___last_global++;
		if (min_d < n_nodes)
		{
			MESSAGE (dbl___PR_VERBOSE__, "relabeling to %u from %u", min_d, node_pt->d);
			numb[min_d]++;
			EGeListAddAfter (&(node_pt->T_cut), DIST + min_d);
			node_pt->d = min_d;
			EGeListMoveAfter (&(node_pt->LVL_list), LEVEL + min_d);
		}
		else
		{
			MESSAGE (dbl___PR_VERBOSE__,
							 "relabeling to %u from %u and moving it to the S" " cut",
							 n_nodes, node_pt->d);
			node_pt->d = min_d;
			dbl_UPDATE (dbl___PR_profile_move);
			EGeListDel (&(node_pt->LVL_list));
			node_pt->LVL_list = (EGeList_t)
			{
			0, 0};
		}
	}


	/* ending */
	dbl_EGlpNumClearVar (flow);
	MESSAGE (dbl___PR_VERBOSE__, "done ");
	return rval;
}

/* ========================================================================= */
/** @brief Once we have found a 'hole' in the numb arrray, all nodes with
 * distance labels  above the given value, can be set to the number of nodes in
 * the network (and thus be assumed to be in the S cut).
 * @param hole first zero value in the @a numb array.
 * @param max_numb highest non-zero index value in the array numb
 * @param n_nodes number of total nodes in the working graph.
 * @param DIST pointer at the array of lists containing all nodes within each
 * distance level.
 * @param numb array of integers where numb[k] contain the number of nodes
 * with distance labels equal to k. This array should be of size at least
 * n_nodes.
 * @return zero on success, non-zero otherwise.
 * */
static inline int dbl_EGalgPRnumb (const unsigned int hole,
															 const unsigned int max_numb,
															 const unsigned int n_nodes,
															 unsigned int *const numb,
															 EGeList_t * const DIST)
{
	/* local vaariables */
	dbl_EGalgPRnode_t *node_pt;
	unsigned int d;
	register unsigned int i;
	/* for all nodes in the graph, we check if they are disconnected from the
	 * sink, and if so, set their distance to n_nodes */
	for (d = hole + 1; d < max_numb; d++)
	{
		for (i = numb[d]; i--;)
		{
			MESSAGE (dbl___PR_VERBOSE__, "eliminating node of level %u", d);
			dbl_UPDATE (dbl___PR_profile_numb);
			node_pt = EGcontainerOf (DIST[d].next, dbl_EGalgPRnode_t, T_cut);
			node_pt->d = n_nodes;
			EGeListDel (&(node_pt->T_cut));
			/* if it is active, we de-activate it */
			if (node_pt->LVL_list.next)
			{
				EGeListDel (&(node_pt->LVL_list));
				node_pt->LVL_list = (EGeList_t)
				{
				0, 0};
			}
		}
		/* and reset the counter to zero */
		numb[d] = 0;
	}
	MESSAGE (dbl___PR_VERBOSE__, "done ");
	return 0;
}

/* ========================================================================= */
/** @brief Re-compute the global distance labels.
 * @param s pointer to the dbl_EGalgPRnode_t* dbl_source node in the network.
 * @param t pointer to the dbl_EGalgPRnode_t* sink node in the network.
 * @param G pointer to the dbl_EGalgPRgraph_t* in wich we will work.
 * @param numb array of integers where numb[k] contain the number of nodes
 * with distance labels equal to k. This array should be of size at least
 * n_nodes.
 * @param n_nodes number of nodes in the graph.
 * @param DIST array of EGeList_t lists, DIST[i] is a list of all nodes
 * with distance label i.
 * @param LEVEL array of EGeList_t lists, LEVEL[i] is a list of all ACTIVE nodes
 * with distance label i.
 * @return zero on success, non-zero otherwise.
 * @par Description:
 * This function will recompute the values stored in numb, in LEVEL and all
 * distance labels exactly, without changing the current pre-flow in the
 * network.
 * */
static inline int dbl_EGalgPRglobalRelabel (dbl_EGalgPRgraph_t * const G,
																				dbl_EGalgPRnode_t * const s,
																				dbl_EGalgPRnode_t * const t,
																				unsigned int const n_nodes,
																				unsigned int *const numb,
																				EGeList_t * const LEVEL,
																				EGeList_t * const DIST)
{
	/* local variables */
	int rval = 0;
	dbl_EGalgPRnode_t *active = 0;
	EGeList_t *const n_head = &(G->G.nodes);
	EGeList_t *n_it = n_head->next;
	register unsigned int i = n_nodes;
	unsigned int d;
	MESSAGE (dbl___PR_VERBOSE__, "entering ");
	/* initialize everything */
	while (i--)
	{
		EGeListInit (LEVEL + i);
		EGeListInit (DIST + i);
	}
	memset (numb, 0, sizeof (unsigned int) * n_nodes);
	for (; n_it != n_head; n_it = n_it->next)
	{
		active = EGcontainerOf (n_it, dbl_EGalgPRnode_t, v.node_cn);
		active->LVL_list = (EGeList_t)
		{
		0, 0};
		active->T_cut = (EGeList_t)
		{
		0, 0};
		active->d = n_nodes;
	}

	/* compute exact distance labels from s */
	rval = dbl_EGalgPRcomputeLabels (G, t, numb, DIST);
	CHECKRVAL (rval);

	/* rebuild the LVL_list information with the exact labels */
	n_it = n_head->next;
	for (; n_it != n_head; n_it = n_it->next)
	{
		active = EGcontainerOf (n_it, dbl_EGalgPRnode_t, v.node_cn);
		if (active == s)
			continue;
		d = active->d;
		/* discard nodes cut away from the dbl_source */
		if (d >= n_nodes)
			continue;
		/* discard non-active nodes */
		if (dbl_EGlpNumIsLeq (active->e, dbl_epsLpNum))
			continue;
		/* add the active node to their corresponding active list */
		MESSAGE (dbl___PR_VERBOSE__, "Activating node %p with level %u excess %lg",
						 (void *) active, d, dbl_EGlpNumToLf (active->e));
		EGeListAddAfter (&(active->LVL_list), LEVEL + d);
	}

	/* ending */
	return rval;
}

/* ========================================================================= */
int dbl_EGalgPRminSTcut (dbl_EGalgPRgraph_t * const G,
										 dbl_EGalgPRnode_t * const s,
										 dbl_EGalgPRnode_t * const t)
{
	/* local variables */
	int rval = 0;
	register unsigned int i;
	unsigned int old_d = 0;
	const unsigned int n_nodes = G->G.n_nodes;
	unsigned int *numb = EGsMalloc (unsigned int, n_nodes),
	  max_numb = n_nodes;
	EGeList_t *LEVEL = EGsMalloc (EGeList_t, n_nodes);
	EGeList_t *DIST = EGsMalloc (EGeList_t, n_nodes);
	unsigned int level = n_nodes - 1;
	dbl_EGalgPRnode_t *active = 0;
#if dbl_EG_PR_RELABEL
	unsigned const int limit = dbl_EG_PR_RELABEL_FREC * n_nodes;
#endif
	MESSAGE (dbl___PR_VERBOSE__, "entering ");

	/* initialize all level lists */
	for (i = n_nodes; i--;)
	{
		EGeListInit (DIST + i);
		EGeListInit (LEVEL + i);
	}
	memset (numb, 0, sizeof (unsigned int) * n_nodes);

	/* we start by preprocesing the graph and sset-up our data structures */
	dbl_EGalgPRpreprocess (G, s, t, numb, LEVEL, DIST);
	/* now determine the lowest zero value (i.e. hole) in numb */
	for (i = 1; i < n_nodes; i++)
		if (!(numb[i]))
			break;
	if (i < n_nodes)
	{
		rval = dbl_EGalgPRnumb (i, max_numb, n_nodes, numb, DIST);
		CHECKRVALG (rval, CLEANUP);
		max_numb = i;
	}

	/* identify the highest labeled active node */
	while (EGeListIsEmpty (LEVEL + level) && level)
	{
		level--;
		dbl_UPDATE (dbl___PR_profile_lvel);
	}
	/* now we do the loop among active nodes */
	while (level > 0)
	{
		MESSAGE (dbl___PR_VERBOSE__, "level %u", level);
		active = EGcontainerOf (LEVEL[level].next, dbl_EGalgPRnode_t, LVL_list);
		old_d = active->d;
		dbl_EGalgPRpushRelabel (active, numb, n_nodes, LEVEL, DIST);
		/* check that if we did a relabel operation, and the resulting old level is
		 * empty, then we can move nodes to the S cut */
		if (active->d < n_nodes && old_d < active->d)
		{
			if (max_numb < active->d + 1)
				max_numb = active->d + 1;
			if (!numb[old_d])
			{
				rval = dbl_EGalgPRnumb (old_d, max_numb, n_nodes, numb, DIST);
				CHECKRVALG (rval, CLEANUP);
				max_numb = old_d;
			}
		}
		/* we have to change the level */
		if (active->d < n_nodes && old_d < active->d)
		{
#if dbl_EG_PR_RELABEL
			if (dbl___last_global > limit)
			{
				dbl___last_global = 0;
				rval = dbl_EGalgPRglobalRelabel (G, s, t, n_nodes, numb, LEVEL, DIST);
				CHECKRVALG (rval, CLEANUP);
				level = n_nodes - 1;
				max_numb = n_nodes;
			}
			else
#endif
				level = active->d;
			dbl_UPDATE (dbl___PR_profile_lvel);
		}
		/* find again the highest active level */
		while (EGeListIsEmpty (LEVEL + level) && level)
		{
			level--;
			dbl_UPDATE (dbl___PR_profile_lvel);
		}
	}

	/* ending */
CLEANUP:
	EGfree (numb);
	EGfree (LEVEL);
	EGfree (DIST);
	MESSAGE (dbl___PR_VERBOSE__, "done ");
	return rval;
}

/* ========================================================================= */
int dbl_EGalgPRmaxSTflow (dbl_EGalgPRgraph_t * const G,
											dbl_EGalgPRnode_t * const s,
											dbl_EGalgPRnode_t * const t)
{
	/* local variables */
	int rval = 0;
	unsigned int old_d = 0;
	const unsigned int n_nodes = G->G.n_nodes;
	unsigned int *numb = EGsMalloc (unsigned int, n_nodes);
	EGeList_t *LEVEL = EGsMalloc (EGeList_t, n_nodes);
	EGeList_t *DIST = EGsMalloc (EGeList_t, n_nodes);
	unsigned int level = n_nodes - 1;
	dbl_EGalgPRnode_t *active = 0;
#if dbl_EG_PR_RELABEL
	unsigned const int limit = dbl_EG_PR_RELABEL_FREC * n_nodes;
#endif
	MESSAGE (dbl___PR_VERBOSE__, "entering ");

	/* compute levels and everything */
	rval = dbl_EGalgPRglobalRelabel (G, t, s, n_nodes, numb, LEVEL, DIST);
	CHECKRVALG (rval, CLEANUP);
	/* now we just loop as in the minSTcut code */
	/* identify the highest labeled active node */
	while (EGeListIsEmpty (LEVEL + level) && level)
	{
		level--;
		dbl_UPDATE (dbl___PR_profile_lvel);
	}
	/* now we do the loop among active nodes */
	while (level > 0)
	{
		MESSAGE (dbl___PR_VERBOSE__, "level %u", level);
		active = EGcontainerOf (LEVEL[level].next, dbl_EGalgPRnode_t, LVL_list);
		old_d = active->d;
		dbl_EGalgPRpushRelabel (active, numb, n_nodes, LEVEL, DIST);
		/* we have to change the level */
		if (old_d < active->d && active->d < n_nodes)
		{
#if dbl_EG_PR_RELABEL
			if (dbl___last_global > limit)
			{
				dbl___last_global = 0;
				rval = dbl_EGalgPRglobalRelabel (G, t, s, n_nodes, numb, LEVEL, DIST);
				CHECKRVALG (rval, CLEANUP);
				level = n_nodes - 1;
			}
			else
#endif
				level = active->d;
			dbl_UPDATE (dbl___PR_profile_lvel);
		}
		/* find again the highest active level */
		while (EGeListIsEmpty (LEVEL + level) && level)
		{
			level--;
			dbl_UPDATE (dbl___PR_profile_lvel);
		}
	}

	/* ending */
CLEANUP:
	EGfree (numb);
	EGfree (DIST);
	EGfree (LEVEL);
	MESSAGE (dbl___PR_VERBOSE__, "done ");
	return rval;
}

/* ========================================================================= */
int dbl_EGalgPRoptimalityTest (dbl_EGalgPRgraph_t * const G,
													 dbl_EGalgPRnode_t * const s,
													 dbl_EGalgPRnode_t * const t,
													 double * error)
{
	/* local variables */
	int rval = 0;
	EGeList_t *n_it,
	 *n_head,
	 *e_it,
	 *e_head;
	dbl_EGalgPRnode_t *node_pt,
	 *inv_node;
	dbl_EGalgPRse_t *edge_pt;
	double flow;
	double cut;
	register unsigned int i;
	unsigned const int n_nodes = G->G.n_nodes;
	unsigned int *numb = EGsMalloc (unsigned int, n_nodes);
	EGeList_t *DIST = EGsMalloc (EGeList_t, n_nodes);
	memset (numb, 0, sizeof (unsigned int) * n_nodes);
	for (i = n_nodes; i--;)
		EGeListInit (DIST + i);
	dbl_EGlpNumZero (*error);
	dbl_EGlpNumInitVar (flow);
	dbl_EGlpNumInitVar (cut);
	/* we re-compute the distance labels to get again the information of the s-t
	 * cut */
	n_head = &(G->G.nodes);
	n_it = n_head->next;
	for (; n_it != n_head; n_it = n_it->next)
	{
		node_pt = EGcontainerOf (n_it, dbl_EGalgPRnode_t, v.node_cn);
		node_pt->LVL_list = (EGeList_t)
		{
		0, 0};
		node_pt->d = n_nodes;
	}
	rval = dbl_EGalgPRcomputeLabels (G, t, numb, DIST);
	CHECKRVALG (rval, CLEANUP);

	/* loop through all nodes */
	dbl_EGlpNumZero (cut);
	n_head = &(G->G.nodes);
	n_it = n_head->next;
	for (; n_it != n_head; n_it = n_it->next)
	{
		node_pt = EGcontainerOf (n_it, dbl_EGalgPRnode_t, v.node_cn);
		if (node_pt != s && node_pt != t)
		{
			dbl_EGlpNumSetToMaxAbs (*error, node_pt->e);
			/* check that the excess condition holds within epsilon */
			if (dbl_EGlpNumIsNeqZero (node_pt->e, dbl_epsLpNum))
			{
				MESSAGE (dbl___PR_TEST_VERBOSE__, "excess %lg for node %p is above error",
								 dbl_EGlpNumToLf (node_pt->e), (void *) node_pt);
				rval++;
			}
		}
		/* check flow condition and compute cut value */
		dbl_EGlpNumZero (flow);
		e_head = &(node_pt->v.out_edge);
		e_it = e_head->next;
		for (; e_it != e_head; e_it = e_it->next)
		{
			edge_pt = EGcontainerOf (e_it, dbl_EGalgPRse_t, e.tail_cn);
			inv_node = EGcontainerOf (edge_pt->e.head, dbl_EGalgPRnode_t, v);
			if (dbl_EGlpNumIsLess (edge_pt->r, edge_pt->u))
			{
				dbl_EGlpNumAddTo (flow, edge_pt->u);
				dbl_EGlpNumSubTo (flow, edge_pt->r);
			}
			if ((node_pt->d >= n_nodes) && (inv_node->d < n_nodes))
			{
				dbl_EGlpNumAddTo (cut, edge_pt->u);
			}
		}
		e_head = &(node_pt->v.in_edge);
		e_it = e_head->next;
		for (; e_it != e_head; e_it = e_it->next)
		{
			edge_pt = EGcontainerOf (e_it, dbl_EGalgPRse_t, e.head_cn);
			inv_node = EGcontainerOf (edge_pt->e.tail, dbl_EGalgPRnode_t, v);
			if (dbl_EGlpNumIsLess (edge_pt->r, edge_pt->u))
			{
				dbl_EGlpNumSubTo (flow, edge_pt->u);
				dbl_EGlpNumAddTo (flow, edge_pt->r);
			}
		}
		if (node_pt != s && node_pt != t)
		{
			dbl_EGlpNumSetToMaxAbs (*error, flow);
			if (dbl_EGlpNumIsNeqZero (flow, dbl_epsLpNum))
			{
				rval++;
				MESSAGE (dbl___PR_TEST_VERBOSE__,
								 "flow balance %lg for node %p is violated",
								 dbl_EGlpNumToLf (node_pt->e), (void *) node_pt);
			}
		}
	}
	/* now check the cut value */
	dbl_EGlpNumCopy (flow, t->e);
	dbl_EGlpNumAddTo (flow, s->e);
	dbl_EGlpNumSetToMaxAbs (*error, flow);
	if (dbl_EGlpNumIsNeqZero (flow, dbl_epsLpNum))
	{
		rval++;
		MESSAGE (dbl___PR_TEST_VERBOSE__, "flow balance between s and t is violated"
						 " by %lg", dbl_EGlpNumToLf (flow));
	}
	dbl_EGlpNumCopy (flow, s->e);
	dbl_EGlpNumAddTo (flow, cut);
	dbl_EGlpNumSetToMaxAbs (*error, flow);
	if (dbl_EGlpNumIsNeqZero (flow, dbl_epsLpNum))
	{
		rval++;
		MESSAGE (dbl___PR_TEST_VERBOSE__, "s-flow and cut don't agree by %lg",
						 dbl_EGlpNumToLf (flow));
	}

	/* ending */
CLEANUP:
	EGfree (numb);
	EGfree (DIST);
	dbl_EGlpNumClearVar (cut);
	dbl_EGlpNumClearVar (flow);
	return rval;
}

/* ========================================================================= */
/** @}
 * end of dbl_eg_push_relabel.c */
