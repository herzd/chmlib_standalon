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
#include "EGlib.h"
/* ========================================================================= */
/** @name Global Variables */
/*@{*/
static int n_seeds = 0; /**< number of readed seeds */
static unsigned long seed[6] = {12345,12345,12345,12345,12345,12345}; /**< default seed to use */
static int n_threads = 5; /**< number of threads to use */
static int n_iter = 1000;	/**< number of operations to perform in each thread */
static EGms_t pool; /**< global memory pool */
/*@}*/
/* ========================================================================= */
/** @brief display usage of this program */
static void usage (char **argv)
{
	fprintf (stderr, "Usage: %s [options]\n", argv[0]);
	fprintf (stdout, "Options:\n");
	fprintf (stdout, "     -h     Print this help and exit\n");
	fprintf (stdout, "     -i n   Number of iterations on each thread, default %d\n",n_iter);
	fprintf (stdout, "     -s n   Initial seed for the RNG (you can give up to six seeds), default %lu,%lu,%lu,%lu,%lu,%lu\n",seed[0],seed[1],seed[2],seed[3],seed[4],seed[5]);
	fprintf (stdout, "     -t n   Number of threads to run, default %d\n",n_threads);
}
/* ========================================================================= */
/* brief essentially, alloc n_iter doubles, generate random value, add them up,
 * and report the sum */
static void work(double*result)
{
	int i=0,j=0;
	double sum=0.0;
	double**all_val=EGsMalloc(double*,n_iter);
	EGrandState_t g;	/**< local random number generator */
	EGrandInit(&g);
	for(i=n_iter;i--;)
	{
		for(j=n_iter;j--;)
		{
			all_val[j] = EGmsAlloc(&pool);
			sum += all_val[j][0] = EGrandU01(&g);
		}
		for(j=n_iter;j--;) EGmsFree(all_val[j]);
	}
	if(2*sum>n_iter*n_iter) fprintf(stderr,"Above average\n");
	else fprintf(stderr,"Below average\n");
	EGfree(all_val);
	(*result)=sum;
}
/* ========================================================================= */
/** @brief parse input */
static int parseargs (
		int argc,
		char **argv)
{
	int c;
	while ((c = getopt (argc, argv, "hi:t:s:")) != EOF)
	{
		switch (c)
		{
		case 'h':
			usage(argv);
			exit(EXIT_SUCCESS);
		case 'i':
			n_iter = atoi(optarg);
			break;
		case 't':
			n_threads = atoi (optarg);
			break;
		case 's':
			if(n_seeds < 6)
			{
				seed[n_seeds++] = (unsigned long) atol(optarg);
			}
			else
			{
				fprintf(stderr,"Seed %s ignored\n",optarg);
			}
			break;	
		default:
			usage (argv);
			return 1;
		}
	}
	/* check consistency of input options */
	if(n_threads<1||n_iter<1)
	{
		usage(argv);
		return 1;
	}
	/* set the seed */
	if(EGrandSetGlobalSeed(seed))
	{
		usage (argv);
		return 1;
	}
	/* report options */
	fprintf (stderr, "Running %s with options:\n\tN_iter: %d\n\tN_threads %d\n\tSeed [%lu,%lu,%lu,%lu,%lu,%lu]\n", argv[0], n_iter, n_threads, seed[0], seed[1], seed[2], seed[3], seed[4], seed[5]);
	return 0;
}
/* ========================================================================= */
int main(int argc,char**argv)
{
	#if HAVE_EG_THREAD
	int rval=0,i;
	pthread_t* thread= 0;
	double*res=0,sum=0.0;
	EGlib_info();
	EGlib_version();
	EGcallD(parseargs(argc,argv));
	EGmsInit(&pool,sizeof(double),0,0);
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	res = EGsMalloc(double,n_threads);
	thread = EGsMalloc(pthread_t,n_threads);
	for(i=n_threads;i--;) pthread_create(thread+i,0,(void*)work,(void*)(res+i));
	for(i=n_threads;i--;) pthread_join(thread[i],0);
	for(i=n_threads;i--;) sum+=res[i];
	fprintf(stderr,"Total sum: %lf\n",sum);
	CLEANUP:
	EGmsClear(&pool);
	EGfree(thread);
	EGfree(res);
	return rval;
	#else
	fprintf(stderr,"No thread support was compiled in EGlib, ending now\n");
	return 0;
	#endif
}
