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
 * @ingroup EGsrkGraph */
/** @addtogroup EGsrkGraph */
/** @{ */
/* ========================================================================= */
#include "mpf_eg_shrink_graph.h"

/* ========================================================================= */
mpf_EGsrkNode_t *mpf_EGsrkIdentifyNodes (mpf_EGsrkGraph_t * const G,
																 mpf_EGsrkNode_t * const base,
																 mpf_EGsrkNode_t * const srkN)
{
	mpf_EGsrkNode_t *o_end;
	mpf_EGsrkEdge_t *c_edge,
	 *o_edge;
	EGeUgraphEP_t *c_ep;
	EGeList_t *e_it,
	 *e_end,
	 *e_next;
	unsigned o_type;
	/* now we start performing the srhinking procedure */
	EGesLink (&(base->parent), &(srkN->parent));
	mpf_EGlpNumAddTo (base->weight, srkN->weight);
	/* first put the other elements in the shrunken node list in the new list */
	EGeListSplice (&(srkN->members), &(base->members));
	/* and then put the srunken element in the new member list */
	EGeListAddAfter (&(srkN->members), &(base->members));
	base->mmb_sz += srkN->mmb_sz + 1;

	/* now we initialize all things related to the base node */
	e_end = &(base->node.edges);
	for (e_it = e_end->next; e_it != e_end; e_it = e_it->next)
	{
		c_ep = EGcontainerOf (e_it, EGeUgraphEP_t, cn);
		c_edge = EGcontainerOf (c_ep, mpf_EGsrkEdge_t, edge.ep[c_ep->type]);
		o_type = c_ep->type ? 0U : 1U;
		o_end = EGcontainerOf (c_edge->edge.ep[o_type].node, mpf_EGsrkNode_t, node);
		o_end->hit = c_edge;
	}
	e_end = &(srkN->node.edges);
	e_next = e_it = e_end->next;
	/* we have to be carefull, note that we will move the endpoint pointed by
	 * e_it, so we keep also the next iterator when we enter the iteration */
	for (; e_it != e_end; e_it = e_next)
	{
		e_next = e_next->next;
		c_ep = EGcontainerOf (e_it, EGeUgraphEP_t, cn);
		c_edge = EGcontainerOf (c_ep, mpf_EGsrkEdge_t, edge.ep[c_ep->type]);
		o_type = c_ep->type ? 0U : 1U;
		o_end = EGcontainerOf (c_edge->edge.ep[o_type].node, mpf_EGsrkNode_t, node);
		/* if the edge will become a loop, we delete it from the graph */
		if (o_end == base)
		{
			mpf_EGlpNumSubTo (base->weight, c_edge->weight);
			mpf_EGlpNumSubTo (base->weight, c_edge->weight);
			EGeUgraphDelEdge (&(G->G), &(c_edge->edge));
			continue;
		}
		o_edge = o_end->hit;
		/* now if this edge touch a previously untouch node, just change it's
		 * endpoint */
		if (!o_edge)
		{
			EGeUgraphChangeEP (&(G->G), &(c_edge->edge), &(base->node), c_ep->type);
			continue;
		}
		/* otherwise, this edge is entering an already touched node, in wich case,
		 * we update the weight of the edge already in place, splice the
		 * edge-members list, update it's size, and delete this current edge */
		mpf_EGlpNumAddTo (o_edge->weight, c_edge->weight);
		EGeListSplice (&(c_edge->members), &(o_edge->members));
		EGeListAddAfter (&(c_edge->members), &(o_edge->members));
		o_edge->mmb_sz += c_edge->mmb_sz + 1;
		EGeUgraphDelEdge (&(G->G), &(c_edge->edge));
	}
	/* now we finally delete the shrunken node from the graph. */
	EGeUgraphDelNode (&(G->G), &(srkN->node));

	/* and reset the hit list to NULL */
	e_end = &(base->node.edges);
	for (e_it = e_end->next; e_end != e_it; e_it = e_it->next)
	{
		c_ep = EGcontainerOf (e_it, EGeUgraphEP_t, cn);
		c_edge = EGcontainerOf (c_ep, mpf_EGsrkEdge_t, edge.ep[c_ep->type]);
		o_type = c_ep->type ? 0U : 1U;
		o_end = EGcontainerOf (c_edge->edge.ep[o_type].node, mpf_EGsrkNode_t, node);
		o_end->hit = 0;
	}

	/* ending */
	return base;
}

/* ========================================================================= */
/** @}
 * end of mpf_eg_shrink_graph.c */
