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
 * @ingroup EGeBTree
 * */
/** @addtogroup EGeBTree */
/** @{ */
#include "EGlib.h"
static int verbose = 0;
/* ========================================================================= */
/** @brief usage function, if we give the wrong number of parameters, we return
 * an error message and print a help.
 * @param program Name of the command from comand-line
 * */
static void ebt_usage (char const *const program)
{
	fprintf (stderr, "Usage: %s [options]\n", program);
	fprintf (stderr, "Options:\n");
	fprintf (stderr, "    -v   Be verbose on screen\n");
	fprintf (stderr, "    -s   unsigned int, seed of the RNG\n");
	fprintf (stderr, "    -z   unsigned int, number of elements to "
					 "generate in the tree\n");
	/* we allways exit with an error code */
	exit (EXIT_FAILURE);
}

/* ========================================================================= */
/** @brief parse external arguments.
 * @param argc int, number of parameters to process.
 * @param argv char**, array of the parameters.
 * @param z unsigned int*, pointer to the number of elements in the tree.
 * @param s unsigned int*, pointer to the seed.
 * @return zero on success, non-zero otherwise */
static int ebt_parseargs (int argc,
																 char **argv,
																 unsigned int *s,
																 unsigned int *z)
{
	/* local variables */
	int c;
	/* set initial values */
	*z = 0;
	*s = 1;
	/* scan the input */
	while ((c = getopt (argc, argv, "s:z:v")) != EOF)
	{
		switch (c)
		{
		case 's':
			*s = (unsigned) atoi (optarg);
			break;
		case 'z':
			*z = (unsigned) atoi (optarg);
			break;
		case 'v':
			verbose = 1;
			break;
		default:
			ebt_usage (argv[0]);
		}
	}
	if (*z < 1)
	{
		ebt_usage (argv[0]);
		return 1;
	}
	fprintf (stderr, "Running %s with options:\n", argv[0]);
	fprintf (stderr, " verbose      %s\n", verbose ? "on" : "off");
	fprintf (stderr, " size         %7u\n", *z);
	fprintf (stderr, " seed         %7u\n", *s);
	return 0;
}

/* ========================================================================= */
/** @brief display the given tree in-order.
 * @param out_f output stream.
 * @param root tree to display.
 * @return zero on success, non-zero otherwise. */
static int ebt_display (EGioFile_t* out_f,
															 EGeBTree_t * root)
{
	unsigned sz = 0;
	EGeBTree_t *it = EGeBTreeGetFirst (root);
	if (verbose)
		EGioPrintf (out_f, "Display for tree %p:\n", (void *) root);
	for (; it; it = EGeBTreeGetNext (it))
	{
		if (verbose)
			EGioPrintf (out_f, "%zd ", it - root);
		sz++;
	}
	if (verbose)
		EGioPrintf (out_f, "\nReverse order:\n");
	for (it = EGeBTreeGetLast (root); it; it = EGeBTreeGetPrev (it))
	{
		if (verbose)
			EGioPrintf (out_f, "%zd ", it - root);
	}
	if (verbose)
		EGioPrintf (out_f, "\n\tsize: %u\n", sz);
	return 0;
}

/* ========================================================================= */
/** @brief Tester program for the projection structure and functions
 * @param argc int number of comand line options
 * @param argv char** array of strings of length argc contaianing the command
 * line arguments.
 * @return zero on success, non-zero otherwise 
 * @par Description:
 * This function create a set of 'z' elements in a bbtree, and print the
 * resulting tree, perform some random operations.
 * */
int main (int argc,
					char **argv)
{
	/* local variables */
	EGeBTree_t *all_nodes = 0;
	EGeBTree_t *my_tree = 0;
	EGioFile_t *out_f = 0;
	unsigned int n=0,
	  z=0,
	  s=0;
	int rval = 0;
	EGlib_info();
	EGlib_version();
	/* open file */
	out_f = EGioOpen("out.txt.gz","w+");
	if(!out_f) goto CLEANUP;
	/* now process the input */
	rval = ebt_parseargs (argc, argv, &s, &z);
	CHECKRVALG (rval, CLEANUP);
	srandom (s);
	my_tree = all_nodes = EGsMalloc (EGeBTree_t, z);
	EGeBTreeInit (my_tree);
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	ebt_display (out_f, my_tree);

	/* first create a random tree */
	for (n = 1; n < z; n++)
	{
		if (verbose)
			EGioPrintf (out_f, "Adding %d to the tree...\n", n);
		EGeBTreeInit (all_nodes + n);
		if ((n - 1) & 1)
			rval = EGeBTreeAddLeft (all_nodes + (n - 1) / 2, all_nodes + n);
		else
			rval = EGeBTreeAddRight (all_nodes + (n - 1) / 2, all_nodes + n);
		CHECKRVALG (rval, CLEANUP);
		ebt_display (out_f, my_tree);
	}
	/* now we depopulate the random tree */
	for (n = z; --n;)
	{
		rval = EGeBTreeDel (all_nodes + n);
		CHECKRVALG (rval, CLEANUP);
		ebt_display (out_f, my_tree);
	}
	/* re create tree */
	for (n = 1; n < z; n++)
	{
		if (verbose)
			EGioPrintf (out_f, "Adding %d to the tree...\n", n);
		if ((n - 1) & 1)
			rval = EGeBTreeAddLeft (all_nodes + (n - 1) / 2, all_nodes + n);
		else
			rval = EGeBTreeAddRight (all_nodes + (n - 1) / 2, all_nodes + n);
		CHECKRVALG (rval, CLEANUP);
		ebt_display (out_f, my_tree);
	}
	if (verbose)
		EGioPrintf (out_f, "delete %d of the tree...\n", z - 1);
	rval = EGeBTreeDel (all_nodes + z - 1);
	CHECKRVALG (rval, CLEANUP);
	ebt_display (out_f, my_tree);
	if (verbose)
		EGioPrintf (out_f, "replace %d with %d in the tree...\n", z / 2, z - 1);
	rval = EGeBTreeReplace (all_nodes + (z / 2), all_nodes + z - 1);
	CHECKRVALG (rval, CLEANUP);
	ebt_display (out_f, my_tree);
	if (verbose)
		EGioPrintf (out_f, "split tree at node %d...\n", 1);
	rval = EGeBTreeCut (all_nodes + 1);
	CHECKRVALG (rval, CLEANUP);
	ebt_display (out_f, my_tree);
	ebt_display (out_f, all_nodes + 1);
	if (verbose)
		EGioPrintf (out_f, "glue them back\n");
	rval = EGeBTreeAddRight (my_tree, all_nodes + 1);
	CHECKRVALG (rval, CLEANUP);
	ebt_display (out_f, my_tree);
	/* now we create the objective function */
	rval = EGioClose(out_f);
	CHECKRVALG (rval, CLEANUP);
CLEANUP:
	if (all_nodes)
		EGfree (all_nodes);
	return rval;
}

/* ========================================================================= */
/** @} */
/* end of eg_ebtree.ex.c */
