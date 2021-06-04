#include "chmflint.h"
#include "chmstructs.h"


void copy_submat2submat(fmpq_mat_t Y,int yi,int yj,fmpq_mat_t X,int r1,int r2,int c1,int c2 )
/*copies submatrix X(r1:r2,c1:c2) into Y with its left top corner at y(yi,yj) 
 * NOTE:All indices in matlab sense 
 * Doesn't go ahead with copy if there is a dimentions mismatch
 * MAke sure Y matrix is pre-initialized*/
{//corefunction
	int i,j;
	if(Y->c-yj+1<c2-c1+1 || Y->r-yi+1<r2-r1+1 || yi>Y->r || yj> Y->c || r1>r2 || c1>c2 || r2-r1+1>X->r || c2-c1+1>X->c)
		{
		//	printf("\n%d %d %d %d %d %d %d %d %d\n",Y->c-yj+1<c2-c1+1,Y->r-yi+1<r2-r1+1  ,yi>Y->r,yj> Y->c,r1>r2, c1>c2,r2-r1+1>X->r,c2-c1+1>X->c );
		//	printf("Violation!!\n");
		return;
	}
	else
	{
		for(i=0;i<r2-r1+1;i++)
		{
			for(j=0;j<c2-c1+1;j++)
			{
				fmpq_set(fmpq_mat_entry(Y,yi-1+i,yj-1+j),fmpq_mat_entry(X,r1-1+i,c1-1+j));
			}
		}
	}
}

void copyrow(fmpq_mat_t destmat, long destrow, fmpq_mat_t srcmat,long srcrow)
{
	//corefunction 2
	// no protection against index overflow
	// indices of rows in C sense(to aid in for loops)
	int i,k;
	k=srcmat->c>destmat->c?destmat->c:srcmat->c;
	for(i=0;i<k;i++) 
	{
		fmpq_set(fmpq_mat_entry(destmat,destrow,i),fmpq_mat_entry(srcmat,srcrow,i));
	}
}






void copycol(fmpq_mat_t destmat, long destcol, fmpq_mat_t srcmat,long srccol)
{//corefunction
	// no protection against index overflow
	// indices of rows in C sense(to aid in for loops)
	int i;
	for(i=0;i<destmat->r;i++) 
	{
		fmpq_set(fmpq_mat_entry(destmat,i,destcol),fmpq_mat_entry(srcmat,i,srccol));
	}
}


void nullspace2(fmpq_mat_t nullmat2,fmpq_mat_t X1)
{//corefunction
	/* returns nullspace of a fat matrix
	 * Accepts rows of matrix that are extreme points
	 * in lrs format(i.e. with a prepended 1)
	 * rows of nullmat2 are basis of nullspace*/
	fmpq_mat_t Y,testcol,U,X,nullmat1,nullmat;
	fmpq_mat_init(X,X1->r,X1->c-1);
	copy_submat2submat(X,1,1,X1,1,X1->r,2,X1->c);
	int i,k,j,l;
	int idcol;
	fmpq_mat_init(Y,X->r,X->c);
	int rank=fmpq_mat_rref_fraction_free(Y,X);
	//printf("\nrref matrix\n");
	//fmpq_mat_print(Y);
	fmpq_mat_init(testcol,X->r,1);
	int* idcols=malloc(rank*sizeof(int));
	k=1;// this is the index of identity column we are searching for
	for(i=0;i<Y->c && k<=rank;i++)// find indices(C sense) of <rank> identity matrix columns 
	{
		copycol(testcol,0,Y,i);
		idcol=isIDcol(testcol);
		if(idcol>0)// identity col
		{
			if(idcol==k) // we found the idcol we were looking for
			{
				idcols[k-1]=i;// put current column index in idcols array
				k++;
			}
		}
		//fmpq_mat_print(testcol);
	}
	//printf("\nidcols are:\n");
	//for(i=0;i<rank;i++)
	//{
		//printf("%d\t",idcols[i]);
	//}
	fmpq_mat_init(U,rank,Y->c-rank);
	k=0;
	for(i=0;i<Y->c;i++)
	{
		if(ismember2(idcols,rank,i)<0) //i is not identity column index
		{
			for(j=0;j<rank;j++)
			{
				fmpq_set(fmpq_mat_entry(U,j,k),fmpq_mat_entry(Y,j,i));
			}
			k++;
		}
	}
	//printf("\nU matrix\n");
	//fmpq_mat_print(U);
	fmpq_mat_init(nullmat,U->r+U->c,U->c);
	l=0;
	for(i=0;i<nullmat->r;i++)
	{
		if(ismember2(idcols,rank,i)>=0)// i is index of identity colum in rref version of input matrix
		{
			copyrow(nullmat,i,U,l);
			l++;
		}
	}
	fmpq_mat_neg(nullmat,nullmat);
	l=0;
	for(i=0;i<nullmat->r;i++)
	{
		if(ismember2(idcols,rank,i)<0)
		{
			fmpq_one(fmpq_mat_entry(nullmat,i,l));
			l++;
		}
	}
	//fmpq_mat_print(nullmat);
	fmpq_mat_init(nullmat1,nullmat->c,nullmat->r);
	fmpq_mat_transpose(nullmat1,nullmat);
	fmpq_mat_init(nullmat2,nullmat1->r,nullmat1->c+1);
	for(i=0;i<nullmat2->r;i++)
	{
		fmpq_one(fmpq_mat_entry(nullmat2,i,0));
	}
	copy_submat2submat(nullmat2,1,2,nullmat1,1,nullmat1->r,1,nullmat1->c);
	fmpq_mat_clear(Y);
	fmpq_mat_clear(testcol);
	fmpq_mat_clear(U);
	fmpq_mat_clear(X);
	fmpq_mat_clear(nullmat1);
	fmpq_mat_clear(nullmat);
}

