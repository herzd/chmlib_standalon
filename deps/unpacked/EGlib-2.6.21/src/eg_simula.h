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
/* ========================================================================= */
/** @defgroup EGsimula EGsimula
 *
 * Here we define a simple network simulator, the idea is to simulate process
 * through a network, where each node is either a generator of events, a sink
 * (or final node) of events, or a FIFO queue composed of \f$k\f$ identical 
 * servers. And where the events leave a node to the next node depending on the
 * probability on the outgoing edges (the sum of the probabilities on the
 * outgoing edges should be one).
 *
 * @par History:
 * - 2006-08-14
 * 		- First implementation 
 * - 2008-03-22
 * 		- Add suport for variable rate poisson process
 * 		- Change interface for random variables
 *
 * */
/** @file 
 * @ingroup EGsimula */
/** @addtogroup EGsimula */
/** @{ 
 * @example eg_simula.ex.c
 * This is a simple example of the usage of this simulator
 * @ref EGsimula */
#include "eg_config.h"
#include "eg_macros.h"
#include "eg_random.h"
#include "eg_edgraph.h"
#include "dbl_eg_eheap.h"
#include "eg_memslab.h"
#ifndef __EG_SIMULA_H__
#define __EG_SIMULA_H__

/* ========================================================================= */
/** @brief verbose level */
#define EG_SIM_VERBOSE 1000
/* ========================================================================= */
/** @brief debug level */
#define EG_SIM_DEBUG 10000
/* ========================================================================= */
/** @brief strcture that holds an edge (with probability weight). */
typedef struct 
{
	double p;	/**< @brief Probability that we will use this (outgoing) edge. */
	EGeDgraphEdge_t edge;	/**< @brief Actual underlying edge */
} EGsimEdge_t;
/* ========================================================================= */
/** @brief all types of availables nodes */
typedef enum 
{
	EGsimG, /**< @brief Type of generator nodes */
	EGsimT, /**< @brief Type of sink nodes */
	EGsimN  /**< @brief Type of normal nodes */
} EGsimNodeType_t;
/* ========================================================================= */
/** @brief all types of random variables */
typedef enum
{
	EGsimRV_novar=0,/**< @brief no random variable */
	EGsimRV_2param,	/**< @brief 2-parameter random variables */
	EGsimRV_VRP			/**< @brief variable rate poisson process */
} EGsimRandomType_t;
/* ========================================================================= */
/** @brief structure to hold a variable rate poisson process. The idea is to 
 * be able to simulate a variable rate poison process whose rate changes a
 * finite number of times, and the changes are cyclic in time. i.e. 
 * \f$ \lambda(t) = \lambda(t\mathrm{mod} t_max) \f$ */
typedef struct
{
	size_t nint;			/**< @brief number of rate changes */ 
	double*interval;	/**< @brief length of the interval */
	double*rate;			/**< @brief variable rate for the corresponding interval */
	double*weight; 		/**< @brief the value of 
												 \f$\int_{0}^{t_i}\lambda_ie^{\lambda_it}dt\f$*/
	EGrandState_t g;	/**< @brief random state to use for this variable */
	double max_t;			/**< @brief sum of time intervals */
	size_t* ngen;
	double* sumxi;
} EGsimVRPoisson_t;
/* ========================================================================= */
/** @brief initialize an #EGsimVRPoisson_t structure */
#define EGsimVRPInit(__VRP) do{\
	EGsimVRPoisson_t*const __EGVRP = (__VRP);\
	memset(__EGVRP,0,sizeof(EGsimVRPoisson_t));\
	EGrandInit(&(__EGVRP->g));}while(0)
/* ========================================================================= */
/** @brief display statistics for #EGsimVRPoisson_t */
#if EG_SIM_DEBUG <= DEBUG
#define EGsimVRPStats(__EGvrp) do{\
	size_t __EGvrpi = 0;\
	fprintf(stderr,"stats for %p:\n",__EGvrp);\
	for( ; __EGvrpi < __EGvrp->nint ; __EGvrpi++){\
		fprintf(stderr,"l %.2lf rate %.2lf n %6zu avg %.5lf\n",\
						__EGvrp->interval[__EGvrpi], __EGvrp->rate[__EGvrpi],\
						__EGvrp->ngen[__EGvrpi], \
						__EGvrp->sumxi[__EGvrpi]/__EGvrp->ngen[__EGvrpi]);}\
	EGfree(__EGvrp->ngen);\
	EGfree(__EGvrp->sumxi);}while(0)
