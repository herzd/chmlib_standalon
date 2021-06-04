#include "hset.h"
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
		intvecset_add(intset,arrlist[i]+1,arrlist[i][0]);
	}
	fprintf(stderr,"\nintvecset has %d elements", intset->numel);
	uint32_t** arrlist1=intvecset_veclist(intset);
	//for(i=0;i<intset->numel;i++) // print int arrays
	//{
		//fprintf(stderr,"\n %d",arrlist1[i][0]);
		//for(j=1;j<=arrlist1[i][0];j++)
		//fprintf(stderr," %d",arrlist1[i][j]);
	//}
	for(i=0;i<200;i++)
	{
		if(intvecset_ismember(intset,arrlist[i]+1,arrlist[i][0])==0)
		fprintf(stderr,"\n Membership test failed! i= %d",i);
	} 
	for(i=199;i>=0;i--)
	{
		intvecset_delete(intset, arrlist[i]+1,arrlist[i][0]);
	}
	fprintf(stderr,"\nintvecset has %d elements", intset->numel);
	intvecset_free(intset);
	//for(i=0;i<200;i++)
	//{
		//free(arrlist[i]);
	//}
	free(arrlist);
	return 0;
}

uint32_t random_at_most(long max) {
  unsigned long
    // max <= RAND_MAX < ULONG_MAX, so this is okay.
    num_bins = (unsigned long) max + 1,
    num_rand = (unsigned long) RAND_MAX + 1,
    bin_size = num_rand / num_bins,
    defect   = num_rand % num_bins;

  long x;
  do {
   x = random();
  }
  // This is carefully written not to overflow
  while (num_rand - defect <= (unsigned long)x);

  // Truncated division is intentional
  return (uint32_t)(x/bin_size);
}
uint32_t** randarrlist(int n,int max)
{
	// list of n arrays with length varying between 5 to 25
	uint32_t** arrlist=(uint32_t**)malloc(n*sizeof(uint32_t*));
	int i,j;
	for (i=0;i<n;i++)
	{
		
		int len=rand()%20+5;
		arrlist[i]=(uint32_t*)malloc((len+1)*sizeof(uint32_t));
		arrlist[i][0]=len;
		for(j=1;j<len+1;j++)
		{
			arrlist[i][j]=rand() % max;
		}
	}
	return arrlist;
}


hash_table_t *init_hash_table(int size)
{
	int i;
    hash_table_t *new_table;
    
    if (size<1) return NULL; /* invalid size for table */

    /* Attempt to allocate memory for the table structure */
    if ((new_table = malloc(sizeof(hash_table_t))) == NULL) {
        return NULL;
    }
    
    /* Attempt to allocate memory for the table itself */
    if ((new_table->table = malloc(sizeof(list_t *) * size)) == NULL) {
        return NULL;
    }

    /* Initialize the elements of the table */
    for(i=0; i<size; i++) new_table->table[i] = NULL;

    /* Set the table's size */
    new_table->size = size;
	new_table->prevhash=0;
    return new_table;
}

uint32_t hashword_lookup3(hash_table_t *hashtable, uint32_t*arr, int arrlen)
{
	return hashword(arr,arrlen,hashtable->prevhash);
}

list_t *lookup_arr(hash_table_t *hashtable, uint32_t* arr,int arrlen)
{
    list_t *list;
    uint32_t hashval = hashword_lookup3(hashtable, arr, arrlen)%hashtable->size;
	//fprintf(stderr,"\nlookuptime hash %ld",(long)hashval);
    /* Go to the correct list based on the hash value and see if arr is
     * in the list.  If it is, return return a pointer to the list element.
     * If it isn't, the item isn't in the table, so return NULL.
     */
    for(list = hashtable->table[hashval]; list != NULL; list = list->next) 
    {
        if (arrcmp(arr,arrlen, list->arr,list->arrlen) == 0) return list;
    }
    return NULL;
}

int arrcmp(uint32_t*arr1,int arr1len,uint32_t*arr2,int arr2len)
{
	int i;
	if(arr1len != arr2len && arr1len!=0)
	return 1;
	else
	{
		//fprintf(stderr,"\ncomp arrays.");
		for(i=0;i<arr1len;i++)
		{
			if(arr1[i]!=arr2[i])
			{
				//fprintf(stderr,"\nBad comp");
				return 1;
			}
		}
		return 0;
	}
}

