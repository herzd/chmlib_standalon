#include "chmutils.h"

int qsload_fromlrsfile (fmpq_mat_t,const char*,mpq_QSprob *); //chmqs
void solvelp_withqsopt(fmpq_t, fmpq_mat_t, fmpq_mat_t,int,long,mpq_QSprob *);//chmqs
int change_qsobjective(mpq_QSprob *probptr, fmpq_mat_t newobj);//chmqs
int qsload_fromlrsmatrix(fmpq_mat_t ,int , int* , mpq_QSprob *); //chmqs
void solvelp_withqsopt_dual(fmpq_t, fmpq_mat_t , fmpq_mat_t ,int ,long ,mpq_QSprob* );//chmqs
