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
 * @ingroup EGalgDijkstra */
/** @addtogroup EGalgDijkstra */
/** @{ */
/* ========================================================================= */
#include "EGlib.h"
static char dbl_fname[1024];
static int32_t dbl_source=0;
static int dbl_verbose=1;
/* ========================================================================= */
static inline void dbl_djk_usage(const char*const progname)
{
	fprintf(stderr,"Usage: %s [options]\n",progname);
	fprintf(stderr,"\t-h    print this message and exit\n");
	fprintf(stderr,"\t-v n  verbosity level, default %d\n",dbl_verbose);
	fprintf(stderr,"\t-f s  input file to use, must be a graph as in concorde's x-format (mandatory argument)\n");
	fprintf(stderr,"\t-s n  node from where start dijkstra\n");
	return;
}
/* ========================================================================= */
/** @brief parse-args */
static inline void dbl_djk_parseargs(int argc,char**argv)
{
	int c, has_file=0;
	while((c=getopt(argc,argv,"f:v:hs:"))!=EOF)
	{
		switch(c)
		{
			case 'f':
				snprintf(dbl_fname,1023,"%s",optarg);
				has_file=1;
				break;
			case 'v':
				dbl_verbose=atoi(optarg);
				break;
			case 'h':
				dbl_djk_usage(argv[0]);
				exit(EXIT_SUCCESS);
			case 's':
				dbl_source=atoi(optarg);
				break;
			default:
				fprintf(stderr,"Unknown parameter %c\n",c);
				dbl_djk_usage(argv[0]);
				exit(EXIT_FAILURE);
		}
	}
	if(!has_file)
	{
		dbl_djk_usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	fprintf(stderr,"\tInput file   : %s\n",dbl_fname);
	fprintf(stderr,"\tverbose level: %d\n",dbl_verbose);
	fprintf(stderr,"\tsource       : %"PRId32"\n",dbl_source);
	return;
}
/* ========================================================================= */
/** @brief A simple example of a directed graph using (EGdEgraph) structures.
 * @return zero on success, non-zero- otherwise.
 * @par Description:
 * Show how to use a directed graph, modify it and display it's contents */
int main (int argc,char**argv)
{
	int rval=0,i;
	EGtimer_t rt,pp,dt;
	int32_t n,m,*edges=0,*in_perm=0,*in_d=0,*in_beg=0,*in_e=0,*t=0,*father=0;
	double*weight=0,*sweight=0,*dist=0;
	EGioFile_t* input=0;
	EGtimerReset(&dt);
	EGtimerReset(&rt);
	EGtimerReset(&pp);
	EGtimerStart(&rt);
	EGlib_info();
	EGlib_version();
	EGlpNumStart();
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	dbl_djk_parseargs(argc,argv);
	input = EGioOpen(dbl_fname,"r");
	EGcallD(dbl_EGguReadXgraph(input,&n,&m,&edges,&weight));
	EGioClose(input);
	EGtimerStop(&rt);
	EGtimerStart(&pp);
	t=EGsMalloc(int32_t,n);
	father=EGsMalloc(int32_t,n);
	memset(t,1,sizeof(int32_t)*n);
	dist=dbl_EGlpNumAllocArray(n);
	sweight=dbl_EGlpNumAllocArray(m);
	in_e=EGsMalloc(int32_t,m);
	in_d=EGsMalloc(int32_t,n);
	in_beg=EGsMalloc(int32_t,n);
	in_perm=EGsMalloc(int32_t,m);
	EGcallD(EGguDegree(n,m,edges,in_d,in_beg,in_e,in_perm,0,0,0,0));
	for(i=m;i--;)
		dbl_EGlpNumCopy(sweight[in_perm[i]],weight[i]);
	EGtimerStop(&pp);
	EGtimerStart(&dt);
	EGcallD(dbl_EGalgDJK(n,m,in_d,in_beg,in_e,sweight,dbl_source,n,t,father,dist));
	EGtimerStop(&dt);
	fprintf(stderr,"Stats:\n");
	fprintf(stderr,"\tnnodes %"PRId32" nedges %"PRId32"\n",n,m);
	fprintf(stderr,"\tread time       : %.3lf\n",rt.time);
	fprintf(stderr,"\tpre-process time: %.3lf\n",pp.time);
	fprintf(stderr,"\tdijkstra time   : %.3lf\n",dt.time);
	if(dbl_verbose>1)
	{
		for(i=0;i<n;i++)
		{
			fprintf(stderr,"\t\tNode %d father %"PRId32" dist %.3lf\n",i,father[i],dbl_EGlpNumToLf(dist[i]));
		}
	}
	CLEANUP:
	dbl_EGlpNumFreeArray(weight);
	dbl_EGlpNumFreeArray(sweight);
	dbl_EGlpNumFreeArray(dist);
	EGfree(edges);
	EGfree(in_d);
	EGfree(in_e);
	EGfree(in_beg);
	EGfree(in_perm);
	EGfree(t);
	EGfree(father);
	EGlpNumClear();
	return rval;
}



/* ========================================================================= */
/** @} */


