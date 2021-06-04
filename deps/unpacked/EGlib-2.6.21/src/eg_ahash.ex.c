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
 * @ingroup EGaHash */
/** @addtogroup EGaHash */
/** @{ */
/* ========================================================================= */
/** @brief This program randomly generate a hash table of size 1000 and fill it
 * with 1000 elements, and then output its statistics.
 * */
/* ========================================================================= */
#include "EGlib.h"
#define HASH_SIZE 20000U
#define HASH_MAXSIZE 20U
#define HASH_MAXFACTOR 400000U
#define TRY 40U

/* ========================================================================= */
/** @brief main function */
int main (void)
{
	unsigned factor = 1;
	double ratio = 1;
	int rval = 0;
	EGtimer_t timer;
	EGrandState_t rstate;
	EGaHashTable_t htable=EGaHashTableNull;
	EGaHashInfo_t hinfo[HASH_SIZE];
	char*const base=(char*)hinfo;
	const size_t stsz=sizeof(EGaHashInfo_t);
	unsigned success = 0;
	register int i = HASH_SIZE, j = TRY;
	EGkey_t key;
	int info;
	EGlib_info();
	EGlib_version();
	EGrandInit(&rstate);
	EGaHashTableInit(&htable,HASH_MAXSIZE);
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	while(i--)
	{
		hinfo[i].key = EGrand(&rstate);
	}
	EGtimerReset(&timer);
	EGtimerStart(&timer);
	for( i = HASH_SIZE ; i-- ; )
	{
		EGaHashTableAdd(base,stsz,&htable,i);
	}
	EGtimerStop(&timer);
	MESSAGE(0,"Average time adding %u elements to the table %15le", 
					HASH_SIZE,timer.time/HASH_SIZE);
	for(factor = 2 ; factor<HASH_MAXFACTOR ; factor=(factor*3)/2) 
	{
		EGtimerReset(&timer);
		EGtimerStart(&timer);
		success = 0;
		j = TRY;
		while(j--)
		{
			for( i = HASH_SIZE; i-- ;)
			{
				key = EGrand(&rstate);
				info = EGaHashTableFind(base,stsz,&htable,key);
				if(info<INT_MAX)
				{
					success++;
					info = EGaHashTableFindNext(base,stsz,&htable,info);
				}
			}
		}
		EGtimerStop(&timer);
		ratio = (double)htable.sz;
		ratio = ratio/((double)(htable.max_sz));
		MESSAGE(0,"Average time finding %15u elements in table %15le (%3u success), ratio %15le size %10zd max_sz %10zd", 
						TRY*HASH_SIZE, timer.time/(TRY*HASH_SIZE), success, ratio,htable.sz,htable.max_sz);
		EGaHashTableResize(base,stsz,&htable,HASH_MAXSIZE*factor);
	}
	CLEANUP:
	EGaHashTableClear(&htable);
	return rval;
}

/* ========================================================================= */
/** @} */
