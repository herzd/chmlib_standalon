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
/* ========================================================================= */
/** @file
 * @ingroup EGsimula */
/** @addtogroup EGsimula */
/** @{ */
#include "EGlib.h"

/* ========================================================================= */
/** @brief display usage of this program */
static void sim_usage (char **argv)
{
	fprintf (stdout, "Generate demand from a variable rate poisson process\n"
										"as described by the given file, for the prescribed time length\n"); 
	fprintf (stdout, "Usage: %s [options]\n", argv[0]);
	fprintf (stdout, "Options:\n");
	fprintf (stdout, "     -s n   initial seed for the RNG (you can give up to six seeds)\n");
	fprintf (stdout, "     -t n   time to run the simulation.\n");
	fprintf (stdout, "     -f n   file name.\n");
}

/* ========================================================================= */
/** @name Global Variables */
/*@{*/
static int n_seeds = 0; /**< number of readed seeds */
static unsigned long seed[6] = {12345,12345,12345,12345,12345,12345}; /**< default seed to use */
char const*file_name = 0; /**< pointer to the file name to use */
static double max_time = 1440;/**< maximum time for the simulation */
FILE* input;/**< input stream */
/*@}*/

/* ========================================================================= */
/** @brief parse input */
static inline int sim_parseargs (int argc,
																char **argv)
{
	int c;
	while ((c = getopt (argc, argv, "f:t:s:")) != EOF)
	{
		switch (c)
		{
		case 'f':
			file_name = optarg;
			break;
		case 't':
			max_time = strtod (optarg,0);
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
			sim_usage (argv);
			return 1;
		}
	}
	/* test that we have an input file */
	if (!file_name)
	{
		sim_usage (argv);
		return 1;
	}
	input = fopen(file_name,"r");
	if(!input)
	{
		fprintf(stderr, "fopen failed with code error %d (%s)\n", errno,
						strerror(errno));
		sim_usage (argv);
		return 1;
	}
	/* set the seed */
	if(EGrandSetGlobalSeed(seed))
	{
		sim_usage (argv);
		return 1;
	}
	/* report options */
	fprintf (stderr, "\tFile %s\n\tMax_time %lf\n\tSeed [%lu,%lu,%lu,%lu,%lu,%lu]\n",
					 file_name, max_time, seed[0], seed[1], seed[2], seed[3], seed[4], seed[5]);
	return 0;
}


/* ========================================================================= */
/** @brief this program read a generic network from a file, a given (set 
 * of) seed(s),  and a maximum simulation time, and output the overall 
 * statistics for the complete simulation */
int main (int argc,char**argv)
{
	int rval = sim_parseargs(argc,argv);
	EGsimVRPoisson_t data;
	double cur_t=0.0,t;
	EGlib_info();
	EGlib_version();
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	CHECKRVAL(rval);
	/* now we initialize structures, read the file, set the maximum time ,
	 * simulate, clean-up memory and exit */
	EGsimVRPInit(&data);
	rval = EGsimVRPLoadFile(&data,input);
	CHECKRVAL(rval);
	fclose(input);
	while(cur_t < max_time)
	{
		t = EGsimVRPoisson(cur_t,&data);
		cur_t += t;
		fprintf(stdout,"%.3lf\n",cur_t);
	}
	CLEANUP:
	EGsimVRPClear(&data);
  return rval;
}
/** @} */
