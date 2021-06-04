#include "core.h"
/****************************************************************/
//**This file contains the main function.**/
int main (int argc, char *argv[])
{
	//polyhedron_t inpoly,inpolyrdd,inpolyrddproc;
	//int projdim1;
	//readlrsfile2poly(inpoly,argv[1]);
	//fmpq_mat_t objfun;
	//fmpq_mat_init(objfun,1,511+3);
	//fmpq_one(fmpq_mat_entry(objfun,0,0));
	//fmpq_one(fmpq_mat_entry(objfun,0,1));
	//fmpq_one(fmpq_mat_entry(objfun,0,2));
	//fmpq_mat_print(objfun);
	//fmpq_t objval;
	//fmpq_mat_t optvtx;
	mpq_QSprob prob;
	mpq_QSprob* probptr=malloc(sizeof(mpq_QSprob));
	prob=probptr[0];
	////fprintf(stderr,"\nRedund...");
	////redund_polyhedron(inpolyrdd,inpoly);
	////fprintf(stderr,"\nFininding dimension of input...");
	////ubpoly_preproc( inpolyrddproc,  inpolyrdd);
	////projdim1=inpolyrddproc->projdim;
	//fmpq_mat_print(inpoly->lrsmtx);
	//qsload_fromlrsmatrix(inpoly->lrsmtx,inpoly->linearities->len, inpoly->linearities->array,probptr); 
	//solvelp_withqsopt(objval,optvtx,objfun,2,QS_MAX,&prob);
	//fmpq_mat_print(optvtx);
	fmpq_mat_t X;
	qsload_fromlrsfile (X,"ines/butterfly_shannon.ine",probptr); 
}
