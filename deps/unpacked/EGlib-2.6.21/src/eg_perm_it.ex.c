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
 * @ingroup EGpermIt */
/** @addtogroup EGpermIt */
/** @{ */
/* ========================================================================= */
/** @brief given a number, print to standard output a gray code on n bits. */
/* ========================================================================= */
#include "EGlib.h"

/* ========================================================================= */
/** @brief display the usage message for this program */
void prit_usage (char *program)
{
	fprintf (stdout, "Usage: %s n y\n", program);
	fprintf (stdout, "\twhere n is the number of elements to permutate\n\ty should be zero (non verbose) or one (verbose) output\n");
}

/* ========================================================================= */
/** @brief main function */
int main (int argc,
					char **argv)
{
	int n = 0, i, redo, pos,verbose=2,rval=0;
	const int*tuple = 0;
	EGpermIt_t prit;
	EGlib_info();
	EGlib_version();
	if((argc<2) ||(argc>3) || ((n=atoi(argv[1]))<1))
	{
		prit_usage(argv[0]);
		return 1;
	}
	if(argc == 3) verbose = atoi(argv[2]);
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	fprintf(stderr,"using full permutator over %d (%s) elements:\n", n, argv[1]);
	/* initialization */
	EGpermItInit(&prit,n);
	tuple = EGpermItGetTuple(&prit);
	/* main loop */
	do
	{
		if(verbose)
		{
			for( i = 0 ; i < n ; i++) fprintf(stdout,"%d ",tuple[i]);
		}
		redo = EGpermItNext(&prit);
		pos = EGpermItGetChange(&prit);
		if(verbose) fprintf(stdout," change %d\n",pos);
	}while(redo);

	/* Liberating allocated memory */
	CLEANUP:
	EGpermItClear(&prit);
	return 0;
}

/* ========================================================================= */
/** @} */
