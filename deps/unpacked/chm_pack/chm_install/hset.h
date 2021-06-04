#include "lookup3.h"
#include <stdlib.h>

#define INIT_TABSIZE 100

typedef struct _list_t_ {
    uint32_t *arr;
    int arrlen;
    struct _list_t_ *next;
    struct _list_t_ *prev;
} list_t;

typedef struct _hash_table_t_ {
    int size;       /* the size of the table */
    list_t **table; /* the table elements */
    uint32_t prevhash; /*Previous hash for use with lookup3*/
} hash_table_t;

typedef struct _intvecset {
	int numel;
	hash_table_t *hset;
} intvecset;
typedef intvecset intvecset_t[1];

typedef struct _intveclist{
	int numel;
	list_t* head; // head to a linked list
} intveclist;
typedef intveclist intveclist_t[1];

/*hash table functions*/	
hash_table_t *init_hash_table(int size);
uint32_t hashword_lookup3(hash_table_t *hashtable, uint32_t*arr, int arrlen);
list_t *lookup_arr(hash_table_t *hashtable, uint32_t* arr,int arrlen);
int arrcmp(uint32_t*arr1,int arr1len,uint32_t*arr2,int arr2len);
uint32_t* copyarr1(uint32_t* arr,int arrlen);
uint32_t* copyarr2(uint32_t* arr,int arrlen);
int add_arr(hash_table_t *hashtable, uint32_t*arr, int arrlen);
int rem_arr(hash_table_t *hashtable, uint32_t*arr, int arrlen);
void free_table(hash_table_t *hashtable);
uint32_t random_at_most(long max);
uint32_t** randarrlist(int n,int max);

/*Set of uint32_t vectors functions*/
void intvecset_init(intvecset_t vecset, int tablesize);
void intvecset_free(intvecset_t vecset);
void intvecset_delete(intvecset_t vecset, uint32_t* arr,uint32_t arrlen);
void intvecset_add(intvecset_t vecset, uint32_t* arr,uint32_t arrlen);
int intvecset_ismember(intvecset_t vecset, uint32_t* arr, uint32_t arrlen);
void intvecset_copy(intvecset_t copy_vecset,intvecset_t vecset);
uint32_t** intvecset_veclist(intvecset_t vecset);
void intvecset_intersect(intvecset_t iset,intvecset_t vecset1, intvecset_t vecset2);// vecset1\cap vecset2
void intvecset_union(intvecset_t uset,intvecset_t vecset1, intvecset_t vecset2);//vecset1\cup vecset2
void intvecset_setdiff(intvecset_t setdiff,intvecset_t vecset1, intvecset_t vecset2);//vecset1 \ vecset2

/*list of uint32_t vectors (used as a set) functions*/
void intveclist_init(intveclist_t veclist);
void intveclist_free(intveclist_t veclist);
void intveclist_delete(intveclist_t veclist, uint32_t* arr,uint32_t arrlen);
void intveclist_add(intveclist_t veclist, uint32_t* arr,uint32_t arrlen);
list_t* intveclist_ismember(intveclist_t veclist, uint32_t* arr, uint32_t arrlen);
void intveclist_copy(intveclist_t copy_veclist,intveclist_t veclist);
void intveclist_intersect(intveclist_t iveclist,intveclist_t veclist1, intveclist_t veclist2);// veclist1\cap veclist2
void intveclist_union(intveclist_t uveclist,intveclist_t veclist1, intveclist_t veclist2);// veclist1\cup veclist2
void intveclist_setdiff(intveclist_t diffveclist,intveclist_t vecset1, intveclist_t vecset2);//vecset1 \ vecset2
