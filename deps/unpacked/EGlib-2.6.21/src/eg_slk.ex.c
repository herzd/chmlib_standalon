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
 * @ingroup EGslin_kern */
/** @addtogroup EGslin_kern */
/** @{ */
#include "EGlib.h"
/* ========================================================================= */
/** @brief display usage of this program */
static void slk_usage (char **argv)
{
	fprintf (stderr, "Usage: %s [options]\n", argv[0]);
	fprintf (stdout, "Options:\n");
	fprintf (stdout, "     -s n   initial seed for the RNG (you can give up to six seeds)\n");
	fprintf (stdout, "     -m n   if 0, run in TSP mode; if 1, run in CVRP mode\n");
	fprintf (stdout, "     -f n   input file name.\n");
}

/* ========================================================================= */
/** @name Global Variables */
/*@{*/
static int n_seeds = 0; /**< @brief number of readed seeds */
static int mode = 0;		/**< @brief if 0, compute a 2-opt simple tour, if 1, compute a 2-opt CVRP solution */
static unsigned long seed[6] = {12345,12345,12345,12345,12345,12345}; /**< default seed to use */
char const*file_name = 0; /**< pointer to the file name to use */
static EGrandState_t g;/**< Random state to use */
EGioFile_t* input = 0;/**< input stream */
/*@}*/

/* ========================================================================= */
/** @brief parse input */
static inline int slk_parseargs (int argc,
																char **argv)
{
	int c;
	while ((c = getopt (argc, argv, "f:m:s:")) != EOF)
	{
		switch (c)
		{
		case 'm':
			if(atoi(optarg)>0) mode = 1; 
			else mode = 0;
			break;
		case 'f':
			file_name = optarg;
			break;
		case 's':
			if(n_seeds < 6)
			{
				seed[n_seeds++] = (unsigned long)atol(optarg);
			}
			else
			{
				fprintf(stderr,"Seed %s ignored\n",optarg);
			}
			break;	
		default:
			slk_usage (argv);
			return 1;
		}
	}
	/* test that we have an input file */
	if (!file_name)
	{
		slk_usage (argv);
		return 1;
	}
	input = EGioOpen(file_name,"r");
	if(!input)
	{
		fprintf(stderr, "fopen failed with code error %d (%s)\n", errno,
						strerror(errno));
		slk_usage (argv);
		return 1;
	}
	/* set the seed */
	EGrandInit(&g);
	if(EGrandSetSeed(&g,seed))
	{
		slk_usage (argv);
		return 1;
	}
	/* report options */
	fprintf (stderr, "\tFile %s\n\tSeed [%lu,%lu,%lu,%lu,%lu,%lu]\n\tMode %s\n",
					 file_name, seed[0], seed[1], seed[2], seed[3], seed[4], seed[5],
					 mode ? "CVRP" : "TSP");
	return 0;
}

/* ========================================================================= */
/** @brief helper heap structure */
typedef struct SlkHcn_t{
	dbl_EGeHeapCn_t cn;
	unsigned node;
} SlkHcn_t;

/* ========================================================================= */
/** @brief this program read a generic network from a file, a given (set 
 * of) seed(s),  and a maximum simulation time, and output the overall 
 * statistics for the complete simulation */
