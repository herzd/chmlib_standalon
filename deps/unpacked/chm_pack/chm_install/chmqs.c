#include "chmqs.h"
#include "chmflint.h"


int qsload_fromlrsfile (fmpq_mat_t X, const char* filename,mpq_QSprob *p) //,mpq_QSprob *p)
{//corefunction
// returns matrix X which is matrix read out of lrsfile
FILE *fp; 
int m,n,i,j,firsttime,skip;
int nlin=0;
mpq_t z;
int* linrows;
fmpq_t q;
fmpq_init(q);
fmpq_mat_t Y;
char* linptr;
const char* begin="begin";
char buf[4096]; 
char* str;
readlrsfile(filename,X, &nlin,&linrows);
m=X->r;
n=X->c;
  // fmpq_mat_print(X);
/**************Adjust for first row bug****************/
fmpq_mat_t newX;
fmpq_mat_init(newX,m+1,n);
fmpq_mat_zero(newX);
for(i=0;i<m;i++)
{
for(j=0;j<n;j++)
{
fmpq_set(fmpq_mat_entry(newX,i+1,j),fmpq_mat_entry(X,i,j));
}
}
m=m+1;
fmpq_mat_clear(X);
fmpq_mat_init(X,m,n);
fmpq_mat_set(X,newX);
//fmpq_mat_print(X);
for(j=0;j<nlin;j++)
{
linrows[j]=linrows[j]+1;	
}
/*********************************Now start filling into the mpq_QSprob datastructure***************************/
   /**************************************/
   /*These four arguments are all arrays that describe the constraint matrix. 
    * QSopt needs to know only the nonzero coefficients. These are grouped by 
    * column in the array cmatval. Every column must be stored in sequential locations 
    * in this array with cmatbeg[j] containing the index of the beginning of column j 
    * and cmatcnt[j] containing the number of entries in column j. Note that the 
    * components of matbeg must be in ascending order. For each k, cmatind[k] indicates 
    * the row number of the corresponding coefficient, cmatval[k]. 
   /**************************************/
   int rval=0;
   // cmat is the A matrix
   int cmatcnt[n-1];// = { 2, 2, 1 };//tells how to partition cmatval vector to get A matrix columns
   int cmatbeg[n-1];// = { 0, 2, 4 };//tells begining index of each column in cmatval vector
   for(i=0;i<n-1;i++)
   {
cmatcnt[i]=m;
cmatbeg[i]=m*i;
   }
   
   int cmatind[m*(n-1)]; //'i'th element here tells row in which 'i'th element of cmatval is contained 

   
   for(i=0;i<(m*(n-1));i++)
   {
cmatind[i]=i % m;
}
fmpz_mat_t nummtx;
fmpz_mat_init(nummtx,X->r,X->c);
fmpz* denoms;
denoms= _fmpz_vec_init ( X->r);
fmpq_mat_get_fmpz_mat_rowwise (nummtx , denoms , X);
fmpq_mat_set_fmpz_mat ( X , nummtx);
free(denoms);
mpq_t cmatval[m*(n-1)];
long tempr;
long tempc;
for(i=0;i<m*(n-1);i++)
{
mpq_init(cmatval[i]);
tempr=i%m;
tempc=((long)floor((i+m)/m));
mpq_set_si(cmatval[i],-(fmpq_mat_entry(X,tempr,tempc)->num),fmpq_mat_entry(X,tempr,tempc)->den);
//printf("\n%d\t%d\t%ld\t",tempr,tempc,-(fmpq_mat_entry(X,tempr,tempc)->num));
//gmp_printf ("%Qi\n",cmatval[i]);	
}
      
     
   char* sense=malloc(m*sizeof(char)); // = { 'L', 'E' };// what are constraints
   int iseqcon;

   for(i=0;i<m;i++)// fill sense vector, loop over rows
   {
iseqcon=0;
for (j=0;j<nlin;j++)
{
if(linrows[j]-1==i)
{
iseqcon=1;
//printf("iseqcon %d for row %d",iseqcon,i);
break;
}
}
if(iseqcon==1)
{
sense[i]= 'E';
}
else
{
sense[i]='L';
}
}
//printf("%s",sense);

char* tmpstr=(char*)malloc(10*sizeof(char));
char* tmpstr1=(char*)malloc(10*sizeof(char));
char *colnames[n-1];// = { "x", "y", "z" };// variable names
/* char *arrayIndices[10]; */
for( i=0; i<n-1; ++i )
{
colnames[i] = (char*)malloc(10*sizeof( char));  /* 1 character + '\0' */
}
for(i=0;i<n-1;i++)
{
sprintf(tmpstr, "%d", i+1);
strcpy(tmpstr1,"x");//tmpstr1="x"
strcat(tmpstr1,tmpstr);//tmpstr1=x<i+1>
strcpy(colnames[i],tmpstr1);
}

   char *rownames[m];// = { "c1", "c2"};// ?
for(i=0;i<m;i++)
{
sprintf(tmpstr, "%d", i);
strcpy(tmpstr1,"c");
strcat(tmpstr1,tmpstr);
rownames[i]=(char*)malloc(10*sizeof(char));
strcpy(rownames[i],tmpstr1);
}
//objective vector//
   mpq_t obj[n-1];// objective vector
   
   for (i = 0; i < n-1; i++) 
   {
   mpq_init (obj[i]); // fill in the objective
   mpq_set_d (obj[i], 1);
   }
   
   //b vector//
   mpq_t rhs[m];// b
   for (i = 0; i < m; i++) 
   {
mpq_init (rhs[i]);
   //fmpq_get_mpq ( rhs[i] , fmpq_mat_entry(X,i,1));
   	mpq_set_d(rhs[i],fmpq_mat_entry(X,i,0)->num);
}
   
   mpq_t lower[n-1];// lower bounds 
   for (i = 0; i < n-1; i++) 
   {
   mpq_init (lower[i]); // fill in the lower bounds
   mpq_set (lower[i], mpq_ILL_MINDOUBLE);
   }
   
   
   mpq_t upper[n-1];// upper bounds
   for (i = 0; i < n-1; i++) 
   {
   mpq_init (upper[i]); // fill in the upper bounds
   mpq_set (upper[i], mpq_ILL_MAXDOUBLE);
   }       
    /*  CPXcopylpwnames  */
   *p = mpq_QSload_prob ("small", n-1,m, cmatcnt, cmatbeg, cmatind, cmatval,
                     1, obj, rhs, sense, lower, upper, (const char**)colnames,
                     (const char**)rownames);
   for(i=0;i<m;i++)
   {
//mpq_QSchange_sense (*p, i, sense[i]);	
}
mpq_QSdelete_row (*p, 0);
//mpq_QSwrite_prob (*p, "logfile4.lp","LP");
   if (*p == (mpq_QSprob) NULL) {
       fprintf (stdout, "Unable to load the LP problem\n");
       rval = 1;  //goto CLEANUP;
   }
   fmpq_mat_init(newX,X->r-1,X->c);
   fmpq_mat_submatrix(newX,X,2,X->r,1,X->c);
   fmpq_mat_init(X,newX->r,newX->c);
   fmpq_mat_set(X,newX);
   CLEANUP:

   for (i = 0; i < m*(n-1); i++) mpq_clear (cmatval[i]);
   for (i = 0; i < n-1; i++) mpq_clear (obj[i]);
   for (i = 0; i < m; i++) mpq_clear (rhs[i]);
   for (i = 0; i < n-1; i++) mpq_clear (lower[i]);
   for (i = 0; i < n-1; i++) mpq_clear (upper[i]);
   free(tmpstr);
   free(tmpstr1);
   return rval;
}