#else
#define EGsimVRPStats(__EGvrp)
#endif
/* ========================================================================= */
/** @brief clear an #EGsimVRPoisson_t structure */
#define EGsimVRPClear(__VRP) do{\
	EGsimVRPoisson_t*const __EGVRP = (__VRP);\
	EGsimVRPStats(__EGVRP);\
	EGfree(__EGVRP->interval);\
	EGfree(__EGVRP->rate);\
	EGfree(__EGVRP->weight);}while(0)
/* ========================================================================= */
/** @brief structure to hold data for random variables on two parameters */
typedef struct
{
	double a;					/**< @brief first parameter */
	double b;					/**< @brief second parameter */
	EGrandState_t g;	/**< @brief Random number generator for the node */
	size_t ngen;
	double sumxi;
} EGsimRData_t;
/* ========================================================================= */
/** @brief load interval description from file, the format of the file is the
 * following:
 * nintervals
 * interval_length_i rate_interval_i
 * @param file from where we will read the description
 * @param data where to store the information
 * @return zero on success, non-zero otherwise.
 * */
int EGsimVRPLoadFile(EGsimVRPoisson_t*const data,FILE*file);
/* ========================================================================= */
/** @brief Initialize an #EGsimRData_t structure */
#define EGsimRDInit(__RD) do{\
	EGsimRData_t*const __EGRD = (__RD);\
	memset(__EGRD,0,sizeof(EGsimRData_t));\
	EGrandInit(&(__EGRD->g));}while(0)
/* ========================================================================= */
/** @brief display statistics for #EGsimRData_t */
#if EG_SIM_DEBUG <= DEBUG
#define EGsimRDStats(__EGrd) do{\
	fprintf(stderr,"stats for %p:\n",__EGrd);\
		fprintf(stderr,"n %6zu avg %.5lf\n",\
						__EGrd->ngen, __EGrd->sumxi/__EGrd->ngen);}while(0)
#else
#define EGsimRDStats(__EGrd)
#endif
/* ========================================================================= */
/** @brief clear an #EGsimRData_t structure */
#define EGsimRDClear(__RD) do{\
	EGsimRData_t*const __EGRD = (__RD);\
	EGsimRDStats(__EGRD);\
	memset(__EGRD,0,sizeof(EGsimRData_t));\
	}while(0)
/* ========================================================================= */
/** @brief type of distribution functions, the first and second parameters are
 * double values, and the third parameter is the random seed to be used */
typedef double (*EGsimDist_f) (const double,void*const);
/* ========================================================================= */
/** @brief Exponential distribution function .
 * @param t time (unused)
 * @param data pointer to an #EGsimRData_t structure */
double EGsimExp(const double t,void*const data);
/* ========================================================================= */
/** @brief Uniform distribution function 
 * @param t time (unused)
 * @param data pointer to an #EGsimRData_t structure */
double EGsimUniform(const double t,void*const data);
/* ========================================================================= */
/** @brief Normal distribution function 
 * @param t time (unused)
 * @param data pointer to an #EGsimRData_t structure */
double EGsimNormal(const double t, void*const data);
/* ========================================================================= */
/** @brief variable rate poisson process
 * @param t defining the rate to start with.
 * @param data pointer to an #EGsimVRPoisson_t structure */
