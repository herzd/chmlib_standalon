/* EGlib "Efficient General Library" provides some basic structures and
 * algorithms commons in many optimization algorithms.
 *
 * Copyright (C) 2005-2010 Daniel Espinoza
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
 * @ingroup EGrSet */
/** @addtogroup EGrSet */
/** @{ */
#include "eg_raset.h"
/* ========================================================================= */
void EGrsetSetgrnd(
	EGrset_t*const EGs,
	const int32_t grsz)
{
	int32_t i=0,r=0;
	/* resize arrays if needed */
	if(grsz > EGs->asz)
	{
		EGs->inv = EGrealloc(EGs->inv,sizeof(int32_t)*((size_t)grsz));
		EGs->set = EGrealloc(EGs->set,sizeof(int32_t)*((size_t)grsz));
		for(i=grsz ; i-->EGs->asz;){
			EGs->inv[i] = EGs->set[i] = i;}
		EGs->asz = grsz;
	}
	/* prune possibly bad elements in set */
	if(grsz < EGs->grsz)
	{
		for(i=EGs->grsz ;i-->grsz;)
		{
			/* delete element */
			EGrsetDel(EGs,i);
			/* swap elements out of the grsz set */
			r=EGs->set[i];
			__EGrsetSwap(EGs,i,r);
		}
	}
	/*set new ground size */
	EGs->grsz = grsz;
}
/* ========================================================================= */
/** @} */
