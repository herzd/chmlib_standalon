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
#include "mpf_eg_dijkstra.h"
/* ========================================================================= */
int mpf_EGalgDJK (
		int32_t const nnodes,
		int32_t const nedges,
		int32_t const*const ou_d,
		int32_t const*const ou_beg,
		int32_t const*const ou_e,
		mpf_t const*const weight,
		int32_t const s,
		int32_t const nto,
		int32_t const*const t,
		int32_t*const father,
		mpf_t*const dist)
{
	int rval=0,i;
	int32_t nt=nto,h,tail,e;
	mpf_EGeHeapCn_t *hcn = EGsMalloc(mpf_EGeHeapCn_t,nnodes), *ccn=0;
	mpf_EGeHeap_t heap;
	mpf_t v;
	mpf_EGlpNumInitVar(v);
	mpf_EGeHeapInit(&heap);
	mpf_EGeHeapChangeD(&heap,2);
	mpf_EGeHeapResize(&heap,nnodes);
	for(i=nedges;i--;) mpf_EGlpNumAddTo(v,weight[i]);
	mpf_EGlpNumMultUiTo(v,2U);
	/* set initual bound to infinity */
	for(i=nnodes;i--;)
	{
		mpf_EGlpNumCopy(hcn[i].val,v);
		mpf_EGeHeapAdd(&heap,hcn+i);
		father[i]=-1;
	}
	/* set mpf_source distance to zero */
	mpf_EGlpNumSet(v,0);
	mpf_EGlpNumSet(dist[s],0);
	mpf_EGeHeapChangeVal(&heap,hcn+s,v);
	father[s]=s;
	/* loop in the heap */
	while(heap.sz)
	{
		ccn=mpf_EGeHeapGetMin(&heap);
		mpf_EGeHeapDel(&heap,ccn);
		tail=ccn-hcn;
		/* store distance */
		mpf_EGlpNumCopy(dist[tail],ccn->val);
		/* update number of target nodes, if zero, stop loop */
		if(t[tail]) nt--;
		if(!nt) break;
		for(i=ou_d[tail];i--;)
		{
			h=ou_e[e=(ou_beg[tail]+i)];
			if(mpf_EGlpNumIsSumLess(ccn->val,weight[e],hcn[h].val))
			{
				father[h]=tail;
				mpf_EGlpNumCopy(v,ccn->val);
				mpf_EGlpNumAddTo(v,weight[e]);
				mpf_EGeHeapChangeVal(&heap,hcn+h,v);
			}
		}
	}
	mpf_EGlpNumClearVar(v);
	mpf_EGeHeapClear(&heap);
	EGfree(hcn);
	EG_RETURN(rval);
}
/* ========================================================================= */
int mpf_EGguReadXgraph(
		EGioFile_t*const input,
		int32_t*const np,
		int32_t*const mp,
		int32_t**const edgesp,
		mpf_t**const weightp)
{
	int rval,argc,i;
	int32_t n=0,m=0,*edges=0;
	mpf_t*weight=0;
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
	weight=mpf_EGlpNumAllocArray(m);
	for(i=0;i<m;i++)
	{
		do{
			EGioGets(str,4095,input);
			EGioNParse(str,1024," ","%#",&argc,argv);
		}while(!argc);
		TESTG((rval=(argc!=3)),CLEANUP,"expected ``tail_%d head_%d weight_%d'' but read %d tokens",i,i,i,argc);
		edges[2*i]=atoi(argv[0]);
		edges[2*i+1]=atoi(argv[1]);
		mpf_EGlpNumReadStr(weight[i],argv[2]);
	}
	CLEANUP:
	*np=n,*mp=m,*edgesp=edges,*weightp=weight;
	EG_RETURN(rval);
}
/* ========================================================================= */
/** @} */