int isIDcol(fmpq_mat_t col)
{
	/*returns a positive number giving which 
	 * identity matrix column we have found(in matlab sense)
	 *  if column passed is identity column
	 * 0 if its not and -1 if its all zero*/
	int i;
	int foundone=0;
	int foundzero=0;
	fmpq_t zero;
	fmpq_t one;
	fmpq_init(zero);
	fmpq_one(one);
	int oneloc=0;
	for(i=0;i<(col->r);i++)
		{
			if(fmpq_equal(fmpq_mat_entry(col,i,0),zero)>0)
			{
				foundzero++;
			}
			else if(fmpq_equal(fmpq_mat_entry(col,i,0),one)>0)
			{
				foundone++;
				oneloc=i+1;
			}
			else  
			{
				return 0;
			}
		}
		if(foundone==1 && foundzero==col->r-1)
		{
			return oneloc;// ID
		}
		else if(foundzero==col->r)
		{
			return -1;// Allzero
		}
		else
		{
			return 0;// other
		}
}

int find_depindices(fmpq_mat_t lrsmtx,int** ind,int* num_lin)
{//corefunction
	/*given a subset of row indices ind[] finds which other rows of mat depend on them */
	int** ind2ptr=malloc(sizeof(int*));
	int* ind2=malloc(lrsmtx->r*sizeof(int));
	ind2ptr[0]=ind2;
	int rank=0,i,j,k,rank1=0;
	fmpq_mat_t testmat_dep,rreftestmat_dep;
	fmpq_mat_init(testmat_dep,num_lin[0]+1,lrsmtx->c);
	
	// copy ind into ind2
	for(i=0;i<num_lin[0];i++)
	{
		ind2[i]=ind[0][i];
	}
	//printf("lrsmtx size is %ldX%ld",lrsmtx->r,lrsmtx->c);
	for(i=0;i<num_lin[0];i++)//copy known equalities into testmat_dep
	{
		copyrow(testmat_dep,i,lrsmtx,ind2[i]-1);
	}
	fmpq_mat_init(rreftestmat_dep,testmat_dep->r,testmat_dep->c);
	rank1=fmpq_mat_rref(rreftestmat_dep,testmat_dep);// rank of original set of equalities
	for(i=0;i<lrsmtx->r;i++)
	// loop over rows of lrsmtx checking if that row is 
	// linearly dependent on rows with equality in ind
	// if it is, add that dependent row index to ind2
		{
			k=ismember2(ind2,num_lin[0],i+1);
			//while(k< num_lin[0] && i+1!=ind2[k])
			//k++;
			if(k==-1)// i+1 is not present in numlin, we must test row i
			{
				copyrow(testmat_dep,testmat_dep->r-1,lrsmtx,i);
				//printf("\nTesting rank for row %d, testmat_dep is:\n",i+1);
				//fmpq_mat_print(testmat_dep);
				rank=fmpq_mat_rref(rreftestmat_dep,testmat_dep);
				//printf("\nrank is %d\n",rank);
				if(rank==rank1)// depdendent equality found
				{
					ind2[num_lin[0]]=i+1;
					num_lin[0]++;
				}
			}
		}
		//printf("numlin=%d",num_lin[0]);
		//printf("\nExtended equality indices:\t");
			//for(i=0;i<num_lin[0];i++)//copy known equalities into testmat_dep
			//{
				//printf("%d\t",ind2[i]);
			//}
	free(ind[0]);
	free(ind2ptr);
	fmpq_mat_clear(testmat_dep);
	fmpq_mat_clear(rreftestmat_dep);
	ind[0]=ind2;
	return rank1;
}

void dotrr(fmpq_t c,fmpq_mat_t X,fmpq_mat_t Y) 
{//corefunction
int i;
fmpq_mat_t X1,X2;
fmpq_mat_init(X2,Y->c,1);
fmpq_init(c);

	for(i=0;i<Y->c;i++)
	{
		fmpq_set(fmpq_mat_entry(X2,i,0),fmpq_mat_entry(Y,0,i));
	}
	fmpq_mat_init(X1,1,1);
	fmpq_mat_mul( X1, X,X2);
	c->num=fmpq_mat_entry(X1,0,0)->num;
	c->den=fmpq_mat_entry(X1,0,0)->den;
	//fmpq_set(c,fmpq_mat_entry(X1,0,0));
	fmpq_mat_clear(X1);
	fmpq_mat_clear(X2);
}


void fliplr(fmpq_mat_t fmat,fmpq_mat_t mat)
{//corefunction
	// aliasing of fmat and mat is not allowed
	// This function mimics MATLAB's fliplr
	int i,j;
	fmpq_mat_init(fmat,mat->r,mat->c);
	for(i=0;i<mat->r;i++)
	{
		for(j=0;j<mat->c;j++)
		{
			fmpq_set(fmpq_mat_entry(fmat,i,mat->c-j-1),fmpq_mat_entry(mat,i,j));
		}
	}
	
}
