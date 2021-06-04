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
 * @ingroup EGslin_kern */
/** @addtogroup EGslin_kern */
/** @{ */
#include "eg_slk.h"
/* ========================================================================= */
int EGslkGetNext(EGflipper_t*const flip,const int node)
{
	const int*const tour = flip->tour;
	const int pos = flip->inv[node];
	const size_t sz = flip->sz - 1;
	if(flip->reverse)
	{
		if(pos == 0) return tour[sz];
		return tour[pos-1];
	}
	else
	{
		if(pos == (int)sz) return tour[0];
		return tour[pos+1];
	}
}

/* ========================================================================= */
int EGslkGetPrev(EGflipper_t*const flip,const int node)
{
	const int*const tour = flip->tour;
	const int pos = flip->inv[node];
	const size_t sz = flip->sz - 1;
	if(flip->reverse)
	{
		if(pos == (int)sz) return tour[0];
		return tour[pos+1];
	}
	else
	{
		if(pos == 0) return tour[sz];
		return tour[pos-1];
	}
}

/* ========================================================================= */
/** @brief compute the length of a segment in a flipper structure.
 * flip pointer to the lipper structure.
 * a beggining of the interval.
 * b end of the interval.
 * */
static int EGslkGetLength(EGflipper_t*const flip, int a, int b)
{
	const int a_pos = flip->inv[a];
	const int b_pos = flip->inv[b];
	const size_t sz = flip->sz;
	int length = 0;
	if(flip->reverse)
	{
		if( a_pos > b_pos ) length = a_pos - b_pos + 1;
		else length = (int)sz + a_pos + 1 - b_pos;
	}
	else
	{
		if( a_pos < b_pos ) length = b_pos - a_pos + 1;
		else length = (int)sz + b_pos + 1 - a_pos;
	}
	return length;
}

/* ========================================================================= */
int EGslkFlip(EGflipper_t*const flip,int a, int b)
{
	int*const tour = flip->tour;
	int*const inv = flip->inv;
	const size_t sz = flip->sz;
	int a_pos = inv[a], b_prev;
	int b_pos = inv[b], a_next;
	int itmp;
	int length = EGslkGetLength(flip,a,b);
	int reverse = 0;
	MESSAGE(EG_SLK_VERB,"Fliping tour[%d] = %d  and tour[%d] = %d length %d",a_pos,a,b_pos,b,length); 
	if(length > (int)sz / 2)
	{
		b_pos = inv[EGslkGetPrev(flip,a)];
		a_pos = inv[EGslkGetNext(flip,b)];
		a = tour[a_pos];
		b = tour[b_pos];
		length = EGslkGetLength(flip,a,b);
		reverse = 1;
	}
	a_next = EGslkGetNext(flip,a);
	b_prev = EGslkGetPrev(flip,b);
	MESSAGE(EG_SLK_VERB,"Fliping tour[%d] = %d  and tour[%d] = %d length %d",a_pos,a,b_pos,b,length); 
	EXIT(!length,"Imposible");
	while(length > 1)
	{
		MESSAGE(EG_SLK_VERB,"flip tour[%d] = %d  and tour[%d] = %d length %d",a_pos,a,b_pos,b,length); 
		itmp = tour[a_pos];
		tour[a_pos] = tour[b_pos];
		tour[b_pos] = itmp;
		itmp = inv[a];
		inv[a] = inv[b];
		inv[b] = itmp;
		a = a_next;
		b = b_prev;
		a_next = EGslkGetNext(flip,a);
		b_prev = EGslkGetPrev(flip,b);
		a_pos = inv[a];
		b_pos = inv[b];
		length -= 2;
	}
	if(reverse) flip->reverse = ~(flip->reverse);
	return 0;
}

/* ========================================================================= */
int EGflipperRemoveNode(EGflipper_t*const flip,const int node)
{
	int*const tour = flip->tour;
	int*const inv = flip->inv;
	int*const map = flip->map;
	register size_t i;
	map[node] = map[--(flip->sz)];
	for( i = 0 ; i < flip->sz ; i++) tour[i] = inv[i] = (int)i;
	return 0;
}

