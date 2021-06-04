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
 * @ingroup EGaBheap */
/** @addtogroup EGaBheap */
/** @{ */
/* ========================================================================= */
/** @brief This program reads a list of double values from a text file and 
 * uses a heap to sort them. It also allows the user to change the value of 
 * one of the read numbers after they have been placed in the heap. The 
 * purpose of this program is to illustrate the use of the @ref EGaBheap 
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
void int_aheap_usage (char *program)
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
int int_aheap_parseargs (int argc,
										 char **argv,
										 unsigned int *d,
										 unsigned int *ch,
										 int * v,
										 char **int_file_name)
{
	int c;
	while ((c = getopt (argc, argv, "f:d:c:v:")) != EOF)
	{
		switch (c)
		{
		case 'f':
			*int_file_name = optarg;
			break;
		case 'd':
			*d = atoi (optarg);
			break;
		case 'c':
			*ch = atoi (optarg);
			break;
		case 'v':
			int_EGlpNumReadStr (*v, optarg);
			break;
		default:
			int_aheap_usage (argv[0]);
			return 1;
		}
	}
	/* reporting the options */
	if (!*int_file_name)
	{
		int_aheap_usage (argv[0]);
		return 1;
	}
	fprintf (stdout, "Parsed Options:\n");
	fprintf (stdout, "input         : %s\n", *int_file_name);
	fprintf (stdout, "d             : %u\n", *d);
	if (*ch != UINT_MAX)
	{
		fprintf (stdout, "c             : %u\n", *ch);
		fprintf (stdout, "v             : %lf\n", int_EGlpNumToLf (*v));
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
	char *int_file_name = 0,
	  str1[1024];
	FILE *file;
	unsigned int nval,ccn;
	int v;
	int_EGaBheap_t my_heap;
	int_EGaBheapCn_t *all_cn = 0;
	EGlib_info();
	EGlib_version();
	EGlpNumStart();
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	int_EGlpNumInitVar (v);
	int_EGlpNumZero (v);

	/* Parse command line input to get 'file name' and 'd'. */
	EGcallD(int_aheap_parseargs(argc, argv, &d, &c, &v, &int_file_name));
	int_EGaBheapInit(&my_heap);

	/* Read the objects to sort from the file */
	file = fopen (int_file_name, "r");
	TEST (!file, "unable to open file %s", int_file_name);
	fscanf (file, "%u", &nval);
	all_cn=EGsMalloc(int_EGaBheapCn_t, nval);
	int_EGaBheapResize(&my_heap, nval);
	IFMESSAGE (1, "Inserting %u elements into the heap", nval);
	for (i = 0; i < nval; i++)
	{
		int_EGaBheapCnInit(all_cn+i);
		fscanf (file, "%s", str1);
		int_EGlpNumReadStr (all_cn[i].val, str1);
		IFMESSAGE (1, "Adding value (%s,%lg) to the heap", str1, int_EGlpNumToLf (all_cn[i].val));
		int_EGaBheapAdd (all_cn,sizeof(int_EGaBheapCn_t),&my_heap,i);
	}
	fclose (file);

	/* Check if change value is in range */
	TESTG (c != UINT_MAX && c >= nval, CLEANUP,
				 "Change item (%u) is out of range (only %u objects)", c, nval);

	/* Popping the values from the heap */
	fprintf (stderr, "\nRemoving:\n\n");
	for (i = 0; i < nval; i++)
	{
		ccn = int_EGaBheapGetMin (&my_heap);
		IFMESSAGE (1, "%u: item %u : %lg", i, ccn, int_EGlpNumToLf (all_cn[ccn].val));
		int_EGaBheapDel (all_cn,sizeof(int_EGaBheapCn_t),&my_heap, ccn);
	}

	if (c == UINT_MAX)
		goto CLEANUP;

	/* Re-inserting the values into the heap */
	fprintf (stderr, "\nRe-Inserting.\n\n");
	for (i = 0; i < nval; i++) int_EGaBheapAdd (all_cn,sizeof(int_EGaBheapCn_t),&my_heap, i);

	/* Changing value of an item */
	fprintf (stderr, "Changing value of item %u from %lf to %lf.\n", c, int_EGlpNumToLf (all_cn[c].val), int_EGlpNumToLf (v));
	int_EGaBheapChangeVal (all_cn,sizeof(int_EGaBheapCn_t),&my_heap,c,v);
	/* Popping the values from the heap */
	fprintf (stderr, "\nRemoving:\n\n");
	for (i = 0; i < nval; i++)
	{
		ccn = my_heap.cn[my_heap.sz-1];
		IFMESSAGE (1, "%u: item %u : %lf", i, ccn, int_EGlpNumToLf (all_cn[ccn].val));
		int_EGaBheapDel (all_cn,sizeof(int_EGaBheapCn_t),&my_heap, ccn);
	}

	/* Liberating allocated memory */
CLEANUP:
	if (all_cn) EGfree(all_cn);
	int_EGaBheapClear(&my_heap);
	int_EGlpNumClearVar(v);
	EGlpNumClear();
	return rval;
}
/* ========================================================================= */
/** @} */
