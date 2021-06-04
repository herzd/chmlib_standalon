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
 * @ingroup EGgraphUtil */
/** @addtogroup EGgraphUtil */
/** @{ */
/* ========================================================================= */
#include "eg_graph_util.h"
#include "eg_mem.h"
int EGguDegree(
/* ========================================================================= */
	int32_t const n,
	int32_t const m,
	int32_t const*const edges,
	int32_t*const in_d,
	int32_t*const in_beg,
	int32_t*const in_e,
	int32_t*const in_perm,
	int32_t*const ou_d,
	int32_t*const ou_beg,
	int32_t*const ou_e,
	int32_t*const ou_perm)
{
	int rval=0,i;
	int32_t h,t;
	if(in_d)
	{
		memset(in_d,0,sizeof(int32_t)*n);
		for(i=m;i--;) in_d[edges[2*i+1]]++;
		if(in_beg)
		{
			memset(in_beg,0,sizeof(int32_t)*n);
			for(i=1;i<n;i++) in_beg[i]=in_beg[i-1]+in_d[i-1];
			if(in_e)
			{
				memset(in_d,0,sizeof(int32_t)*n);
				for(i=m;i--;)
				{
					h=edges[2*i+1], t=edges[2*i];
					in_e[in_beg[h]+in_d[h]++]=t;
					if(in_perm) in_perm[i]=in_beg[h]+in_d[h]-1;
				}
			}
		}
	}
	if(ou_d)
	{
		memset(ou_d,0,sizeof(int32_t)*n);
		for(i=m;i--;) ou_d[edges[2*i]]++;
		if(ou_beg)
		{
			memset(ou_beg,0,sizeof(int32_t)*n);
			for(i=1;i<n;i++) ou_beg[i]=ou_beg[i-1]+ou_d[i-1];
			if(ou_e)
			{
				memset(ou_d,0,sizeof(int32_t)*n);
				for(i=m;i--;)
				{
					h=edges[2*i+1], t=edges[2*i];
					ou_e[ou_beg[t]+ou_d[t]++]=h;
					if(ou_perm) ou_perm[i]=ou_beg[t]+ou_d[t]-1;
				}
			}
		}
	}
	EG_RETURN(rval);
}

/* ========================================================================= */
/** @} */
