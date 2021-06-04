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
 * @ingroup EGeHeap */
/** @addtogroup EGeHeap */
/** @{ */
/* ========================================================================= */
/** @brief This program reads a list of double values from a text file and 
 * uses a heap to sort them. It also allows the user to change the value of 
 * one of the read numbers after they have been placed in the heap. The 
 * purpose of this program is to illustrate the use of the @ref EGeHeap 
 * structure and its associated functions. 
 * 
 * The input file format is: 
 * n
 * Value_0
 * Value_1
 * Value_2
 * Value_3
 *   ...  
 * Value_n 
 * */
/* ========================================================================= */
#include "EGlib.h"

/* ========================================================================= */
/** @brief display the usage message for this program */
void ldbl_eheap_usage (char *program)
{
	fprintf (stdout, "Usage: %s [options]\n", program);
	fprintf (stdout, "Options:\n");
	fprintf (stdout, "     -d n   'd' value.\n");
	fprintf (stdout, "     -f n   file name.\n");
	fprintf (stdout, "     -c n   item whose value to change.\n");
	fprintf (stdout, "     -v n   new item value (0 by default).\n");

}

/* ========================================================================= */
/** @brief parse the input argumenbts for the program */
int ldbl_eheap_parseargs (int argc,
										 char **argv,
										 unsigned int *d,
										 unsigned int *ch,
										 long double * v,
										 char **ldbl_file_name)
{
	int c;
	while ((c = getopt (argc, argv, "f:d:c:v:")) != EOF)
	{
		switch (c)
		{
		case 'f':
			*ldbl_file_name = optarg;
			break;
		case 'd':
			*d = atoi (optarg);
			break;
		case 'c':
			*ch = atoi (optarg);
			break;
		case 'v':
			ldbl_EGlpNumReadStr (*v, optarg);
			break;
		default:
			ldbl_eheap_usage (argv[0]);
			return 1;
		}
	}
	/* reporting the options */
	if (!*ldbl_file_name)
	{
		ldbl_eheap_usage (argv[0]);
		return 1;
	}
	fprintf (stdout, "Parsed Options:\n");
	fprintf (stdout, "input         : %s\n", *ldbl_file_name);
	fprintf (stdout, "d             : %u\n", *d);
	if (*ch != UINT_MAX)
	{
		fprintf (stdout, "c             : %u\n", *ch);
		fprintf (stdout, "v             : %lf\n", ldbl_EGlpNumToLf (*v));
	}
	return 0;
}

/* ========================================================================= */
/** @brief main function */
int main (int argc,
					char **argv)
{

	int rval = 0;
	unsigned int i,
	  c = UINT_MAX,
	  d = 2;
	char *ldbl_file_name = 0,
	  str1[1024];
	FILE *file;
	unsigned int nval;
	long double v;
	ldbl_EGeHeap_t my_heap;
	ldbl_EGeHeapCn_t *all_cn = 0,
	 *ccn;
	EGlib_info();
	EGlib_version();
	EGlpNumStart();
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	ldbl_EGlpNumInitVar (v);
	ldbl_EGlpNumZero (v);
	ldbl_EGeHeapInit (&my_heap);
	rval = ldbl_EGeHeapCheck (&my_heap);
	CHECKRVALG (rval, CLEANUP);

	/* Parse command line input to get 'file name' and 'd'. */
	rval = ldbl_eheap_parseargs (argc, argv, &d, &c, &v, &ldbl_file_name);
	CHECKRVALG (rval, CLEANUP);
	ldbl_EGeHeapChangeD (&my_heap, d);
	rval = ldbl_EGeHeapCheck (&my_heap);
	CHECKRVALG (rval, CLEANUP);

	/* Read the objects to sort from the file */
	file = fopen (ldbl_file_name, "r");
	TEST (!file, "unable to open file %s", ldbl_file_name);
	fscanf (file, "%u", &nval);
	all_cn = EGsMalloc (ldbl_EGeHeapCn_t, nval);
	ldbl_EGeHeapResize (&my_heap, nval);
	rval = ldbl_EGeHeapCheck (&my_heap);
	CHECKRVALG (rval, CLEANUP);
	IFMESSAGE (1, "Inserting %u elements into the heap", nval);
	for (i = 0; i < nval; i++)
	{
		ldbl_EGeHeapCnInit (all_cn + i);
		fscanf (file, "%s", str1);
		ldbl_EGlpNumReadStr (all_cn[i].val, str1);
		IFMESSAGE (1, "Adding value (%s,%lg) to the heap", str1,
						 ldbl_EGlpNumToLf (all_cn[i].val));
		rval = ldbl_EGeHeapAdd (&my_heap, all_cn + i);
		CHECKRVALG (rval, CLEANUP);
		rval = ldbl_EGeHeapCheck (&my_heap);
		CHECKRVALG (rval, CLEANUP);
	}
	fclose (file);

	/* Check if change value is in range */
	TESTG (c != UINT_MAX && c >= nval, CLEANUP,
				 "Change item (%u) is out of range (only %u objects)", c, nval);

	/* Popping the values from the heap */
	fprintf (stderr, "\nRemoving:\n\n");
	for (i = 0; i < nval; i++)
	{
		ccn = ldbl_EGeHeapGetMin (&my_heap);
		IFMESSAGE (1, "%u: item %zd : %lg", i, ccn - all_cn,
						 ldbl_EGlpNumToLf (ccn->val));
		ldbl_EGeHeapDel (&my_heap, ccn);
		rval = ldbl_EGeHeapCheck (&my_heap);
		CHECKRVALG (rval, CLEANUP);
	}

	if (c == UINT_MAX)
		goto CLEANUP;

	/* Re-inserting the values into the heap */
	fprintf (stderr, "\nRe-Inserting.\n\n");
	for (i = 0; i < nval; i++)
	{
		rval = ldbl_EGeHeapAdd (&my_heap, all_cn + i);
		CHECKRVALG (rval, CLEANUP);
		rval = ldbl_EGeHeapCheck (&my_heap);
		CHECKRVALG (rval, CLEANUP);
	}

	/* Changing value of an item */
	fprintf (stderr, "Changing value of item %u from %lf to %lf.\n", c,
					 ldbl_EGlpNumToLf (all_cn[c].val), ldbl_EGlpNumToLf (v));
	rval = ldbl_EGeHeapChangeVal (&my_heap, all_cn + c, v);
	CHECKRVALG (rval, CLEANUP);
	rval = ldbl_EGeHeapCheck (&my_heap);
	CHECKRVALG (rval, CLEANUP);

	/* Popping the values from the heap */
	fprintf (stderr, "\nRemoving:\n\n");
	for (i = 0; i < nval; i++)
	{
		//ccn = ldbl_EGeHeapGetMin (&my_heap);
		ccn = my_heap.cn[my_heap.sz-1];
		IFMESSAGE (1, "%u: item %zd : %lf", i, ccn - all_cn,
						 ldbl_EGlpNumToLf (ccn->val));
		ldbl_EGeHeapDel (&my_heap, ccn);
		rval = ldbl_EGeHeapCheck (&my_heap);
		CHECKRVALG (rval, CLEANUP);
	}

	/* Liberating allocated memory */
CLEANUP:
	if (all_cn) EGfree (all_cn);
	ldbl_EGeHeapClear (&my_heap);
	ldbl_EGlpNumClearVar (v);
	EGlpNumClear();
	return rval;
}
/* ========================================================================= */
/** @} */