int add_arr(hash_table_t *hashtable, uint32_t*arr, int arrlen)
{
    list_t *new_list;
    list_t *current_list;
    int i;
    uint32_t hashval = hashword_lookup3(hashtable, arr, arrlen)%hashtable->size;
	fprintf(stderr,"\naddtime hash %ld",(long)hashval);
    /* Does item already exist? */
    current_list = lookup_arr(hashtable, arr,arrlen);
        /* item already exists, don't insert it again. */
    if (current_list != NULL) return 2;
    /* Attempt to allocate memory for list */
    if ((new_list = malloc(sizeof(list_t))) == NULL) return 1;
    /* Insert into list */
    new_list->arr = (uint32_t*)malloc(arrlen*sizeof(uint32_t));
    for (i=0;i<arrlen;i++)
    new_list->arr[i]=arr[i];
    new_list->next = hashtable->table[hashval];
    new_list->arrlen = arrlen;
    new_list->prev = NULL;
    if(hashtable->table[hashval]!=NULL)
    {
		fprintf(stderr,"\nMore than 1 items!");
		hashtable->table[hashval]->prev=new_list;
	}
    hashtable->table[hashval] = new_list;
    return 0;
}

int rem_arr(hash_table_t *hashtable, uint32_t*arr, int arrlen)
{
    list_t *new_list;
    list_t *current_list;
    int i;
    uint32_t hashval = hashword_lookup3(hashtable, arr, arrlen)%hashtable->size;
	fprintf(stderr,"\nremtime hash %ld",(long)hashval);
    /* Does item exist at all? */
    current_list = lookup_arr(hashtable, arr,arrlen);
    /* item doesn't exist, don't try removing it. */
    if (current_list == NULL) return 1;
    /* Remove from list */
    /* 1) fix pointers*/
    fprintf(stderr, "\nrem initiated");
	if(current_list->next!=NULL && current_list->prev !=NULL) // interim element of list
	{
		fprintf(stderr," rem - case 1");
		current_list->next->prev=current_list->prev;
		current_list->prev->next=current_list->next;
	}
	else if(current_list->next== NULL && current_list->prev!=NULL) // last element
	{
		fprintf(stderr," rem - case 2");
		current_list->prev->next=NULL;
	}
	else if(current_list->prev== NULL && current_list->next!=NULL)// first element
	{
		fprintf(stderr," rem - case 3");
		current_list->next->prev=NULL;
		hashtable->table[hashval] = current_list->next;
	}
	else
	{
		// only element in list
		fprintf(stderr," rem - case 4");
		hashtable->table[hashval] = NULL;
	}
	/*2) free memory*/
	if(current_list->arr!=NULL)
	free(current_list->arr);
	free(current_list);
    return 0;
}


void free_table(hash_table_t *hashtable)
{
    int i;
    list_t *list, *temp;

    if (hashtable==NULL) return;

    /* Free the memory for every item in the table, including the 
     * strings themselves.
     */
    for(i=0; i<hashtable->size; i++) {
        list = hashtable->table[i];
        while(list!=NULL) {
            temp = list;
            list = list->next;
            free(temp->arr);
            free(temp);
        }
    }

    /* Free the table itself */
    free(hashtable->table);
    free(hashtable);
}

/*functions for a set of  uint32_t vectors */
void intvecset_init(intvecset_t vecset, int tablesize)
{
	vecset->hset= init_hash_table(tablesize);
	vecset->numel=0;
}

void intvecset_delete(intvecset_t vecset, uint32_t* arr,uint32_t arrlen)
{
	if(rem_arr(vecset->hset, arr, arrlen)==0)
	vecset->numel--;
}

void intvecset_add(intvecset_t vecset, uint32_t* arr,uint32_t arrlen)
{
	/****** debug script : test handling of repeats ******
	int k;
	k = add_arr(vecset->hset,arr,arrlen)==0;
	if(k==0)
	vecset->numel++;
	else if(k==2)
	fprintf(stderr,"\n Repeated element!!!");
	******************************************************/
	if(add_arr(vecset->hset,arr,arrlen)==0)
	vecset->numel++;
}

