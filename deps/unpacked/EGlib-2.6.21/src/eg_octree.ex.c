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
 * @ingroup EGeOctree */
/** @addtogroup EGeOctree */
/** @{ */
/* ========================================================================= */
/** @brief This program reads from a file with at least three columns the
 * following information:
 * max_x max_y max_z n (everything else discarded in the line in every line)
 * x_0 y_0 z_0
 * x_1 y_1 z_1
 *   ...  
 * x_n y_n z_n
 *
 * store the structure in an octree, and show different forms of traversing it.
 * */
/* ========================================================================= */
#include "EGlib.h"

/* ========================================================================= */
/** @brief display the usage message for this program */
void oct_usage (char *program)
{
	fprintf (stdout, "Usage: %s [options]\n", program);
	fprintf (stdout, "Options:\n");
	fprintf (stdout, "     -f n   file name.\n");

}

/* ========================================================================= */
/** @brief parse the input argumenbts for the program */
int oct_parseargs (int argc,
									 char **argv,
									 char **file_name)
{
	int c;
	while ((c = getopt (argc, argv, "f:")) != EOF)
	{
		switch (c)
		{
		case 'f':
			*file_name = optarg;
			break;
		default:
			oct_usage (argv[0]);
			return 1;
		}
	}
	/* reporting the options */
	if (!*file_name)
	{
		oct_usage (argv[0]);
		return 1;
	}
	fprintf (stdout, "Parsed Options:\n");
	fprintf (stdout, "input         : %s\n", *file_name);
	return 0;
}

/* ========================================================================= */
/** @brief main function */
int main (int argc,
					char **argv)
{
	unsigned max_x, max_y, max_z, n, i;
	int rval = 0;
	char *file_name = 0,
	  str1[1024], *argv1[128];
	int argc1;
	EGioFile_t *file = 0;
	EGrandState_t seed;
	EGeOctdata_t*alldata = 0, *cur, *next;
	EGeOctree_t oct;

	/* Parse command line input to get 'file name' and 'd'. */
	EGlib_info();
	EGlib_version();
	rval = oct_parseargs (argc, argv, &file_name);
	CHECKRVALG (rval, CLEANUP);
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);

	/* Read the objects to sort from the file */
	file = EGioOpen (file_name, "r");
	TEST (!file, "unable to open file %s", file_name);
	EGioGets(str1,1024,file);
	EGioNParse(str1,128," ","%#",&argc1,argv1);
	TESTG((rval=(argc1<4)),CLEANUP,"Expected at least four tokens, found %d",
				argc1);
	max_x = (unsigned)atoi(argv1[0]);
	max_y = (unsigned)atoi(argv1[1]);
	max_z = (unsigned)atoi(argv1[2]);
	n = (unsigned)atoi(argv1[3]);
	alldata = EGsMalloc(EGeOctdata_t,n);
	rval = EGeOctinit(&oct, (uint16_t)max_x, (uint16_t)max_y, (uint16_t)max_z, 0, EGeOctalloc, EGeOctfree);
	CHECKRVALG(rval,CLEANUP);
	/* we add all elements */
	for( i = 0 ; i < n ; i++)
	{
		do{
			EGioGets(str1,1024,file);
			EGioNParse(str1,128," ","%#",&argc1,argv1);
		} while(!EGioEof(file) && !argc1);
		TESTG((rval=(argc1<3)),CLEANUP,"Expected at least three tokens, found %d",
						argc1);
		alldata[i].x = (uint16_t)atoi(argv1[0]);
		alldata[i].y = (uint16_t)atoi(argv1[1]);
		alldata[i].z = (uint16_t)atoi(argv1[2]);
		rval = EGeOctaddleaf(&oct,alldata+i);
		CHECKRVALG(rval,CLEANUP);
	}
	/* now we iterate through all nodes at depth 3 */
	cur = (EGeOctdata_t*)oct.root;
	while(cur)
	{
		if(cur->depth == 3U)
		{
			fprintf(stdout,"Branch %p {%5u,%5u,%5u,%2u,%1u,%1u,%p}\n", cur,
							cur->x, cur->y, cur->z, cur->depth, cur->key, cur->nson,
							cur->up);
			next = EGeOctbrother(cur);
			if(!next) next = EGeOctcousin(cur);
		}
		else
		{
			next = EGeOctson(cur);
			if(!next) next = EGeOctbrother(cur);
			if(!next) next = EGeOctcousin(cur);
		}
		cur = next;
	}
	/* now we randomly find points in the grid */
	EGrandInit(&seed);
	for( i = 0 ; i < 50 ; i++)
	{
		max_x = (unsigned)EGrandInt(&seed,0,oct.max_x);
		max_y = (unsigned)EGrandInt(&seed,0,oct.max_y);
		max_z = (unsigned)EGrandInt(&seed,0,oct.max_z);
		cur = EGeOctfind(&oct, 0, max_x, max_y, max_z);
		if(cur)
			fprintf(stdout,"Found %p {%5u,%5u,%5u,%2u,%1u,%1u,%p}\n", cur,
							cur->x, cur->y, cur->z, cur->depth, cur->key, cur->nson,
							cur->up);
		else
			fprintf(stdout,"Not Found {%5u,%5u,%5u}\n",max_x, max_y, max_z);
	}
	/* find aproximate branches at level 2 */
	for( i = 0 ; i < 50 ; i++)
	{
		max_x = (unsigned)EGrandInt(&seed,0,oct.max_x);
		max_y = (unsigned)EGrandInt(&seed,0,oct.max_y);
		max_z = (unsigned)EGrandInt(&seed,0,oct.max_z);
		cur = EGeOctfind(&oct, 2, max_x, max_y, max_z);
		if(cur)
			fprintf(stdout,"Aprox Found %p {%5u,%5u,%5u,%2u,%1u,%1u,%p}\n", cur,
							cur->x, cur->y, cur->z, cur->depth, cur->key, cur->nson,
							cur->up);
		else
			fprintf(stdout,"Aprox Not Found {%5u,%5u,%5u}\n",max_x, max_y, max_z);
	}

	/* now we select a point and delete it */
	for( i =0 ; i < 100 ; i++)
	{
		argc1 = EGrandInt(&seed,0,((int)n)-1);
		fprintf(stdout,"Delete leaf %d\n",argc1);
		rval = EGeOctdelleaf(&oct,alldata+argc1);
		CHECKRVALG(rval,CLEANUP);
	}
	/* Liberating allocated memory */
CLEANUP:
	if(file) EGioClose(file);
	EGeOctclear(&oct);
	if(alldata) EGfree(alldata);
	return rval;
}

/* ========================================================================= */
/** @} */

