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
#include "eg_mem.h"
#include "eg_adgraph.h"
/** @file
 * @ingroup EGaDgraph */
/** @addtogroup EGaDgraph */
/** @{ */
/* ========================================================================= */
void EGaDGInit(
		EGaDG_t*const G, 
		const uint32_t n,
		const uint32_t m,
		const size_t szof_node,
		const size_t szof_edge,
		char*const all_nodes,
		char*const all_edges)
{
	register int i;
	uint32_t u,v;
	EGaDN_t *cn=0, *hn=0, *tn=0;
	EGaDE_t*ce = 0;
	memset(G,0,sizeof(EGaDG_t));
	G->n_nodes = n,
	G->n_edges = m,
	G->szof_node = szof_node,
	G->szof_edge = szof_edge,
	G->all_nodes = all_nodes,
	G->all_edges = all_edges;
	G->all_out = EGsMalloc(uint32_t,m);
	G->all_in = EGsMalloc(uint32_t,m);
	for( i = (int)n ; i-- ; ) 
	{
		cn = EGaDGgetNode(i,G);
		cn->in_sz = cn->out_sz = 0;
	}
	for( i = (int)m ; i-- ; )
	{
		ce = EGaDGgetEdge(i,G);
		EGaDGgetNode(ce->head,G)->in_sz++,
		EGaDGgetNode(ce->tail,G)->out_sz++;
	}
	for( u=0, i=0, v=0 ; i<(int)n ; i++)
	{
		cn = EGaDGgetNode(i,G);
		cn->out_beg = u,
		cn->in_beg = v;
		u += cn->out_sz,
		v += cn->in_sz;
		cn->out_sz = cn->in_sz = 0;
	}
	for( i = (int)m ; i-- ; )
	{
		ce = EGaDGgetEdge(i,G);
		hn = EGaDGgetNode(ce->head,G),
		tn = EGaDGgetNode(ce->tail,G);
		G->all_in[hn->in_beg+(hn->in_sz++)] = (uint32_t)i;
		G->all_out[tn->out_beg+(tn->out_sz++)] = (uint32_t)i;
	}
}
/* ========================================================================= */
/** @} */
