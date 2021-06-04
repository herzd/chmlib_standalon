#pragma once
#include "chmutils.h"
#include "chmstructs.h"
void copy_submat2submat(fmpq_mat_t ,int ,int ,fmpq_mat_t ,int ,int ,int ,int  );//chmflint
void copyrow(fmpq_mat_t , long , fmpq_mat_t ,long );//chmflint
struct intarray* maximal_ind(fmpq_mat_t);//chmflint
int isIDcol(fmpq_mat_t);//chmflint
void copycol(fmpq_mat_t , long , fmpq_mat_t ,long );//chmflint
void nullspace2(fmpq_mat_t ,fmpq_mat_t );//chmflint
void Vset2diffmat(fmpq_mat_t ,fmpq_mat_t ,int );//chmflint
void fliplr(fmpq_mat_t,fmpq_mat_t);//chmflint
void dotrr(fmpq_t c,fmpq_mat_t X,fmpq_mat_t Y); 
int find_depindices(fmpq_mat_t ,int** ,int* );