double EGsimVRPoisson(const double t, void*const data);
/* ========================================================================= */
/** @brief structure that holds a node in the network */
typedef struct 
{
	EGeDgraphNode_t node;	/**< @brief Actual node storing the network information */
	EGrandState_t g;			/**< @brief random state variable for edge/selection */
	EGeList_t queue;			/**< @brief Head of the incomming queue */
	double start_time;		/**< @brief Time of first agent entering this node */
	double last_time;			/**< @brief Last time statistics where updated */
	double avg_cycle;			/**< @brief Average cycle time of served agents */
	double avg_queue;			/**< @brief Average queue length (up to last update time, 
															and starting from #EGsimNode_t::start_time ) */
	double avg_usage;			/**< @brief Average percentage of used time */
	double avg_q5;				/**< @brief Average time of queue with 5 or more clients*/
	double avg_wtime;			/**< @brief Average waiting time on queue */
	EGsimDist_f gen;			/**< @brief Distribution function for the process time */
	void*data;						/**< @brief asociated data for generator */
	EGsimRandomType_t rtype;/**< @brief random variable type */
	EGsimNodeType_t type;	/**< @brief Type of the node */
	unsigned tot_serv;		/**< @brief servers available at the node */
	unsigned ava_serv;		/**< @brief Total available servers in current time */
	unsigned queue_sz;		/**< @brief Current length of the input queue */	
	unsigned num;					/**< @brief Number of agents that have pass through the
															node */
} EGsimNode_t;
/* ========================================================================= */
/** @brief structure that holds an agent going trough the network */
typedef struct 
{
	dbl_EGeHeapCn_t heap_cn;/**< connector in the heap containing the agent */
	EGeList_t queue_cn;		/**< Connector in the queue holding it */
	EGsimNode_t* node;		/**< Node where this agent is being served or queue */
	double queue_time;		/**< Time spend waiting on queue */
	double service_time;	/**< Time spend being serviced on the network */
	double last_time;			/**< Last time when statistics where updated */
} EGsimAgent_t;
/* ========================================================================= */
/** @brief structure holding the network being simulated as well as the heap
 * containing the events to be provessed */
typedef struct 
{
	EGeDgraph_t G;		/**< Actual network being simulated */
	dbl_EGeHeap_t hp;	/**< Heap containing events to be processed */
	unsigned agent_sz;/**< Number of agents (currently) traversing the network */
	unsigned agent_tot;/**< Agents that traversed the network */
	double cur_time;	/**< Current time on the system */
	double max_time;	/**< Horizon where to stop the simulation */
	EGmemSlabPool_t mp;/**< Memory pool holding the agents in the system */
	EGsimNode_t*all_nodes;/**< array containing all nodes in the network */
	EGsimEdge_t*all_edges;/**< array containing all edges in the network */
} EGsim_t;
/* ========================================================================= */
/** @brief Read a file describing a simulation network.
 * @param sim #EGsim_t structure where we will load the network, #EGsimInit
 * should have been called before.
 * @param file from where we should read the graph.
 * @return zero on success, non-zero otherwise 
 * @note The random state will be taken from the global seed also, each node
 * has an independent random stream that is 2^128 steps away from the random
 * stream of the previous node. 
 * */
int EGsimLoadFile(EGsim_t*const sim,FILE*file); 
/* ========================================================================= */
/** @brief Given a simulation network, run the simulation from
 * #EGsim_t::cur_time until #EGsim_t::max_time
 * @param sim #EGsim_t structure where we will run the simulation.
 * @return zero on success, non-zero otherwise.
 * @note The random state will be taken from the global seed also, each node
 * has an independent random stream that is 2^128 steps away from the random
 * stream of the previous node. 
 * */
int EGsim(EGsim_t*const sim);
/* ========================================================================= */
/** @brief initialize an agent as a new agent starting at c_time.
 * @param c_time time when this agent is being born 
 * @param agent agent to set */
#define EGsimSetAgent(agent,c_time) ({\
	EGsimAgent_t*const __EGsimA = (agent);\
	__EGsimA->last_time = (c_time);\
	dbl_EGeHeapCnInit(&(__EGsimA->heap_cn));\
	EGeListInit(&(__EGsimA->queue_cn));\
	__EGsimA->node = 0;\
	__EGsimA->queue_time = __EGsimA->service_time = 0.0;})
/* ========================================================================= */
/** @brief Clear a structure of type #EGsim_t
 * @param sim structure to initialize */
