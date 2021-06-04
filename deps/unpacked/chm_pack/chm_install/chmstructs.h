#pragma once
#include "chmutils.h"
#include "chmflint.h"
struct projpolytope
{
fmpq_mat_t Vset;
struct raylist* head;
};
struct isfacet_returnset{//corestruct
int isfacet;
fmpq_mat_t ineqcon;
};
struct insineqcon_returnset{//corestruct
int lastindex;
struct raylist* head;
};

typedef struct {//corestruct
int len;
int* array;

} arrayofints;
typedef arrayofints arrayofints_t[1];



struct intarray{
	int* array;
	int len;
};

struct diminfo{//corestruct
	struct intarray* linearities;
	int dim;
};

struct raylist {//corestruct
	fmpq_t score;
	struct raylist* nexts;// points to next +/0/- ray based  on sign of score
	struct raylist* prevs;// points to previous +/0/- ray based  on sign of score
	int flag1; // can be used to indicate whether a facet is terminal
	int flag2; // can be used as integer indicator of whether a ray is +/-/0
	int rayindex;
	fmpq_mat_t ray;           // stores the ray itself
	struct raylist* nextray;  // points to next ray in circular linked list
	struct raylist* prevray; // points to previous ray in circular linked list
	struct adjlist* head_adjlist;  // head of adjacency list
	struct tightlist* ray_tightlist; // handle to the linked list of tight inequalities
	int rnum; // stores number of rays adjacent to the given ray
	int hnum; // stores number of inequalities that are tight at that ray
	set_t tightset;
	int* tightarray;
	int tlen;
};

struct adjlist {//corefunction
	struct raylist* parent;
	struct adjlist* adjadj;
	int rayindex;
	struct raylist* adjray;
	struct adjlist* nextadj;
	struct adjlist* prevadj;
};

struct chm_fileinfo{
	char* preprend;
	char*foreword;
};
typedef struct
{
	fmpq_mat_t lrsmtx;
	fmpq_mat_t VR;
	int linpresent;
	arrayofints_t linearities;
	int redpresent;
	arrayofints_t redundancy;
	int isbounded;
	int dim;
	char* prepend;
	char* append;
	fmpq_mat_t objective;
	fmpq_t optimum;
	fmpq_mat_t optvtx;
	arrayofints_t projelimvars;
	int projdim;
	int projeqpresent;
	fmpq_mat_t projeqmtx;
	fmpq_mat_t projVset;
	struct raylist* head;
} polyhedron;


typedef polyhedron polyhedron_t[1];

void arrayofints_init(arrayofints_t,int);//chmstructs
void arrayofints_clear(arrayofints_t);//chmstructs
struct raylist* create_new_raylist(fmpq_mat_t ,fmpq_mat_t );//chmstructs
void newray_createadjlist(struct raylist* , struct raylist *,fmpq_mat_t,int);//chmstructs
void remove_nray(struct raylist*);//chmstructs
void print_slist(struct raylist* );//chmstructs
long raylist_numrays(struct raylist*);//chmstructs
void remove_nray2(struct raylist* );//chmstructs
void newray_add2raylist2(fmpq_mat_t ,struct raylist *,fmpq_mat_t , int ,int );//chmstructs
void remove_nray(struct raylist*);//chmstructs
void clear_adjlist(struct adjlist* );//chmstructs
void print_slist(struct raylist* );//chmstructs
long raylist_numrays(struct raylist*);//chmstructs
void create_new_tightset(set_t ,fmpq_mat_t ,fmpq_mat_t ,int* );//chmstructs
void newray_add2raylist2(fmpq_mat_t ,struct raylist *,fmpq_mat_t , int ,int );//chmstructs
void clear_raylist2(struct raylist* );//chmstructs
void addray2adjlist(struct raylist*, struct raylist* );//chmstructs
void populate_adjlist_algebraic(struct raylist* , fmpq_mat_t ,int);//chmstructs
