/* EGlib "Efficient General Library" provides some basic structures and
 * algorithms commons in many optimization algorithms.
 *
 * Copyright (C) 2005 Daniel Espinoza
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
 * @ingroup EGeKHeap */
/** @addtogroup EGeKHeap */
/** @{ */
/* ========================================================================= */
/** @brief This program reads a list of double values from a text file and 
 * uses a heap to sort them. It also allows the user to change the value of 
 * one of the read numbers after they have been placed in the heap. The 
 * purpose of this program is to illustrate the use of the @ref EGeKHeap 
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
void dbl_ekheap_usage (char *program)
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
int dbl_ekheap_parseargs (int argc,
										 char **argv,
										 unsigned int *d,
										 unsigned int *ch,
										 double * v,
										 char **dbl_file_name)
{
	int c;
	while ((c = getopt (argc, argv, "f:d:c:v:")) != EOF)
	{
		switch (c)
		{
		case 'f':
			*dbl_file_name = optarg;
			break;
		case 'd':
			*d = atoi (optarg);
			break;
		case 'c':
			*ch = atoi (optarg);
			break;
		case 'v':
			dbl_EGlpNumReadStr (*v, optarg);
			break;
		default:
			dbl_ekheap_usage (argv[0]);
			return 1;
		}
	}
	/* reporting the options */
	if (!*dbl_file_name)
	{
		dbl_ekheap_usage (argv[0]);
		return 1;
	}
	fprintf (stdout, "Parsed Options:\n");
	fprintf (stdout, "input         : %s\n", *dbl_file_name);
	fprintf (stdout, "d             : %u\n", *d);
	if (*ch != UINT_MAX)
	{
		fprintf (stdout, "c             : %u\n", *ch);
		fprintf (stdout, "v             : %lf\n", dbl_EGlpNumToLf (*v));
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
	char *dbl_file_name = 0,
	  str1[1024], *argv1[128];
	int argc1;
	EGioFile_t *file = 0;
	unsigned int nval = 0;
	double v[dbl_EG_EKHEAP_ENTRY];
	dbl_EGeKHeap_t my_heap;
	dbl_EGeKHeapCn_t *all_cn = 0,
	 *ccn;
	EGlib_info();
	EGlib_version();
	EGlpNumStart();
	for( i = dbl_EG_EKHEAP_ENTRY ; i-- ; )
	{
		dbl_EGlpNumInitVar (v[i]);
		dbl_EGlpNumZero (v[i]);
	}
	dbl_EGeKHeapInit (&my_heap);
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	rval = dbl_EGeKHeapCheck (&my_heap);
	CHECKRVALG (rval, CLEANUP);

	/* Parse command line input to get 'file name' and 'd'. */
	rval = dbl_ekheap_parseargs (argc, argv, &d, &c, v, &dbl_file_name);
	CHECKRVALG (rval, CLEANUP);
	dbl_EGeKHeapChangeD (&my_heap, d);
	rval = dbl_EGeKHeapCheck (&my_heap);
	CHECKRVALG (rval, CLEANUP);

	/* Read the objects to sort from the file */
	file = EGioOpen (dbl_file_name, "r");
	TEST (!file, "unable to open file %s", dbl_file_name);
	EGioGets(str1,1024,file);
	EGioNParse(str1,128," ","%#",&argc1,argv1);
	TESTG((rval=(argc1!=1)),CLEANUP,"Expected one token, found %d",argc1);
	nval = atoi(argv1[0]);
	all_cn = EGsMalloc (dbl_EGeKHeapCn_t, nval);
	dbl_EGeKHeapResize (&my_heap, nval);
	rval = dbl_EGeKHeapCheck (&my_heap);
	CHECKRVALG (rval, CLEANUP);
	IFMESSAGE(1, "Inserting %u elements into the heap", nval);
	for (i = 0; i < nval; i++)
	{
		dbl_EGeKHeapCnInit (all_cn + i);
		EGioGets(str1,1024,file);
		EGioNParse(str1,128," ","%#",&argc1,argv1);
		TESTG((rval=(argc1!=1)),CLEANUP,"Expected one token, found %d",argc1);
		dbl_EGlpNumReadStr (all_cn[i].val[0], argv1[0]);
		IFMESSAGE(1, "Adding value (%s,%lf) to the heap", str1,
						 dbl_EGlpNumToLf (all_cn[i].val[0]));
		rval = dbl_EGeKHeapAdd (&my_heap, all_cn + i);
		CHECKRVALG (rval, CLEANUP);
		rval = dbl_EGeKHeapCheck (&my_heap);
		CHECKRVALG (rval, CLEANUP);
	}
	EGioClose (file);
	file = 0;
	/* Check if change value is in range */
	TESTG (c != UINT_MAX && c >= nval, CLEANUP,
				 "Change item (%u) is out of range (only %u objects)", c, nval);

	/* Popping the values from the heap */
	fprintf (stderr, "\nRemoving:\n\n");
	for (i = 0; i < nval; i++)
	{
		ccn = dbl_EGeKHeapGetMin (&my_heap);
		IFMESSAGE(1, "%u: item %zd : %lf", i, ccn - all_cn,
						 dbl_EGlpNumToLf (ccn->val[0]));
		dbl_EGeKHeapDel (&my_heap, ccn);
		rval = dbl_EGeKHeapCheck (&my_heap);
		CHECKRVALG (rval, CLEANUP);
	}

	if (c == UINT_MAX)
		goto CLEANUP;

	/* Re-inserting the values into the heap */
	fprintf (stderr, "\nRe-Inserting.\n\n");
	for (i = 0; i < nval; i++)
	{
		rval = dbl_EGeKHeapAdd (&my_heap, all_cn + i);
		CHECKRVALG (rval, CLEANUP);
		rval = dbl_EGeKHeapCheck (&my_heap);
		CHECKRVALG (rval, CLEANUP);
	}

	/* Changing value of an item */
	fprintf (stderr, "Changing value of item %u from %lf to %lf.\n", c,
					 dbl_EGlpNumToLf (all_cn[c].val[0]), dbl_EGlpNumToLf (v[0]));
	rval = dbl_EGeKHeapChangeVal (&my_heap, all_cn + c, v);
	CHECKRVALG (rval, CLEANUP);
	rval = dbl_EGeKHeapCheck (&my_heap);
	CHECKRVALG (rval, CLEANUP);

	/* Popping the values from the heap */
	fprintf (stderr, "\nRemoving:\n\n");
	for (i = 0; i < nval; i++)
	{
		ccn = dbl_EGeKHeapGetMin (&my_heap);
		IFMESSAGE(1, "%u: item %zd : %lf", i, ccn - all_cn,
						 dbl_EGlpNumToLf (ccn->val[0]));
		dbl_EGeKHeapDel (&my_heap, ccn);
		rval = dbl_EGeKHeapCheck (&my_heap);
		CHECKRVALG (rval, CLEANUP);
	}

	/* Liberating allocated memory */
CLEANUP:
	if(file) EGioClose(file);
	if (all_cn)
	{
		for( i = nval ; i-- ; ) dbl_EGeKHeapCnClear(all_cn+i);
		EGfree (all_cn);
	}
	dbl_EGeKHeapClear (&my_heap);
	for( i = dbl_EG_EKHEAP_ENTRY ; i-- ; ) dbl_EGlpNumClearVar (v[i]);
	EGlpNumClear();
	return rval;
}
/* ========================================================================= */
/** @} */