void solvelp_withqsopt(fmpq_t objval, fmpq_mat_t optvtx, fmpq_mat_t flintobj1,int projdim,long newobjsense,mpq_QSprob* q)
{//corefunction
// strictly provide projdim=realprojdim from outside the function. appends an extra 1 at the start of optvtx to account for homogenization
// Also accepts flintobj with a prepended 1( or any number) in keeping with lrs style. This means first column of flintobj1 is ignored
mpq_t objcoef,value,*x;
int rval,i;
   mpq_init (value);
   mpq_init (objcoef);
int status =0;
int ncols=0;
mpq_QSwrite_prob(*q,"solvelp.lp","LP");
   ncols = mpq_QSget_colcount (*q);
fmpq_mat_t flintobj;
fmpq_mat_init(flintobj,1,flintobj1->c-1);
fmpq_mat_submatrix(flintobj,flintobj1,1,1,2,flintobj1->c);
//printf("\nflintobj=%d,ncols=%d,flintobj1=%d",flintobj->c,ncols,flintobj1->c);
int objsense;
rval = mpq_QSget_objsense (*q, &objsense);
change_qsobjective(q, flintobj);
if(objsense!=newobjsense)// change objective sense only if needed
{
rval=mpq_QSchange_objsense (*q, newobjsense);
}
//printf("\nObjsese rval=%d",rval);
fmpq_mat_clear(flintobj);
   /*  CPXlpopt  */
 	 //printf("\nstatus before solving(in solvelp_qithqsopt):%d\n",q[0]->qstatus);
 	q[0]->qstatus=1;
   rval = QSexact_solver (*q, NULL, NULL, NULL, DUAL_SIMPLEX, &status);
   /*  CPXsolninfo  */
   
   rval = mpq_QSget_status (*q, &status);
   //printf("\nstatus: %d\n",status);
   rval = mpq_QSget_objval (*q, &value);
   fmpq_init(objval);
   fmpq_set_mpq(objval,value);
   /*  CPXgetx  */
x=mpq_EGlpNumAllocArray(ncols);
   //x = (mpq_t *) malloc (ncols * sizeof (mpq_t));
   for (i = 0; i < ncols; i++) mpq_init (x[i]);
   rval = mpq_QSget_x_array (*q, x);
   fmpq_mat_init(optvtx,1,projdim+1);
   //printf("mpq vertex:::::::>>>>>><<<");
   for (i = 0; i < projdim; i++)
   {
//gmp_printf ("%Qi\t",x[i]);	
fmpq_set_mpq (fmpq_mat_entry(optvtx,0,i+1), x[i]);
}
fmpq_one(fmpq_mat_entry(optvtx,0,0));// to comply with lrs vertex format and also to homoginize the ray
mpq_EGlpNumFreeArray (x);
}	


