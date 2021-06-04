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
#include "mpq_eg_dijkstra.h"
/* ========================================================================= */
int mpq_EGalgDJK (
		int32_t const nnodes,
		int32_t const nedges,
		int32_t const*const ou_d,
		int32_t const*const ou_beg,
		int32_t const*const ou_e,
		mpq_t const*const weight,
		int32_t const s,
		int32_t const nto,
		int32_t const*const t,
		int32_t*const father,
		mpq_t*const dist)
{
	int rval=0,i;
	int32_t nt=nto,h,tail,e;
	mpq_EGeHeapCn_t *hcn = EGsMalloc(mpq_EGeHeapCn_t,nnodes), *ccn=0;
	mpq_EGeHeap_t heap;
	mpq_t v;
	mpq_EGlpNumInitVar(v);
	mpq_EGeHeapInit(&heap);
	mpq_EGeHeapChangeD(&heap,2);
	mpq_EGeHeapResize(&heap,nnodes);
	for(i=nedges;i--;) mpq_EGlpNumAddTo(v,weight[i]);
	mpq_EGlpNumMultUiTo(v,2U);
	/* set initual bound to infinity */
	for(i=nnodes;i--;)
	{
		mpq_EGlpNumCopy(hcn[i].val,v);
		mpq_EGeHeapAdd(&heap,hcn+i);
		father[i]=-1;
	}
	/* set mpq_source distance to zero */
	mpq_EGlpNumSet(v,0);
	mpq_EGlpNumSet(dist[s],0);
	mpq_EGeHeapChangeVal(&heap,hcn+s,v);
	father[s]=s;
	/* loop in the heap */
	while(heap.sz)
	{
		ccn=mpq_EGeHeapGetMin(&heap);
		mpq_EGeHeapDel(&heap,ccn);
		tail=ccn-hcn;
		/* store distance */
		mpq_EGlpNumCopy(dist[tail],ccn->val);
		/* update number of target nodes, if zero, stop loop */
		if(t[tail]) nt--;
		if(!nt) break;
		for(i=ou_d[tail];i--;)
		{
			h=ou_e[e=(ou_beg[tail]+i)];
			if(mpq_EGlpNumIsSumLess(ccn->val,weight[e],hcn[h].val))
			{
				father[h]=tail;
				mpq_EGlpNumCopy(v,ccn->val);
				mpq_EGlpNumAddTo(v,weight[e]);
				mpq_EGeHeapChangeVal(&heap,hcn+h,v);
			}
		}
	}
	mpq_EGlpNumClearVar(v);
	mpq_EGeHeapClear(&heap);
	EGfree(hcn);
	EG_RETURN(rval);
}
/* ========================================================================= */
int mpq_EGguReadXgraph(
		EGioFile_t*const input,
		int32_t*const np,
		int32_t*const mp,
		int32_t**const edgesp,
		mpq_t**const weightp)
{
	int rval,argc,i;
	int32_t n=0,m=0,*edges=0;
	mpq_t*weight=0;
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
	weight=mpq_EGlpNumAllocArray(m);
	for(i=0;i<m;i++)
	{
		do{
			EGioGets(str,4095,input);
			EGioNParse(str,1024," ","%#",&argc,argv);
		}while(!argc);
		TESTG((rval=(argc!=3)),CLEANUP,"expected ``tail_%d head_%d weight_%d'' but read %d tokens",i,i,i,argc);
		edges[2*i]=atoi(argv[0]);
		edges[2*i+1]=atoi(argv[1]);
		mpq_EGlpNumReadStr(weight[i],argv[2]);
	}
	CLEANUP:
	*np=n,*mp=m,*edgesp=edges,*weightp=weight;
	EG_RETURN(rval);
}
/* ========================================================================= */
/** @} */

