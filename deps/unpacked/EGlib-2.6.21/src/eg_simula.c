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
#include "eg_simula.h"
#include "eg_io.h"

/* ========================================================================= */
double EGsimExp(const double t, void*const data)
{
	EGsimRData_t*const rd=(EGsimRData_t*)data;
	double dtmp = EGrandExp(&(rd->g),rd->a);
	#if EG_SIM_DEBUG <= DEBUG
	rd->ngen++;
	rd->sumxi+=dtmp;
	#endif
	return dtmp;
}
/* ========================================================================= */
double EGsimUniform(const double t, void*const data)
{
	EGsimRData_t*const rd=(EGsimRData_t*)data;
	double dtmp = EGrandUab(&(rd->g),rd->a,rd->b);
	#if EG_SIM_DEBUG <= DEBUG
	rd->ngen++;
	rd->sumxi+=dtmp;
	#endif
	return dtmp;
}
/* ========================================================================= */
double EGsimNormal(const double t, void*const data)
{
	EGsimRData_t*const rd=(EGsimRData_t*)data;
	double val;
	while((val=EGrandNormal(&(rd->g),rd->a,rd->b))<0);
	#if EG_SIM_DEBUG <= DEBUG
	rd->ngen++;
	rd->sumxi+=val;
	#endif
	return val;
}
/* ========================================================================= */
/** @brief Create a new agent from a given agent generator node 
 * @param sim #EGsim_t structure that we are simulating.
 * @param gen #EGsimNode_t structure holding the generator. 
 * @note The new agent will be added to the proper structures in the network.
 * @return zero on success, non-zero otherwise. */ 
static inline int EGsimNewEvent(EGsim_t*const sim,EGsimNode_t*const gen)
{
	int rval = 0;
	/* alocate and set agent */
	EGsimAgent_t*la = (EGsimAgent_t*)EGmemSlabPoolAlloc(&(sim->mp));
	EGsimSetAgent(la,sim->cur_time + gen->gen(sim->cur_time,gen->data));
	la->heap_cn.val = la->last_time;
	la->node = gen;
	/* check heap size is right */
	if(sim->hp.sz == sim->hp.max_sz) 
		dbl_EGeHeapResize(&(sim->hp),sim->hp.sz*2);
	/* add to the heap of elements to be processed */
	rval = dbl_EGeHeapAdd(&(sim->hp),&(la->heap_cn));
	CHECKRVALG(rval,CLEANUP);
	/* update some statistics */
	sim->agent_sz++;
	sim->agent_tot++;
	MESSAGE(EG_SIM_VERBOSE,"new Agent at %lf",la->heap_cn.val);
	/* done */
	CLEANUP:
	return rval;
}

/* ========================================================================= */
#define EGsimNextToken(file) do{\
	EGioParse(&next,&curr,delim,coment);\
	do{\
		if(!next){\
			rval = EGioReadLine(str,(size_t)2048,file);\
			CHECKRVALG(rval,CLEANUP);\
			curr = str;\
			n_line++;\
			EGioParse(&next,&curr,delim,coment);n_token=0;}\
	}while(!next && !feof(input) && !ferror(file));\
	FTESTG((rval=!next), CLEANUP,"File ended prematurelly, line %d, token %d",n_line,n_token);\
	n_token++;\
	}while(0)

#if EG_SIM_VERBOSE <= DEBUG
#define EGsimDisplayAgent(agent,file) do{\
	fprintf(file,"Agent %p, node %p, queue_t %lf, serv_t %lf, last_t %lf\n",(agent),(agent)->node, (agent)->queue_time, (agent)->service_time, (agent)->last_time);}while(0)