/* ========================================================================= */
int EGflipperAddNode(EGflipper_t*const flip,const int node)
{
	int*tour = 0;
	int*inv = 0;
	int*map = 0;
	if(flip->max_sz == flip->sz)
		EGflipperResize(flip,flip->sz+10);
	tour = flip->tour;
	inv = flip->inv;
	map = flip->map;
	map[flip->sz] = node;
	inv[flip->sz] = tour[flip->sz] = (int)(flip->sz);
	flip->sz++;
	return 0;
}

/* ========================================================================= */
void EGflipperCopy(const EGflipper_t*const src,EGflipper_t*const dst)
{
	register size_t i = src->sz;
	if(dst->max_sz < src->sz)
	EGflipperResize(dst,src->sz);
	while(i--)
	{
		dst->map[i] = src->map[i];
		dst->inv[i] = src->inv[i];
		dst->tour[i] = src->tour[i];
	}
	dst->sz = src->sz;
	return;
}

/* ========================================================================= */
int EGflipperLoadTour(EGflipper_t*const flip,const int*const tour,size_t const tsz)
{
	register size_t i = tsz;
	if(flip->max_sz < tsz) EGflipperResize(flip,tsz);
	for( ; i-- ; )
	{
		flip->tour[i] = (int)i;
		flip->inv[i] = (int)i;
		flip->map[i] = tour[i];
	}
	flip->sz = tsz;
	return 0;
}

/* ========================================================================= */
int EGflipperGetTour(EGflipper_t*const flip,int*const tour)
{
	register size_t i = flip->sz;
	int cur = 0;
	for( ; i-- ; )
	{
		tour[i] = flip->map[cur];
		cur = EGslkGetPrev(flip,cur);
	}
	return 0;
}

/* ========================================================================= */
int EGflipperDisplay(EGflipper_t*const flip,EGioFile_t* out)
{
	const int sz = (int)flip->sz;
	int i;
	EGioPrintf(out,"flipper %p, reverse %u, sz %d, max_sz %zd\n",((void*)flip),flip->reverse,sz,flip->max_sz);
	EGioPrintf(out,"tour");
	for( i = 0 ; i < sz ; i++)
		EGioPrintf(out," %3d",flip->tour[i]);
	EGioPrintf(out,"\ninv ");
	for( i = 0 ; i < sz ; i++)
		EGioPrintf(out," %3d",flip->inv[i]);
	EGioPrintf(out,"\nmap ");
	for( i = 0 ; i < sz ; i++)
		EGioPrintf(out," %3d",flip->map[i]);
	EGioPrintf(out,"\n");
	return 0;
}

/* ========================================================================= */
int EGslkEdgeCost(EGslkData_t*const data,const int a,const int b)
{
	return data->cost[a][b];
}

/* ========================================================================= */
int EGslkTourValue(EGslkData_t*const data,const int*const tour,const size_t sz)
{
	size_t i = 0;
	int cost = EGslkEdgeCost(data,tour[sz-1],tour[0]);
	for(; ++i < sz ;)
		cost += EGslkEdgeCost(data,tour[i-1],tour[i]);
	return cost;
}

/* ========================================================================= */
int EGflipperGetCost(EGslkData_t*const data,EGflipper_t*const flip)
{
	const int*const map = flip->map;
	const int*const tour = flip->tour;
	register size_t i = flip->sz-1;
	int cost=EGslkEdgeCost(data,map[tour[flip->sz-1]],map[tour[0]]);
	while(i--)
	{
		cost += EGslkEdgeCost(data,map[tour[i]],map[tour[i+1]]);
	}
	return cost;
}

