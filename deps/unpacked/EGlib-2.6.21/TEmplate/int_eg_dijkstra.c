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
#include "int_eg_dijkstra.h"
/* ========================================================================= */
int int_EGalgDJK (
		int32_t const nnodes,
		int32_t const nedges,
		int32_t const*const ou_d,
		int32_t const*const ou_beg,
		int32_t const*const ou_e,
		int const*const weight,
		int32_t const s,
		int32_t const nto,
		int32_t const*const t,
		int32_t*const father,
		int*const dist)
{
	int rval=0,i;
	int32_t nt=nto,h,tail,e;
	int_EGeHeapCn_t *hcn = EGsMalloc(int_EGeHeapCn_t,nnodes), *ccn=0;
	int_EGeHeap_t heap;
	int v;
	int_EGlpNumInitVar(v);
	int_EGeHeapInit(&heap);
	int_EGeHeapChangeD(&heap,2);
	int_EGeHeapResize(&heap,nnodes);
	for(i=nedges;i--;) int_EGlpNumAddTo(v,weight[i]);
	int_EGlpNumMultUiTo(v,2U);
	/* set initual bound to infinity */
	for(i=nnodes;i--;)
	{
		int_EGlpNumCopy(hcn[i].val,v);
		int_EGeHeapAdd(&heap,hcn+i);
		father[i]=-1;
	}
	/* set int_source distance to zero */
	int_EGlpNumSet(v,0);
	int_EGlpNumSet(dist[s],0);
	int_EGeHeapChangeVal(&heap,hcn+s,v);
	father[s]=s;
	/* loop in the heap */
	while(heap.sz)
	{
		ccn=int_EGeHeapGetMin(&heap);
		int_EGeHeapDel(&heap,ccn);
		tail=ccn-hcn;
		/* store distance */
		int_EGlpNumCopy(dist[tail],ccn->val);
		/* update number of target nodes, if zero, stop loop */
		if(t[tail]) nt--;
		if(!nt) break;
		for(i=ou_d[tail];i--;)
		{
			h=ou_e[e=(ou_beg[tail]+i)];
			if(int_EGlpNumIsSumLess(ccn->val,weight[e],hcn[h].val))
			{
				father[h]=tail;
				int_EGlpNumCopy(v,ccn->val);
				int_EGlpNumAddTo(v,weight[e]);
				int_EGeHeapChangeVal(&heap,hcn+h,v);
			}
		}
	}
	int_EGlpNumClearVar(v);
	int_EGeHeapClear(&heap);
	EGfree(hcn);
	EG_RETURN(rval);
}
/* ========================================================================= */
int int_EGguReadXgraph(
		EGioFile_t*const input,
		int32_t*const np,
		int32_t*const mp,
		int32_t**const edgesp,
		int**const weightp)
{
	int rval,argc,i;
	int32_t n=0,m=0,*edges=0;
	int*weight=0;
	char *argv[1024],str[4096];
	/* find next meaningfull set of tokens */
	do{
		EGioGets(str,4095,input);
		EGioNParse(str,1024," ","%#",&argc,argv);
	}while(!argc);
	TESTG((rval=(argc!=2)),CLEANUP,"expected ``nnodes nedges'' but read %d tokens",argc);
	n=atoi(argv[0]), m=atoi(argv[1]);
	TESTG((rval=(n<1||m<0)),CLEANUP,"invalid values, nnodes=%"PRId32" nedges=%"PRId32,n,m);
	edges=EGsMalloc(int32_t,m*2);
	weight=int_EGlpNumAllocArray(m);
	for(i=0;i<m;i++)
	{
		do{
			EGioGets(str,4095,input);
			EGioNParse(str,1024," ","%#",&argc,argv);
		}while(!argc);
		TESTG((rval=(argc!=3)),CLEANUP,"expected ``tail_%d head_%d weight_%d'' but read %d tokens",i,i,i,argc);
		edges[2*i]=atoi(argv[0]);
		edges[2*i+1]=atoi(argv[1]);
		int_EGlpNumReadStr(weight[i],argv[2]);
	}
	CLEANUP:
	*np=n,*mp=m,*edgesp=edges,*weightp=weight;
	EG_RETURN(rval);
}
/* ========================================================================= */
/** @} */

