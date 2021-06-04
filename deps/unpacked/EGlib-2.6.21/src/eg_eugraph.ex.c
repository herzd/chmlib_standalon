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
 * @ingroup EGeUgraph
 * */
/** @addtogroup EGeUgraph */
/** @{ */
#include "EGlib.h"
/* ========================================================================= */
/** @brief example of a graph structure using the embeded substructures. */
typedef struct my_ugraph_t
{
	int id;
	EGeUgraph_t G;
}
my_ugraph_t;

/* ========================================================================= */
/** @brief example of a node structure using the embeded substructures. */
typedef struct my_unode_t
{
	int id;
	EGeUgraphNode_t v;
}
my_unode_t;

/* ========================================================================= */
/** @brief example of an edge structure using the embeded substructures. */
typedef struct my_uedge_t
{
	int id;
	EGeUgraphEdge_t e;
}
my_uedge_t;

/* ========================================================================= */
/** @brief Display the contents of our graph structure */
static inline void display_UG (my_ugraph_t * myG)
{
	EGeUgraph_t *G = &(myG->G);
	my_unode_t *cn;
	my_uedge_t *ce;
	EGeUgraphEP_t *lep;
	EGeList_t *node_it,
	 *edge_it;
	fprintf (stderr, "Graph %d (%d nodes, %d edges):\n", myG->id, G->n_nodes,
					 G->n_edges);
	/* we display the nodes and it's contents only if it is not empty */
	if (!EGeListIsEmpty (&(G->nodes)))
	{
		fprintf (stderr, "Nodes:\n");
		for (node_it = G->nodes.next; node_it != &(G->nodes);
				 node_it = node_it->next)
		{
			cn = EGcontainerOf (node_it, my_unode_t, v.node_cn);
			fprintf (stderr, "\t%d: ", cn->id);
			if (!EGeListIsEmpty (&(cn->v.edges)))
			{
				fprintf (stderr, "(edges) ");
				for (edge_it = cn->v.edges.next; edge_it != &(cn->v.edges);
						 edge_it = edge_it->next)
				{
					lep = EGcontainerOf (edge_it, EGeUgraphEP_t, cn);
					ce = EGcontainerOf (lep, my_uedge_t, e.ep[lep->type]);
					fprintf (stderr, "%d ", ce->id);
				}
			}
			fprintf (stderr, "\n");
		}
	}

}

/* ========================================================================= */
/** @brief A simple example of a directed graph using (EGdEgraph) structures.
 * @return zero on success, non-zero- otherwise.
 * @par Description:
 * Show how to use a directed graph, modify it and display it's contents */
int main (void)
{
	int rval = 0;
	my_ugraph_t myG;
	my_unode_t nodes[5];
	my_uedge_t edges[5];
	int i;

	/* initialize all structures */
	EGlib_info();
	EGlib_version();
	EGeUgraphInit (&(myG.G));
	myG.id = 0;
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	display_UG (&myG);
	/* note that this is a backward loop that should be more eficient than the
	 * regular forward loop (you save the last update condition ) */
	for (i = 5; i--;)
	{
		EGeUgraphNodeInit (&(nodes[i].v));
		EGeUgraphEdgeInit (&(edges[i].e));
		nodes[i].id = i;
		edges[i].id = i;
	}
	/* now we can use all edges and nodes and add them to the graph, we will
	 * create a circular graph on 5 edges */
	for (i = 5; i--;)
		EGeUgraphAddNode (&(myG.G), &(nodes[i].v));
	display_UG (&myG);
	for (i = 5; i--;)
		EGeUgraphAddEdge (&(myG.G), &(nodes[i].v), &(nodes[(i + 1) % 5].v),
											&(edges[i].e));
	display_UG (&myG);
	/* now we will push all incomming edges to a previous node, and all outgoing
	 * edges to the next edge */
	for (i = 5; i--;)
	{
		EGeUgraphChangeHead (&(myG.G), &(edges[i].e), &(nodes[(i + 4) % 5].v));
		EGeUgraphChangeTail (&(myG.G), &(edges[i].e), &(nodes[(i + 2) % 5].v));
	}
	display_UG (&myG);
	/* now we delete one edge at a time */
	for (i = 5; i--;)
	{
		EGeUgraphDelEdge (&(myG.G), &(edges[i].e));
		display_UG (&myG);
	}
	/* and delete nodes one by one */
	for (i = 5; i--;)
	{
		EGeUgraphDelNode (&(myG.G), &(nodes[i].v));
		display_UG (&myG);
	}
	/* ending */
	CLEANUP:
	EGeUgraphClear (&myG);
	for (i = 5; i--;)
	{
		EGeUgraphNodeClear (&(nodes[i].v));
		EGeUgraphEdgeClear (&(edges[i].e));
	}
	return rval;
}

/* ========================================================================= */
/** @} */