int change_qsobjective(mpq_QSprob *probptr, fmpq_mat_t newobj1)
{//corefunction
int i,rval;
fmpq_mat_t newobj;
mpq_t objcoef;
mpq_init(objcoef);
long num,den;
int ncols = mpq_QSget_colcount (*probptr);
//printf("ncols=%d\n",ncols);
mpq_t* obj=0;
obj=mpq_EGlpNumAllocArray(ncols);
if(ncols>newobj1->c)// smaller size obj is given. keep rest of the coeffs 0
{
fmpq_mat_init(newobj,1,ncols);
for(i=0;i<newobj1->c;i++)
{
fmpq_set(fmpq_mat_entry(newobj,0,i),fmpq_mat_entry(newobj1,0,i));
}
for(i=0;i<ncols;i++)
{
num=(fmpq_mat_entry(newobj,0,i))->num;
den=(fmpq_mat_entry(newobj,0,i))->den;
mpq_set_si(objcoef,num,den);
rval=mpq_QSchange_objcoef ( *probptr, i, objcoef);  
//printf("rval=%d",rval);
rval=0;
}
mpq_QSget_obj (*probptr, obj);
//printf("after change\n");
//for(i=0;i<ncols;i++)
//{
//gmp_printf ("%Qi\t",obj[i]);	
//}
mpq_EGlpNumFreeArray (obj);
fmpq_mat_clear(newobj);
mpq_EGlpNumFreeArray (obj);
return 1;
}
else // id est if we are given bigger objective than there are ncols we simply use first ncols columns
{
fmpq_mat_init(newobj,newobj1->r,newobj1->c);
fmpq_mat_set(newobj,newobj1);
mpq_QSget_obj (*probptr, obj);
//printf("before change\n");
//for(i=0;i<ncols;i++)
//{
//gmp_printf ("%Qi\t",obj[i]);	
//}
for(i=0;i<ncols;i++)
{
num=(fmpq_mat_entry(newobj,0,i))->num;
den=(fmpq_mat_entry(newobj,0,i))->den;
mpq_set_si(objcoef,num,den);
rval=mpq_QSchange_objcoef ( *probptr, i, objcoef);  
//printf("rval=%d",rval);
rval=0;
}
mpq_QSget_obj (*probptr, obj);
//printf("after change\n");
//for(i=0;i<ncols;i++)
//{
//gmp_printf ("%Qi\t",obj[i]);	
//}
mpq_EGlpNumFreeArray (obj);
fmpq_mat_clear(newobj);
mpq_EGlpNumFreeArray (obj);
return 0;
} 
}


