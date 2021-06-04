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
 * @ingroup EGpermIt */
/** @addtogroup EGpermIt */
/** @{ */
#include "eg_perm_it.h"
/* ========================================================================= */
#if __PERM_IT_NO_INLINE__
int EGpermItNext(EGpermIt_t*const prit)
{
	int*const c = prit->c_focus;
	int*const o = prit->o_focus;
	int*const arr = prit->tuple; 
	int tmp, j=prit->sz-1, s=0, pos=0;
	int  q=c[j]+o[j];
	/* find where we should make the following switch */
	while(q<0 || q>j)
	{
		if(q>j)
		{
			if (!j) return 0;
			s+=1;
		}
		o[j]=-o[j];
		j-=1;
		q=c[j]+o[j];
	}
	/* now we just switch positions j-c_j+s and j-q+s */
	prit->changed_pos = pos = c[j] > q ? j-c[j]+s:j-q+s;
	c[j] = q;
	tmp = arr[pos];
	arr[pos] = arr[pos+1];
	arr[pos+1] = tmp;
	/* and we are done */
	return 1;
}
#endif
	
/* ========================================================================= */
/** @} */
/* end of eg_gcit.c */