int intvecset_ismember(intvecset_t vecset, uint32_t* arr, uint32_t arrlen)
{
	if(lookup_arr(vecset->hset, arr,arrlen)!=NULL)
	return 1;
	else
	return 0;
}

void intvecset_free(intvecset_t vecset)
{
	free_table(vecset->hset);
}


uint32_t* copyarr1(uint32_t* arr,int arrlen)// return array without length specified as first element
{
	if(arrlen==0)
	return NULL;
	int i;
	uint32_t* copy_arr=(uint32_t*)malloc(arrlen*sizeof(uint32_t));
	for(i=0;i<arrlen;i++)
	copy_arr[i]=arr[i];
	return copy_arr;
}

uint32_t* copyarr2(uint32_t* arr,int arrlen)// return array of length arrlen+1 with length specified as first element
{
	if(arrlen==0)
	return NULL;
	int i;
	uint32_t* copy_arr=(uint32_t*)malloc((arrlen+1)*sizeof(uint32_t));
	copy_arr[0]=arrlen;
	for(i=1;i<=arrlen;i++)
	copy_arr[i]=arr[i-1];
	return copy_arr;
}



uint32_t** intvecset_veclist(intvecset_t vecset)
{
	int i,j,k,l;
	list_t* cur_list;
	if(vecset->numel==0)
	return NULL;
	j=0;
	list_t** table;
	if(vecset->hset->table != NULL)
	{
		table = vecset->hset->table;
		uint32_t** veclist=(uint32_t**)malloc(vecset->numel*sizeof(uint32_t*));
		for(i=0;i<vecset->hset->size;i++)// loop over list of lists
		{
			if(table[i]!=NULL)//loop over list
			{
				cur_list=table[i];
				do
				{
					if(cur_list->arrlen>0)
					{
						veclist[j]=copyarr2(cur_list->arr,cur_list->arrlen);
						fprintf(stderr,"\n length = %d ",veclist[j][0]);
						for(k=1;k<=veclist[j][0];k++)
						fprintf(stderr,"%d ",veclist[j][k]);
					}
					else
					veclist[j]=NULL;
					j++;
					cur_list=cur_list->next;
				}
				while(cur_list!=NULL);
			}
		}
		return veclist;
	}
	else
	return NULL;
}

void intvecset_intersect(intvecset_t iset,intvecset_t vecset1, intvecset_t vecset2)// vecset1\cap vecset2
{
	int i;
	intvecset_init(iset,INIT_TABSIZE);
	if(vecset1->numel<=0 || vecset2->numel<=0)
	{
		return;
	}
	
	if(vecset1->numel<=vecset2->numel)
	{
		uint32_t** vecset1_list=intvecset_veclist(vecset1);
		for(i=0;i<vecset1->numel;i++)
		{
			if(lookup_arr(vecset2->hset, vecset1_list[i]+1,vecset1_list[i][0])!=NULL)
			intvecset_add(iset,vecset1_list[i]+1,vecset1_list[i][0]);
		}
	}
	else
	{
		uint32_t** vecset2_list=intvecset_veclist(vecset2);
		for(i=0;i<vecset2->numel;i++)
		{
			if(lookup_arr(vecset1->hset, vecset2_list[i]+1,vecset2_list[i][0])!=NULL)
			intvecset_add(iset,vecset2_list[i]+1,vecset2_list[i][0]);
		}
	}
}


void intvecset_union(intvecset_t uset,intvecset_t vecset1, intvecset_t vecset2)//vecset1\cup vecset2
{
	int i;
	intvecset_init(uset,INIT_TABSIZE);
	uint32_t** vecset1_list=intvecset_veclist(vecset1);
	for(i=0;i<vecset1->numel;i++)
	{
		intvecset_add(uset,vecset1_list[i]+1,vecset1_list[i][0]);
	}
	uint32_t** vecset2_list=intvecset_veclist(vecset2);
	for(i=0;i<vecset2->numel;i++)
	{
		intvecset_add(uset,vecset2_list[i]+1,vecset2_list[i][0]);
	}
}