int qsload_fromlrsmatrix(fmpq_mat_t Xin,int nlin, int* linrows, mpq_QSprob *p) //,mpq_QSprob *p)
{	
//corefunction


   	/**************Adjust for first row bug****************/
   	fmpq_mat_t X;
   	fmpq_mat_init(X,Xin->r,Xin->c);
   	fmpq_mat_set(X,Xin);
	int i,j;
	int m=X->r;
	int n=X->c;
	fmpq_mat_t newX;
	fmpq_mat_init(newX,m+1,n);// put extra row at top in X to create newX
	fmpq_mat_zero(newX);
	for(i=0;i<m;i++)
	{
		for(j=0;j<n;j++)
		{
			fmpq_set(fmpq_mat_entry(newX,i+1,j),fmpq_mat_entry(X,i,j));
		}
	}
	m=m+1;
	fmpq_mat_clear(X);
	fmpq_mat_init(X,m,n);
	fmpq_mat_set(X,newX);
	for(j=0;j<nlin;j++)
	{
		linrows[j]=linrows[j]+1;	
	}
    fmpq_mat_clear(newX);
	/*********************************Now start filling into the mpq_QSprob datastructure***************************/
	/**************************************/
	/*These four arguments are all arrays that describe the constraint matrix. 
	 * QSopt needs to know only the nonzero coefficients. These are grouped by 
     * column in the array cmatval. Every column must be stored in sequential locations 
     * in this array with cmatbeg[j] containing the index of the beginning of column j 
     * and cmatcnt[j] containing the number of entries in column j. Note that the 
     * components of matbeg must be in ascending order. For each k, cmatind[k] indicates 
     * the row number of the corresponding coefficient, cmatval[k]. 
    /**************************************/
	int rval=0;
	// cmat is the A matrix
	int* cmatcnt=malloc((n-1)*sizeof(int));// = { 2, 2, 1 };//tells how to partition cmatval vector to get A matrix columns
	int* cmatbeg=malloc((n-1)*sizeof(int));// = { 0, 2, 4 };//tells begining index of each column in cmatval vector
	for(i=0;i<n-1;i++)
	{
		cmatcnt[i]=m;
		cmatbeg[i]=m*i;
	}
   
    //int cmatind[m*(n-1)]; //'i'th element here tells row in which 'i'th element of cmatval is contained 
	int *cmatind=malloc(m*(n-1)*sizeof(int));
    fmpz_mat_t nummtx;
    fmpz_mat_init(nummtx,X->r,X->c);
	fmpz* denoms;
	denoms= _fmpz_vec_init ( X->r);

	fmpq_mat_get_fmpz_mat_rowwise (nummtx , denoms , X);
	fmpq_mat_set_fmpz_mat ( X , nummtx);
	for(i=0;i<(m*(n-1));i++)
	{
		cmatind[i]=i % m;
	}
	mpq_t *cmatval;//[m*(n-1)];
	cmatval=mpq_EGlpNumAllocArray(m*(n-1));
	//mpq_t cmatval[m*(n-1)];
	long tempr;
	long tempc;
	for(i=0;i<m*(n-1);i++)
	{
		mpq_init(cmatval[i]);
		tempr=i%m;
		tempc=((long)floor((i+m)/m));
		mpq_set_si(cmatval[i],-(fmpq_mat_entry(X,tempr,tempc)->num),fmpq_mat_entry(X,tempr,tempc)->den);
		//printf("\n%d\t%d\t%ld\t",tempr,tempc,-(fmpq_mat_entry(X,tempr,tempc)->num));
		//gmp_printf ("%Qi\n",cmatval[i]);	
	}
	char* sense=malloc((m)*sizeof(char)); // = { 'L', 'E' };// what are constraints
	int iseqcon;
	for(i=0;i<m;i++)// fill sense vector, loop over rows
	{
		iseqcon=0;
		for (j=0;j<nlin;j++)
		{
			if(linrows[j]-1==i)//check if current i is in linrows vector
			{
				iseqcon=1;
				//printf("iseqcon %d for row %d",iseqcon,i);
				break;
			}
		}
		if(iseqcon==1)
		{
			sense[i]= 'E';
		}
		else
		{	
			sense[i]='L';
		}
	}
//sense[m]='\0';
//printf("%s\n",sense);

char* tmpstr=(char*)malloc(10*sizeof(char));
char* tmpstr1=(char*)malloc(10*sizeof(char));
char *colnames[n-1];// = { "x", "y", "z" };// variable names
/* char *arrayIndices[10]; */
for( i=0; i<n-1; ++i )
{
colnames[i] = malloc(10*sizeof( char));  /* 1 character + '\0' */
}
for(i=0;i<n-1;i++)
{
sprintf(tmpstr, "%d", i+1);
strcpy(tmpstr1,"x");
strcat(tmpstr1,tmpstr);
strcpy(colnames[i],tmpstr1);
}

   char *rownames[m];// = { "c1", "c2"};// ?
for(i=0;i<m;i++)
{
sprintf(tmpstr, "%d", i);
strcpy(tmpstr1,"c");
strcat(tmpstr1,tmpstr);
rownames[i]=(char*)malloc(10*sizeof(char));
strcpy(rownames[i],tmpstr1);
}
//objective vector//
   mpq_t obj[n-1];// objective vector
   
   for (i = 0; i < n-1; i++) 
   {
   mpq_init (obj[i]); // fill in the objective
   mpq_set_d (obj[i], 1);
   }
   
   //b vector//
   mpq_t rhs[m];// b
   for (i = 0; i < m; i++) 
   {
mpq_init (rhs[i]);
   //fmpq_get_mpq ( rhs[i] , fmpq_mat_entry(X,i,1));
   	mpq_set_d(rhs[i],fmpq_mat_entry(X,i,0)->num);
}
   
   mpq_t lower[n-1];// lower bounds 
   for (i = 0; i < n-1; i++) 
   {
   mpq_init (lower[i]); // fill in the lower bounds
   mpq_set (lower[i], mpq_ILL_MINDOUBLE);
   }
   
   
   mpq_t upper[n-1];// upper bounds
   for (i = 0; i < n-1; i++) 
   {
   mpq_init (upper[i]); // fill in the upper bounds
   mpq_set (upper[i], mpq_ILL_MAXDOUBLE);
   }       
    /*  CPXcopylpwnames  */
   *p = mpq_QSload_prob ("small", n-1,m, cmatcnt, cmatbeg, cmatind, cmatval,
                     -1, obj, rhs, sense, lower, upper, (const char**)colnames,
                     (const char**)rownames);
   //for(i=0;i<m;i++)
   //{
////mpq_QSchange_sense (*p, i, sense[i]);	
//}
mpq_QSdelete_row (*p, 0);
//mpq_QSwrite_prob (*p, "logfile.lp","LP");
   if (*p == (mpq_QSprob) NULL) {
       fprintf (stdout, "Unable to load the LP problem\n");
       rval = 1;  //goto CLEANUP;
   }
   mpq_EGlpNumFreeArray (cmatval);
   fmpq_mat_init(newX,X->r-1,X->c);
   fmpq_mat_submatrix(newX,X,2,X->r,1,X->c);
   fmpq_mat_clear(X);
   fmpq_mat_init(X,newX->r,newX->c);
   fmpq_mat_set(X,newX);
   fmpq_mat_clear(newX);
   fmpz_mat_clear(nummtx);
   _fmpz_vec_clear ( denoms , X->r);
   //for (i = 0; i < m*(n-1); i++) mpq_clear (cmatval[i]);
   for (i = 0; i < n-1; i++) mpq_clear (obj[i]);
   for (i = 0; i < m; i++) mpq_clear (rhs[i]);
   for (i = 0; i < n-1; i++) mpq_clear (lower[i]);
   for (i = 0; i < n-1; i++) mpq_clear (upper[i]);
  // for (i = 0; i < n-1; i++) free(colnames[i]);
   for (i = 0; i < m; i++) free(rownames[i]);
   free(tmpstr);
   free(tmpstr1);
   for(j=0;j<nlin;j++)
{
linrows[j]=linrows[j]-1;	
}
   free(cmatind);
   free(sense);
   return rval;
}


