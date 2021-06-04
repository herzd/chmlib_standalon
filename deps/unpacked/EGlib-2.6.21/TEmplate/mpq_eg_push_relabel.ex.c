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
#include "EGlib.h"
/* ========================================================================= */
/** @brief display usage of this program */
static inline void mpq_pr_usage (char **argv)
{
	fprintf (stderr, "Usage: %s input_file\n", argv[0]);
}

/* ========================================================================= */
/** @brief example of usage of the Preflow-Push algorithm as implemented in
 * (EGalgPushRelabel).
 * 
 * We read a file from the input whose two first numbers are the number of nodes
 * and edges (we assume that the graph is undirected ), and then a 3-tupla with
 * head tail capacity. we use the last field (capacity) as the capacity of the
 * algorithm, and compute the shortest path from node 0 to node 1. */
int main (int argc,
					char **argv)
{
	int rval = 0;
	EGioFile_t *in_file = 0;
	unsigned int n_nodes = 0;
	unsigned int n_edges = 0;
	register unsigned int i;
	unsigned int head,
	  tail;
	mpq_t cap;
	EGtimer_t cut_timer;
	mpq_EGalgPRgraph_t G;
	mpq_EGalgPRnode_t *nodes = 0/*,
	 *node_pt*/;
	mpq_EGalgPRedge_t *edges = 0;
	/*EGeList_t *n_it;*/
	char l_str[1024];
	char*l_argv[128];
	int l_argc;
	EGlib_info();
	EGlib_version();
	EGlpNumStart();
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	EGtimerReset (&cut_timer);
	mpq_EGalgPRgraphInit (&G);
	mpq_EGlpNumInitVar (cap);
	/* test we have an input file */
	if (argc < 2)
	{
		mpq_pr_usage (argv);
		rval = 1;
		goto CLEANUP;
	}
	in_file = EGioOpen (argv[1], "r");
	if (!in_file)
	{
		fprintf (stderr, "Can't open file %s\n", argv[1]);
		mpq_pr_usage (argv);
		rval = 1;
		goto CLEANUP;
	}
	/* now we start reading the file */
	EGioGets(l_str,1024,in_file);
	EGioNParse(l_str,128," ","%#",&l_argc,l_argv);
	TESTG((rval=(l_argc!=2)),CLEANUP,"Expected two tokens, but found %d",l_argc);
	n_nodes = (unsigned) atoi(l_argv[0]);
	n_edges = (unsigned) atoi(l_argv[1]);
	if (!n_nodes || !n_edges)
	{
		fprintf (stderr, "Problem is trivial with %u nodes and %u edges\n",
						 n_nodes, n_edges);
		goto CLEANUP;
	}
	fprintf (stderr, "Reading graph on %u nodes and %u edges...", n_nodes,
					 n_edges);
	nodes = EGsMalloc (mpq_EGalgPRnode_t, n_nodes);
	edges = EGsMalloc (mpq_EGalgPRedge_t, n_edges);
	for (i = n_nodes; i--;)
	{
		mpq_EGalgPRnodeInit (nodes + i);
		EGeDgraphAddNode (&(G.G), &(nodes[i].v));
	}
	for (i = n_edges; i--;)
	{
		mpq_EGalgPRedgeInit (edges + i);
		EGioGets(l_str,1024,in_file);
		EGioNParse(l_str,128," ","%#",&l_argc,l_argv);
		TESTG((rval=(l_argc!=3)),CLEANUP,"Expected three tokens, but found %d",l_argc);
		head = atoi(l_argv[0]);
		tail = atoi(l_argv[1]);
		mpq_EGlpNumReadStr (cap, l_argv[2]);
		EGeDgraphAddEdge (&(G.G),
											&(nodes[head].v), &(nodes[tail].v), &(edges[i].fw.e));
		EGeDgraphAddEdge (&(G.G),
											&(nodes[tail].v), &(nodes[head].v), &(edges[i].bw.e));
		mpq_EGlpNumCopy (edges[i].fw.u, cap);
		mpq_EGlpNumCopy (edges[i].bw.u, cap);
	}
	EGioClose(in_file);
	in_file = 0;
	fprintf (stderr, "...done\nComputing Min Cut 0 %u:", n_nodes >> 1);

	/* now we call the min cut algorithm */
	EGtimerStart (&cut_timer);
	rval = mpq_EGalgPRminSTcut (&G, nodes, nodes + (n_nodes >> 1));
	EGtimerStop (&cut_timer);
	fprintf (stderr, "...done in %lf seconds\n", cut_timer.time);
	/* now display all nodes in the cut */
#if 0
	fprintf (stderr, "Minimum Cut: ");
	for (n_it = G.G.nodes.next; n_it != &(G.G.nodes); n_it = n_it->next)
	{
		node_pt = EGcontainerOf (n_it, mpq_EGalgPRnode_t, v.node_cn);
		if (node_pt->d < G.G.n_nodes)
			fprintf (stderr, "%u ", (unsigned) (node_pt - nodes));
	}
	fprintf (stderr, "\n");
#endif
	fprintf (stderr, "Computing max flow...");
	EGtimerReset (&cut_timer);
	EGtimerStart (&cut_timer);
	rval = mpq_EGalgPRmaxSTflow (&G, nodes, nodes + (n_nodes >> 1));
	EGtimerStop (&cut_timer);
	fprintf (stderr, "...done in %lf seconds\n", cut_timer.time);
	/* now we print the value of the cut */
	fprintf (stderr, "Minimum Cut Capacity %lf\n",
					 mpq_EGlpNumToLf (nodes[n_nodes >> 1].e));
	/* check the solution */
	rval = mpq_EGalgPRoptimalityTest (&G, nodes, nodes + (n_nodes >> 1), &cap);
	TESTG (rval, CLEANUP, "Worst error %lg, standard expected error %lg,"
				 " number of errors %d", mpq_EGlpNumToLf (cap), mpq_EGlpNumToLf (mpq_epsLpNum),
				 rval);
	CHECKRVALG (rval, CLEANUP);

	/* ending */
CLEANUP:
	if (in_file) EGioClose (in_file);
	mpq_EGlpNumClearVar (cap);
	mpq_EGalgPRgraphClear (&G);
	for (i = n_nodes; i--;)
		mpq_EGalgPRnodeClear (nodes + i);
	for (i = n_edges; i--;)
		mpq_EGalgPRedgeClear (edges + i);
	EGfree (nodes);
	EGfree (edges);
	mpq_EGalgPRprofile();
	EGlpNumClear();
	return rval;
}

/* ========================================================================= */
/** @} */
