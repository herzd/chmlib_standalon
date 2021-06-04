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
/* ========================================================================= */
/** @file
 * @ingroup EGalgMaxClosure */
/** @addtogroup EGalgMaxClosure */
/** @{ */
#include "EGlib.h"
static char *file_name = 0;
static int lvl = 5;
static int algo = EG_ALG_MAXCLOSURE_DEFAULT;
/* ========================================================================= */
/** @brief display usage of this program */
static void mc_usage (char **argv)
{
	fprintf (stdout, "Usage: %s [options]\n", argv[0]);
	fprintf (stdout, "Options:\n");
	fprintf (stdout, "     -a n   algorithm to use\n");
	fprintf (stdout, "     -v n   verbosity level.\n");
	fprintf (stdout, "     -f n   file name.\n");
}

/* ========================================================================= */
/** @brief parse input */
static inline int mc_parseargs (int argc,
																char **argv)
{
	int c;
	while ((c = getopt (argc, argv, "f:v:a:")) != EOF)
	{
		switch (c)
		{
		case 'f':
			file_name = optarg;
			break;
		case 'v':
			lvl = atoi (optarg);
			break;
		case 'a':
			algo = atoi (optarg);
			break;
		default:
			mc_usage (argv);
			return 1;
		}
	}
	/* test that we have an input file */
	if (!file_name)
	{
		mc_usage (argv);
		return 1;
	}
	/* report options */
	fprintf (stdout, "\tFile: %s\n\tVerbosity_Level: %d\n\tAlgorithm: %d\n",
					 file_name, lvl, algo);
	return 0;
}

/* ========================================================================= */
/** @brief example of usage of the Min Cut algorithm as implemented in
 * (EGalgMinCut).
 * 
 * We read a file from the input whose two first numbers are the number of nodes
 * and edges (we assume that the graph is undirected ), and then a 3-tupla with
 * head tail capacity. we use the last field (capacity) as the capacity of the
 * algorithm, and compute the minimum global cut. */
int main (int argc,
					char **argv)
{
	int rval = 0, n_read, n = 5, m = 4, *edges = 0, *closure = 0, csz = 0;
	double*weight = 0;
	register int i;
	int head, tail;
	char l_str[4096];
	char*l_argv[128];
	EGioFile_t *in_file = 0;
	EGtimer_t t;
	int edgesb[] = {3,0,3,1,4,1,4,2};
	double weightb[] = {-1.0,-1.0,-7.0,3.0,3.0};
	/* test we have an input file */
	EGlib_info();
	EGlib_version();
	EGcall(rval,CLEANUP,mc_parseargs (argc, argv));
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	/* test on simple example */
	closure = EGsMalloc(int,n);
	fprintf (stdout, "...done\nComputing Max Closure in test...");
	EGtimerReset(&t);
	EGtimerStart (&t);
	EGcall(rval, CLEANUP, EGalgMaxClosure(algo, n, m, edgesb, weightb, &csz, closure, lvl));
	EGtimerStop (&t);
	fprintf (stdout, "...done in %lf seconds\n\tClosure size: %d\n", t.time, csz);
	fprintf (stdout, "closure should have 3 nodes {0,1,3}\nclosure has %d nodes {",csz);
	for( i = 0 ; i < csz ; i++) fprintf (stdout, "%d,",closure[i]);
	fprintf (stdout,"\b}\n");
	EGfree(closure);
	/* build from file */
	in_file = EGioOpen (file_name, "r");
	if (!in_file)
	{
		fprintf (stdout, "Can't open file %s\n", argv[1]);
		mc_usage (argv);
		rval = 1;
		goto CLEANUP;
	}
	/* now we start reading the file */
	EGioGets(l_str,(size_t)4095,in_file);
	EGioNParse(l_str,128," ","%#",&n_read,l_argv);
	TESTG ((rval = (n_read != 2)), CLEANUP, "Can't read n_nodes and n_endges "
				 "from file");
	n = atoi(l_argv[0]);
	m = atoi(l_argv[1]);
	if (!n || !m)
	{
		fprintf (stdout, "Problem is trivial with %u nodes and %u edges\n",
						 n, m);
		n = m = 0;
		goto CLEANUP;
	}
	fprintf (stdout, "Reading graph on %u nodes and %u edges...", n, m);
	weight = EGsMalloc(double,n);
	closure = EGsMalloc(int,n);
	edges = EGsMalloc(int,2*m);
	for (i = m; i--;)
	{
		EGioGets(l_str,(size_t)4095,in_file);
		EGioNParse(l_str,128," ","%#",&n_read,l_argv);
		TESTG ((rval = (n_read != 3)), CLEANUP, "Can't read head tail capacity");
		edges[2*i] = head = atoi(l_argv[0]);
		edges[2*i+1] = tail = atoi(l_argv[1]);
		/* if the tail is even we add the weight, otherwise we substract, this is
		 * to be able to use the examples from TSPLIB solutions as inputs */
		if(tail%2==0) weight[head] += strtod(l_argv[2],0);
		else weight[head] -= strtod(l_argv[2],0);
	}
	EGioClose (in_file);
	in_file = 0;

	/* now we call the min cut algorithm */
	fprintf (stdout, "...done\nComputing Max Closure...");
	EGtimerReset(&t);
	EGtimerStart (&t);
	EGcall(rval, CLEANUP, EGalgMaxClosure(algo, n, m, edges, weight, &csz, closure, lvl));
	EGtimerStop (&t);
	fprintf (stdout, "...done in %lf seconds\n\tClosure size: %d\n", t.time, csz);
	/* ending */
	CLEANUP:
	EGfree(edges);
	EGfree(weight);
	EGfree(closure);
		return rval;
}

/* ========================================================================= */
/** @} */