void solvelp_withqsopt_dual(fmpq_t objval, fmpq_mat_t optvtx, fmpq_mat_t flintobj1,int projdim,long newobjsense,mpq_QSprob* q)
{//corefunction
// strictly provide projdim=realprojdim from outside the function. appends an extra 1 at the start of optvtx to account for homogenization
// Also accepts flintobj with a prepended 1( or any number) in keeping with lrs style. This means first column of flintobj1 is ignored
mpq_t objcoef,value,*x;
int rval,i;
   mpq_init (value);
   mpq_init (objcoef);
int status =0;
int ncols=0;
int nrows=0;
   ncols = mpq_QSget_colcount (*q);
   nrows = mpq_QSget_rowcount (*q);
fmpq_mat_t flintobj;
fmpq_mat_init(flintobj,1,flintobj1->c-1);
fmpq_mat_submatrix(flintobj,flintobj1,1,1,2,flintobj1->c);
//printf("\nflintobj=%d,ncols=%d,flintobj1=%d",flintobj->c,ncols,flintobj1->c);
int objsense;
rval = mpq_QSget_objsense (*q, &objsense);
change_qsobjective(q, flintobj);
if(objsense!=newobjsense)// change objective sense only if needed
{
rval=mpq_QSchange_objsense (*q, newobjsense);
}
//printf("\nObjsese rval=%d",rval);
fmpq_mat_clear(flintobj);
   /*  CPXlpopt  */
 	 //printf("\nstatus before solving(in solvelp_qithqsopt):%d\n",q[0]->qstatus);
 	q[0]->qstatus=1;
   rval = QSexact_solver (*q, NULL, NULL, NULL, DUAL_SIMPLEX, &status);
   /*  CPXsolninfo  */
   
   rval = mpq_QSget_status (*q, &status);
  // printf("\nstatus: %d\n",status);
   rval = mpq_QSget_objval (*q, &value);
   fmpq_init(objval);
   fmpq_set_mpq(objval,value);
   /*  CPXgetx  */
x=mpq_EGlpNumAllocArray(nrows);
   //x = (mpq_t *) malloc (ncols * sizeof (mpq_t));
   for (i = 0; i < nrows; i++) mpq_init (x[i]);
   rval = mpq_QSget_pi_array (*q, x);
   fmpq_mat_init(optvtx,1,nrows+1);
   for (i = 1; i < nrows+1; i++)
   {
//gmp_printf ("%Qi\n",x[i]);	
fmpq_set_mpq (fmpq_mat_entry(optvtx,0,i), x[i-1]);
}
fmpq_one(fmpq_mat_entry(optvtx,0,0));
//fmpq_one(fmpq_mat_entry(optvtx,0,0));// to comply with lrs vertex format and also to homoginize the ray
mpq_EGlpNumFreeArray (x);
}