/* ========================================================================= */
int EGslk2opt(EGslkData_t*const data,EGflipper_t*const flip)
{
	const int sz = (int)(flip->sz);
	int*const tour = flip->tour;
	int*const map = flip->map;
	int a = 0,b = 2,a_pos, b_pos, a_prev, b_next, best_a = 0, best_b = 2, gain, best_gain = 0, diff;
	/* if sz < 4, there is nothing to do */
	if(sz < 4) return 0;
	/* first loop, work while the gain is positive */
	do{
		best_gain = 0;
		/* we do a double loop selecting the edges to remove, and evaluate the 
		 * change */
		/* we will remove edge (a-1,a) */ 
		for( a_pos = 0 ; a_pos < sz ; a_pos++)
		{
			a = tour[a_pos];
			a_prev = EGslkGetPrev(flip,a);
			/* and edge (b,b+1) */
			for( b_pos = 0 ; b_pos < sz ; b_pos++) 
			{
				/* discard pair of edges that are adjacents or the same */
				diff = b_pos - a_pos;
				if(diff < 0) diff += sz;
				if(diff > sz/2) diff = sz-diff;
				if(diff < 2) continue;
				b = tour[b_pos];
				b_next = EGslkGetNext(flip,b);
				/* now price the move */
				gain = EGslkEdgeCost(data,map[a_prev],map[a]) + 
							 EGslkEdgeCost(data,map[b],map[b_next]) -
							 EGslkEdgeCost(data,map[a_prev],map[b]) -
							 EGslkEdgeCost(data,map[a],map[b_next]);
				/* save the best move */
				if(gain > best_gain)
				{
					MESSAGE(EG_SLK_VERB,"Best move is %d %d gain %d, (%d,%d)=%d + "
						"(%d,%d)=%d > (%d,%d)=%d + (%d,%d)=%d", a, b, gain, 
						map[a_prev], map[a], EGslkEdgeCost(data,map[a_prev],map[a]),
						map[b], map[b_next], EGslkEdgeCost(data,map[b],map[b_next]),
						map[a_prev],map[b], EGslkEdgeCost(data,map[a_prev],map[b]),
						map[a],map[b_next], EGslkEdgeCost(data,map[a],map[b_next]));
					best_a = a;
					best_b = b;
					best_gain = gain;
					/* if we are not doing a full search, we just stop and do the move */
					if(!EG_SLK_FULL_SEARCH) goto DO_MOVE;
				}
			}
		}
		/* if the best gain is positive, do the flip */
		if(best_gain > 0)
		{
			DO_MOVE:
			EGslkFlip(flip,best_a,best_b);
		}
		}while(best_gain >0);
	return 0;
}

/* ========================================================================= */
void EGslkDataClear(EGslkData_t*const data)
{
	register size_t i = data->dim;
	while(i--)
	{
		EGfree(data->cost[i]);
	}
	if(data->dim) EGfree(data->cost);
	if(data->curr_tour) EGfree(data->curr_tour);
	if(data->best_tour) EGfree(data->best_tour);
	if(data->x) EGfree(data->x);
	if(data->y) EGfree(data->y);
	if(data->demand) EGfree(data->demand);
	data->best_val = 0;
	data->dim = 0;
	data->cost = 0;
	data->demand = data->best_tour = data->curr_tour = 0;
	data->x = data->y = 0;
}

