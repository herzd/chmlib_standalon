#include <stdio.h>//coreheader
#include <stdlib.h>//coreheader
#include <string.h>//coreheader
#include <errno.h>
#include <time.h>//coreheader
#include "EGlib.h"//coreheader
#include "QSopt_ex.h"//coreheader
#include "chmutils.h"
#include "chmstructs.h"
#include "chmflint.h"
#define STR_LEN_INC 819200 //coremacro

/*********linked list***********/




//struct tightlist {
	//int hindex;
	//struct tightlist* nexth;
	//struct tightlist* prevh;
//};

/***********LRS/****************/
static char **argv2;//coreglobal
static int argc2;//coreglobal
void copy_optvtx2Vset(fmpq_mat_t ,fmpq_mat_t ,int* );//chmchm
void compute_newray(fmpq_mat_t,fmpq_mat_t ,fmpq_mat_t ,fmpq_mat_t );//chmchm
void updatehset(fmpq_mat_t,fmpq_mat_t);//chmchm
void newray_scaledown(fmpq_mat_t);//chmchm

void chm_readpolyhedron(polyhedron_t , char*);// chmio
struct raylist* create_new_raylist(fmpq_mat_t ,fmpq_mat_t );//chmstructs
void initialHrep(fmpq_mat_t ,fmpq_mat_t ) ;// Rset has its rows as rays. Rset must be a square matrix 2
void newray_createadjlist(struct raylist* , struct raylist *,fmpq_mat_t,int);//chmstructs
void newray_add2raylist(fmpq_mat_t,struct raylist *,fmpq_mat_t, int,int);
void remove_nray(struct raylist*);//chmstructs
char* chm_makebounded(char*,int );//chmchm
void fmpq_eye(fmpq_mat_t,int ,int);//chmchm
int qsload_fromlrsfile (fmpq_mat_t,const char*,mpq_QSprob *); //chmqs
void readlrsfile(const char* ,fmpq_mat_t , int*,int**);//chmio
char* readlrsfile2(const char*,fmpq_mat_t , int*,int** );//chmio
void solvelp_withqsopt(fmpq_t, fmpq_mat_t, fmpq_mat_t,int,long,mpq_QSprob *);//chmqs
void readfromfile(fmpq_mat_t,char*);//chmio
int change_qsobjective(mpq_QSprob *probptr, fmpq_mat_t newobj);//chmqs
int qsload_fromlrsmatrix(fmpq_mat_t ,int , int* , mpq_QSprob *); //chmqs
void solvelp_withqsopt_dual(fmpq_t, fmpq_mat_t , fmpq_mat_t ,int ,long ,mpq_QSprob* );//chmqs
int writelrsfile(char* , char* ,fmpq_mat_t ,int ,int* , char* ,char*);//chmio
int ismember(int* ,int ,int);//chmgeneric
struct intarray* redund_naive(fmpq_mat_t , char* , int , int* );//chmchm
int ismember2(int* ,int ,int);//chmgeneric
void findinitialhull7(fmpq_mat_t ,fmpq_mat_t,int);//chmchm
struct intarray* shovepolytope_insubspace(fmpq_mat_t,struct intarray**,fmpq_mat_t,fmpq_mat_t ,int ,int*,int);//chmchm
int variable_isdisposable(fmpq_mat_t ,int ,int *,int);//chmchm
void substituteeq(fmpq_mat_t ,fmpq_mat_t , int );//chmchm
void eliminatevar(fmpq_mat_t ,int , int );//chmchm
struct intarray* chm_preprocess(fmpq_mat_t, fmpq_mat_t,int,int*,int);//chmchm
char* chm_makebounded2(fmpq_mat_t,fmpq_mat_t , char* ,int*,char** );//chmchm
void embedvtx_inhyperplanes(fmpq_mat_t,fmpq_mat_t,fmpq_mat_t);//chmchm
void raylist2ineq(fmpq_mat_t, struct raylist* );//chmchm
void findinitialhull8(fmpq_mat_t,fmpq_mat_t,int);//chmchm
char* chm_ubpreproc(fmpq_mat_t ,fmpq_mat_t , char* ,int* );//chmchm
char* redund_naive_hfilewrt(char* );//chmio
struct isfacet_returnset* isfacet2(fmpq_mat_t ,mpq_QSprob*,int );//chmchm
void clear_adjlist(struct adjlist* );//chmstructs
struct diminfo* finddim_polyhedron2(fmpq_mat_t ,int , int* ,char* );//chmchm
void print_slist(struct raylist* );//chmstructs
long raylist_numrays(struct raylist*);//chmstructs
void lrsmtxcone_scale2integers(fmpq_mat_t ,fmpq_mat_t);//chmchm
void create_new_tightset(set_t ,fmpq_mat_t ,fmpq_mat_t ,int* );//chmstructs
int adjtest_algebraic3(struct raylist* , struct raylist* , fmpq_mat_t , int );//chmchm
struct raylist*  chm_bounded4(char*,fmpq_mat_t ,fmpq_mat_t, int);//chmchm
void remove_nray2(struct raylist* );//chmstructs
void newray_add2raylist2(fmpq_mat_t ,struct raylist *,fmpq_mat_t , int ,int );//chmstructs
struct insineqcon_returnset* insineqcon_withadj3(fmpq_mat_t ,struct raylist* ,int ,fmpq_mat_t,int );//chmchm
void clear_raylist2(struct raylist* );//chmstructs
void addray2adjlist(struct raylist*, struct raylist* );//chmstructs
void populate_adjlist_algebraic(struct raylist* , fmpq_mat_t ,int);//chmstructs
void redund_polyhedron(polyhedron_t , polyhedron_t );// chmchm
void writepoly2lrsfile(char* , polyhedron_t, int );
void ubpoly_preproc(polyhedron_t outpoly, polyhedron_t inpoly);// core
void ubpoly_preproc_boundedcone(polyhedron_t outpoly, polyhedron_t inpoly);//core
void chm_unbounded2bounded(polyhedron_t ,polyhedron_t );
void chm_adddualprojeqs(polyhedron_t projpoly);
void chm_bounded2unbounded(polyhedron_t projpoly, polyhedron_t dualprojpoly);
struct raylist* chm_bounded(polyhedron_t projpoly,polyhedron_t inpoly);
void projeqs2linearities(fmpq_mat_t out,arrayofints_t linearities, fmpq_mat_t projeqmtx,fmpq_mat_t in);


