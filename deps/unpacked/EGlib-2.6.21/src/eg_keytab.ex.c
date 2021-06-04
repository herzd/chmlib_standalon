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
 * @ingroup EGkeytab */
/** @addtogroup EGkeytab */
/** @{ */
/* ========================================================================= */
/** @brief This program reads a file given from the input and try to put all
 * words within the symbol table. 
 * */
/* ========================================================================= */
#include "EGlib.h"
/* ========================================================================= */
/** @brief number of keys to generate */
static int nkeys = 100;
static int nlookup = 1000;
/* ========================================================================= */
/** @brief parsing arguments */
void keytab_usage(char*program)
{
	fprintf(stdout,"Usage: %s [options]\n",program);
	fprintf(stdout,"Options:\n\t-n n number of keys to generate\n");
	fprintf(stdout,"\t-l n number of keys to lookup\n");
}
/* ========================================================================= */
/** @brief parse input */
int keytab_parse(int argc,char**argv)
{
	int c;
	while((c = getopt(argc,argv,"l:n:")) != EOF)
	{
		switch(c)
		{
			case 'l':
				nlookup = atoi(optarg);
				break;
			case 'n':
				nkeys = atoi(optarg);
				break;
			default:
				keytab_usage(argv[0]);
				return 1;
		}
	}
	return 0;
}
/* ========================================================================= */
/** @brief main function */
int main (int argc,char**argv)
{
	int rval=0,i,pos;
	uint64_t key;
	EGrandState_t g1;
	EGkeytab_t keytab;
	EGlib_info();
	EGlib_version();
	EGrandInit(&g1);
	EGkeytabInit(&keytab);
	rval = keytab_parse(argc,argv);
	CHECKRVALG(rval,CLEANUP);
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	/* now we generate the keys */
	for( i = 0 ; i < nkeys ; i++)
	{
		key = EGrand(&g1);
		key = key << 32;
		key = EGrand(&g1);
		rval = EGkeytabAdd( &keytab, key, 1);
		rval = 0;
	}
	for( i = 0 ; i < nlookup ; i++)
	{
		key = EGrand(&g1);
		key = key << 32;
		key = EGrand(&g1);
		if(EGkeytabLookUp(&keytab,key,&pos))
			fprintf(stdout,"Found key %"PRIu64" in position %d\n",key,pos);
	}
	CLEANUP:
	EGkeytabClear(&keytab);
	return rval;
}
/* ========================================================================= */
/** @} */
