#include "setops.h"


//void setops_intersection(set_t intersection,set_t set1,set_t set2)
//{
	
//}
//void setops_union(set_t uni,set_t set1,set_t set2);
//void setops_setdiff(set_t diff,set_t set1,set_t set2);
void setops_init(set_t set1)
{
	set1->elements=calloc(1,sizeof(uint64_t));
	set1->nchunks=1;
}

void setops_printset_bits(set_t set1)
{
	int i,j;
	uint64_t n;
	for(i=0;i<set1->nchunks;i++)
	{
		n=set1->elements[i];
		for(j=0;j<64;j++)
		{
			if ((n & ((UINT64_MAX-1)/2)+1)>0LLU)
			{
				printf("1");
			}
			else
			{
				printf("0");
			}
			n <<= 1;
		}
	}
printf("\n");
}

void setops_insert_ul(set_t set1, unsigned long newel)
{
	uint64_t n;
	float k;
	int oldnchunks=set1->nchunks;
	int chunkindex,bitindex,i;
	chunkindex=(int)(newel/64) +2;
	//printf("chunkindex is %d",chunkindex);
	if (chunkindex>set1-> nchunks) // we need to augment!
	{
		set1->elements=realloc(set1->elements,chunkindex*sizeof(uint64_t));
		set1->nchunks=chunkindex;
	}
	for(i=oldnchunks;i<set1->nchunks;i++)
	{
		set1->elements[i]=0LLU;
	}
	set1->elements[chunkindex-1]|=1LLU<<(64-(newel%64));
}	

int setops_ismember_ul(set_t set1, unsigned long el)
{
	uint64_t n;
	float k;
	int chunkindex,bitindex,j,i;
	chunkindex=(int)(el/64) +2;
	if (chunkindex>set1->nchunks) // not a member
	{
		return 0;
	}
	else
	{
		if((set1->elements[chunkindex-1] & (1LLU<<(64-(el%64))))>0LLU) // member!
		{
			return 1;
		}
		else // not a member
		{
			return 0;
		}
	}
}

void setops_delete_ul(set_t set1, unsigned long delel)
{
	uint64_t n;
	float k;
	int chunkindex,bitindex;
	chunkindex=ceil(delel/64) +1;
	if (chunkindex>set1-> nchunks) // delel is not in the set
	{
		return;
	}
	n=(uint64_t)pow(2,64-(delel%64));
	set1->elements[chunkindex-1]=set1->elements[chunkindex-1] &= ~(1LLU << (64-(delel%64)));
}	

void setops_intersection(set_t intersection,set_t set1,set_t set2)
{
	// no need to initialize intersection
	int i,k;
	int oldnchunks=set1->nchunks;
	setops_init(intersection);
	k=set1->nchunks<set2->nchunks ? set1->nchunks: set2->nchunks;
	if(k>intersection->nchunks) //augment intersection if needed 
	{
		intersection->elements=realloc(intersection->elements,k*sizeof(uint64_t));
		intersection->nchunks=k;
	}
	for(i=oldnchunks;i<intersection->nchunks;i++)
	{
		intersection->elements[i]=0LLU;
	}
	for(i=0;i<k;i++)
	{
		intersection->elements[i]=set1->elements[i] & set2->elements[i];
	}
}


unsigned int bitcnt(uint64_t num)
{
	// a simple divide and conquer approach thats will find number of 1s(hamming weight)
	// in an n-bit number in log_2(n) steps
	unsigned int k;
	uint64_t x=num;
	x = (x & 0x5555555555555555) + ((x >> 1) & 0x5555555555555555);
	x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
	x = (x & 0x0F0F0F0F0F0F0F0F) + ((x >> 4) & 0x0F0F0F0F0F0F0F0F);
	x = (x & 0x00FF00FF00FF00FF) + ((x >> 8) & 0x00FF00FF00FF00FF);
	x = (x & 0x0000FFFF0000FFFF) + ((x >> 16) & 0x0000FFFF0000FFFF);
	x = (x & 0x00000000FFFFFFFF) + ((x >> 32) & 0x00000000FFFFFFFF);
	k=(unsigned int)x;// we can do this since the number will never exceed 64
	return x;
}

unsigned int setops_cardinality(set_t set1)
{
	int i;
	unsigned int cnt=0;
	for(i=0;i<set1->nchunks;i++)
	{
		cnt+=bitcnt(set1->elements[i]);
	}
	return cnt;
}

void setops_union(set_t setunion,set_t set1,set_t set2)
{
	// no need to initialize setunion
	uint64_t* elements1,*elements2;
	int i,k,l;
	setops_init(setunion);
	k=set1->nchunks<set2->nchunks ? set2->nchunks: set1->nchunks;//max chunks
	elements1=set1->nchunks<set2->nchunks ? set2->elements: set1->elements;// elements of larger set
	l=set1->nchunks<set2->nchunks ? set1->nchunks: set2->nchunks;//min chunks
	elements2=set1->nchunks<set2->nchunks ? set1->elements: set2->elements;// elements of smaller set
	if(k>setunion->nchunks) //augment intersection if needed 
	{
		setunion->elements=realloc(setunion->elements,k*sizeof(uint64_t));
		setunion->nchunks=k;
	}
	for(i=0;i<k;i++)
	{
		if(i<l)
		{
			setunion->elements[i]=elements1[i] | elements2[i];
		}
		else
		{
			setunion->elements[i]=elements1[i];
		}
	}
}

void setops_setdiff(set_t setdiff,set_t set1,set_t set2)
{
	int i;
	setdiff->elements=calloc(set1->nchunks,sizeof(uint64_t));	
	setdiff->nchunks=set1->nchunks;
	for(i=0;i<set1->nchunks;i++)
	{
		setdiff->elements[i]=set1->elements[i] & (~set2->elements[i]);
	}
}

void setops_printset_pretty(set_t set1)
{
	int i;
	long j;
	printf("\n{");
	for(i=0;i<set1->nchunks;i++)
	{
		for(j=0;j<64;j++)
		{
			if(setops_ismember_ul(set1,i*64+j+1)==1)
			{
				printf("  %ld",i*64+j+1);
			}
		}
	}
	printf("  }\n");
}

void setops_clear(set_t set1)
{
	free(set1->elements);
}

int* setops_set2intarray1(set_t set1,unsigned int cardinality)
{
	int i,k=0;
	long j;
	int* setarray;
	setarray=malloc(cardinality*sizeof(int));
	for(i=0;i<set1->nchunks;i++)
	{
		for(j=0;j<64;j++)
		{
			if(setops_ismember_ul(set1,i*64+j+1)==1)
			{
				setarray[k]=i*64+j+1;
				k++;
			}
		}
	}
	return setarray;
}