//void intvecset_setdiff(intvecset_t setdiff,intvecset_t vecset1, intvecset_t vecset2)//vecset1 \ vecset2
//{
	//int i;
	//intvecset_init(setdiff,INIT_TABSIZE);

	//if(vecset1->numel<=vecset2->numel)
	//{
		//uint32_t** vecset1_list=intvecset_veclist(vecset1);
		//for(i=0;i<vecset1->numel;i++)
		//{
			//if(lookup_arr(vecset2->hset, vecset1_list[i]+1,vecset1_list[i][0])!=NULL)
			//intvecset_add(iset,vecset1_list[i]+1,vecset1_list[i][0]);
		//}
	//}
	//else
	//{
		//uint32_t** vecset2_list=intvecset_veclist(vecset2);
		//for(i=0;i<vecset2->numel;i++)
		//{
			//if(lookup_arr(vecset1->hset, vecset2_list[i]+1,vecset2_list[i][0])!=NULL)
			//intvecset_add(iset,vecset2_list[i]+1,vecset2_list[i][0]);
		//}
	//}
//}


/**functions for array of array of uint32_t vectors**/
void intveclist_init(intveclist_t veclist)
{
	veclist->numel=0;
	veclist->head=NULL;
}

void intveclist_free(intveclist_t veclist)
{
	int i;
	list_t* cur_vec;
	list_t* cur_vec2;
	if(veclist->numel==0)
	return;
	else
	{
		cur_vec=veclist->head;
		do
		{
			if(cur_vec->arrlen>0)
			free(cur_vec->arr);
			cur_vec2=cur_vec->next;
			free(cur_vec);
			cur_vec=cur_vec2;
		}
		while(cur_vec!=NULL);
	}
}

list_t* intveclist_ismember(intveclist_t veclist, uint32_t* arr, uint32_t arrlen)
{
	if(veclist->numel==0)
	return NULL;
	else
	{
		list_t* list;
		for(list = veclist->head; list != NULL; list = list->next) 
		{
			if (arrcmp(arr,arrlen, list->arr,list->arrlen) == 0) return list;
		}
		return NULL;
	}
}


void intveclist_add(intveclist_t veclist, uint32_t* arr,uint32_t arrlen)
{
	if(intveclist_ismember(veclist,arr,arrlen)==NULL)
	{
		list_t* head;
		head=malloc(sizeof(list_t));
		head->next = veclist->head;
		if(veclist->head!=NULL)
		{
			veclist->head->prev=head;
			veclist->head=head;
		}
		head->prev=NULL;
		head->arr=copyarr1(arr,arrlen);
		head->arrlen=arrlen;
		veclist->numel++;
	}
}

void intveclist_delete(intveclist_t veclist, uint32_t* arr,uint32_t arrlen)
{
	list_t* vec=intveclist_ismember(veclist,arr,arrlen);
	if(vec!=NULL)
	{
		if(vec->prev!=NULL)
		vec->prev->next=vec->next;
		if(vec->next!=NULL)
		vec->next->prev=vec->prev;
		if(vec==veclist->head)
		veclist->head=vec->next;
		free(vec->arr);
		free(vec);
	}
}

void intveclist_copy(intveclist_t copy_veclist,intveclist_t veclist)
{
	/************incomplete*************/
	intveclist_init(copy_veclist);
	if(veclist->numel==0)
	return;
	else
	{
		list_t* new_list,*list,*prevlist;
		for(list = veclist->head; list != NULL; list = list->next) 
		{
			new_list = malloc(sizeof(list_t));
			new_list->arr=copyarr1(list->arr,list->arrlen);
			new_list->arrlen=list->arrlen;
			/***************Resume here*****************/
			if(list==veclist->head)
			{
				copy_veclist->head=new_list;
				copy_veclist->head->prev=NULL;
				copy_veclist->head->next=NULL;
			}
			else
			{
				new_list->next=NULL;
				prevlist->next=new_list;
				new_list->prev=prevlist;
			}
			copy_veclist->numel++;
		}
	}
}