/* ========================================================================= */
int EGslkDataLoadTSPLIB95(EGslkData_t*const data,EGioFile_t*input)
{
	const char delim[3] = " :";
	const char coment[3] = "%#";
	register size_t i= 0,j = 0;
	int pos=0;
	int todo=1, gotdim=0, gotnodes=0, gotdist=0, gotcap=0, gotdepot=0, gotdem = 0;
	char str[2048], *argv[1024];
	int argc,rval = 0;
	/* this set data->dim to zero and free any memory allocated before */
	EGslkDataClear(data);
	/* we read line by line the file */
	while(todo)
	{
		EGioGets(str,2048,input);
		TEST((EGioEof(input) || EGioError(input)),"File ended prematurelly");
		EGioNParse(str,1024,delim,coment,&argc,argv);
		/* skip empty lines */
		if(!argc) continue;
		/* for the moment, we discard this information */
		if(strncmp(argv[0],"NAME",(size_t)33) == 0) continue;
		/* for the moment, we discard this information */
		else if(strncmp(argv[0],"COMMENT",(size_t)33) == 0) continue;
		/* for the moment, check type equal to CVRP */
		else if(strncmp(argv[0],"TYPE",(size_t)33) == 0)
		{
			if(argc != 2)
			{
				rval = 1;
				MESSAGE(EG_SLK_DEBUG,"TYPE specification has not 2 tokens");
				goto CLEANUP;
			}
			if(strncmp(argv[1],"CVRP",(size_t)33))
				{ 
					rval = 1; 
					MESSAGE(EG_SLK_DEBUG,"Problem type is not CVRP, instead %s",argv[1]);
					goto CLEANUP;
				}	
		}
		/* allocate all arrays */
		else if(strncmp(argv[0],"DIMENSION",(size_t)33) == 0)
		{
			if(gotdim)
			{
				rval = 1; 
				MESSAGE(EG_SLK_DEBUG,"Data file has more than one DIMENSION specification");
				goto CLEANUP;
			}
			if(argc != 2)
			{
				rval = 1; 
				MESSAGE(EG_SLK_DEBUG,"wrong number of tokens for DIMENSION");
				goto CLEANUP;
			}
			/* we allocate the problem */
			data->dim = atoi(argv[1]);
			if(data->dim < 1)
			{
				rval = 1;
				MESSAGE(EG_SLK_DEBUG,"DIMENSION is less than 1");
				goto CLEANUP;
			}
			data->cost = EGsMalloc(int*,data->dim);
			for( i = data->dim ; i-- ; )
				data->cost[i] = EGsMalloc(int,data->dim); 
			data->curr_tour = EGsMalloc(int,data->dim);
			data->best_tour = EGsMalloc(int,data->dim);
			data->x = EGsMalloc(double,data->dim);
			data->y = EGsMalloc(double,data->dim);
			data->demand = EGsMalloc(int,data->dim);
			gotdim = 1;
		}
		/* get type of distance */
		else if(strncmp(argv[0],"EDGE_WEIGHT_TYPE",(size_t)33) == 0)
		{
			if (argc != 2)
			{
				rval = 1; 
				MESSAGE(EG_SLK_DEBUG,"wrong number of tokens for EDGE_WEIGHT_TYPE");
				goto CLEANUP;
			}
			if(strncmp(argv[1],"EUC_2D",(size_t)33))
			{
				rval = 1;
				MESSAGE(EG_SLK_DEBUG,"only distance EUC_2D are supported");
				goto CLEANUP;
			}
			gotdist = 1;
		}
		/* get the truck's capacity */
		else if(strncmp(argv[0],"CAPACITY",(size_t)33) == 0)
		{
			if (argc != 2)
			{
				rval = 1; 
				MESSAGE(EG_SLK_DEBUG,"wrong number of tokens for CAPACITY");
				goto CLEANUP;
			}
			data->capacity = atoi(argv[1]);
			MESSAGE(EG_SLK_VERB,"Capacity is %d",data->capacity);
			if(data->capacity < 1)
			{
				rval = 1; 
				MESSAGE(EG_SLK_DEBUG,"CAPACITY is less than 1");
				goto CLEANUP;
			}
			gotcap = 1;
		}
		/* now we read the node coordinate section */
		else if(strncmp(argv[0],"NODE_COORD_SECTION",(size_t)33) == 0)
		{
			if (argc != 1)
			{
				rval = 1; 
				MESSAGE(EG_SLK_DEBUG,"wrong number of tokens for NODE_COORD_SECTION");
				goto CLEANUP;
			}
			if (!gotdim)
			{
				rval = 1; 
				MESSAGE(EG_SLK_DEBUG,"no dimension while reading NODE_COORD_SECTION");
				goto CLEANUP;
			}
			for( i = 0; (size_t)i < data->dim ; i++ )
			{
				EGioGets(str,2048,input);
				TEST((EGioEof(input) || EGioError(input)),"File ended prematurelly");
				EGioNParse(str,1024,delim,coment,&argc,argv);
				/* skip empty lines */
				if(!argc) continue;
				if(argc != 3)
				{
					rval = 1; 
					MESSAGE(EG_SLK_DEBUG,"wrong number of tokens for NODE_COORD_SECTION, line %zd",i);
					goto CLEANUP;
				}
				pos = atoi(argv[0]);
				if((pos < 1) || ((size_t)pos > data->dim))
				{
					rval = 1; 
					MESSAGE(EG_SLK_DEBUG,"in NODE_COORD_SECTION, line %zd, wrong node number",i);
					goto CLEANUP;
				}
				data->x[pos-1] = strtod(argv[1],0);
				data->y[pos-1] = strtod(argv[2],0);
			}
			/* now we are ready to compute all distances */
			for( i = data->dim ; i-- ; )
			{
				data->cost[i][i]=0;
				for( j = i ; j-- ;)
				{
					data->cost[i][j] = data->cost[j][i] = ceil((hypot(data->x[i]-data->x[j],data->y[i]-data->y[j])));
				}
			}
			gotnodes = 1;
		}
		/* now we read the demand section */
		else if(strncmp(argv[0],"DEMAND_SECTION",(size_t)33) == 0 )
		{
			if (argc != 1)
			{
				rval = 1; 
				MESSAGE(EG_SLK_DEBUG,"wrong number of tokens for DEMAND_SECTION");
				goto CLEANUP;
			}
			for( i = 0; (size_t)i <data->dim ; i++ )
			{
				EGioGets(str,2048,input);
				TEST((EGioEof(input) || EGioError(input)),"File ended prematurelly");
				EGioNParse(str,1024,delim,coment,&argc,argv);
				/* skip empty lines */
				if(!argc) continue;
				if(argc != 2)
				{
					rval = 1; 
					MESSAGE(EG_SLK_DEBUG,"wrong number of tokens for DEMAND_SECTION, line %zd",i);
					goto CLEANUP;
				}
				pos = atoi(argv[0]);
				if((pos < 1) || ((size_t)pos > data->dim))
				{
					rval = 1; 
					MESSAGE(EG_SLK_DEBUG,"in DEMAND_SECTION, line %zd, wrong node number",i);
					goto CLEANUP;
				}
				data->demand[pos-1] = atoi(argv[1]);
			}
			gotdem = 1;
		}
		/* now we read the depot section */
		else if(strncmp(argv[0],"DEPOT_SECTION",(size_t)33) == 0)
		{
			if (argc != 1)
			{
				rval = 1; 
				MESSAGE(EG_SLK_DEBUG,"wrong number of tokens for DEPOT_SECTION");
				goto CLEANUP;
			}
			do	
			{
				EGioGets(str,2048,input);
				TEST((EGioEof(input) || EGioError(input)),"File ended prematurelly");
				EGioNParse(str,1024,delim,coment,&argc,argv);
				/* skip empty lines */
				if(!argc) continue;
				if(argc != 1)
				{
					rval = 1; 
					MESSAGE(EG_SLK_DEBUG,"wrong number of tokens for DEPOT_SECTION, line %zd",i);
					goto CLEANUP;
				}
				pos = atoi(argv[0]);
				if(!gotdepot)
				{
					if((pos < 1) || ((size_t)pos > data->dim))
					{
						rval = 1; 
						MESSAGE(EG_SLK_DEBUG,"in DEPOT_SECTION, line %zd, wrong node number",i);
						goto CLEANUP;
					}
					data->depot = pos-1;
					gotdepot = 1;
				}
			}while(pos != -1);
		}
		else if(strncmp(argv[0],"EOF",(size_t)33) == 0)
		{
			rval = !(gotdim && gotnodes && gotdist && gotcap && gotdepot && gotdem);
			TESTG(rval,CLEANUP,"EOF reached, but data not complete!");
			goto CLEANUP;
		}
		/* default case */
		else
		{
			rval = 1;
			MESSAGE(EG_SLK_DEBUG,"Unknown token %s",argv[0]);
			goto CLEANUP;
		}
		/* end loop */
		todo = !(gotdim && gotnodes && gotdist && gotcap && gotdepot && gotdem);
	}
	CLEANUP:
	if(rval) EGslkDataClear(data);
	return rval;
}

/* ========================================================================= */
void EGflipperResize(EGflipper_t*const flip,const size_t sz)
{
	flip->tour = EGrealloc((flip->tour),sz*sizeof(int));
	flip->inv = EGrealloc((flip->inv),sz*sizeof(int));
	flip->map = EGrealloc((flip->map),sz*sizeof(int));
	flip->max_sz = sz;
	if(flip->sz > sz) flip->sz = 0;
}

/* ========================================================================= */
int EGslkKick(EGflipper_t*const flip)
{
	int rval = 0;
	return rval;
}

/* ========================================================================= */
/** @} */
