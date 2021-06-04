/* test scripts for various functions */
/********************hash table in hset.c*******************************
#include "hset.h"
int main()
{
	int i,j;
	list_t*arrloc;
	hash_table_t *htable=init_hash_table(100); // initialize
	srand(time(NULL)); // random seed 
	uint32_t** arrlist = randarrlist(200,20); // generate 200 arrays of max length 20 (min length 5)
	for(i=0;i<200;i++) // print all
	{
		fprintf(stderr,"\n");
		for(j=1;j<arrlist[i][0];j++)
		fprintf(stderr," %d",arrlist[i][j]);
	}
	for(i=0;i<200;i++) // add all to hash table 
	{
		if(add_arr(htable, arrlist[i]+1, arrlist[i][0])==2)
		fprintf(stderr," repeated item");
	}
	for(i=0;i<200;i++) lookup all 
	{
		arrloc=lookup_arr(htable, arrlist[i]+1,arrlist[i][0]);
		if (arrloc==NULL)
		fprintf(stderr,"\n Lookup failed! i= %d",i);
	}
	for(i=99;i>=0;i--) // remove 100th to 1st element
	{
		if(rem_arr(htable, arrlist[i]+1,arrlist[i][0])==1)
		fprintf(stderr,"rem failed!");
	}
	for(i=199;i>=100;i--) // remove 2000th to 101th element
	{
		if(rem_arr(htable, arrlist[i]+1,arrlist[i][0])==1)
		fprintf(stderr,"rem failed!");
	}
	//free all
	free_table(htable); 
	for(i=0;i<200;i++)
	{
		free(arrlist[i]);
	}
	free(arrlist);
	return 0;
}
**********************************************************************/
/*****************************intvecset: add,veclist,membership,delete test*******************************
int main()
{
	int i,j;
	// initialize
	intvecset_t intset;
	intvecset_init(intset,INIT_TABSIZE);
	srand(time(NULL));
	// generate random elements
	uint32_t** arrlist = randarrlist(200,20);
	for(i=0;i<200;i++) // print int arrays
	{
		fprintf(stderr,"\n length = %d", arrlist[i][0]);
		for(j=1;j<=arrlist[i][0];j++)
		fprintf(stderr," %d",arrlist[i][j]);
	}	
	for(i=0;i<200;i++)  // add all to intvecset
	{   
		intvecset_add(intset,arrlist[i]+1,arrlist[i][0]);									//intvecset_add
	}
	fprintf(stderr,"\nintvecset has %d elements", intset->numel);
	uint32_t** arrlist1=intvecset_veclist(intset);											//intvecset_veclist
	//for(i=0;i<intset->numel;i++) // print int arrays
	//{
		//fprintf(stderr,"\n %d",arrlist1[i][0]);
		//for(j=1;j<=arrlist1[i][0];j++)
		//fprintf(stderr," %d",arrlist1[i][j]);
	//}
	for(i=0;i<200;i++)
	{
		if(intvecset_ismember(intset,arrlist[i]+1,arrlist[i][0])==0)						//intvecset_ismember
		fprintf(stderr,"\n Membership test failed! i= %d",i);
	} 
	for(i=199;i>=0;i--)
	{
		intvecset_delete(intset, arrlist[i]+1,arrlist[i][0]);								//intvecset_delete
	}
	fprintf(stderr,"\nintvecset has %d elements", intset->numel);
	intvecset_free(intset);
	//for(i=0;i<200;i++)
	//{
		//free(arrlist[i]);
	//}
	free(arrlist);
/***********************************************************************/
