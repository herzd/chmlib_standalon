/* algorithms commons in many optimization algorithms.
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
/** @file 
 * @ingroup EGalgMaxClosure */
/** @addtogroup EGalgMaxClosure */
/** @{ */
#include "eg_config.h"
#include "eg_macros.h"
#include "eg_mem.h"
#include "dbl_eg_push_relabel.h"
#include "eg_maxclosure.h"
/* ========================================================================= */
static int EGalgMaxClosure_PR(
		const int n,
		const int m,
		const int*const edge_array,
		const double*const weight,
		int*const closure_sz,
		int*const closure,
		const int display)
{
	int rval = 0,i,j;
	dbl_EGalgPRgraph_t G;
	dbl_EGalgPRnode_t *nodes = EGsMalloc(dbl_EGalgPRnode_t,n+2), *nt, *nh;
	dbl_EGalgPRedge_t *edges = EGsMalloc(dbl_EGalgPRedge_t,m+n), *ce;
	double absw = 0.0,posw = 0.0,w;
	IFMESSAGE(display-1,"Initializing ");
	/* build graph and weights */
	dbl_EGalgPRgraphInit(&G);
	dbl_EGalgPRnodeInit(nodes+n);
	EGeDgraphAddNode(&(G.G),&(nodes[n].v));
	dbl_EGalgPRnodeInit(nodes+n+1);
	EGeDgraphAddNode(&(G.G),&(nodes[n+1].v));
	for( i = 0 ; i < n ; i++)
	{
		ce = edges + i + m,
		nh = nodes + i;
		dbl_EGalgPRedgeInit(ce);
		dbl_EGalgPRnodeInit(nh);
		EGeDgraphAddNode(&(G.G),&(nodes[i].v));
		if((w=weight[i])>0)
		{
			nt = nodes + n,
			posw += w,
			absw += w,
			EGeDgraphAddEdge(&(G.G), &(nh->v), &(nt->v), &(ce->fw.e));
			EGeDgraphAddEdge(&(G.G), &(nt->v), &(nh->v), &(ce->bw.e));
			ce->fw.u = w,
			ce->bw.u = 0.0;
		}
		else
		{
			nt = nodes + n + 1,
			absw -= w;
			EGeDgraphAddEdge(&(G.G), &(nt->v), &(nh->v), &(ce->fw.e));
			EGeDgraphAddEdge(&(G.G), &(nh->v), &(nt->v), &(ce->bw.e));
			ce->fw.u = -w,
			ce->bw.u = 0.0;
		}
	}
	absw = exp2(logb(absw)+1);
	/* add precedences */
	for( i = 0 ; i < m ; i++)
	{
		ce = edges + i,
		nt = nodes + edge_array[2*i],
		nh = nodes + edge_array[2*i+1];
		dbl_EGalgPRedgeInit(ce);
		EGeDgraphAddEdge(&(G.G), &(nh->v), &(nt->v), &(ce->fw.e));
		EGeDgraphAddEdge(&(G.G), &(nt->v), &(nh->v), &(ce->bw.e));
		ce->fw.u = absw,
		ce->bw.u = 0;
	}
	/* end building graph, now compute ST cut in auxiliary graph */
	IFMESSAGE(display-1,"Solving MAX-FLOW ");
	EGcall(rval,CLEANUP,dbl_EGalgPRminSTcut(&G, nodes+n, nodes+n+1));
	/* build solution set */
	IFMESSAGE(display-1,"Building Solution ");
	for( j = i = 0 ; i < n ; i++)
	{
		if(nodes[i].d<G.G.n_nodes) continue;
		closure[j++] = i;
	}
	*closure_sz = j;
	IFMESSAGE(display,"Closure %d/%d nodes ",j,n);
	CLEANUP:
	EGfree(nodes);
	EGfree(edges);
	return rval;
}
/* ========================================================================= */
int EGalgMaxClosure(
		const int algo,
		const int n,
		const int m,
		const int*const edges,
		const double*const weight,
		int*const closure_sz,
		int*const closure,
		const int display)
{
	int rval=0;
	*closure_sz=0;
	switch(algo)
	{
		case EG_ALG_MAXCLOSURE_DEFAULT:
		case EG_ALG_MAXCLOSURE_PR:
			EGcallD(EGalgMaxClosure_PR(n,m,edges,weight,closure_sz,closure,display));
			break;
		default:
			TESTG((rval=1),CLEANUP,"Unknown algorithm type %d\n",algo);
			break;
	}
	CLEANUP:
	return rval;
}
/* ========================================================================= */
/** @} */
