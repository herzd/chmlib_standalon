/* EGlib "Efficient General Library" provides some basic structures and
 * algorithms commons in many optimization algorithms.
 *
 * Copyright (C) 2005-2010 Daniel Espinoza
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
 * @ingroup EGgcIt */
/** @addtogroup EGgcIt */
/** @{ */
/* ========================================================================= */
/** @brief given a number, print to standard output a gray code on n bits. */
/* ========================================================================= */
#include "EGlib.h"

/* ========================================================================= */
/** @brief display the usage message for this program */
void gcit_usage (char *program)
{
	fprintf (stdout, "Usage: %s n\n", program);
	fprintf (stdout, "\twhere n is the number of bits in the graycode\n");
}

/* ========================================================================= */
/** @brief main function */
int main (int argc,
					char **argv)
{
	char* string=0;
	int n = 0, i, redo, pos, rval=0;
	EGgcIt_t gc;
	EGlib_info();
	EGlib_version();
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	if((argc!=2) || ((n=atoi(argv[1]))<1))
	{
		gcit_usage(argv[0]);
		return 1;
	}
	fprintf(stderr,"using gray code of %d (%s) bits:\n", n, argv[1]);
	EGgcItInit(&gc,n);
	string = EGsMalloc(char,n+1);
	string[n]='\0';
	/* initialization */
	for( i = n ; i-- ; )
	{
		if((EGgcItGetTuple(&gc))[i]) string[i]='1';
		else string[i]='0';
	}
	/* main loop */
	do
	{
		fprintf(stdout,"%s\n",string);
		redo = EGgcItNext(&gc);
		pos = EGgcItGetChange(&gc);
		if(string[pos] == '0') 
			string[pos] = '1';
		else 
			string[pos]='0';
	}while(redo);

	/* Liberating allocated memory */
	CLEANUP:
	EGgcItClear(&gc);
	EGfree(string);
	return 0;
}

/* ========================================================================= */
/** @} */
