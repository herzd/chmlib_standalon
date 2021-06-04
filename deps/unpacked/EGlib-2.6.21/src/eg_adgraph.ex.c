/* EGlib "Efficient General Library" provides some basic structures and
 * algorithms commons in many optimization algorithms.
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
/** @file
 * @ingroup EGaDgraph
 * */
/** @addtogroup EGaDgraph */
/** @{ */
#include "EGlib.h"
/* ========================================================================= */
/** @brief example of a graph structure using the embeded substructures.
 * Note that we assume that the structure STARTS with EGaDG_t */
typedef struct my_adgraph_t
{
	EGaDG_t G;
	int ex_id;
}
adgraph_t;

/* ========================================================================= */
/** @brief example of a node structure using the embeded substructures. 
 * Note that we assume that the structure STARTS with EGaDN_t */
typedef struct my_adnode_t
{
	EGaDN_t v;
	int ex_id;
}
adnode_t;

/* ========================================================================= */
/** @brief example of an edge structure using the embeded substructures. 
 * Note that we assume that the structure STARTS with EGaDE_t */
typedef struct my_adedge_t
{
	EGaDE_t e;
	int ex_id;
}
adedge_t;

/* ========================================================================= */
/** @brief Display the contents of our graph structure */
static inline void display_ADG (const adgraph_t *const myG)
{
	int i,j;
	adnode_t *cn;
	adedge_t *ce;
	fprintf (stderr, "Graph %d (%d nodes, %d edges):\n", myG->ex_id, myG->G.n_nodes,
					 myG->G.n_edges);
	/* display node contents */
	for( i = 0 ; i < (int)(EGaDGgetNedges(myG)) ; i++)
	{
		ce = (adedge_t*)(EGaDGgetEdge(i,myG));
		fprintf(stderr,"\tEdge %d: %d %d\n",ce->ex_id, ce->e.tail, ce->e.head);
	}
	for( i = 0 ; i < (int)(EGaDGgetNnodes(myG)) ; i++)
	{
		cn = ((adnode_t*)(EGaDGgetNode(i,myG)));
		fprintf(stderr, "\tNode %d:\n\t\tIn %d: edges: ", cn->ex_id, (int)(EGaDGgetNin(cn)));
		for( j = 0 ; j < (int)(EGaDGgetNin(cn)) ; j++)
			fprintf(stderr,"%"PRIu32", ",
				EGaDGgetInE(j,cn,myG));
		fprintf(stderr,"\b\b\n\t\tOut %d: edges: ", (int)(EGaDGgetNout(cn)));
		for( j = 0 ; j < (int)(EGaDGgetNout(cn)) ; j++)
			fprintf(stderr,"%"PRIu32", ",
				EGaDGgetOutE(j,cn,myG));
		fprintf(stderr,"\b\b\n");
	}
}

/* ========================================================================= */
/** @brief A simple example of a directed graph using (EGdEgraph) structures.
 * @return zero on success, non-zero- otherwise.
 * @par Description:
 * Show how to use a directed graph, modify it and display it's contents */
int main (void)
{
	adgraph_t myG;
	adnode_t nodes[5];
	adedge_t edges[6];
	int i,rval;
	EGlib_info();
	EGlib_version();
	/* set values for edges */
	edges[0] = (adedge_t){(EGaDE_t){0,1},0};
	edges[1] = (adedge_t){(EGaDE_t){1,0},1};
	edges[2] = (adedge_t){(EGaDE_t){2,3},2};
	edges[3] = (adedge_t){(EGaDE_t){3,4},3};
	edges[4] = (adedge_t){(EGaDE_t){4,0},4};
	edges[5] = (adedge_t){(EGaDE_t){0,2},5};
	for( i = 0 ; i < 5; i++) nodes[i].ex_id = i;
	/* initialize all structures */
	EGaDGInit(&(myG.G),5,6,sizeof(adnode_t),sizeof(adedge_t),(char*)nodes,(char*)edges);
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	myG.ex_id = 0;
	display_ADG (&myG);
	/* note that this is a backward loop that should be more eficient than the
	 * regular forward loop (you save the last update condition ) */
	for (i = 5; i--;)
	{
		nodes[i].ex_id = i;
	}
	/* clear internal memory */
	CLEANUP:
	EGaDGClear(&myG);
	return 0;
}

/* ========================================================================= */
/** @} */