int main (int argc,char**argv)
{
	int rval = 0,val;
	register int i;
	EGslkData_t data;
	EGtimer_t timer;
	EGlib_info();
	EGlib_version();
	rval  = slk_parseargs(argc,argv);
	CHECKRVAL(rval);
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	/* now we initialize structures, read the file, set the maximum time ,
	 * simulate, clean-up memory and exit */
	EGtimerReset(&timer);
	EGslkDataInit(&data);
	rval = EGslkDataLoadTSPLIB95(&data,input);
	CHECKRVAL(rval);
	EGioClose(input);
	/* if in TSP mode, we just cal 2-opt */
	if(mode == 0)
	{
		EGflipper_t flip;
		/* build a current and best tour for the data */
		for( i = (int)(data.dim) ; i-- ; )
		{
			data.curr_tour[i] = data.best_tour[i] = i;
		}
		data.best_val = EGslkTourValue(&data,data.curr_tour,data.dim);
		fprintf(stdout,"Starting solution value %d\n",data.best_val);
		/* now we init the flipper structure */
		EGflipperInit(&flip);
		EGflipperLoadTour(&flip,data.curr_tour,data.dim);
		/* now call 2opt */
		EGtimerStart(&timer);
		rval = EGslk2opt(&data,&flip);
		EGtimerStop(&timer);
		CHECKRVAL(rval);
		EGflipperGetTour(&flip,data.curr_tour);
		val = EGslkTourValue(&data,data.curr_tour,data.dim);
		fprintf(stdout,"2-opt ended with %d value in %lf seconds\n",val,timer.time);
		if(val < data.best_val)
		{
			data.best_val = val;
			memcpy(data.best_tour,data.curr_tour,sizeof(int)*data.dim);
		}
		/* free local-memopry */
		EGflipperClear(&flip);
	}
	/* if in CVRP mode, we create a feasible solution, and then perform a double
	 * 2-opt search (within each route, and between routes */
	if(mode == 1)
	{
		/* we create data->dim routes, we fill them-up and do then some local
		 * improvements */
		int redo = 0;
		register int j,k,l;
		SlkHcn_t*hcn=EGsMalloc(SlkHcn_t,data.dim),*hit;
		dbl_EGeHeapCn_t *htmp=0;
		dbl_EGeHeap_t heap;
		double nval = 0.0;
		EGflipper_t*flip = EGsMalloc(EGflipper_t,UCHAR_MAX);
		EGflipper_t*tour1 = flip+UCHAR_MAX-2;
		EGflipper_t*tour2 = flip+UCHAR_MAX-1;
		unsigned cur_truck=0,cur_length=0;
		int c1,c2,delta;
		int *rcap = EGsMalloc(int,UCHAR_MAX);
		unsigned char *truck = EGsMalloc(unsigned char,data.dim);
		unsigned nleft = data.dim - 1;
		data.best_val = 0;
		dbl_EGeHeapInit(&heap);
		dbl_EGeHeapChangeD(&heap,2);
		dbl_EGeHeapResize(&heap,data.dim);
		for( i = UCHAR_MAX ; i-- ; )
		{
			EGflipperInit(flip+i);
			rcap[i] = data.capacity;
		}
		for( i = ((int)data.dim) ; i-- ; )
		{
			truck[i] = UCHAR_MAX;
			hcn[i].node = (unsigned)i;
			hcn[i].cn.val = DBL_MAX;
			dbl_EGeHeapAdd(&heap,&(hcn[i].cn));
		}
		truck[data.depot] = 0;
		/* now we built a first feasible solution */
		MESSAGE(EG_SLK_VERB, "Starting truck %d at depot %d", cur_truck, 
						data.depot);
		while(nleft ) 
		{
			/* test that we don't overwrite memory */
			EXIT(cur_truck >= UCHAR_MAX-2, "Exceded maximum number of trucks (%d)",
						UCHAR_MAX-2);
			/*, we first find the farthest apart node from the depot, and start 
			 * adding close nodes to that one */
			cur_length=0;
			data.curr_tour[cur_length++]=data.depot;
			for( i = ((int)data.dim) ; i-- ; )
			{
				if(truck[i] != UCHAR_MAX) continue;
				nval = -1.0*data.cost[data.depot][i];
				rval = dbl_EGeHeapChangeVal(&heap,&(hcn[i].cn),nval);
				CHECKRVALG(rval,CLEANUP);
			}
			htmp = dbl_EGeHeapGetMin(&heap);
			hit = EGcontainerOf(htmp,SlkHcn_t,cn);
			rval = dbl_EGeHeapChangeVal(&heap,htmp,DBL_MAX);
			CHECKRVALG(rval,CLEANUP);
			data.curr_tour[cur_length++]=(int)(hit->node);
			nleft--;
			truck[hit->node] = (unsigned char)cur_truck;
			rcap[cur_truck] -= data.demand[hit->node];
			MESSAGE(EG_SLK_VERB, "Adding node %d demand %d to truck %d capacity "
							"left %d nodes left %d", hit->node, data.demand[hit->node], 
							cur_truck, rcap[cur_truck], nleft);
			/* now we sort nodes according to distance to active nodes in the current
			 * route */
			for( i = (int)data.dim ; i-- ; )
			{
				if(truck[i] != UCHAR_MAX) continue;
				nval = data.cost[hit->node][i];
				rval = dbl_EGeHeapChangeVal(&heap,&(hcn[i].cn),nval);
				CHECKRVALG(rval,CLEANUP);
			}
			/* add while capacity remans open */
			dbl_EGeHeapGetMinVal(&heap,nval);
			while(nval<DBL_MAX && rcap[cur_truck]>0)
			{
				htmp = dbl_EGeHeapGetMin(&heap);
				hit = EGcontainerOf(htmp,SlkHcn_t,cn);
				MESSAGE(EG_SLK_VERB, "Best value now is %lf, capacity %d, node %d", 
								hit->cn.val, data.demand[hit->node], hit->node);
				rval = dbl_EGeHeapChangeVal(&heap,htmp,DBL_MAX);
				CHECKRVALG(rval,CLEANUP);
				if(data.demand[hit->node] <= rcap[cur_truck])
				{
					data.curr_tour[cur_length++]=(int)(hit->node);
					nleft--;
					truck[hit->node] = (unsigned char)cur_truck;
					rcap[cur_truck] -= data.demand[hit->node];
					MESSAGE(EG_SLK_VERB, "Adding node %d demand %d to truck %d capacity"
									" left %d nodes left %d", hit->node,
									data.demand[hit->node], cur_truck, rcap[cur_truck], nleft);
					for( i = (int)(data.dim) ; i-- ; )
					{
						if(truck[i] != UCHAR_MAX) continue;
						nval = data.cost[hit->node][i];
						if(nval < hcn[i].cn.val)
						{
							rval = dbl_EGeHeapChangeVal(&heap,&(hcn[i].cn),nval);
							CHECKRVALG(rval,CLEANUP);
						}
					}
				}
				dbl_EGeHeapGetMinVal(&heap,nval);
			}
			/* now we have a set of customers in a truck, so we route them */
			EGflipperLoadTour(flip+cur_truck,data.curr_tour,cur_length);
			rval = EGslk2opt(&data,flip+cur_truck);
			CHECKRVALG(rval,CLEANUP);
			data.best_val += EGflipperGetCost(&data,flip+cur_truck);
			/* increase number of trucks */
			cur_truck++;
		}
		/* now we start improvement */
		fprintf(stdout, "Initial solution with %d trucks, value %d\n",
						cur_truck, data.best_val);
		/* we try to move a client to a different truck */
		REDO:
		redo = 0;
		for( i = (int)cur_truck ; i-- ; )
		{
			for( j = (int)cur_truck ; j-- ; )
			{
				if(j==i) continue;
				for( k= (int)(flip[i].sz) ; k-- ;)
				{
					c1 = flip[i].map[k];
					if(c1 == data.depot) continue;
					/* try to move a node from truck i to truck j */
					if(rcap[j] >= data.demand[c1])
					{ 
						EGflipperCopy(flip+i,tour1);
						EGflipperCopy(flip+j,tour2);
						EGflipperRemoveNode(tour1,k);
						EGflipperAddNode(tour2,c1);
						EGslk2opt(&data,tour1);
						EGslk2opt(&data,tour2);
						delta = EGflipperGetCost(&data,tour1) +
										EGflipperGetCost(&data,tour2) -
										EGflipperGetCost(&data,flip+i) -
										EGflipperGetCost(&data,flip+j);
						if(delta<0)
						{
							data.best_val += delta;
							EGflipperCopy(tour1,flip+i);
							EGflipperCopy(tour2,flip+j);
							rcap[i] += data.demand[c1];
							rcap[j] -= data.demand[c1];
							truck[c1] = (unsigned char)j;
							fprintf(stdout, "Improved solution with %d trucks, value %d"
											", moving node %d from truck %d to truck %d \n",
											cur_truck, data.best_val, c1, i, j);
							redo = 1;
							/* re-compute current status */
							if(((size_t)k)==flip[i].sz) continue;
							c1 = flip[i].map[k];
							if(c1 == data.depot) continue;
						}
					}
					/* try to swap a node between truck i and truck j */
					for( l = (int)(flip[j].sz) ; l-- ; )
					{
						c2 = flip[j].map[l];
						if(c2 == data.depot) continue;
						if((rcap[i]+data.demand[c1] < data.demand[c2]) ||
							 (rcap[j]+data.demand[c2] < data.demand[c1])) continue;
						EGflipperCopy(flip+i,tour1);
						EGflipperCopy(flip+j,tour2);
						EGflipperRemoveNode(tour1,k);
						EGflipperRemoveNode(tour2,l);
						EGflipperAddNode(tour2,c1);
						EGflipperAddNode(tour1,c2);
						EGslk2opt(&data,tour1);
						EGslk2opt(&data,tour2);
						delta = EGflipperGetCost(&data,tour1) +
										EGflipperGetCost(&data,tour2) -
										EGflipperGetCost(&data,flip+i) -
										EGflipperGetCost(&data,flip+j);
						if(delta<0)
						{
							data.best_val += delta;
							EGflipperCopy(tour1,flip+i);
							EGflipperCopy(tour2,flip+j);
							rcap[i] += data.demand[c1] - data.demand[c2];
							rcap[j] += data.demand[c2] - data.demand[c1];
							truck[c1] = (unsigned char)j;
							truck[c2] = (unsigned char)i;
							fprintf(stdout, "Improved solution with %d trucks, value %d"
											", moving node %d to truck %d, and node %d to truck "
											"%d\n", cur_truck, data.best_val, c1, j, c2, i);
							redo = 1;
							/* re-compute current status */
							if(((size_t)k)==flip[i].sz) continue;
							c1 = flip[i].map[k];
							if(c1 == data.depot) continue;
						}
					}
				}
			}
		}
		if(redo) goto REDO;
		fprintf(stdout, "Current solution with %d trucks, value %d\n", 
						cur_truck, data.best_val);
		/* free local-memory */
		for( i = UCHAR_MAX ; i-- ; ) EGflipperClear(flip+i);
		EGfree(flip);
		EGfree(truck);
		EGfree(rcap);
		EGfree(hcn);
		dbl_EGeHeapResize(&heap,0);
	}
	CLEANUP:
	/* ending */
	EGslkDataClear(&data);
  return rval;
}
/** @} */