#define EGsimClear(sim) do{\
	EGsim_t*const __EGsim = (sim);\
	dbl_EGeHeapCn_t* __EGsimHcn;\
	unsigned __EGsimi = __EGsim->G.n_nodes;\
	while((__EGsimHcn = dbl_EGeHeapGetMin(&(__EGsim->hp)))){\
		dbl_EGeHeapDel(&(__EGsim->hp),__EGsimHcn);\
		EGmemSlabPoolFree(__EGsimHcn);}\
	dbl_EGeHeapResize(&(__EGsim->hp),0);\
	while(__EGsimi--){EGsimNodeClear(__EGsim->all_nodes+__EGsimi);}\
	EGeDgraphClear(&(__EGsim->G));\
	EGfree(__EGsim->all_nodes);\
	EGfree(__EGsim->all_edges);\
	EGmemSlabPoolClear(&(__EGsim->mp));}while(0)
/* ========================================================================= */
/** @brief Initialize a structure of type #EGsim_t 
 * @param sim structure to initialize */
#define EGsimInit(sim) do{\
	EGsim_t*const __EGsim = (sim);\
	EGeDgraphInit(&(__EGsim->G));\
	dbl_EGeHeapInit(&(__EGsim->hp));\
	__EGsim->hp.d = 2;\
	dbl_EGeHeapResize(&(__EGsim->hp),1024);\
	__EGsim->agent_sz = __EGsim->agent_tot = 0;\
	__EGsim->cur_time = __EGsim->max_time = 0.0;\
	__EGsim->all_nodes = 0;\
	__EGsim->all_edges = 0;\
	EGmemSlabPoolInit(&(__EGsim->mp),sizeof(EGsimAgent_t), 0, 0);}while(0)
/* ========================================================================= */
/** @brief Intialize a structure of type #EGsimEdge_t
 * @param e pointer to the structure to initialize */
#define EGsimEdgeInit(e) do{\
	EGsimEdge_t*const __EGsimE = (e);\
	EGeDgraphEdgeInit(&(__EGsimE->edge));}while(0)
/* ========================================================================= */
/** @brief Clear a structure of type #EGsimEdge_t
 * @param e pointer to the structure to clear */
#define EGsimEdgeClear(e) do{\
	EGsimEdge_t*const __EGsimE = (e);\
	EGeDgraphEdgeClear(&(__EGsimE->edge));}while(0)
/* ========================================================================= */
/** @brief Initialize a structure of type #EGsimNode_t
 * @param nod pointer to the structure */
#define EGsimNodeInit(nod) do{\
	EGsimNode_t*const __EGsimN = (nod);\
	EGeDgraphNodeInit(&(__EGsimN->node));\
	EGeListInit(&(__EGsimN->queue));\
	__EGsimN->start_time = __EGsimN->last_time = __EGsimN->avg_cycle = __EGsimN->avg_queue = __EGsimN->avg_usage = __EGsimN->avg_q5 = __EGsimN->avg_wtime =  0.0 ;\
	__EGsimN->tot_serv= __EGsimN->ava_serv= __EGsimN->queue_sz = __EGsimN->num = 0;\
	__EGsimN->type = EGsimT;\
	__EGsimN->rtype = EGsimRV_novar;\
	__EGsimN->data=0;\
	EGrandInit(&(__EGsimN->g));\
	__EGsimN->gen=0;}while(0)
/* ========================================================================= */
/** @brief Clear a structure of type #EGsimNode_t
 * @param __nod pointer to the structure */
#define EGsimNodeClear(__nod) do{\
	EGsimNode_t*const __EGsimN = (__nod);\
	switch(__EGsimN->rtype){\
		case EGsimRV_novar:\
			break;\
		case EGsimRV_2param:\
			EGsimRDClear((__EGsimN->data));\
			EGfree(__EGsimN->data);\
			break;\
		case EGsimRV_VRP:\
			EGsimVRPClear((__EGsimN->data));\
			EGfree(__EGsimN->data);\
			break;\
		default:\
			EXIT(1,"Unknown random variable type %d!",__EGsimN->rtype);\
			break;}\
	EGeDgraphNodeClear(&(__EGsimN->node));}while(0)
/* ========================================================================= */
/** @} */
#endif