#define EGsimDisplayNode(node,file) do{\
	fprintf(file,"Node (%p,%s,%7s)",(node),(node)->type == EGsimG ? "G" : ((node)->type == EGsimN ? "N" : ((node)->type == EGsimT ? "T" : "unknown")), (node)->gen == EGsimNormal ? "Normal" : ((node)->gen == EGsimExp ? "Exp" : ((node)->gen == EGsimUniform ? "Uniform" : ((node)->gen == 0 ? "Null" : ((node)->gen == EGsimVRPoisson ? "VRP": "Unknown")))));\
	fprintf(file," lt %lf, st %lf, acyc %lf, aque %lf, ausg %lf, q5 %lf, awt %lf, serv %u, quesz %u, users %u\n",(node)->last_time, (node)->start_time, (node)->avg_cycle,(node)->avg_queue,(node)->avg_usage,(node)->avg_q5,(node)->avg_wtime,(node)->ava_serv,(node)->queue_sz,(node)->num);}while(0)
#else
#define EGsimDisplayNode(node,file)
#define EGsimDisplayAgent(agent,file)
#endif

/* ========================================================================= */
int EGsimLoadFile(EGsim_t*const sim,
									FILE*input)
{
	const char delim[3] = " ";
	const char coment[3] = "%#";
	char str[2048], *curr,*next;
	FILE*fin=0;
	EGsimRData_t*rd=0;
	EGsimVRPoisson_t*vrp=0;
	int rval = 0, i, j, k, n_nodes, n_edges, n_line = 1, n_token = 0;
	/* test that the structure is empty */
	TEST((sim->G.n_nodes || sim->G.n_edges || sim->all_nodes || sim->all_edges),
				"structure is not empty!");
	/* first read the number of nodes and edges */
	TEST((feof(input) || ferror(input)),"File ended prematurelly");
	rval = EGioReadLine(str,(size_t)2048,input);
	CHECKRVALG(rval,CLEANUP);
	curr = str;
	EGsimNextToken(input);
	n_nodes = atoi(next);
	EGsimNextToken(input);
	n_edges = atoi(next);
	/* alocate memory */
	sim->all_nodes = EGsMalloc(EGsimNode_t,n_nodes);
	sim->all_edges = EGsMalloc(EGsimEdge_t,n_edges);
	/* read all nodes */
	for( i = 0 ; i < n_nodes ; i++)  
	{
		/* read type and number of servers */
		EGsimNodeInit(sim->all_nodes+i);
		EGsimNextToken(input);
		switch(next[0])
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				sim->all_nodes[i].type = EGsimN;
				sim->all_nodes[i].tot_serv = sim->all_nodes[i].ava_serv = atoi(next);
				break;
			case 'G':
			case 'g':
				sim->all_nodes[i].type = EGsimG;
				sim->all_nodes[i].tot_serv = sim->all_nodes[i].ava_serv = 0;
				break;
			case 'T':
			case 't':
				sim->all_nodes[i].type = EGsimT;
				sim->all_nodes[i].tot_serv = sim->all_nodes[i].ava_serv = 0;
				sim->all_nodes[i].gen = 0;
				sim->all_nodes[i].rtype = EGsimRV_novar;
				break;
			default:
				TEST(1,"Wrong input format, line %d, token %d",n_line,n_token);
				break;
		}
		/* read distribution */
		if(sim->all_nodes[i].type !=EGsimT)
		{
			EGsimNextToken(input);
			switch(next[0])
			{
				case 'P':
				case 'p':
					sim->all_nodes[i].gen = EGsimVRPoisson;
					sim->all_nodes[i].rtype = EGsimRV_VRP;
					sim->all_nodes[i].data = vrp = EGsMalloc(EGsimVRPoisson_t,1);
					EGsimVRPInit(vrp);
					break;
				case 'U':
				case 'u':
					sim->all_nodes[i].gen = EGsimUniform;
					sim->all_nodes[i].rtype = EGsimRV_2param;
					sim->all_nodes[i].data = rd = EGsMalloc(EGsimRData_t,1);
					EGsimRDInit(rd);
					break;
				case 'E':
				case 'e':
					sim->all_nodes[i].gen = EGsimExp;
					sim->all_nodes[i].rtype = EGsimRV_2param;
					sim->all_nodes[i].data = rd = EGsMalloc(EGsimRData_t,1);
					EGsimRDInit(rd);
					break;
				case 'N':
				case 'n':
					sim->all_nodes[i].gen = EGsimNormal;
					sim->all_nodes[i].rtype = EGsimRV_2param;
					sim->all_nodes[i].data = rd = EGsMalloc(EGsimRData_t,1);
					EGsimRDInit(rd);
					break;
				default:
					TEST(1,"Wrong input format, line %d, token %d",n_line,n_token);
					break;
			}
			/* depending on type, read the random variable parameters */
			if(sim->all_nodes[i].rtype == EGsimRV_2param)
			{
				EGsimNextToken(input);
				rd->a = strtod(next,0);
					TEST(rd->a < 0,"Parameter should be positive, line %d,"
							" token %d", n_line, n_token);
				if(sim->all_nodes[i].gen != EGsimExp)
				{
					EGsimNextToken(input);
					rd->b = strtod(next,0);
					TEST(rd->b < 0,"Parameter should be positive, line %d,"
							" token %d", n_line, n_token);
				}
			}
			if(sim->all_nodes[i].rtype == EGsimRV_VRP)
			{
				EGsimNextToken(input);
				fin = EGsfopen(next,"r");
				rval = EGsimVRPLoadFile(vrp,fin);
				fclose(fin);
				CHECKRVALG(rval,CLEANUP);
			}
		}
		/* add the node to the graph */
		EGeDgraphAddNode(&(sim->G),&(sim->all_nodes[i].node));
		EGsimDisplayNode((sim->all_nodes+i),stderr); 
		/* if the node is a generator, we add a next event */
		if(sim->all_nodes[i].type == EGsimG)
		{
			rval = EGsimNewEvent(sim,sim->all_nodes+i);
			CHECKRVALG(rval,CLEANUP);
		}
	}
	/* read all edges */
	for( i = 0 ; i < n_edges ; i++)
	{
		EGsimNextToken(input);
		j = atoi(next);
		EGsimNextToken(input);
		k = atoi(next);
		EGsimEdgeInit(sim->all_edges+i);
		EGsimNextToken(input);
		sim->all_edges[i].p = strtod(next,0);
		EGeDgraphAddEdge(&(sim->G), &(sim->all_nodes[k-1].node),
										 &(sim->all_nodes[j-1].node), &(sim->all_edges[i].edge));
		MESSAGE(EG_SIM_VERBOSE,"Adding edge (%d,%d)[%lf]",j-1,k-1,sim->all_edges[i].p);
	}
	CLEANUP:
	return rval;
}

