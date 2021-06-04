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
	fprintf (stderr, "Usage: %s [options]\n", argv[0]);
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
static int sim_parseargs (int argc,
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
	int rval = 0;
	unsigned i;
	EGsim_t sim;
	EGlib_info();
	EGlib_version();
	rval  = sim_parseargs(argc,argv);
	CHECKRVAL(rval);
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	/* now we initialize structures, read the file, set the maximum time ,
	 * simulate, clean-up memory and exit */
	EGsimInit(&sim);
	rval = EGsimLoadFile(&sim,input);
	CHECKRVAL(rval);
	fclose(input);
	sim.cur_time = 0;
	sim.max_time = max_time;
	rval = EGsim(&sim);
	CHECKRVAL(rval);
	/* now we present the results per node */
	fprintf(stdout,"Total_Elements %u Elements_in_Network %u\n",sim.agent_tot,sim.agent_sz);
	for( i = 0 ; i < sim.G.n_nodes; i++)
	{
		fprintf(stdout,"Node %u avg_queue %lf avg_cycle %lf avg_usage %lf"
						" avg_q5 %lf avg_wtime %lf num %u queue %u\n", i+1, 
						sim.all_nodes[i].avg_queue, sim.all_nodes[i].avg_cycle, 
						sim.all_nodes[i].avg_usage, sim.all_nodes[i].avg_q5, 
						sim.all_nodes[i].avg_wtime, sim.all_nodes[i].num, 
						sim.all_nodes[i].queue_sz);
	}
	CLEANUP:
	EGsimClear(&sim);
  return rval;
}
/** @} */
