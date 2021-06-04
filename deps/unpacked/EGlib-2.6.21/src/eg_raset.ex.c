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
 * @ingroup EGrSet */
/** @addtogroup EGrSet */
/** @{ */
/* ========================================================================= */
/** @brief given a groud set size, perform the given number of
 * insertion/deletion operations, and grow the set twice. */
/* ========================================================================= */
#include "EGlib.h"

/* ========================================================================= */
/** @brief display the usage message for this program */
void usage (char *program)
{
	fprintf (stdout, "Usage: %s sz nop [display]\n", program);
	fprintf (stdout, "\twhere `sz' is the starting size of the groud sent\n");
	fprintf (stdout, "\twhere `nop' is the number of random additions/deletions/peek operations to perform in every loop\n");
	fprintf (stdout, "\twhere `display' is an integer setting the display level\n");
}

/* ========================================================================= */
/** @brief main function */
int main (int argc,
					char **argv)
{
	int grsz = 0, nop=0, i,j,cnt=0, display=0,rval;
	uint32_t k;
	EGrandState_t seed;
	EGrset_t set=EGrsetNull;
	EGtimer_t t1, t2, t3;
	EGlib_info();
	EGlib_version();
	EGtimerReset(&t1);
	EGtimerReset(&t2);
	EGtimerReset(&t3);
	EGrandInit(&seed);
	EGrsetInit(&set,0);
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	if((argc<3) || (argc>4) || ((grsz=atoi(argv[1]))<1) || ((nop=atoi(argv[2]))<1))
	{
		usage(argv[0]);
		return 1;
	}
	if(argc==4) display=atoi(argv[3]);
	fprintf(stdout,"\tGroud set size: %d\n\tOperations: %d\n\tDisplay %d\n", grsz, nop, display);
	/* starting log */
	EGrsetSetgrnd(&set,10);
	EGcallD(EGrsetCheck(&set));
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetAdd(&set,0);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetAdd(&set,4);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetAdd(&set,4);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetAdd(&set,4);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetAdd(&set,4);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetAdd(&set,6);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetAdd(&set,1);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetAdd(&set,8);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetAdd(&set,9);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetAdd(&set,3);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetAdd(&set,5);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetAdd(&set,7);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetAdd(&set,2);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetDel(&set,0);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetDel(&set,0);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetDel(&set,0);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetDel(&set,0);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetDel(&set,1);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetDel(&set,2);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetDel(&set,3);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetDel(&set,4);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetDel(&set,5);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetDel(&set,6);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetDel(&set,7);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetDel(&set,8);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetDel(&set,9);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetDel(&set,9);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetDel(&set,9);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetDel(&set,9);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetAdd(&set,9);
	EGrsetAdd(&set,0);
	EGrsetAdd(&set,3);
	EGrsetAdd(&set,5);
	EGrsetAdd(&set,7);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetSetgrnd(&set,8);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetSetgrnd(&set,10);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetSetgrnd(&set,6);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetSetgrnd(&set,9);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetSetgrnd(&set,4);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	EGrsetSetgrnd(&set,8);
	if(display) EGrsetDisplay(&set,stdout);
	EGcallD(EGrsetCheck(&set));
	/* main loop */
	for(i=1 ; i < 4 ; i++)
	{
		if(display) fprintf(stdout,"==========================================\nWorking on ground %d\n==========================================\n",grsz*i);
		EGrsetSetgrnd(&set,grsz*i);
		if(display) EGrsetDisplay(&set,stdout);
		EGcallD(EGrsetCheck(&set));
		EGtimerStart(&t1);
		for(j=nop;j--;)
		{
			k=EGrand(&seed)%(grsz*i);
			EGrsetAdd(&set,(int32_t)k);
		}
		EGtimerStop(&t1);
		if(display) EGrsetDisplay(&set,stdout);
		EGcallD(EGrsetCheck(&set));
		EGtimerStart(&t3);
		for(j=nop;j--;)
		{
			k=EGrand(&seed)%(grsz*i);
			cnt+=EGrsetPeek(&set,k);
		}
		EGtimerStop(&t3);
		if(display) EGrsetDisplay(&set,stdout);
		EGcallD(EGrsetCheck(&set));
		EGtimerStart(&t2);
		for(j=nop;j--;)
		{
			k=EGrand(&seed)%(grsz*i);
			EGrsetDel(&set,(int32_t)k);
		}
		EGtimerStop(&t2);
	}
	for(i=4 ; i > 0 ; i--)
	{
		if(display) fprintf(stdout,"==========================================\nWorking on ground %d\n==========================================\n",grsz*i);
		EGrsetSetgrnd(&set,grsz*i);
		if(display) EGrsetDisplay(&set,stdout);
		EGcallD(EGrsetCheck(&set));
		EGtimerStart(&t1);
		for(j=nop;j--;)
		{
			k=EGrand(&seed)%(grsz*i);
			EGrsetAdd(&set,(int32_t)k);
		}
		EGtimerStop(&t1);
		if(display) EGrsetDisplay(&set,stdout);
		EGcallD(EGrsetCheck(&set));
		EGtimerStart(&t3);
		for(j=nop;j--;)
		{
			k=EGrand(&seed)%(grsz*i);
			cnt+=EGrsetPeek(&set,(int32_t)k);
		}
		EGtimerStop(&t3);
		if(display) EGrsetDisplay(&set,stdout);
		EGcallD(EGrsetCheck(&set));
		EGtimerStart(&t2);
		for(j=nop;j--;)
		{
			k=EGrand(&seed)%(grsz*i);
			EGrsetDel(&set,(int32_t)k);
		}
		EGtimerStop(&t2);
	}
	/* Liberating allocated memory */
	EGrsetClear(&set);
	EGcallD(EGrsetCheck(&set));
	CLEANUP:
	fprintf(stdout,"Statistics:\n");
	fprintf(stdout,"\tAdd Cals  %9d Time %9.5lg Average %9.5lg\n",nop*3,t1.time, t1.time/(nop*3));
	fprintf(stdout,"\tDel Cals  %9d Time %9.5lg Average %9.5lg\n",nop*3,t2.time, t2.time/(nop*3));
	fprintf(stdout,"\tPeek Cals %9d Time %9.5lg Average %9.5lg Success %d %.2lf%%\n",nop*3,t3.time, t3.time/(nop*3), cnt, ((double)cnt)/(nop*3));
	return rval;
}

/* ========================================================================= */
/** @} */