/* ========================================================================= */
int EGsim(EGsim_t*const sim)
{
	int rval = 0;
	EGsimAgent_t *ca = 0, *na = 0;
	EGsimNode_t* cn = 0, *nn;
	EGsimEdge_t* ce;
	EGeList_t *head, *it;
	double dtmp, dtmp2, tt,dt;
	/* main simulation step */
	while(sim->cur_time < sim->max_time)
	{
		/* get the minimum element */
		ca = EGcontainerOf(dbl_EGeHeapGetMin(&(sim->hp)), EGsimAgent_t,heap_cn);
		sim->cur_time = ca->heap_cn.val; 
		MESSAGE(EG_SIM_VERBOSE,"Current time %lf",sim->cur_time);
		EGsimDisplayAgent(ca,stderr);
		cn = ca->node;
		/* update statistics */
		ca->service_time += sim->cur_time - ca->last_time;
		EGsimDisplayNode(cn,stderr);
		tt = cn->num+1-cn->queue_sz;
		dtmp = (tt-1)/tt;
		cn->avg_cycle = cn->avg_cycle*dtmp +(ca->queue_time+ca->service_time)/tt;
		dt = sim->cur_time - cn->last_time ;
		if(dt > 0.0)
		{
			tt = (sim->cur_time - cn->start_time);
			cn->avg_queue = cn->avg_queue*((tt-dt)/tt)+((double)(cn->queue_sz))*(dt/tt);
			if(cn->queue_sz>4)
				cn->avg_q5 = cn->avg_q5*((tt-dt)/tt)+dt/tt;
			else
				cn->avg_q5 *= ((tt-dt)/tt);
			if(cn->tot_serv)
				cn->avg_usage = cn->avg_usage*((tt-dt)/tt)+((cn->tot_serv-cn->ava_serv)*dt)/(cn->tot_serv*tt);
		}
		cn->last_time = ca->last_time = sim->cur_time;
		EGsimDisplayNode(cn,stderr);
		/* process the event depending on the type of node that we are attached to
		 * */
		switch(cn->type)
		{
			case EGsimN:
				/* in this case two things happen, first we free a server at cn, and
				 * second, we have to move this agent to the next node */ 
				/* we start by taking care of the server being freed */
				if(EGeListIsEmpty(&(cn->queue)))
					cn->ava_serv++;
				else
				{
					na = EGcontainerOf(cn->queue.next,EGsimAgent_t,queue_cn);
					EGeListDel(cn->queue.next);
					dt = cn->num - cn->queue_sz + 1;
					cn->avg_wtime = (cn->avg_wtime*(dt-1))/dt+(sim->cur_time-na->last_time)/dt;
					na->queue_time += sim->cur_time - na->last_time;
					na->last_time = sim->cur_time;
					na->heap_cn.val = sim->cur_time + cn->gen(sim->cur_time,cn->data);
					dbl_EGeHeapSiftUp(&(sim->hp),&(na->heap_cn));
					cn->queue_sz--;
				}
				/* now we move the current agent in the network */
				/* detect the node where this agent is moving to */
				head = &(cn->node.out_edge);
				it  = head->next;
				dtmp2 = dtmp = EGrandU01(&(cn->g));
				ce = EGcontainerOf(it,EGsimEdge_t,edge.tail_cn);
				while( (it != head) && (ce->p < dtmp) )
				{
					dtmp -= ce->p;
					it = it->next;
					if(it != head) ce = EGcontainerOf(it,EGsimEdge_t,edge.tail_cn);
					else break;
				}
				TEST(it == head, "We were expecting another edge comming out from node %zd, total outgoing probability less than %lf",cn - sim->all_nodes + 1, dtmp2);
				nn = EGcontainerOf(ce->edge.head,EGsimNode_t,node);
				/* update the next node, if there is a server available, use it */
				if(!nn->num) nn->start_time = sim->cur_time;
				nn->num++;
				EGsimDisplayNode(nn,stderr);
				dt = sim->cur_time - nn->last_time ;
				tt = (sim->cur_time - nn->start_time);
				if((dt > 0.0) && (tt > 1.0))
				{
					nn->avg_queue = nn->avg_queue*((tt-dt)/(tt))+((double)(nn->queue_sz))*(dt/tt);
					if(nn->queue_sz>4)
						nn->avg_q5 = nn->avg_q5*((tt-dt)/tt)+dt/tt;
					else
						nn->avg_q5 *= ((tt-dt)/tt);
					if(nn->tot_serv)
						nn->avg_usage = nn->avg_usage*((tt-dt)/tt)+((nn->tot_serv-nn->ava_serv)*dt)/(nn->tot_serv*tt);
				}
				nn->last_time = sim->cur_time;
				EGsimDisplayNode(nn,stderr);
				if(nn->type == EGsimT)
					ca->heap_cn.val = sim->cur_time;
				else if(nn->ava_serv)
				{
					dt = nn->num - nn->queue_sz + 1;
					nn->avg_wtime = (nn->avg_wtime*(dt-1))/dt;
					nn->ava_serv--;
					ca->heap_cn.val = sim->cur_time + nn->gen(sim->cur_time,nn->data);
				}
				/* otherwise, the element ends up in the queue of the next node */
				else
				{
					nn->queue_sz++;
					ca->heap_cn.val = DBL_MAX;
					EGeListAddBefore(&(ca->queue_cn),&(nn->queue));
				}
				ca->node = nn;
				rval = dbl_EGeHeapSiftDown(&(sim->hp),&(ca->heap_cn));
				CHECKRVALG(rval,CLEANUP);
				/* done */
				break;
			case EGsimT:
				sim->agent_sz--; 
				MESSAGE(EG_SIM_VERBOSE,"Agent done, cycle_t = %lf, queue_t = %lf, serv_t = %lf",ca->service_time+ca->queue_time, ca->queue_time, ca->service_time);
				dbl_EGeHeapDel(&(sim->hp),&(ca->heap_cn));
				EGmemSlabPoolFree(ca);
				break;
			case EGsimG:
				/* again, two things happen, first, we create a new event, and then
				 * move the current event through the network */
				/* first we move the current agent in the network */
				/* detect the node where this agent is moving to */
				head = &(cn->node.out_edge);
				it  = head->next;
				dtmp2 = dtmp = EGrandU01(&(cn->g));
				ce = EGcontainerOf(it,EGsimEdge_t,edge.tail_cn);
				while( (it != head) && (ce->p < dtmp) )
				{
					MESSAGE(EG_SIM_VERBOSE,"Discarding edge %zd, weight %lf",ce - sim->all_edges+1,ce->p);
					dtmp -= ce->p;
					it = it->next;
					if(it != head) ce = EGcontainerOf(it,EGsimEdge_t,edge.tail_cn);
					else break;
				}
				TEST(it == head , "We were expecting another edge comming out from node %zd, total outgoing probability less than %lf, outgoing edges %u",cn - sim->all_nodes + 1, dtmp2, cn->node.out_size);
				nn = EGcontainerOf(ce->edge.head,EGsimNode_t,node);
				/* update the next node, if there is a server available, use it */
				if(!nn->num) nn->start_time = sim->cur_time;
				nn->num++;
				EGsimDisplayNode(nn,stderr);
				dt = sim->cur_time - nn->last_time ;
				tt = (sim->cur_time - nn->start_time);
				if((dt > 0.0) && (tt > 1.0))
				{
					nn->avg_queue = nn->avg_queue*((tt-dt)/(tt))+((double)(nn->queue_sz))*(dt/tt);
					if(nn->queue_sz>4)
						nn->avg_q5 = nn->avg_q5*((tt-dt)/tt)+dt/tt;
					else
						nn->avg_q5 *= ((tt-dt)/tt);
					if(nn->tot_serv)
						nn->avg_usage = nn->avg_usage*((tt-dt)/tt)+((nn->tot_serv-nn->ava_serv)*dt)/(nn->tot_serv*tt);
				}
				nn->last_time = sim->cur_time;
				EGsimDisplayNode(nn,stderr);
				if(nn->type == EGsimT)
					ca->heap_cn.val = sim->cur_time;
				else if(nn->ava_serv)
				{
					nn->ava_serv--;
					ca->heap_cn.val = sim->cur_time + nn->gen(sim->cur_time,nn->data);
					dt = nn->num - nn->queue_sz + 1;
					nn->avg_wtime = (nn->avg_wtime*(dt-1))/dt;
				}
				/* otherwise, the element ends up in the queue of the enxt node */
				else
				{
					nn->queue_sz++;
					ca->heap_cn.val = DBL_MAX;
					EGeListAddBefore(&(ca->queue_cn),&(nn->queue));
				}
				ca->node = nn;
				rval = dbl_EGeHeapSiftDown(&(sim->hp),&(ca->heap_cn));
				CHECKRVALG(rval,CLEANUP);
				/* and now create a new event */
				rval = EGsimNewEvent(sim,cn);
				CHECKRVALG(rval,CLEANUP);
				/* done */
				break;
			default:
				TEST(1,"Unknown node type %d, unable to process",cn->type);
				break;
		}

	}
	CLEANUP:
	return rval;
}
/* ========================================================================= */
double EGsimVRPoisson(const double t, void*const data)
{
	EGsimVRPoisson_t*const vrp=(EGsimVRPoisson_t*)data; 
	const int max_int = (int)(vrp->nint);
	const int laps = floor(t/vrp->max_t);
	double cur_t = t- laps*vrp->max_t, u = EGrandU01(&(vrp->g)), rdbl=0.0, adj=0.0;
	int cur_int = 0;
	/* find current interval */
	while( vrp->interval[cur_int] < cur_t)
	{
		cur_t -= vrp->interval[cur_int];
		cur_int = (cur_int+1)%max_int;
	}
	/* compute adjusted probability for interval */
	adj = 1 - exp(-1*(vrp->interval[cur_int]-cur_t)*vrp->rate[cur_int]);
	/* now we compute the return value */
	MESSAGE(EG_SIM_VERBOSE,"U %.2lf, interval %d, rdbl %.2lf, t %.2lf, ct %.2lf, 1-w %.6lg adj %.2lf, rate %.2lf, lng %.2lf",u,cur_int,rdbl, t, cur_t, 1-vrp->weight[cur_int], adj, vrp->rate[cur_int], vrp->interval[cur_int]);
	while(u > adj)
	{
		rdbl += vrp->interval[cur_int] - cur_t;
		cur_t = 0;
		u = (u-adj)/(1-adj);
		cur_int = (cur_int+1)%max_int;
		adj = vrp->weight[cur_int];
		MESSAGE(EG_SIM_VERBOSE,"U %.2lf, interval %d, rdbl %.2lf, t %.2lf, ct %.2lf, 1-w %.6lg adj %.2lf, rate %.2lf, lng %.2lf",u,cur_int,rdbl, t, cur_t, 1-vrp->weight[cur_int], adj, vrp->rate[cur_int], vrp->interval[cur_int]);
	}
	rdbl -= log(u)/vrp->rate[cur_int];
	#if EG_SIM_DEBUG <= DEBUG
	vrp->ngen[cur_int]++;
	vrp->sumxi[cur_int]+=rdbl;
	#endif
	MESSAGE(EG_SIM_VERBOSE,"Next time %.6lg",rdbl);
	return rdbl;
}
/* ========================================================================= */
int EGsimVRPLoadFile(EGsimVRPoisson_t*const data,FILE*input)
{
	const char delim[3] = " ";
	const char coment[3] = "%#";
	char str[2048], *curr,*next;
	size_t i=0;
	int n_line=1, n_token=0, rval= EGioReadLine(str,(size_t)2048,input);
	CHECKRVALG(rval,CLEANUP);
	curr = str;
	EGsimNextToken(input);
	data->nint = atoi(next);
	data->interval = EGsMalloc(double,data->nint);
	data->rate = EGsMalloc(double,data->nint);
	data->weight = EGsMalloc(double,data->nint);
	data->max_t = 0.0;
	#if EG_SIM_DEBUG <= DEBUG
	data->ngen = EGsMalloc(size_t,data->nint);
	data->sumxi = EGsMalloc(double,data->nint);
	#endif
	for( i = 0 ; i < data->nint ; i++)
	{
		EGsimNextToken(input);
		data->max_t += data->interval[i] = strtod(next,0);
		EGsimNextToken(input);
		data->rate[i] = strtod(next,0);
		data->weight[i] = 1 - exp(-1*data->interval[i]*data->rate[i]);
	}
	CLEANUP:
	return rval;
}
/* ========================================================================= */
/** @} */
