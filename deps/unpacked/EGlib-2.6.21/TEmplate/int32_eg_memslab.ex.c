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
 * @ingroup EGmemSlab
 * */
/** @addtogroup EGmemSlab */
/** @{ */
#include "EGlib.h"
/* ========================================================================= */
/** brief simple destructor for heap connectors */
static void int32_my_constr (void *ptr)
{
	int32_EGeHeapCnInit ((int32_EGeHeapCn_t *) ptr);
}

/* ========================================================================= */
/** @brief simple destructor for heap connectors */
static void int32_my_dest (void *ptr)
{
	int32_EGeHeapCnClear ((int32_EGeHeapCn_t *) ptr);
	ptr = 0;
}

/* ========================================================================= */
/** @brief this program expect only one parameter, the number of elements to be
 * created. */
int main (int argc,
					char **argv)
{
	unsigned int n_elem = 0;
	unsigned int cnt = 0;
	int rval=0;
	EGrandState_t seed;
	int32_EGeHeap_t my_heap;
	int32_EGeHeapCn_t *hp_cn=0;
	EGmemSlabPool_t pool;
	double dbl = 0;
	EGlib_info();
	EGlib_version();
	EGlpNumStart();
	EGrandInit(&seed);
	/* parsing input */
	if (argc != 2)
	{
	USAGE:
		fprintf (stderr, "Usage: %s N\n\twhere N is the number of elements to "
						 "create randomly", argv[0]);
		return 1;
	}
	n_elem = atoi (argv[1]);
	if (!n_elem)
		goto USAGE;
	/* initializing internals */
	EGmemSlabPoolInit (&pool, sizeof (int32_EGeHeapCn_t), int32_my_constr, int32_my_dest);
	int32_EGeHeapInit (&my_heap);
	int32_EGeHeapChangeD (&my_heap, 4);
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	/* rounds of 1000, create 1000, delete 875 smallers */
	while (n_elem >= 1000)
	{
		for (cnt = 1000; cnt--;)
		{
			if (int32_EGeHeapIsFull (&my_heap))
				int32_EGeHeapResize (&my_heap, 1000 + my_heap.sz);
			hp_cn = (int32_EGeHeapCn_t *) EGmemSlabPoolAlloc (&pool);
			dbl = EGrandU01(&seed);
			int32_EGlpNumSet (hp_cn->val, dbl);
			int32_EGeHeapAdd (&my_heap, hp_cn);
		}
		for (cnt = 875; cnt--;)
		{
			hp_cn = int32_EGeHeapGetMin (&my_heap);
			int32_EGeHeapDel (&my_heap, hp_cn);
			EGmemSlabPoolFree (hp_cn);
		}
		n_elem -= 1000;
	}
	/* last tail, add the elements on the heap */
	while (n_elem--)
	{
		if (int32_EGeHeapIsFull (&my_heap))
			int32_EGeHeapResize (&my_heap, 1000 + my_heap.sz);
		hp_cn = (int32_EGeHeapCn_t *) EGmemSlabPoolAlloc (&pool);
		dbl = EGrandU01(&seed);
		int32_EGlpNumSet (hp_cn->val, dbl);
		int32_EGeHeapAdd (&my_heap, hp_cn);
	}
	/* now we free the remaining elements in batchs of 1000, and then shrink the
	 * memory pool */
	while (my_heap.sz)
	{
		for (cnt = 1000; cnt-- && my_heap.sz;)
		{
			hp_cn = int32_EGeHeapGetMin (&my_heap);
			int32_EGeHeapDel (&my_heap, hp_cn);
			EGmemSlabPoolFree (hp_cn);
		}
		EGmemSlabPoolShrink (&pool);
	}

	/* ending */
	CLEANUP:
	int32_EGeHeapResize (&my_heap, 0);
	int32_EGeHeapClear (&my_heap);
	EGmemSlabPoolClear (&pool);
	EGlpNumClear();
	return rval;
}

/* ========================================================================= */
/** @} */
