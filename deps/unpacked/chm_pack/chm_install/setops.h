#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <malloc.h>
#include <xmmintrin.h>
#include <string.h>

typedef struct
{
	uint64_t* elements;
	int nchunks;
} set;

typedef set set_t[1];

void setops_insert_ul(set_t, unsigned long newel);
void setops_delete_ul(set_t, unsigned long delel);
void setops_intersection_sse(set_t,set_t,set_t);
void setops_intersection(set_t,set_t,set_t);
void setops_union(set_t,set_t,set_t);
void setops_setdiff(set_t,set_t,set_t);
void setops_init(set_t);
void setops_printset_bits(set_t);
void setops_printset_pretty(set_t);
unsigned int bitcnt(uint64_t);
unsigned int setops_cardinality(set_t);
int setops_ismember_ul(set_t, unsigned long);
void setops_clear(set_t);
int* setops_set2intarray1(set_t ,unsigned int);
void print128_num(__m128i);
void print128_bitwise(__m128i);
void* aligned_realloc(void*,size_t ,size_t ,int);
