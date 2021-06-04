#include "core.h"
/*/* computes dot product of 2 row vectors
*/



/*fmpq_mat_submatrix gives submatrix fmpq_mat_t SubMat of a given fmpq_mat_t Mat 
SubMat=Mat(row1:row2,col1:col2) in matlab notation*/

void fmpq_mat_submatrix(fmpq_mat_t SubMat,fmpq_mat_t Mat,int row1,int row2,int col1,int col2)  
{//corefunction
int i,j;
fmpq_mat_init(SubMat,row2-row1+1,col2-col1+1);
for(i=row1-1;i<row2;i++)
	{
	for(j=col1-1;j<col2;j++)
		{
		fmpq_set(fmpq_mat_entry(SubMat,i-(row1-1),j-(col1-1)),fmpq_mat_entry(Mat,i,j));
		}
	}
}
/*End of fmpq_mat_submatrix*/

void mul_mat_qscalar(fmpq_mat_t newMat,fmpq_t scalar,fmpq_mat_t oldMat) // scalar multiply oldMat by a rational number
{
int i,j;
for(i=0;i<oldMat->r;i++)
	{
	for(j=0;j<oldMat->c;j++)
		{
			fmpq_mul(fmpq_mat_entry(newMat,i,j),scalar,fmpq_mat_entry(oldMat,i,j));
		}
	}	
}




struct raylist* create_new_raylist(fmpq_mat_t Vset,fmpq_mat_t Hset)
{//corefunction
	int i;
	int* hnum;
	hnum= malloc(sizeof(int)); 
	hnum[0]=0;
	struct raylist* head=NULL;
	struct raylist* newray;
	fmpq_mat_t temp_ray;
	// first create a head node
	head=malloc(sizeof(struct raylist));
	head->flag1=0;
	head->rayindex=0;
	fmpq_init(head->score);
	fmpq_mat_submatrix(head->ray,Vset,1,1,1,Vset->c);
	//printf("\nconsidering ray: 0\n",i);
	//head->ray_tightlist=create_new_tightlist(head->ray,Hset,hnum);
	create_new_tightset(head->tightset,head->ray,Hset,hnum);
	head->hnum=hnum[0];
	head->nextray=head;
	head->prevray=head;
	head->rnum=0;
	// Now we fill in rest of the rays
	for (i=1;i<Vset->r;i++)
	{
		//printf("\nconsidering ray: %d\n",i);
		newray=malloc(sizeof(struct raylist));
		newray->flag1=0;
		newray->rayindex=i;
		fmpq_init(newray->score);
		fmpq_mat_init(newray->ray,1,Vset->c);
		fmpq_mat_submatrix(newray->ray,Vset,i+1,i+1,1,Vset->c); //ray
		hnum[0]=0; 
		//newray->ray_tightlist=create_new_tightlist(newray->ray,Hset, hnum);//ray_tightlist, hnum
		create_new_tightset(newray->tightset,newray->ray,Hset,hnum);
		newray->hnum=hnum[0];
		newray->rnum=0;
		/* set newray pointers */
		newray->nextray=head;
		newray->prevray=head->prevray;
		/*set pointers in prospective neighbours of newray to point to newray*/          //head->prevray <====>  head
		head->prevray->nextray=newray;													//head->prevray(newray->prevray) <===> newray <====> head
		head->prevray=newray;
	}
	free(hnum);
	return head;
}

int adjtest_algebraic(struct raylist* currentray, struct raylist* adjcandidate, fmpq_mat_t Hset, int dim)
{
	int i,j,common_count,temp_ineqindex,*intersection,hnum1,hnum2,rank;
	fmpq_mat_t temp_intersection;
	fmpq_mat_t mat_intersection;
	fmpq_mat_t mat_intersection_echelon;
	hnum1=currentray->hnum;
	hnum2=adjcandidate->hnum;
	//printf("\nadjacency test between %d and %d \n",currentray->rayindex,adjcandidate->rayindex);
	//printf("\n The hnums are: %d & %d",hnum1,hnum2);
	if(hnum1<=hnum2) 
	{  
	intersection=malloc(hnum1*sizeof(int));
	}
	else
	{
		 intersection=malloc(hnum2*sizeof(int));
	}
	
	common_count=intersect_tightsets(intersection,currentray->tightarray,hnum1,adjcandidate->tightarray,hnum2);
	//printf("CommonCount is: %d",common_count);
	if(common_count<dim-2)
	{	
		free(intersection);
		return 0; // there are less than dim-2 common tight inequalities. Get out of here!
	}
	else 
	{
		fmpq_mat_init(mat_intersection,common_count,Hset->c);
		fmpq_mat_init(mat_intersection_echelon,common_count,Hset->c);
		for(i=0;i<common_count;i++)              // fill inside mat_intersection
		{		
			//printf("intersection row number: %d",intersection[i]);
			fmpq_mat_submatrix(temp_intersection,Hset,intersection[i],intersection[i],1,Hset->c);
			for (j=0;j<Hset->c;j++)
			{
				fmpq_set(fmpq_mat_entry(mat_intersection,i,j),fmpq_mat_entry(temp_intersection,0,j));
			}
			fmpq_mat_clear(temp_intersection);
		}
		rank=fmpq_mat_rref(mat_intersection_echelon,mat_intersection);
		if (rank!=dim-2)
		{
			free(intersection);
			fmpq_mat_clear(mat_intersection);
			//fmpq_mat_clear(temp_intersection);
			fmpq_mat_clear(mat_intersection_echelon);
			return 0;
		}
		else
		{
			free(intersection);
			fmpq_mat_clear(mat_intersection_echelon);
			fmpq_mat_clear(mat_intersection);
			//fmpq_mat_clear(temp_intersection);
			return 1;  // rays are adjacent!
		}
	}
}
//make sure intersection array is initialized
 //int intersect_tightsets(int*intersection,int* tightset1,int hnum1,int* tightset2,int hnum2) //takes tightset arrays and puts common elements into intersection array.
//{
	//int i,j,k;
	//k=0;
	//for(i=0;i<hnum1;i++)
	//{
		//for(j=0;j<hnum2;j++)
		//{
			//if(tightset1[i]==tightset2[j]){intersection[k]=tightset1[i];k++;}
		//}
	//}
//return k;
//}

//make sure intersection array is initialized
int intersect_tightsets(int*intersection,int* tightset1,int hnum1,int* tightset2,int hnum2) //takes tightset arrays and puts common elements into intersection array.
{
	int i,j,k,len,i1,i2;
	k=0;
	i1=0;
	i2=0;
	while(1)
	{
			if(tightset1[i1]==tightset2[i2])
			{
				intersection[k]=tightset1[i1];
				k++;
				i1++;
				i2++;
			}
			else 
			{
				if(tightset1[i1]<tightset2[i2])
				{
					i1++;				
				}
				else
				{
					i2++;
				}
			}
			if(i1==hnum1 || i2==hnum2)
			{
				break;
			}
	}
	//printf("\n Intersecting:\n array1:\t");
//printarray(tightset1,hnum1);
//printf("array2:\t");
//printarray(tightset2,hnum2);
//printf("\n Intersection:");
//printarray(intersection,k);
return k;

}

void initialHrep(fmpq_mat_t Hset,fmpq_mat_t Rset) // Rset has its rows as rays. Rset must be a square matrix
{//corefunction
	fmpq_mat_init(Hset,Rset->r,Rset->c);
	//fmpq_mat_t transpose_Rset;
	//fmpq_mat_init(transpose_Rset,Rset->c,Rset->c);
	//fmpq_mat_transpose ( transpose_Rset , Rset);
	fmpq_mat_inv ( Hset, Rset);
	//fmpq_mat_clear(Rset);
}















char* chm_makebounded(char* filename,int projdim)
//writes a file with name filenamebounded.ine correnponding to the filename provided(provided filename already has .ine at the end)
{
	FILE *fp; 
	int m,n,i,j;
	mpq_t z;
	fmpq_t q;
	fmpq_init(q);
	fmpq_mat_t X,Y;
	const char* begin="begin";
	char buf[4096]; 
	fp=fopen(filename,"r");
    while(fp!=NULL && fgets(buf, sizeof(buf), fp)!=NULL)
    {
        if (strstr(buf, "begin"))
        {
        //printf("begin found!");
        gmp_fscanf(fp,"%d%d",&m,&n);
        //printf("size %d %d",m,n);
        fmpq_mat_init ( X , m, n);
		fmpq_mat_zero ( X);
		do// skip empty lines
		{
		fgets(buf, sizeof(buf), fp);
		}
		while(strlen(buf)==0);
		mpq_init(z);
		for(i=0;i<m;i++)
		{
			for(j=0;j<n;j++)
			{
				gmp_fscanf(fp,"%Qi",z);		
				fmpq_set_mpq(fmpq_mat_entry(X,i,j),z);
			} 
			do// skip empty lines
			{
				fgets(buf, sizeof(buf), fp);
			}
			while(strlen(buf)==0);
		}
		mpq_clear(z);
		break;
		}
    }
    if(fp!=NULL) 
    fclose(fp);
   // fmpq_mat_print(X);
    char *filename2;
    filename2=malloc((strlen(filename+8))*sizeof(char));
    strncpy(filename2,filename,strlen(filename)-4);
    fmpq_mat_init(Y,X->c+X->r,X->r+projdim+1);// Y matrix corresponds to bounded version of X
	fmpq_mat_zero(Y);// initialize Y to all zero
	//X is m by n+1  matrix
	fmpq_one(fmpq_mat_entry(Y,X->c-1,0));// 1 for \lambda_1+\lambda_2...\lambda_m
	fmpq_mat_t Ip;
	fmpq_eye(Ip,projdim,-1);
	copy_submat2submat(Y,1,2,Ip,1,Ip->r,1,Ip->c);
	fmpq_mat_t A,At;
	fmpq_mat_submatrix(A,X,1,X->r,2,X->c);
	//fmpq_mat_print(A);
	fmpq_mat_init(At,A->c,A->r);
	fmpq_mat_transpose(At,A);
	copy_submat2submat(Y,1,projdim+2,At,1,At->r,1,At->c);
	for(i=projdim+2-1;i<Y->c;i++)
	{
		fmpq_one(fmpq_mat_entry(Y,X->c-1,i));// 1-\sum_i{\lambda_i}=0
		fmpq_neg(fmpq_mat_entry(Y,X->c-1,i),fmpq_mat_entry(Y,X->c-1,i));
	}
	fmpq_mat_t Ilambda;
	fmpq_eye(Ilambda,X->r,1);
	copy_submat2submat(Y,X->c+1,projdim+2,Ilambda,1,Ilambda->r,1,Ilambda->c);
//	fmpq_mat_print(Y);
	strcat(filename2,"bounded.ine");
    fp=fopen(filename2,"w");
	fputs ("H-representation\n",fp );
	fputs("linearity  ",fp);
	sprintf(buf,"%ld   ",X->c);
	fputs(buf,fp);
	for(i=1;i<=X->c;i++)
	{
	sprintf(buf,"%d  ",i);
	fputs(buf,fp);
	}
	fputs("\n",fp);
	fputs("begin\n",fp);
	sprintf(buf,"%ld\t%ld\trational\n",Y->r,Y->c);
	fputs(buf,fp);
	for(i=0;i<Y->r;i++)
	{
		for(j=0;j<Y->c;j++)
		{
			fmpq_fprint ( fp , fmpq_mat_entry(Y,i,j));
			fputs("\t",fp);
		}
		fputs("\n",fp);
	}
	fputs("end\n",fp);
	fclose(fp);
    printf("%s",filename2);
    return filename2;
}

void fmpq_eye(fmpq_mat_t I,int size,int sign)
//makes I a sizeXsize identity matrix 
{//corefunction
	int i,j;
	fmpq_mat_init(I,size,size);
	for (i=0;i<I->r;i++)
	{
		for (j=0;j<I->r;j++)
		{
			if(i==j)
			{
				fmpq_one(fmpq_mat_entry(I,i,j));
				if(sign<0)
				fmpq_neg(fmpq_mat_entry(I,i,j),fmpq_mat_entry(I,i,j));
			}
			else
			{
				fmpq_zero(fmpq_mat_entry(I,i,j));
			}
		}
	}
}







/**************************New functions for use with QSopt*****************/


//void findinitialhull5(fmpq_mat_t Vset, fmpq_mat_t Hset,mpq_QSprob* probptr, int projdim,int* vnum)
//{
	///*Replaces lrs by qsopt
	 //* Uses DD Method from |K|=1*/
	
	//FILE *fp;
	//int i=0,j,q,p,ncols,nrows;
	//int rank=0;
	//fmpq_mat_t subVset1,subVset,rrefsubVset;
	//fmpq_mat_t flintobj;
	//fmpq_mat_init(Vset,projdim,projdim);//
	//fmpq_mat_t temp_Vset;//quit
	//fmpq_t objval;
	//fmpq_mat_t optvtx,optvtx1;
	//nrows = mpq_QSget_rowcount (*probptr);
    //printf ("Number of constraints: %d\n", nrows);
    //ncols = mpq_QSget_colcount (*probptr);
    //printf ("Number of variables: %d\n", ncols);
	
	//fmpq_mat_init(flintobj,1,ncols);
	//int* Xz;
	//Xz=malloc((projdim-1)*sizeof(int));	
	//int cardalphabet=3;
	
	////while(rank<projdim)
	////{
		//cardalphabet=cardalphabet+2;
		//printf("\nAlphabet Cardinality=%d\n",cardalphabet);
		//for(i=0;i<projdim-1;i++)// initialize Xz vector to maximum value
		//{
			//Xz[i]=cardalphabet-1;
		//}
		//int success=1;//indicator for finding linearly indep vectors
		//lex_nextobj(flintobj,Xz,cardalphabet,projdim);// get lexicographically next(smaller) obj
		//printf("first objective");
		//fmpq_mat_print(flintobj);
		//fmpq_one(fmpq_mat_entry(flintobj,0,0));
		///*****************for using lrs lp solver**********/
		///*Q= reinit_lrs_dat(&Q);
		//P= reinit_lrs_dic(&P,&Q);
		//solvelp_withlrs(objval,optvtx1,P,Q,flintobj,projdim,MAXIMIZE);// get first ever vertex by solving lp*/
		///*************for using wsopt lp solver**********************/
		//solvelp_withqsopt2(objval, optvtx, flintobj,projdim,QS_MAX,probptr);
		///* //printf("\n first vertex:\n");
		////fmpq_mat_print(optvtx1);
		////printf("vnum is %d",vnum[0]);
		//copy_optvtx2Vset(Vset,optvtx1,vnum);// copy the first vertex into Vset. vnum points to next row(empty) in C sense and last used row in MATLAB sense 
		////fmpq_mat_print(Vset);
		//q=0;p=0;
		//while(rank<projdim)
		//{
			//p++;
			//printf("Vertices found so far: %d",vnum[0]);
			//fmpq_mat_submatrix(subVset1,Vset,1,vnum[0],1,Vset->c);// form subVset1 from Vset. it has the vertices found so far
			////fmpq_mat_print(subVset1);
			//fmpq_mat_init(subVset,subVset1->r+1,subVset1->c);// SubVset: has extra space for 1 vertex ;)
			//for(i=0;i<subVset->r-1;i++)// copy vertices found so far in subVset
			//{
				//for(j=0;j<subVset->c;j++)
				//{
					//fmpq_set(fmpq_mat_entry(subVset,i,j),fmpq_mat_entry(subVset1,i,j));
				//}
			//}
			
			//while(success && rank < vnum[0]+1)
			//{
				//q++;
				//for(i=0;i<subVset->c;i++)
				//{
					//fmpq_zero(fmpq_mat_entry(subVset,subVset->r-1,i));
				//}
				////printf("\nNew objective:\n");
				//if(isallzero(flintobj)!=0)// we don't want all-zero objective, it gives redudent vertices
				//{
					//fmpq_one(fmpq_mat_entry(flintobj,0,1));
				//}
				////fmpq_mat_print(flintobj);
				////printf("\nqvalue: %d\n",q);
				///**********Use lrs******/
				///*Q= reinit_lrs_dat(&Q);
				//P= reinit_lrs_dic(&P,&Q);
				////Q->verbose=TRUE;
				//solvelp_withlrs(objval,optvtx,P,Q,flintobj,projdim,MAXIMIZE);*/				
				///***********use qsopt***********/
				////printf("\nCandidate vertex:\n");
				////fmpq_mat_print(optvtx);
				///*solvelp_withqsopt(objval, optvtx, flintobj,projdim,QS_MAX,probptr);
				//for(i=0;i<subVset->c;i++)
				//{
					//fmpq_set(fmpq_mat_entry(subVset,subVset->r-1,i),fmpq_mat_entry(optvtx,0,i));
				//}
				//fmpq_mat_init(rrefsubVset,subVset->r,subVset->c);
				//rank=fmpq_mat_rref(rrefsubVset, subVset);
				////fmpq_mat_print(subVset);
			//}
			//if (success==0){
				//printf("\nAlphabet over\n");
				//fmpq_mat_print(subVset);
				//break;
				//}
			//copy_optvtx2Vset(Vset,optvtx,vnum);
			////fmpq_mat_print(subVset);
		//}
	//}*/
	
//}

void readlrsfile(const char* filename,fmpq_mat_t X, int*nlin,int** linrowsptr)
{//corefunction
	FILE *fp; 
	int *linrows;
	int m,n,i,j,firsttime,skip,num;
	char tempstr[15];
	nlin[0]=0;
	mpq_t z;
	fmpq_t q;
	fmpq_init(q);
	fmpq_mat_t Y;
	char* linptr;
	const char* begin="begin";
	char buf[4096]; 
	char* str;
	fp=fopen(filename,"r");
	firsttime=FALSE;
    while(fp!=NULL && fgets(buf, sizeof(buf), fp)!=NULL)
    {
		linptr=strstr(buf, "linearity") ;
		if (linptr !=NULL && firsttime==FALSE)
		{
			linptr=linptr+9;
			sscanf ( linptr, "%d", &num);
			//printf("\nNUmber of equalities: %d\n",num);
			nlin[0]=num;
			sprintf(tempstr, "%d", num);
			linptr=strstr(buf,tempstr)+strlen(tempstr);
			while(linptr[0]==9 || linptr[0]==32)
			{
				linptr=linptr+1;
			}
			linrows=(int*)malloc((nlin[0]+1)*sizeof(int));
			for (i=0;i<nlin[0];i++)
			{
				sscanf(linptr,"%d",&linrows[i]);
				//printf("\nfound number %d\n",linrows[i]);
				sprintf(tempstr, "%d", linrows[i]);
				linptr=linptr+strlen(tempstr);
				while(linptr[0]==9 || linptr[0]==32)
				{
				linptr=linptr+1;
				}
			}
			firsttime=TRUE;
		}
        if (strstr(buf, "begin"))
        {
        //printf("begin found!");
        gmp_fscanf(fp,"%d%d",&m,&n);
        //printf("size %d %d",m,n);
        fmpq_mat_init ( X , m, n);
		fmpq_mat_zero ( X);
		do// skip empty lines
		{
		fgets(buf, sizeof(buf), fp);
		}
		while(strlen(buf)==0);
		mpq_init(z);
		for(i=0;i<m;i++)
		{
			for(j=0;j<n;j++)
			{
				gmp_fscanf(fp,"%Qi",z);		
				fmpq_set_mpq(fmpq_mat_entry(X,i,j),z);
			} 
			do// skip empty lines
			{
				fgets(buf, sizeof(buf), fp);
			}
			while(strlen(buf)==0);
		}
		mpq_clear(z);
		break;
		}
    }
    if(fp!=NULL) 
    fclose(fp);
    *linrowsptr=linrows;
}








int writelrsfile(char* filename, char* type,fmpq_mat_t lrsmtx,int nlin,int* linearities, char* prepend,char* append)
{//corefunction	
	int i,j;
	char type1[1]="h";
	FILE* fp;
	if(filename!=(char*)NULL)
	{
	fp=fopen(filename,"w");
	}
	if(prepend!=NULL) 
	{
		if(filename!=(char*)NULL)
		{
			fputs(prepend,fp);
		}
		else
		{
			puts(prepend);
		}
	}
	if(type1[0]==type[0])//h-polyhedron: we must prepend linearity 
	{
		//fprintf(stderr,"\nh-poly, nlin=%d\n",nlin);
		if(nlin>0 && linearities!=(int*)NULL )
		{
		//fprintf(stderr,"\n linearities!\n");
		if(filename!=(char*)NULL)
		{
			fprintf(fp,"linearity  %d",nlin);
		}
		else
		{
			fprintf(stdout,"linearity  %d",nlin);
		}
		
		for(i=0;i<nlin;i++)
		{
			if(filename!=(char*)NULL)
			{
				fprintf(fp,"\t%d",linearities[i]);
			}
			else
			{
				fprintf(stdout,"\t%d",linearities[i]);
			}
		}
		}
		else
		//fprintf(stderr,"No linearities");

	if(filename!=(char*)NULL)
		fprintf(fp,"\n");
	else
		fprintf(stdout,"\n");
	}
	if(filename!=(char*)NULL)
	{
	fprintf(fp,"begin\n");
	fprintf(fp,"%ld   %ld   rational\n" ,lrsmtx->r,lrsmtx->c);
	}
	else
	{
		fprintf(stdout,"begin\n");
		fprintf(stdout,"%ld   %ld   rational\n" ,lrsmtx->r,lrsmtx->c);
	}
	for(i=0;i<lrsmtx->r;i++)
	{
		for(j=0;j<lrsmtx->c;j++)
		{
			//fprintf(fp,"%ld/%ld", fmpq_mat_entry(lrsmtx,i,j)->num, fmpq_mat_entry(lrsmtx,i,j)->den);
			if(fmpq_sgn(fmpq_mat_entry(lrsmtx,i,j))==0)
			{
				if(filename!=(char*)NULL)
				fprintf(fp,"0\t");
				else
				fprintf(stdout,"0\t");
			}
			else
			{
				if(filename!=(char*)NULL)
				{
					fmpq_fprint ( fp , fmpq_mat_entry(lrsmtx,i,j));
					fprintf(fp,"\t");
				}
				else
				{
					fmpq_fprint ( stdout , fmpq_mat_entry(lrsmtx,i,j));
					fprintf(stdout,"\t");
				}
			}
		}
		if(filename!=(char*)NULL)
		{
			fprintf(fp,"\n");
		}
		else
		{
			fprintf(stdout,"\n");
		}
	}
	//fprintf(stderr,"Done printing numbers");	
	if(filename!=(char*)NULL)
	{
	fprintf(fp,"end\n");
	}
	else
	fprintf(stdout,"end\n");
	if(append!=NULL)
	{
		if(filename!=(char*)NULL)
		{
			fputs(append,fp);
		}
		else
		puts(append);
	}
	if(filename!=(char*)NULL)
	{
		fclose(fp);
	}
}




struct intarray* redund_naive(fmpq_mat_t lrsmtx, char* type, int nlin, int* linrows)
{
	//corefunction
	// returns indices in C sense
	char type1[1]="h";
	char type2[1]="v";
	int rows,cols,status,rval;
	fmpq_mat_t lrsmtx2,mat,linmat;
	int iscone,i,j,k,p,m,nineq,q;
	mpq_t* coef,*rhsold,*rhsnew,*crow,*obj,*value,*one,*minusone,*zero;
	int rwindex=0;
	coef=mpq_EGlpNumAllocArray(1);
	one=mpq_EGlpNumAllocArray(1);
	minusone=mpq_EGlpNumAllocArray(1);
	zero=mpq_EGlpNumAllocArray(1);
	mpq_set_si(zero[0],0,1);
	mpq_set_si(one[0],1,1);
	mpq_set_si(minusone[0],-1,1);
	int numred=0;
	mpq_QSprob prob;
	int* redrows=malloc((lrsmtx->r)*sizeof(int));
	struct intarray* indarr,*redarr;
	printf("\n redund_naive says::");
	for(i=0;i<nlin;i++)
	{
		printf("%d\t",linrows[i]);
	}
	if(type[0]==type2[0])// v-poly
	{
		fmpq_mat_init(lrsmtx2,lrsmtx->r,lrsmtx->c+1);
		for(i=0;i<lrsmtx->r;i++)
		{
			for(j=0;j<lrsmtx->c;j++)
			{
				fmpq_set(fmpq_mat_entry(lrsmtx2,i,j+1),fmpq_mat_entry(lrsmtx,i,j));
			}
		}
		fmpq_mat_neg(lrsmtx2,lrsmtx2);
	}
	
	else if(type[0]==type1[0])// h-poly
	{
		fmpq_mat_init(lrsmtx2,lrsmtx->r,lrsmtx->c);
		fmpq_mat_set(lrsmtx2,lrsmtx);
	}
		if(nlin>0)
		{
			fmpq_mat_init(linmat,nlin,lrsmtx2->c);
			// identify dependent linearities
			for(i=0;i<nlin;i++)
			{
				copyrow(linmat,i,lrsmtx2,linrows[i]-1);
			}
			indarr=maximal_ind(linmat);
		}
		qsload_fromlrsmatrix(lrsmtx2,nlin,linrows, &prob);  
		//mpq_QSwrite_prob (prob, "logfile.lp","LP");
		mpq_QSchange_objsense(prob,QS_MAX);  // we will only need to maximize throughout
		rows=mpq_QSget_rowcount(prob);
		cols=mpq_QSget_colcount(prob);
		//rhsold=(mpq_t*)malloc(rows*sizeof(mpq_t));
		rhsold=mpq_EGlpNumAllocArray(rows);
		rhsnew=mpq_EGlpNumAllocArray(rows);
		crow=mpq_EGlpNumAllocArray(cols);
		obj=mpq_EGlpNumAllocArray(cols);
		value=mpq_EGlpNumAllocArray(1);
		//printf("linrows are:\n");
		mpq_t* tempold=mpq_EGlpNumAllocArray(1);
		mpq_t* tempnew=mpq_EGlpNumAllocArray(1);
		//for(i=0;i<nlin;i++)
		//{
			//printf("%d\t",linrows[i]);
		//}
		
		mpq_QSget_rhs(prob,rhsold);
		for(j=0;j<lrsmtx2->r;j++)
		{
			if(ismember(linrows,nlin,j+1)==0)// we dont want to test equalities
			{
				// add 1 to rhs of row being tested
				mpq_add(tempnew[0],rhsold[j],one[0]);
				mpq_QSchange_rhscoef (prob,j, tempnew[0]);// set up the problem
				// obtain 'i'th row to be used as objective 
				for(i=0;i<cols;i++)
				{
					mpq_QSget_coef(prob,j,i,crow[i]);
				}
				// cchange the objective 
				for(i=0;i<cols;i++)
				{	
					mpq_QSchange_objcoef(prob,i,crow[i]);
				}
				mpq_QSget_obj(prob,obj);

				// solve lp
				fprintf(stderr,"Checking row %d",j);
				rval = QSexact_solver (prob, NULL, NULL, NULL, DUAL_SIMPLEX, &status);
				// get objective value
				rval = mpq_QSget_objval (prob, value);
				//gmp_printf("\n Objective value %Qi and b_k %Qi\t", value[0],rhsold[j]);
				// see if value > rhs[i], if it is, 'i'th inequality is nonredundant
				if(mpq_cmp (value[0],rhsold[j])<=0)//redundant
				{
					fprintf(stderr,"\n%d REDUNDANT!!\n",j);
					redrows[numred]=j;
					numred++;
					//printf("\n%d redundant so far\n",numred);
					for(i=0;i<cols;i++)// nullify redundant row
					{
						mpq_QSchange_coef(prob,j,i,zero[0]);
					}
				}
				// fix rhs in the problem 
				mpq_QSchange_rhscoef (prob,j , rhsold[j]);
			}
		}
		if (nlin>0)
		{
			//printf("indep equalities:");
			//for(i=0;i<indarr->len;i++)
			//{
				//printf(" %d\t",indarr->array[i]);
			//}
			for(i=0;i<nlin;i++)
			{
				if(ismember(indarr->array,indarr->len,i)==0)// linrows[i] is dependent
				{
					redrows[numred]=linrows[i]-1;
					numred++;
				}
			}
		}
	if(prob) mpq_QSfree_prob(prob);
	
	mpq_EGlpNumFreeArray(rhsold);
	mpq_EGlpNumFreeArray(rhsnew);
	mpq_EGlpNumFreeArray(crow);
	mpq_EGlpNumFreeArray(obj);
	mpq_EGlpNumFreeArray(value);
	mpq_EGlpNumFreeArray(tempold);
	mpq_EGlpNumFreeArray(tempnew);
	
	mpq_EGlpNumFreeArray(coef);
	mpq_EGlpNumFreeArray(one);
	mpq_EGlpNumFreeArray(minusone);
	mpq_EGlpNumFreeArray(zero);
	redarr=malloc(sizeof(struct intarray));
	redarr->array=redrows;
	redarr->len=numred;
}


int ismember(int* array,int size,int key)
{	
	//corefunction
	int i;
	if(array!=(int*)NULL && size>0)
	{
		for(i=0;i<size;i++)
		{
			if(array[i]==key)
			{
				return 1;//found key
			}
		}
		return 0;// key not found
	}
	else
	{
		return 0;// if array is empty key is obviously not there
	}
}



	//// construct a matrix lrsmtx2 with all linrows in the beginning
		//fmpq_mat_init(lrsmtx2,lrsmtx->r,lrsmtx->c);
		//p=nlin;// inequality index for loop(C sense)
		//q=0; //equality index for loop
		//for(i=0;i<lrsmtx->r;i++)
		//{
			//if(ismember(linrows,nlin,i+1)==1)//  i is an equality index
			//{
				//copyrow(lrsmtx2,q,lrsmtx,i);
				//q++;
			//}
			//else  // i is inequality index
			//{
				//copyrow(lrsmtx2,p,lrsmtx,i);
				//p++;
			//}
		//}
struct intarray* maximal_ind(fmpq_mat_t rowset)
{	
	//corefunction
	int rank,i,j;
	fmpq_mat_t testmat;
	fmpq_mat_t rtestmat;
	i=2;
	struct intarray* maxindset=malloc(sizeof(struct intarray));
	maxindset->array=malloc(rowset->r*sizeof(int));
	maxindset->len=1;
	maxindset->array[0]=0;// first index is automatically in
	rank=1;
	//printf("\nrowset is:");
	
	//fmpq_mat_print(rowset);
	for(i=1;i<rowset->r;i++)
	{ 
		fmpq_mat_init(testmat,maxindset->len+1,rowset->c);// one extra row(i+1)
		fmpq_mat_init(rtestmat,maxindset->len+1,rowset->c);
		
		// copy known independent rows into testmat
		for(j=0;j<maxindset->len;j++)
		{
			copyrow(testmat,j,rowset,maxindset->array[j]);
		}
		// copy row being tested into testmat
		copyrow(testmat,maxindset->len,rowset,i);
		//fmpq_mat_print(testmat);
		//printf("\nmaximal_ind testing linearity %d and rank is %d\n",i,fmpq_mat_rref(rtestmat,testmat));
		if(rank<fmpq_mat_rref(rtestmat,testmat))// rank increased strictly
		{
			rank++;
			maxindset->array[maxindset->len]=i;
			maxindset->len=maxindset->len+1;
			
		}
		fmpq_mat_clear(testmat);
		fmpq_mat_clear(rtestmat);
	}
	return maxindset;
}



int ismember2(int* array,int size,int key)
{//corefunction
	// returns array index if key is found
	// else 
	int i;
	if(array!=(int*)NULL && size>0)
	{
		for(i=0;i<size;i++)
		{
			if(array[i]==key)
			{
				return i;//found key
			}
		}
		return -1;// key not found
	}
	else
	{
		return -1;// if array is empty key is obviously not there
	}
}




int isnewvtx(fmpq_mat_t testvtx,fmpq_mat_t Vset,int num,int projdimreal)
{//corefunction
	/*Returns 1 if testvtx increases the dimension affine hull of extreme points already in Vset 
	 * else returns num is the number of vertices in Vset(to allow a Vset with extra rows)
	 * all vertices in lrs format(1 prepended) where columns of Vset after projdimreal+1 are slacks
	 * and are ignored for testing affine dimension*/
	if(num==0)
	{
		return 1;
	}
	fmpq_mat_t testmtx;
	fmpq_mat_t rreftestmtx;
	fmpq_mat_init(testmtx,num+1,Vset->c);
	fmpq_mat_init(rreftestmtx,num+1,Vset->c);
	copy_submat2submat(testmtx,1,1,Vset,1,num,1,projdimreal+1);
	copy_submat2submat(testmtx,num+1,1,testvtx,1,1,1,projdimreal+1);
	//printf("num is %d and rank is %ld",num,fmpq_mat_rref(rreftestmtx,testmtx));
	if(fmpq_mat_rref(rreftestmtx,testmtx)>num)
	{
		fmpq_mat_clear(testmtx);
		fmpq_mat_clear(rreftestmtx);
		return 1;
	}
	else
	{
		fmpq_mat_clear(testmtx);
		fmpq_mat_clear(rreftestmtx);
		return 0;
	}
}

//void pts2hpln2(hpln,Vset,vnum)
void Vset2diffmat(fmpq_mat_t diffmat,fmpq_mat_t Vset,int vnum)
/*accepts Vset in lrs format whose rows are
 * [1 v1]...[1 vk] and returns diffmat whose 
 * rows are [v1-v2]...[v1-vk]*/	
{//corefunction
	int i,j;
	fmpq_mat_init(diffmat,vnum-1,Vset->c);
	for(i=0;i<diffmat->r;i++)
	{
		fmpq_one(fmpq_mat_entry(diffmat,i,0));
		for(j=0;j<diffmat->c-1;j++)
		{
			fmpq_sub(fmpq_mat_entry(diffmat,i,j+1),fmpq_mat_entry(Vset,0,j+1),fmpq_mat_entry(Vset,i+1,j+1));
		}
	}
}	


struct intarray* shovepolytope_insubspace(fmpq_mat_t projeqmtx,struct intarray** projeqinfo,fmpq_mat_t lrsmtx,fmpq_mat_t lrsmtx1,int nlin,int*linptr1,int projdimreal)
{//corefunction
	/* Assumes that equalities are linearly independent
	 * hence, nlin equalities can eliminate nlin variables.
	 * Might produce redundancies but polytope is full-dimensionl
	 * in the resultant subspace
	 * Returns indices of columns whose variables were eliminated*/
	//fmpq_mat_t lrsmtx;
	//printf("\n\tShovepolytope projdimreal= %d", projdimreal);
	fmpq_mat_init(lrsmtx,lrsmtx1->r,lrsmtx1->c);
	fmpq_mat_set(lrsmtx,lrsmtx1);
	int i,j,k,l,elim,eq;
	int* projeq;
	if(nlin>0)
	{
		projeq=malloc(nlin*sizeof(int));
	}
	else
	{
		projeq=NULL;
	}
	int*projelimvar=malloc(projdimreal*sizeof(int));
	int* linptr;
	if(nlin>0)
	{
		linptr=malloc(nlin*sizeof(int));
	}
	else
	{
		linptr=NULL;
	}
	//printf("\n Creating local copy of linptr1");
	for(i=0;i<nlin;i++)
	{
		
		linptr[i]=linptr1[i];
		//printf("\t%d\t",linptr[i]);
	}
	struct intarray* ret;
	fmpq_mat_t row;
	fmpq_mat_init(row,1,lrsmtx->c);
	ret=malloc(sizeof(struct intarray));
	fmpq_t coeff;
	fmpq_init(coeff);
	
	int* evars;
	if(nlin>0)
	{
		evars=malloc(nlin*sizeof(int));// stores column indices of variables eliminated 
	}
	else
	{
		evars=NULL;
	}
	int ecount=0;// counter for number of variables eliminated so far
	fmpq_mat_t linrowmtx;
	fmpq_mat_t flinrowmtx;
		fmpq_mat_t rrefflinrowmtx;
		fmpq_mat_init(linrowmtx,nlin,lrsmtx->c);
		for(i=0;i<nlin;i++)
		{
			for(j=0;j<lrsmtx->c;j++)
			{
				fmpq_set(fmpq_mat_entry(linrowmtx,i,j),fmpq_mat_entry(lrsmtx,linptr[i]-1,j));
			}
		}
		//fmpq_mat_print(linrowmtx);
		
		fmpq_mat_init(flinrowmtx,linrowmtx->r,linrowmtx->c);
		fmpq_mat_init(rrefflinrowmtx,linrowmtx->r,linrowmtx->c);
		fliplr(flinrowmtx,linrowmtx);
		//fmpq_mat_print(flinrowmtx);
		fmpq_mat_rref(rrefflinrowmtx,flinrowmtx);
		//fmpq_mat_print(rrefflinrowmtx);
		fliplr(linrowmtx,rrefflinrowmtx);
		//fmpq_mat_print(linrowmtx);
		for(i=0;i<nlin;i++)
		{
			for(j=0;j<lrsmtx->c;j++)
			{
				fmpq_set(fmpq_mat_entry(lrsmtx,linptr[i]-1,j),fmpq_mat_entry(linrowmtx,i,j));
			}
		}
	fmpq_mat_t col;
	fmpq_mat_init(col,nlin,1);
	j=lrsmtx->c-1;// start from highest indexed variable
	eq=1;// index of identity column we search for for indentifying next variable to be eliminated
	k=0;
	while(ecount<nlin)//We continue eliminating until the count reaches nlin
	{
		//elim=variable_isdisposable(lrsmtx,nlin,linptr,j);
		copy_submat2submat(col,1,1,linrowmtx,1,linrowmtx->r,j+1,j+1);
		if(isIDcol(col)==eq)// identity column, next variable to be eliminated found
		{
			if(j<=projdimreal)// we are eliminating projection variable
			{
				//printf("projection equality found:: %d", linptr[eq-1]);
				
				projelimvar[k]=j;
				projeq[k]=linptr[eq-1];// index of equality in lrsmtx(in MATLAB sense)
				k++;
			}
			evars[ecount]=j;
			//printf("\nEliminated variable %d with equality %d",j,linptr[eq-1]-1);
			eliminatevar(lrsmtx,linptr[eq-1]-1,j);
			linptr[eq-1]=-1;
			ecount++;
			eq++;
		}
		j--;
		//if(j<1)
		//{
			//printf("\n Oops j drops below 1\n");
		//}
	}
	projeqinfo[0]=malloc(sizeof(struct intarray));
	projeqinfo[0]->array=projelimvar;
	projeqinfo[0]->len=k;
	fmpq_mat_init(projeqmtx,projeqinfo[0]->len,projdimreal+1);
	//printf("projection equalities");
	
		//for(i=0;i<k;i++)
		//{
			//printf("\t%d\t",projeq[i]);
		//}
	for(i=0;i<projeqmtx->r;i++)
	{
		copy_submat2submat(projeqmtx,1,1,linrowmtx,ismember2(linptr1,nlin,projeq[i])+1,ismember2(linptr1,nlin,projeq[i])+1,1,projdimreal+1);
	}
	//fmpq_mat_print(projeqmtx);
	ret->array=evars;
	ret->len=ecount;
	
}

void eliminatevar(fmpq_mat_t lrsmtx,int eqindex, int varindex)
{//corefunction
	/*eqindex is index of row(in C sense) of lrsmtx that is equality
	 * varindex is index of variable to be eliminated */
	 int i;
	fmpq_mat_t row;
	fmpq_mat_t equality;
	fmpq_t coeff;
	fmpq_t zero;
	fmpq_init(zero);
	fmpq_init(coeff);
	//printf("\ncoeff is:");
	//fmpq_print(coeff);
	fmpq_set(coeff,fmpq_mat_entry(lrsmtx,eqindex,varindex));
	if(fmpq_equal(coeff,zero)!=0)
	{
		printf("\nInvalid variable/inequaliity index");
		return;
	}
	fmpq_mat_init(equality,1,lrsmtx->c);
	fmpq_mat_init(row,1,lrsmtx->c);
	//printf("\neqindex is %d\n",eqindex);
	copyrow(equality,0,lrsmtx,eqindex);
	copyrow(lrsmtx,eqindex,row,0);// will set row at eqindex to all zeros
	//printf("\nequality is: ");
	//fmpq_mat_print(equality);
	//printf("\nlrsmtx before elimination begins:");
	//fmpq_mat_print(lrsmtx);
	for(i=0;i<lrsmtx->r;i++)
	{
		if(i!=eqindex)// && isallzero(row)!=1)// used up equalities will be set to all zeros and must be skipped
		{
		copyrow(row,0,lrsmtx,i);
		//printf("\n processing row %d",i);
		//fmpq_mat_print(row);
		//printf("\nequality is: ");
		//fmpq_mat_print(equality);
		
			substituteeq(row,equality,varindex);
		
		copyrow(lrsmtx,i,row,0);
		}
	}
	
}

void substituteeq(fmpq_mat_t row,fmpq_mat_t equality1, int elimindex)
{//corefunction
	/*Substitutes equality in row(both in lrs format)
	 * thereby eliminating variable corresponding to elimindex*/
	int i;
	fmpq_t temp1,temp2;
	fmpq_init(temp1);
	fmpq_init(temp2);
	fmpq_mat_t equality;
	fmpq_mat_init(equality,equality1->r,equality1->c);
	fmpq_mat_set(equality,equality1);
	//printf("\nProcessing row:");
	//fmpq_mat_print(row);
	//printf("\nElimindex is %d and equality is:",elimindex);
	//fmpq_mat_print(equality1);
	if(elimindex>0)
	{  
		fmpq_set(temp1,fmpq_mat_entry(equality,0,elimindex));
		fmpq_set(temp2,fmpq_mat_entry(row,0,elimindex));
		//fmpq_print(temp1);
		for(i=0;i<equality->c;i++)
		{
			fmpq_div(fmpq_mat_entry(equality,0,i),fmpq_mat_entry(equality,0,i),temp1);
			fmpq_neg(fmpq_mat_entry(equality,0,i),fmpq_mat_entry(equality,0,i));
			fmpq_mul(fmpq_mat_entry(equality,0,i),fmpq_mat_entry(equality,0,i),temp2);
			fmpq_add(fmpq_mat_entry(row,0,i),fmpq_mat_entry(equality,0,i),fmpq_mat_entry(row,0,i));
		}
	}
	fmpq_mat_clear(equality);
	fmpq_clear(temp1);
	fmpq_clear(temp2);

}

struct intarray* chm_preprocess(fmpq_mat_t projeqmtx,fmpq_mat_t lrsmtx1,int nlin,int* linrows,int projdimreal)
{//corefunction
		struct intarray** projeqinfo=malloc(sizeof(struct intarray*));
	    struct diminfo* dimension=NULL;
	    struct intarray* redinfo=NULL,*elim=NULL;
	    char*prepend;
	    //fmpq_mat_t projeqmtx;
	    fmpq_mat_t newlrsmtx,row,projeq,lrsmtx,elimlrsmtx;
	    fmpq_mat_init(lrsmtx,lrsmtx1->r,lrsmtx1->c);
	    fmpq_mat_set(lrsmtx,lrsmtx1);
	    int newnlin,*newlinrows,i,j,zerows,r,c;
	    newnlin=0;
	    //writelrsfile("dimin.ine","h",lrsmtx,nlin,linrows,NULL,NULL);
		dimension=finddim_polyhedron2(lrsmtx,nlin, linrows,"h");
		//writelrsfile("dimop.ine","h",lrsmtx,dimension->linearities->len,dimension->linearities->array,NULL,NULL);
		redinfo=redund_naive(lrsmtx,"h",dimension->linearities->len,dimension->linearities->array);
		if(dimension->linearities->len>0)
		{
			newlinrows=malloc(dimension->linearities->len*sizeof(int));
		}
		else
		{
			newlinrows=NULL;
		}
		fmpq_mat_init(newlrsmtx,lrsmtx->r-redinfo->len,lrsmtx->c);
		j=0;
		//printf("\n Linearities returned by dimension:");
		//for(i=0;i<dimension->linearities->len;i++)
		//{
			//printf("%d\t",dimension->linearities->array[i]);
		//}
		if(redinfo->len>0)
		{
			prepend=malloc(4096*(redinfo->len)*sizeof(char));
			prepend[0]='\0';
		}
		else
		{
			prepend=NULL;
		}
		char* append=malloc(4096*sizeof(char));
		char *strrow=malloc(70*sizeof(char));
		strrow[0]='\0';
		
		append[0]='\0';
		for(i=0;i<redinfo->len;i++)
		{
			if(ismember(dimension->linearities->array,dimension->linearities->len,redinfo->array[i]+1)==1)// redundant row was infact a linearity 
			{
				sprintf(strrow,"*linearity in row %d was dependent and was removed\n",redinfo->array[i]+1);
				strcat(prepend,strrow);			
			}
			else
			{
				sprintf(strrow,"*row %d was redundant and removed\n",redinfo->array[i]+1);
				strcat(prepend,strrow);
			}
		}
		if(prepend!=NULL)
		{
			//printf("\nprepend>>>>>>>>>>");
			//puts(prepend);
		}
		//redinfo indices are in C sense, whereas dimension->linearities is in MATLAB sense
		for(i=0;i<lrsmtx->r;i++)
		{
			if(ismember2(redinfo->array,redinfo->len,i)==-1)// i+1(in MATLAB sense) is nonredundant row
			{
				if(ismember2(dimension->linearities->array,dimension->linearities->len,i+1)!=-1)// i+1 is actually equality
				{
					newlinrows[newnlin]=j+1;
					newnlin++;
				}
				//printf("\ncopy row %d to newlrsmtx\n",i);

				copyrow(newlrsmtx,j,lrsmtx,i);
				j++;
			}
		}
		//writelrsfile("beforeelim.ine","h",newlrsmtx,newnlin,newlinrows,prepend,append);
		//printf("\nlinrows before shovepolytope");
		//for(i=0;i<newnlin;i++)
		//{
			//printf("%d\t",newlinrows[i]);
		//}
		elim= shovepolytope_insubspace(projeqmtx,projeqinfo,elimlrsmtx,newlrsmtx,newnlin,newlinrows,projdimreal);
		//printf("\n\tProjection equalities ");
		//fmpq_mat_print(projeqmtx);
		fmpq_mat_t finalmtx1,finalmtx2;
		fmpq_mat_init(row,1,newlrsmtx->c);
		//printf("\nlinrows after shovepolytope");
		//for(i=0;i<newnlin;i++)
		//{
			//printf("%d\t",newlinrows[i]);
		//}
		zerows=0;
		r=0;
		c=0;
		for(i=0;i<elimlrsmtx->r;i++)
		{
			copyrow(row,0,elimlrsmtx,i);
			if(fmpq_mat_is_zero(row)!=0)
			{
				zerows++;
			}
		}
		fmpq_mat_init(finalmtx1,elimlrsmtx->r-zerows,elimlrsmtx->c-elim->len);
		//writelrsfile("elim.ine","h",elimlrsmtx,0,NULL,NULL,NULL);
		//fmpq_mat_print(newlrsmtx);
		for(i=0;i<elimlrsmtx->r;i++)
		{
			c=0;
			copyrow(row,0,elimlrsmtx,i);
			if(fmpq_mat_is_zero(row)==0)
			{
				//printf("\n copy row %d",i);
				for(j=0;j<elimlrsmtx->c;j++)
				{
					if(ismember2(elim->array,elim->len,j)==-1)
					{
						fmpq_set(fmpq_mat_entry(finalmtx1,r,c),fmpq_mat_entry(elimlrsmtx,i,j));
						c++;
					}
				}
				r++;
			}
		}
		//writelrsfile(NULL,"h",finalmtx1,0,NULL,NULL,NULL);
		fmpq_mat_clear(lrsmtx1);
		fmpq_mat_init(lrsmtx1,finalmtx1->r,finalmtx1->c);
		fmpq_mat_set(lrsmtx1,finalmtx1);
		fmpq_mat_clear(lrsmtx);
		
		fmpq_mat_clear(newlrsmtx);
		fmpq_mat_clear(row);
		//fmpq_mat_clear(projeq);
		fmpq_mat_clear(elimlrsmtx);
		//printf("\n chm_prreprocess says:: projeqmtx is::");
		//fmpq_mat_print(projeqmtx);
		return projeqinfo[0];
		//fmpq_mat_init(projeq,projeqinfo[0]->len,projdimreal+1);
		//fmpq_mat_print(newlrsmtx);
}



char* chm_makebounded2(fmpq_mat_t Y,fmpq_mat_t projeqmtx, char* filename,int* projdimptr,char** forewrdptr)
//writes a file with name filenamebounded.ine correnponding to the filename provided(provided filename already has .ine at the end)
{//corefunction
	//fprintf("\nConverting unbounded problem to bounded problem...");
	int fore=0;
	char* prepend=malloc(STR_LEN_INC*sizeof(char));
	prepend[0]='\0';
	struct intarray* projinfo;
	struct intarray* redinfo;
	int projdim=projdimptr[0];
	int nlin;
	int* linrows,*newlinrows;
	int rowsize, colsize,newnlin;
	FILE *fp; 
	int m,n,i,j;
	mpq_t z;
	fmpq_t q;
	fmpq_init(q);
	fmpq_mat_t X,newY;
	const char* begin="begin";
	char buf[4096]; 
	fp=fopen(filename,"r");
    while(fp!=NULL && fgets(buf, sizeof(buf), fp)!=NULL)
    {
        if (strstr(buf, "begin"))
        {
		fore=1;
        //printf("begin found!");
        gmp_fscanf(fp,"%d%d",&m,&n);
        //printf("size %d %d",m,n);
        fmpq_mat_init ( X , m, n);
		fmpq_mat_zero ( X);
		do// skip empty lines
		{
		fgets(buf, sizeof(buf), fp);
		}
		while(strlen(buf)==0);
		mpq_init(z);
		for(i=0;i<m;i++)
		{
			for(j=0;j<n;j++)
			{
				gmp_fscanf(fp,"%Qi",z);		
				fmpq_set_mpq(fmpq_mat_entry(X,i,j),z);
			} 
			do// skip empty lines
			{
				fgets(buf, sizeof(buf), fp);
			}
			while(strlen(buf)==0);
		}
		mpq_clear(z);
		break;
		}
		if(fore==0 && buf[0]!='*')
		{
			strcat(prepend,buf);
		}
    }
    if(fp!=NULL) 
    fclose(fp);
    //fmpq_mat_print(X);
    char *filename2=NULL;
    filename2=malloc(100*sizeof(char));//(strlen(filename)+8)
    filename2[0]='\0';
    
    strncpy(filename2,filename,strlen(filename)-4);
    rowsize=X->c+X->r;
    colsize=X->r+projdim+1;
    fmpq_mat_init(Y,X->c+X->r,X->r+projdim+1);// Y matrix corresponds to bounded version of X
	fmpq_mat_zero(Y);// initialize Y to all zero
	//X is m by n+1  matrix
	fmpq_one(fmpq_mat_entry(Y,X->c-1,0));// 1 for \lambda_1+\lambda_2...\lambda_m
	fmpq_mat_t Ip;
	fmpq_eye(Ip,projdim,-1);
	copy_submat2submat(Y,1,2,Ip,1,Ip->r,1,Ip->c);// coefficients of \alpha s in repective equalities
	fmpq_mat_t A,At;
	fmpq_mat_submatrix(A,X,1,X->r,2,X->c);
	//fmpq_mat_print(A);
	fmpq_mat_init(At,A->c,A->r);
	fmpq_mat_transpose(At,A);
	for(i=0;i<At->r;i++)
	{
		for(j=0;j<At->c;j++)
		{
			fmpq_neg(fmpq_mat_entry(At,i,j),fmpq_mat_entry(At,i,j));
		}
	}
	copy_submat2submat(Y,1,projdim+2,At,1,At->r,1,At->c);
	for(i=projdim+2-1;i<Y->c;i++)
	{
		fmpq_one(fmpq_mat_entry(Y,X->c-1,i));// 1-\sum_i{\lambda_i}=0
		fmpq_neg(fmpq_mat_entry(Y,X->c-1,i),fmpq_mat_entry(Y,X->c-1,i));
	}
	fmpq_mat_t Ilambda;
	fmpq_eye(Ilambda,X->r,1);
	copy_submat2submat(Y,X->c+1,projdim+2,Ilambda,1,Ilambda->r,1,Ilambda->c);
	/*Y= [I_projdim  A^t ]--->equalities involving \alphas and \lambdas  
	 *   [0          I_m ]---> \lambdas >= 0 constraints */
	 nlin=X->c;
	 linrows=malloc(nlin*sizeof(int));
	 for(i=1;i<=nlin;i++)
	 {
		 linrows[i-1]=i;
	 }
	printf("Bounded problem had dimension %ld originally and had %ld rows",Y->c,Y->r);
	writelrsfile("b4bred.ine","h",Y,nlin,linrows,NULL,NULL);

	redinfo=redund_naive(Y,"h",nlin,linrows);
	fmpq_mat_init(newY,Y->r-redinfo->len,Y->c);
	j=0;
	newnlin=0;
	if(nlin>0)
	{
		newlinrows=malloc(nlin*sizeof(int));
	}
	else
	{
		newlinrows=NULL;
	}
	for(i=0;i<Y->r;i++)
	{
		if(ismember2(redinfo->array,redinfo->len,i)==-1)// i+1(in MATLAB sense) is nonredundant row
		{
			if(ismember2(linrows,nlin,i+1)!=-1)// i+1 is actually equality
			{
				newlinrows[newnlin]=j+1;
				newnlin++;
			}
			//printf("\ncopy row %d to newlrsmtx\n",i);
			copyrow(newY,j,Y,i);
			j++;
		}
	}
	//writelrsfile("b4bproc.ine","h",newY,newnlin,newlinrows,NULL,NULL);
	projinfo=chm_preprocess(projeqmtx,newY,newnlin,newlinrows,projdim); 
	projdimptr[0]=projdim-projinfo->len;
	fmpq_mat_clear(Ilambda);
	strcat(filename2,"bounded.ine");
    fmpq_mat_clear(Y);
    fmpq_mat_init(Y,newY->r,newY->c);
    fmpq_mat_set(Y,newY);
    forewrdptr[0]=prepend;
    writelrsfile(filename2,"h",newY,0,NULL,prepend,NULL);
    free(linrows);
    free(newlinrows);
    return filename2;
}

void embedvtx_inhyperplanes(fmpq_mat_t newvtx,fmpq_mat_t vtx,fmpq_mat_t hyperplanes)
{//corefunction
	/*hyperplanes are rows of bigger size than vtx, and 
	 * the extra part is of the form 0 0 ...1...0 0 0*/
	int i,j,k;
	fmpq_mat_init(newvtx,1,hyperplanes->c);
	fmpq_t temp1;
	fmpq_init(temp1);
	fmpq_t temp2;
	fmpq_init(temp2);	
	fmpq_t temp3;
	fmpq_init(temp3);
	fmpq_t one;
	fmpq_init(one);
	fmpq_one(one);
	for(i=0;i<hyperplanes->c;i++)
	{
		if(i<vtx->c)
		{
			fmpq_set(fmpq_mat_entry(newvtx,0,i),fmpq_mat_entry(vtx,0,i));
		}
		else
		{
			for(j=0;j<hyperplanes->r;j++)
			{
				if(fmpq_equal(one,fmpq_mat_entry(hyperplanes,j,i))!=0)
				{
					fmpq_neg(temp1,fmpq_mat_entry(hyperplanes,j,0));
					printf("temp1::");
					fmpq_print(temp1);
					printf("\t");
					for(k=1;k<vtx->c;k++)
					{	
						fmpq_mul(temp2,fmpq_mat_entry(hyperplanes,j,k),fmpq_mat_entry(vtx,0,k));
						printf("temp2");
						fmpq_print(temp2);
						fmpq_set(temp3,temp1);
						fmpq_sub(temp1,temp3,temp2);
						fmpq_print(temp1);
						printf("\t");
					}
					fmpq_set(fmpq_mat_entry(newvtx,0,i),temp1);
					break;
					
				}
				
			}
		}
	}
}

void raylist2ineq(fmpq_mat_t lrsmtx, struct raylist* head)
{//corefunction
	fmpq_mat_t row;
	fmpq_t temp;
	fmpq_init(temp);
	struct raylist* handle;
	int numrows,i,j;
	if(head!=NULL)
	{
		fmpq_mat_init(row,1,head->ray->c);
		handle=head->nextray;
		numrows=1;
		while(handle!=head)
		{
			handle=handle->nextray;
			numrows++;
		}
		fmpq_mat_init(lrsmtx,numrows,head->ray->c);
		handle=head;
		for(i=0;i<lrsmtx->r;i++)
		{
			copyrow(lrsmtx,i,handle->ray,0);
			handle=handle->nextray;
		}
		fmpq_mat_neg(lrsmtx,lrsmtx);
	}
}

//int isprojVset_fd(fmpq_mat_t Vset,int projdimreal)
//// tests if submatrix of vset corresponding to projection is full dimensional in projection space
//// returns 1 if true else 0
//{
	//int rank;
	//fmpq_mat_t testmat;
	//fmpq_mat_t rreftestmat;
	//fmpq_mat_init(testmat,Vset->r,projdimreal+1);
	//fmpq_mat_init(rreftestmat,Vset->r,projdimreal+1);
	//fmpq_mat_submatrix(testmat,1,1,Vset,1,Vset->r,1,projdimreal+1);
	//rank=fmpq_mat_rref(rreftestmat,testmat);
	
//}

void findinitialhull8(fmpq_mat_t extrays,fmpq_mat_t lrsmtx,int projdimreal)
{//corefunction
	fmpq_mat_t X,Y,testcol,nullmat,U,lrsmtxnew,flintobj,optvtx,testVset,testvtx,diffmat,Vset;
	fmpq_t objval;
	int i,k,j,seedvar,ncols;
	int *vnum=malloc(sizeof(int));
	vnum[0]=0;// 0 extreme points found so far
	mpq_QSprob prob;
	mpq_QSprob* probptr=malloc(sizeof(mpq_QSprob));
	prob=probptr[0];
	//fmpq_mat_print(lrsmtx);
	
	//polytope_ineq2eqcon(lrsmtxnew,lrsmtx,projdimreal);
	//fmpq_mat_print(lrsmtxnew);
	qsload_fromlrsmatrix(lrsmtx,0,NULL,&prob);
	//mpq_QSwrite_prob(prob,"logfile.lp","LP");

	fmpq_mat_init(Vset,1,lrsmtx->c);
	seedvar=1;
	// first vertex
	ncols=mpq_QSget_colcount(prob);
	fmpq_mat_init(flintobj,1,ncols+1);
	fmpq_one(fmpq_mat_entry(flintobj,0,0));
	fmpq_one(fmpq_mat_entry(flintobj,0,seedvar));
	fmpq_mat_init(testvtx,1,lrsmtx->c); 
	fprintf(stderr,"\n an LP was solved");
	solvelp_withqsopt(objval,optvtx,flintobj,projdimreal,QS_MAX,&prob);
	copy_submat2submat(testvtx,1,1,optvtx,1,1,1,projdimreal+1);

	if(isnewvtx(testvtx,Vset,vnum[0],projdimreal))
	{
		copy_optvtx2Vset(Vset,testvtx,vnum);
	}
	
	//second vertex 
	fprintf(stderr,"\n an LP was solved");	
	solvelp_withqsopt(objval,optvtx,flintobj,projdimreal,QS_MIN,&prob);
	copy_submat2submat(testvtx,1,1,optvtx,1,1,1,projdimreal+1);
	if(isnewvtx(testvtx,Vset,vnum[0],projdimreal))
	{
		copy_optvtx2Vset(Vset,testvtx,vnum);
	}
	
	while(vnum[0]<projdimreal+1)
	{
	
	Vset2diffmat(diffmat,Vset,vnum[0]);
	nullspace2(U,diffmat);
	copyrow(flintobj,0,U,0);
	fmpq_mat_clear(U);
	fmpq_mat_clear(diffmat);
	fprintf(stderr,"\n an LP was solved");
	solvelp_withqsopt(objval,optvtx,flintobj,projdimreal,QS_MAX,&prob);
	copy_submat2submat(testvtx,1,1,optvtx,1,1,1,projdimreal+1);

	if(isnewvtx(testvtx,Vset,vnum[0],projdimreal)==1)
	{
		copy_optvtx2Vset(Vset,testvtx,vnum);
	}
	if(vnum[0]>=projdimreal+1)
	{
		break;
	}
	fprintf(stderr,"\n an LP was solved");
	solvelp_withqsopt(objval,optvtx,flintobj,projdimreal,QS_MIN,&prob);
	copy_submat2submat(testvtx,1,1,optvtx,1,1,1,projdimreal+1);
	if(isnewvtx(testvtx,Vset,vnum[0],projdimreal)==1)
	{	
		copy_optvtx2Vset(Vset,testvtx,vnum);
	}
	}

	fmpq_mat_init(extrays,Vset->r,projdimreal+1);
	copy_submat2submat(extrays,1,1,Vset,1,Vset->r,1,projdimreal+1);
	if(prob) mpq_QSfree_prob(prob);
	free(vnum);
}

char* chm_ubpreproc(fmpq_mat_t Y,fmpq_mat_t projeqmtx, char* filename,int* projdimptr)
//writes a file with name filenamebounded.ine correnponding to the filename provided(provided filename already has .ine at the end)
{//corefunction
	printf("\nUnbounded preprocessing...");
	char*forewrd;
	struct intarray* projinfo;
	int projdim=projdimptr[0];
	int rowsize, colsize;
	FILE *fp; 
	int m,n,i,j;
	mpq_t z;
	fmpq_t q;
	fmpq_init(q);
	fmpq_mat_t X;
	int nlin=0;
	int* linrows=NULL;
	forewrd=readlrsfile2(filename,X,&nlin,&linrows);
	char *filename2=NULL;
    filename2=malloc((strlen(filename)+8)*sizeof(char));
    filename2[0]='\0';
    //strncpy(filename2,filename,strlen(filename)-4);
    for(i=0;i<strlen(filename)-4;i++)
    {
		filename2[i]=filename[i];
	}
	filename2[i]='p';
	i++;
	filename2[i]='r';
	i++;
	filename2[i]='o';
	i++;
	filename2[i]='c';
	i++;
	filename2[i]='.';
	i++;
	filename2[i]='i';
	i++;
	filename2[i]='n';
	i++;
	filename2[i]='e';
	i++;
	filename2[i]='\0';
 	projinfo=chm_preprocess(projeqmtx,X,nlin,linrows,projdim); 
	projdimptr[0]=projdim-projinfo->len;
	//	fmpq_mat_print(Y);
	//strcat(filename2,"proc.ine");
    writelrsfile(filename2,"h",X,0,NULL,forewrd,NULL);
    if(nlin>0)
    free(linrows);
    free(forewrd);
    return filename2;
}

char* redund_naive_hfilewrt(char* filename)
{
	//corefunction
	// Used inside chm
	// write a redundancy removed file with name <filename>rdd.ine  
	char* forewrd;
	printf("\nRempving input redundancies");
	int i,j,k,newlin=0,*newlinarray=NULL;
	char*filename2;
	k=strlen(filename);
	filename2=(char*)malloc((k+5)*sizeof(char));
	for(i=0;i<k-4;i++)
	{
		filename2[i]=filename[i];
	}
	filename2[i]='r';
	i++;
	filename2[i]='d';
	i++;
	filename2[i]='d';
	i++;
	filename2[i]='.';
	i++;
	filename2[i]='i';
	i++;
	filename2[i]='n';
	i++;
	filename2[i]='e';
	i++;
	filename2[i]='\0';
	fmpq_mat_t lrsmtx_new,outmtx;
	char*prepend=NULL,*append=NULL,*strrow=NULL;
	struct intarray* redarr;
	int*linrows=NULL;
	int nlin=0;
	forewrd=readlrsfile2(filename,outmtx,&nlin,&linrows);
	redarr=redund_naive(outmtx,"h",nlin,linrows);
	//printf("\n%d rows redundant\n",redarr->len);
	if(redarr->len>0)
	{
	prepend=malloc(50*(redarr->len)*sizeof(char));
	prepend[0]='\0';
	}
	else
	{
		prepend=malloc(sizeof(char));
		prepend[0]='\0';
	}
	
	prepend=strcat(forewrd,prepend);
	append=malloc(130*sizeof(char));
	strrow=malloc(70*sizeof(char));
	strrow[0]='\0';
	append[0]='\0';
	printf("\nlinrows:::::");
	for(i=0;i<nlin;i++)
	{
		printf("%d\t",linrows[i]);
	}
	for(i=0;i<redarr->len;i++)
	{
		if(ismember2(linrows,nlin,redarr->array[i]+1)>=0)// redundant row was infact a linearity 
		{
			sprintf(strrow,"*linearity in row %d was dependent and was removed\n",redarr->array[i]+1);
			strcat(prepend,strrow);			
		}
		else
		{
			sprintf(strrow,"*row %d was redundant and removed\n",redarr->array[i]+1);
			strcat(prepend,strrow);
		}
	}
	k=0;
	if(nlin>0)
	{
		newlinarray=malloc(nlin*sizeof(int));// new linearity array
	}
	else
	{
		newlinarray=NULL;
	}
	fmpq_mat_init(lrsmtx_new,outmtx->r-redarr->len,outmtx->c);// new lrs mtx
	for(i=0;i<outmtx->r;i++)
	{
		if(ismember2(redarr->array,redarr->len,i)<0)// row is not redundant
		{
			copyrow(lrsmtx_new,k,outmtx,i);
			k++;
		}
		if(ismember2(linrows,nlin,i+1)>=0)// current row was a linearity
		{
			if(ismember2(redarr->array,redarr->len,i)<0)// and its isn't redundant(dependent)
			{
				newlinarray[newlin]=k;
				newlin++;
			}
		} 
	}
	//printf("\nnewlinrows:::::");
	//for(i=0;i<newlin;i++)
	//{
	//	printf("%d\t",newlinarray[i]);
	//}
	fmpq_mat_clear(outmtx);
	sprintf(append,"*chm input had %ld rows and %ld columns: %d row(s) redundant\n*chm:chmlib v.0.3 2013.7.23 Jayant Apte",outmtx->r,outmtx->c,redarr->len);
	writelrsfile(filename2, "h" ,lrsmtx_new, newlin,newlinarray, prepend,append);
	free(forewrd);
	return filename2;
}


struct isfacet_returnset* isfacet2(fmpq_mat_t fcandidate,mpq_QSprob* probptr,int projdim) 
{//corefunction
	struct isfacet_returnset* facetinfo;
	facetinfo=malloc(sizeof(struct isfacet_returnset));
	fmpz_t den_common;
	fmpq_mat_t common_den_obj;
	fmpq_mat_t integer_obj;
	fmpz_mat_t temp_fmpzobj;
	fmpq_mat_t nolrs_optvtx;
	fmpq_mat_t no_b_obj;
	fmpq_t isfacet;
	fmpq_t objval;
	fmpq_mat_t optvtx;
	fmpq_init(objval);
	fprintf(stderr,"\n an LP was solved");
	solvelp_withqsopt(objval,optvtx,fcandidate,projdim-1,QS_MIN,probptr);
	//solvelp_withlrs(objval,optvtx,P,Q,fcandidate,projdim,MINIMIZE);
	//printf("\nobjval(isfacet)\t");
//	fmpq_print(objval);
	//printf("\noptvtx(isfacet)\t");
	//fmpq_mat_print(optvtx);
	fmpq_mat_init(facetinfo->ineqcon,optvtx->r,optvtx->c);
	fmpq_mat_set(facetinfo->ineqcon,optvtx);
	//printf("\n here comes the optvtx::>  ");
	//fmpq_mat_print(optvtx); 
	
	fmpz_mat_init(temp_fmpzobj,fcandidate->r,fcandidate->c);
	fmpq_mat_init(integer_obj,fcandidate->r,fcandidate->c);
	fmpq_mat_init(common_den_obj,fcandidate->r,fcandidate->c);
	fmpq_mat_get_fmpz_mat_matwise ( temp_fmpzobj , den_common , fcandidate);
	fmpq_mat_set_fmpz_mat_div_fmpz ( common_den_obj , temp_fmpzobj , den_common);
	fmpq_mat_scalar_mul_fmpz ( integer_obj, common_den_obj , den_common);
	fmpq_mat_submatrix(no_b_obj,integer_obj,1,1,2,integer_obj->c);// take the '-a' part of b-ax>=0
	fmpq_mat_submatrix(nolrs_optvtx,optvtx,1,1,2,optvtx->c);
	dotrr(objval,nolrs_optvtx,no_b_obj);
	fmpq_add(isfacet,fmpq_mat_entry(integer_obj,0,0),objval);
	//printf("\nthe difference between b and obj*optvtx\n");
	//fmpq_print(isfacet);
	facetinfo->isfacet=fmpq_sgn(isfacet);
	fmpq_mat_clear(integer_obj);
	fmpq_mat_clear(common_den_obj);
	fmpq_mat_clear(optvtx);
	return facetinfo;
}





struct diminfo* finddim_polyhedron2(fmpq_mat_t lrsmtx,int nlin, int* linrows,char* type)
/* Follows techniques in http://stat.ethz.ch/ifor/teaching/lectures/poly_comp_ss11/lecture8‎ by K. Fukuda
 * For H-polyhedron, assumes that equalities provided at the start are linearly independent
 * which means one should run redundancy removal beforehand*/
{//corefunction
	int dim,newnlin,*newlinrows;
	struct diminfo* dimension=malloc(sizeof(struct diminfo));
	struct intarray* linearities=malloc(sizeof(struct intarray));
	mpq_QSprob prob;
	int rval;
	char type1[1]="v";
	int i,num_lin,k,r;
	int j,rank,rank1,l;
	int* linindices;	
	fmpq_mat_t lrsmtx2,testmat_dep,rreftestmat_dep,row,lrsmtx3;
	fmpq_mat_t flintobj,optvtx,optvtx_p,optvtx_d;
	fmpq_t objval;
	int lpcount=1;
	if(strcmp(type,type1)==0)// v-polyhedron
	{
		//printf("here1");
	//	fmpq_mat_print(lrsmtx);
		fmpq_mat_t rreflrsmtxT;
		fmpq_mat_t lrsmtxT;
		fmpq_mat_init(lrsmtxT,lrsmtx->c,lrsmtx->r);
		fmpq_mat_transpose(lrsmtxT,lrsmtx);
		fmpq_mat_init(rreflrsmtxT,lrsmtx->c,lrsmtx->r);
		dim=fmpq_mat_rref(rreflrsmtxT,lrsmtxT)-1;
		//fmpq_mat_print(rreflrsmtxT);
		linearities->len=0;
		linearities->array=(int*)NULL;
		dimension->linearities=linearities;
		dimension->dim=dim;
		fmpq_mat_clear(lrsmtxT);
		fmpq_mat_clear(rreflrsmtxT);
		return;
	}
	else // h-polyhedron
	{
		/*Solve following problem:
		 * max x_0
		 * s.t. 
		 * 		b+Ax-x_0 >= 0
		 * 		1   -x_0 >= 0*/
	//printf("Original Linrows: %d\n",nlin);
	//for(i=0;i<nlin;i++)
	//{
		//printf("%d \t",linrows[i]);
	//}
		fmpq_mat_init(lrsmtx2,lrsmtx->r+1,lrsmtx->c+1);
		for(i=0;i<lrsmtx->r;i++)// copy lrsmtx into lrsmtx2
		{
			for(j=0;j<lrsmtx->c;j++)
			{
				fmpq_set(fmpq_mat_entry(lrsmtx2,i,j),fmpq_mat_entry(lrsmtx,i,j));
			}
		}
		fmpq_one(fmpq_mat_entry(lrsmtx2,lrsmtx2->r-1,0)); //1-x_0>=0 
		for(i=0;i<lrsmtx2->r;i++)
		{
			if(ismember2(linrows,nlin,i+1)==-1)// add x_0 to a row obly if it is an inequality
			{
				fmpq_one(fmpq_mat_entry(lrsmtx2,i,lrsmtx2->c-1));
				fmpq_neg(fmpq_mat_entry(lrsmtx2,i,lrsmtx2->c-1),fmpq_mat_entry(lrsmtx2,i,lrsmtx2->c-1));
			}
		}
		rval=qsload_fromlrsmatrix(lrsmtx2,nlin,linrows, &prob);
		//mpq_QSwrite_prob (prob, "logfile1.lp","LP");
		////printf("rval= %d",rval);
        ////fmpq_mat_print(lrsmtx2);
		fmpq_mat_init(flintobj,1,lrsmtx2->c);
		fmpq_one(fmpq_mat_entry(flintobj,0,0));
		fmpq_one(fmpq_mat_entry(flintobj,0,flintobj->c-1));
		//printf("\nstatus before solving:%d\n",prob->qstatus);
		solvelp_withqsopt(objval, optvtx, flintobj,lrsmtx2->c-1,QS_MAX,&prob);
		//printf("objval_p");
		//fmpq_print(objval);
		////printf("optvtx_p, solve number %d\n",lpcount);
		////fmpq_mat_print(optvtx);
		if(fmpq_sgn(objval)>0)
		{
			//printf("\n Full-dimentional polyhedron\n")	;
			linearities->array=linrows;
			linearities->len=nlin;
			dimension->linearities=linearities;
			dimension->dim=lrsmtx->c-1-nlin;
			if(prob)  mpq_QSfree_prob(prob);
			fmpq_mat_clear(lrsmtx2);
			//fmpq_mat_clear(lrsmtx3);
			return dimension;// Full-dimentional polyhedron
		}
		else if(fmpq_sgn(objval)<0)
		{
			//printf("empty!");
			linearities->array=linrows;
			linearities->len=nlin;
			dimension->linearities=linearities;
			dimension->dim=0;
			if(prob)  mpq_QSfree_prob(prob);
			fmpq_mat_clear(lrsmtx2);
			return dimension;//empty polyhedron
		}
		else // dimension deficient polyhedron
		{
			linindices=malloc(lrsmtx2->r*sizeof(int));
			num_lin=nlin;
			for(i=0;i<nlin;i++)
			{
				linindices[i]=linrows[i];
			}
			while(1)
			{
			solvelp_withqsopt_dual(objval, optvtx, flintobj,lrsmtx2->c-1,QS_MAX,&prob);
			lpcount++;
			//printf("Dimension finder LP solve number %d\n",lpcount);
			//printf("Dual lp solution to identify equalities:\n ");
			//fmpq_mat_print(optvtx);
			//printf("optvtx is \t");
			//fmpq_print(objval);
			// find indices in dual optimal solution with >0 values
			//printf("\nequality indices before adding new equalities:\t");
			//for(i=0;i<num_lin;i++)
			//{
				//printf("%d\t",linindices[i]);
			//}
			for(i=1;i<optvtx->c-2;i++)
			/* first column of optvtx is dummy(lrs format), last column corresponds to 1-x_0>=, rest of the columns indicate equality 
			 * in corresponding row of lrsmtx2 if the value is >0 */
			{
				
				if(ismember2(linindices,num_lin,i)==-1)
				{
					if(fmpq_sgn(fmpq_mat_entry(optvtx,0,i))>0)
					{
					
						linindices[num_lin]=i;// linindices stores row indices of lrsmtx3 in C sense
						//printf("index %d in lrsmtx is equality",i-1);
						num_lin++;
					}
				}
			}
			//printf("\nequality indices after solving the dual : %d>>\t", num_lin);
			//for(i=0;i<num_lin;i++)//copy known equalities into testmat_dep
			//{
				//printf("%d\t",linindices[i]);
			//}
		

			//// identify any dependent inequalities 
			////fmpq_mat_print(lrsmtx3);
			rank1=find_depindices(lrsmtx,&linindices,&num_lin);

			//printf("\nExtended equality indices: %d >>\t",num_lin);
			//for(i=0;i<num_lin;i++)//copy known equalities into testmat_dep
			//{
				//printf("%d\t",linindices[i]);
			//}
		
			///* modify lrsmtx2 to the following 
			 //* b_C- A_Cx       = 0
			 //* b_D- A_Dx -x_0 >= 0  
			 //* 1         -x_0 >= 0 
			 //* where C=linrowindices and D is rest of indices*/
			for(i=0;i<lrsmtx2->r-1;i++)
			{
				// see if i+1 is in linindices
				k=ismember2(linindices,num_lin,i+1);	
				if(k!=-1)
				{
					// i is present in linindices, we must modify corresponding row of lrsmtx2
					fmpq_zero(fmpq_mat_entry(lrsmtx2,i,lrsmtx2->c-1));
				}	
			}
			////fmpq_mat_print(lrsmtx2);
			mpq_QSfree_prob(prob);
			rval=qsload_fromlrsmatrix(lrsmtx2,num_lin,linindices, &prob);
			//mpq_QSwrite_prob (prob, "logfile3.lp","LP");
			// solve the new primal problem now
			solvelp_withqsopt(objval, optvtx, flintobj,lrsmtx2->c-1,QS_MAX,&prob);
			//printf("\nprimal solution\t ");
			//fmpq_print(objval);
			if(fmpq_sgn(objval)>0)
			{
		
				//printf("\nequality indices aPROVIDED:\n");
				//for(i=0;i<nlin;i++)//copy known equalities into testmat_dep
				//{
					//printf("%d\t",linrows[i]);
				//}
				//printf("\nCompare: %d vs %ld",r,lrsmtx3->r);
				linearities->array=linindices;
				linearities->len=num_lin;
				dimension->linearities=linearities;
				dimension->dim=lrsmtx->c-1-rank1;
				if(prob)  mpq_QSfree_prob(prob);
				fmpq_mat_clear(lrsmtx2);
				return dimension;// Non-fulldimensional polyhedron
			}
			//printf("\nequality indices after setting up modified problem:\t");
			//for(i=0;i<num_lin;i++)//copy known equalities into testmat_dep
			//{
				//printf("%d\t",linindices[i]);
			//}
			}
		}
	}
}




void lrsmtxcone_scale2integers(fmpq_mat_t intlrsmtxcone,fmpq_mat_t lrsmtxcone)
{//corefunction
	fmpz_mat_t zlrsmtxcone;
	fmpz_mat_init(zlrsmtxcone,lrsmtxcone->r,lrsmtxcone->c);
	fmpq_mat_init(intlrsmtxcone,lrsmtxcone->r,lrsmtxcone->c);
	fmpq_mat_get_fmpz_mat_rowwise ( zlrsmtxcone , NULL ,lrsmtxcone);
	fmpq_mat_set_fmpz_mat ( intlrsmtxcone , zlrsmtxcone);
}

int intarray_findmax(int* array,int len)
{
	int i,max;
	
	if(len>0 && array!=NULL)
	{
		max=array[0];
		for(i=1;i<len;i++)
		{
			if(array[i]>max)
			{
				max=array[i];
			}
		}
		return max;
	}
	else
	{
		return 1;
	}
}



int adjtest_algebraic3(struct raylist* currentray, struct raylist* adjcandidate, fmpq_mat_t Hset, int dim)
{//corefunction
	int i,j,common_count,temp_ineqindex,*intersection,hnum1,hnum2,rank;
	set_t intersectionset;
	fmpq_mat_t temp_intersection;
	fmpq_mat_t mat_intersection;
	fmpq_mat_t mat_intersection_echelon;
	hnum1=currentray->hnum;
	hnum2=adjcandidate->hnum;
	//printf("\nadjacency test between %d and %d \n",currentray->rayindex,adjcandidate->rayindex);
	//printf("\n The hnums are: %d & %d",hnum1,hnum2);
	setops_intersection(intersectionset,currentray->tightset,adjcandidate->tightset);
	common_count=setops_cardinality(intersectionset);
	if(common_count<dim-2)
	{	
		setops_clear(intersectionset);
		return 0; // there are less than dim-2 common tight inequalities. Get out of here!
	}
	else 
	{
		intersection=setops_set2intarray1(intersectionset,common_count);
		fmpq_mat_init(mat_intersection,common_count,Hset->c);
		fmpq_mat_init(mat_intersection_echelon,common_count,Hset->c);
		for(i=0;i<common_count;i++)              // fill inside mat_intersection
		{		
			//printf("intersection row number: %d",intersection[i]);
			fmpq_mat_submatrix(temp_intersection,Hset,intersection[i],intersection[i],1,Hset->c);
			for (j=0;j<Hset->c;j++)
			{
				fmpq_set(fmpq_mat_entry(mat_intersection,i,j),fmpq_mat_entry(temp_intersection,0,j));
			}
			fmpq_mat_clear(temp_intersection);
		}
		rank=fmpq_mat_rref(mat_intersection_echelon,mat_intersection);
		if (rank!=dim-2)
		{
			free(intersection);
			fmpq_mat_clear(mat_intersection);
			//fmpq_mat_clear(temp_intersection);
			fmpq_mat_clear(mat_intersection_echelon);
			return 0;
		}
		else
		{
			free(intersection);
			fmpq_mat_clear(mat_intersection_echelon);
			fmpq_mat_clear(mat_intersection);
			//fmpq_mat_clear(temp_intersection);
			return 1;  // rays are adjacent!
		}
	}
}

char* readlrsfile2(const char* filename,fmpq_mat_t X, int*nlin,int** linrowsptr)
{
	//corefunction
	FILE *fp; 
	int *linrows;
	int m,n,i,j,firsttime,skip,num,inccount=0;
	char* prepend=malloc(STR_LEN_INC*sizeof(char));
	int fore=1;
	prepend[0]='\0';
	char tempstr[15];
	nlin[0]=0;
	mpq_t z;
	fmpq_t q;
	fmpq_init(q);
	fmpq_mat_t Y;
	char* linptr;
	const char* begin="begin";
	char buf[4096]; 
	buf[0]='\0';
	char* str;
	fp=fopen(filename,"r");
	firsttime=FALSE;
    while(fp!=NULL && fgets(buf, sizeof(buf), fp)!=NULL)
    {
		linptr=strstr(buf, "linearity") ;
		if (linptr !=NULL && firsttime==FALSE && strstr(buf,"*linearity")==NULL)// last condition ensures this loop es not evoked by *linearity 
		{
			linptr=linptr+9;
			sscanf ( linptr, "%d", &num);
			//printf("\nNUmber of equalities: %d\n",num);
			nlin[0]=num;
			sprintf(tempstr, "%d", num);
			linptr=strstr(buf,tempstr)+strlen(tempstr);
			while(linptr[0]==9 || linptr[0]==32)
			{
				linptr=linptr+1;
			}
			linrows=(int*)malloc((nlin[0]+1)*sizeof(int));
			for (i=0;i<nlin[0];i++)
			{
				sscanf(linptr,"%d",&linrows[i]);
				//printf("\nfound number %d\n",linrows[i]);
				sprintf(tempstr, "%d", linrows[i]);
				linptr=linptr+strlen(tempstr);
				while(linptr[0]==9 || linptr[0]==32)
				{
				linptr=linptr+1;
				}
			}
			firsttime=TRUE;
			fore=0;
		}
		if(fore==1 && buf[0]!='*' && strstr(buf, "begin")==NULL)
		{
			prepend=strcat(prepend,buf);
		}
        if (strstr(buf, "begin"))
        {
        //printf("begin found!");
        gmp_fscanf(fp,"%d%d",&m,&n);
        //printf("size %d %d",m,n);
        fmpq_mat_init ( X , m, n);
		fmpq_mat_zero ( X);
		do// skip empty lines
		{
		fgets(buf, sizeof(buf), fp);
		}
		while(strlen(buf)==0);
		mpq_init(z);
		for(i=0;i<m;i++)
		{
			for(j=0;j<n;j++)
			{
				gmp_fscanf(fp,"%Qi",z);		
				fmpq_set_mpq(fmpq_mat_entry(X,i,j),z);
			} 
			do// skip empty lines
			{
				fgets(buf, sizeof(buf), fp);
			}
			while(strlen(buf)==0);
		}
		mpq_clear(z);
		break;
		}
    }
    if(fp!=NULL) 
    fclose(fp);
    *linrowsptr=linrows;
    return prepend;
}




struct insineqcon_returnset* insineqcon_withadj3(fmpq_mat_t ineqcon,struct raylist* head,int lastrayindex,fmpq_mat_t Hset,int dim)
{//corefunction
	/*This function creates nlist, plist and zlist which are sub-lists of raylist. Then it would go through nlist and combining rays in nlist with 
	 * those in plist if they happen to be adjacent. No previous adjacency information is needed*/
	int i,zhead=0,phead=0,nhead=0,newraycount=0,adjcount=0,old_rnum;
	//struct adjlist* temp_adjlist;
	struct raylist* temp_raylist, *temp_nray,*tempray,*temp_plist,*temp_nlist,*temp_zlist,*head_plist,*head_nlist,*head_zlist;
	struct insineqcon_returnset* returnset;
	returnset=malloc(sizeof(struct insineqcon_returnset));
	fmpq_mat_t newRay;
	fmpq_init(head->score);
	dotrr(head->score,ineqcon, head->ray); // dot product of head ray and ineqcon
	//fmpq_print(head->score);// score of head ray
	if(fmpq_sgn(head->score)<0)//head is negative
	{
		head->flag2=-1;
		head_nlist=head;
		temp_nlist=head;
		nhead=1;// head found for nlist
	}
	else if (fmpq_sgn(head->score)==0)
	{
		head->flag2=0;
		head_zlist=head;
		temp_zlist=head;
		zhead=1;// head found for zlist
		//updatetightlist(head,Hset->r);
		setops_insert_ul(head->tightset,Hset->r);
	}
	else 
	{
		head->flag2=1;
		phead=1;// head found for plist
		head_plist=head;
		temp_plist=head;
	}
	
	temp_raylist=head->nextray; // go to next ray
	while(temp_raylist!=head) // loop over rest of the raylist calculating the score
	{
		//printf("HERE YAY!");
		dotrr(temp_raylist->score,ineqcon,temp_raylist->ray);
		if (fmpq_sgn(temp_raylist->score)==0)
		{
			temp_raylist->flag2=0;
			if(!zhead)// first ever zero ray
			{
				head_zlist=temp_raylist;
				temp_zlist=temp_raylist;
				zhead=1;
			} 
			else // not first ever zero ray
			{
				temp_zlist->nexts=temp_raylist;
				temp_raylist->prevs=temp_zlist;
				temp_zlist=temp_raylist;
			}
			setops_insert_ul(temp_raylist->tightset,Hset->r);
			//updatetightlist(temp_raylist,Hset->r);
		}
		else if((fmpq_sgn(temp_raylist->score)>0))
		{
			temp_raylist->flag2=1;
			if(!phead)// first ever positive ray
			{
				head_plist=temp_raylist;
				temp_plist=temp_raylist;
				phead=1;
			} 
			else // not first ever positive ray
			{
				temp_plist->nexts=temp_raylist;
				temp_raylist->prevs=temp_plist;
				temp_plist=temp_raylist;
			}
		}
		else
		{
			temp_raylist->flag2=-1;
			if(!nhead)// first ever negative ray
			{
				head_nlist=temp_raylist;
				temp_nlist=temp_raylist;
				nhead=1;
			} 
			else // not first ever negative ray
			{
				temp_nlist->nexts=temp_raylist;
				temp_raylist->prevs=temp_nlist;
				temp_nlist=temp_raylist;
			}
		}
		//printf("\n");
		//fmpq_print(temp_raylist->score);
		//printf("\n");
		temp_raylist=temp_raylist->nextray;
	}
	// tie the loose ends of positive,negative and zero sub-raylists
	if(zhead)// there exists at least 1 zero ray
	{
		temp_zlist->nexts=head_zlist;
		head_zlist->prevs=temp_zlist;
		//printf("\nzlist");
		//print_slist(head_zlist);
	}
	if(phead)// there exists at least 1 positive ray
	{
		temp_plist->nexts=head_plist;
		head_plist->prevs=temp_plist;
		//printf("\nplist");
		//print_slist(head_plist);
		
	}
	//fprintf(stderr,"\n nhead is %d",nhead);
	if(nhead)// there exists at least 1 negative ray 
	{
		temp_nlist->nexts=head_nlist;
		head_nlist->prevs=temp_nlist;
		//printf("\nnlist");
		//print_slist(head_nlist);
	}
	
		temp_raylist=head;
		if(fmpq_sgn(head->score)<0)// head is -ve. find another one
		{
			while(fmpq_sgn(temp_raylist->score)<0)
			{
				temp_raylist=temp_raylist->nextray;
			}
			head=temp_raylist;// we have a new non-ve head now
		}
		
		temp_raylist=head;
	//printf("\nbefore fixing zeros\n");
	//print_all_adj(head);
	//zerorays_fixadj(head ,Hset ,dim );
	//printf("\nafter fixing zeros\n");
	//print_all_adj(head);
	
	/*Now, for every negative ray in nlist we go through plist and combine it with +ve rays in there and add the new rays to raylist*/
	temp_raylist=head;
	temp_plist=head_plist;
	temp_nlist=head_nlist;
	//fprintf(stderr,"\nStarted looking for combining positive and negative rays\n %ld positive and %ld negative rays",slist_numrays(head_plist),slist_numrays(head_nlist));
	while(1)
	{
		temp_plist=head_plist;
		while(1)
		{
			if(adjtest_algebraic3(temp_plist,temp_nlist,Hset,dim)!=0)
			{
				compute_newray(newRay,temp_plist->ray,temp_nlist->ray,ineqcon);// note that order of rays is important
				newray_scaledown(newRay);
				newray_add2raylist2(newRay,head,Hset,lastrayindex,dim);
				lastrayindex++;
			}
			temp_plist=temp_plist->nexts;
			if(temp_plist==head_plist)
			{
				break;
			}
		}	
		temp_nlist=temp_nlist->nexts;
		if(temp_nlist==head_nlist)
		{
			break;
		}
	}
	temp_nlist=head_nlist;
	//fprintf(stdout,"Now removing negative rays");
	while(1)
	{
		tempray=temp_nlist->nexts;
		
		//fprintf(stdout,"removing ray %d",temp_nlist->rayindex );
		remove_nray2(temp_nlist);
		temp_nlist=tempray;
		if(temp_nlist==head_nlist)
		{
			break;
		}
	}
	returnset->head=head;
	returnset->lastindex=lastrayindex;
	return returnset;
}


struct raylist*  chm_bounded4(char* filename,fmpq_mat_t lrsmtx,fmpq_mat_t Vset, int projdimreal)
{//corefunction
/*IMPORTANT: make sure global variables argc2 and argv2 are set right before using this function*/
/*****************************LRS BEGIN************************************/

	long i,j,itcnt=0;
	long m;       /* number of constraints in the problem          */
	long n;       /* number of variables in the problem + 1        */
	long col;	/* output column index for dictionary            */
  
/* Global initialization - done once */
/***************************CHM BEGIN******************************************/
fmpq_mat_t returnVset;// Vset without the ones at the start. For the purpose of returning
mpq_QSprob prob;
fmpq_mat_t qslrsmtx;
qsload_fromlrsfile(qslrsmtx,filename,&prob);
//int projdimreal=2;
int projdim=projdimreal+1;
int * vnum;
vnum= malloc(sizeof(int));
vnum[0]=0;
fmpq_mat_t Hrep,Rset,Hset;
//fmpq_mat_init(Vset,100,projdim);
mpq_QSprob* probptr=&prob;
findinitialhull8(Vset,lrsmtx,projdimreal);
fmpq_mat_init(Rset,Vset->c,Vset->r); // vset is vertices in lrs format. since it already has a 1 in first column, we neednot homogenize seperately
fmpq_mat_transpose(Rset,Vset);// Rset  is transpose of Vset
printf("\n Projdim passed to initialhull is is is: %d",projdim);
printf("\nlrsmtx passed:::");
fmpq_mat_print(lrsmtx);
printf("\nInitial cone rays:::");
fmpq_mat_print(Rset);
initialHrep(Hrep,Rset);
//create the raylist in the polar space
struct raylist* head, *currentray,* adjcandidate,*temp_raylist;
struct isfacet_returnset* facetinfo=NULL;
struct insineqcon_returnset *returnset;
int* tightset;
head=create_new_raylist(Hrep,Vset); // Note that H and V have been exchanged. Behold! We are in polar space now!
fmpq_mat_t fcandidate;
fmpq_mat_print(Vset);
fmpq_mat_print(Hrep);
int terminal=0;
int lastindex=projdimreal;
while(1)
{
	printf("\n\nraylist has %ld rays\n\n",raylist_numrays(head));
	temp_raylist=head;
	while(1)
	{
		
		if(temp_raylist->flag1==0)//potentially nonterminal facet
		{
			facetinfo=isfacet2(temp_raylist->ray,probptr,projdim);
			temp_raylist=temp_raylist->nextray;
			if((facetinfo->isfacet)<0) // found a nonterminal facet
			{
				//{
				//fmpq_mat_print(temp_raylist->prevray->ray);
				//}
				break;
			}
			else //new terminal facet found
			{
				temp_raylist->prevray->flag1=1;
				facetinfo=NULL;
			}
		}
		else // terminal facet
		{
			temp_raylist=temp_raylist->nextray;
		}
		if(temp_raylist==head)
		{
			terminal=1;
			break;
		}
		
	}
	if(terminal!=1)
	{
		//print_raylist(head);
		printf("\nNonterminal facet number %ld\n",itcnt);
		updatehset(Vset,facetinfo->ineqcon);
		//fprintf(stderr,"\nUpdated hset");
		returnset= insineqcon_withadj3(facetinfo->ineqcon,head ,lastindex,Vset,projdim);
		//fprintf(stderr,"\nInserted new constraint");
		head=returnset->head;
		lastindex=returnset->lastindex;
		free(returnset);
		free(facetinfo);
		itcnt++;
	}
	else
	{
		break;
	}
}
//fmpq_mat_init(returnVset,Vset->r,Vset->c-1);
fprintf(stderr,"Done with bounded chm stats: %ld facets %ld vertices",raylist_numrays(head),Vset->r);
free(vnum);
if(prob)  mpq_QSfree_prob(prob);
fmpq_mat_clear(qslrsmtx);
return head;
}





void chmio_polyhedron(polyhedron_t poly, char* lrsfile)
{
	
}

void copy_optvtx2Vset(fmpq_mat_t Vset,fmpq_mat_t optvtx,int* vnum)
{//corefunction
	int i,j;
	fmpq_mat_t Vset1;
	if(vnum[0]>0)
	{
		fmpq_mat_init(Vset1,Vset->r+1,Vset->c);
	}
	else
	{
		fmpq_mat_init(Vset1,1,optvtx->c);
	}
	for(i=0;i<Vset->r;i++)
	{
		for(j=0;j<Vset->c;j++)
		{
			fmpq_set(fmpq_mat_entry(Vset1,i,j),fmpq_mat_entry(Vset,i,j));
		}
	}
	for(i=0;i<Vset->c;i++)
	{
		fmpq_set(fmpq_mat_entry(Vset1,Vset1->r-1,i),fmpq_mat_entry(optvtx,0,i));
	}
	vnum[0]=vnum[0]+1;
	fmpq_mat_clear(Vset);
	fmpq_mat_init(Vset,Vset1->r,Vset1->c);
	fmpq_mat_set(Vset,Vset1);
	fmpq_mat_clear(Vset1);
}

void compute_newray(fmpq_mat_t newRay,fmpq_mat_t pray,fmpq_mat_t nray,fmpq_mat_t ineqcon)
{//corefunction
	fmpq_t x1,x2;
	fmpq_mat_t temp_pray,temp_nray,temp_newRay;
	fmpq_mat_init(temp_pray,pray->r,pray->c);
	fmpq_mat_init(temp_nray,nray->r,nray->c);
	fmpq_mat_set(temp_nray,nray);
	fmpq_mat_set(temp_pray,pray);
	fmpq_mat_init(newRay,pray->r,pray->c);
	dotrr(x1,ineqcon,temp_pray);
	dotrr(x2,ineqcon,temp_nray);
	mul_mat_qscalar(temp_pray,x2,temp_pray);
	mul_mat_qscalar(temp_nray,x1,temp_nray);
	fmpq_mat_sub ( newRay ,temp_nray ,temp_pray);
}

void updatehset(fmpq_mat_t Hset,fmpq_mat_t ineqcon)
{//corefunction
	int i,j;
	fmpq_mat_t newHset;
	fmpq_mat_init(newHset,(Hset->r)+1,(Hset->c));
	for(i=0;i<newHset->r-1;i++)
	{
		for(j=0;j<newHset->c;j++)
		{
			fmpq_set(fmpq_mat_entry(newHset,i,j),fmpq_mat_entry(Hset,i,j));
		}
	}
	for(j=0;j<newHset->c;j++)
		{
			fmpq_set(fmpq_mat_entry(newHset,(newHset->r)-1,j),fmpq_mat_entry(ineqcon,0,j));
		}
		fmpq_mat_clear(Hset);
	fmpq_mat_init(Hset,newHset->r,newHset->c);
	fmpq_mat_set(Hset,newHset);
	fmpq_mat_clear(newHset);
}

void newray_scaledown(fmpq_mat_t newRay)
{//corefunction
	int i;
	fmpq_t scalefactor;
	fmpq_init(scalefactor);
	fmpq_one(scalefactor);
	for(i=0;i<newRay->c;i++)
	{
		fmpq_canonicalise( fmpq_mat_entry(newRay,0,i));
	}
	for(i=0;i<newRay->c;i++)
	{
		if(fmpq_sgn(fmpq_mat_entry(newRay,0,i))!=0)
		{
			break;
		}
	}
	if(i<=newRay->c)
	{
	fmpq_set(scalefactor,fmpq_mat_entry(newRay,0,i));
	fmpq_abs ( scalefactor , scalefactor);
	}
	
	//printf("\nscalefactor\n");
	//fmpq_print(scalefactor);
	for(i=0;i<newRay->c;i++)
	{
		fmpq_div ( fmpq_mat_entry(newRay,0,i) , fmpq_mat_entry(newRay,0,i) , scalefactor);
	}
}

void redund_polyhedron(polyhedron_t outpoly, polyhedron_t inpoly)
{
	//corefunction
	// Used inside chm
	// write a redundancy removed file with name <filename>rdd.ine  
	int i,j,k,newlin=0,*newlinarray=NULL;
	fmpq_mat_t lrsmtx_new,outmtx;
	char*prepend=NULL,*append=NULL,*strrow=NULL;
	struct intarray* redarr;

	// redundancy removal
	//fprintf(stderr,"\n Done removing redund_naive:%ld X %ld, nlin=%d",inpoly->lrsmtx->r,inpoly->lrsmtx->c,inpoly->linearities->len);
	redarr=redund_naive(inpoly->lrsmtx,"h",inpoly->linearities->len,inpoly->linearities->array);
	
	k=0;
	if(inpoly->linearities->len>0)
	{
		newlinarray=malloc(inpoly->linearities->len*sizeof(int));// new linearity array
	}
	else
	{
		newlinarray=NULL;
	}
	fmpq_mat_init(lrsmtx_new,(inpoly->lrsmtx->r)-(redarr->len),inpoly->lrsmtx->c);// new lrs mtx
	
	for(i=0;i<inpoly->lrsmtx->r;i++)
	{
		if(ismember2(redarr->array,redarr->len,i)<0)// row is not redundant
		{
			copyrow(lrsmtx_new,k,inpoly->lrsmtx,i);
			k++;
		}
		if(ismember2(inpoly->linearities->array,inpoly->linearities->len,i+1)>=0)// current row was a linearity
		{
			if(ismember2(redarr->array,redarr->len,i)<0)// and its isn't redundant(dependent)
			{
				newlinarray[newlin]=k;
				newlin++;
			}
		} 
	}
	fmpq_mat_init(outpoly->lrsmtx,lrsmtx_new->r,lrsmtx_new->c);
	fmpq_mat_set(outpoly->lrsmtx,lrsmtx_new);
	outpoly->linearities->array=newlinarray;
	outpoly->linearities->len=newlin;
	outpoly->projdim=inpoly->projdim;
	return;
}

void readlrsfile2poly(polyhedron_t poly, char*filename)
{
	int* nlinptr=(int*)malloc(sizeof(int));
	int** linrowsptr=(int**)malloc(sizeof(int*));
	char* prepend;
	prepend= readlrsfile2(filename,poly->lrsmtx, nlinptr ,linrowsptr);
	poly->linearities->array=linrowsptr[0];
	poly->linearities->len=nlinptr[0];
	poly->redundancy->array=NULL;
	poly->redundancy->len=0;
	poly->prepend=prepend;
}

void writepoly2lrsfile(char* filename, polyhedron_t poly,int reptype)
{
	if(reptype==0) //h-poly
    writelrsfile(filename,"h",poly->lrsmtx,poly->linearities->len,poly->linearities->array,NULL,NULL);
    else
    writelrsfile(filename,"v",poly->VR,0,NULL,NULL,NULL);
}

void ubpoly_preproc(polyhedron_t outpoly, polyhedron_t inpoly)
{
	//Performs boundedness transformation, dimension finding, embedding and redundancy removal
	//corefunction
	struct intarray* projinfo;
	int rowsize, colsize;
	FILE *fp; 
	int m,n,i,j;
	mpq_t z;
	fmpq_t q;
	fmpq_init(q);
	fmpq_mat_t X;
	int nlin=0;
	int* linrows=NULL;
	fmpq_mat_init(outpoly->lrsmtx,inpoly->lrsmtx->r,inpoly->lrsmtx->c);
	fmpq_mat_set(outpoly->lrsmtx,inpoly->lrsmtx);
 	projinfo=chm_preprocess(outpoly->projeqmtx,outpoly->lrsmtx,inpoly->linearities->len,inpoly->linearities->array,inpoly->projdim); 
 	outpoly->linearities->len=0;// since we removed any equalities
 	if(projinfo->len>0)
 	{
		fprintf(stderr,"\nProjection dim reduces by %d", projinfo->len);
	}
 	outpoly->projdim=(inpoly->projdim)-(projinfo->len);
 	if(projinfo->len>0)
 	outpoly->projeqpresent=1;
 	else
 	outpoly->projeqpresent=0;
 	outpoly->projelimvars->array=projinfo->array;
 	outpoly->projelimvars->len=projinfo->len;
}

void ubpoly_preproc_boundedcone(polyhedron_t outpoly, polyhedron_t inpoly)
{
	//Performs boundedness transformation, dimension finding, embedding and redundancy removal
	//corefunction
	struct intarray* projinfo;
	int rowsize, colsize;
	FILE *fp; 
	int m,n,i,j;
	mpq_t z;
	fmpq_t q;
	fmpq_init(q);
	fmpq_mat_t X;
	int nlin=0;
	int* linrows=NULL;
	fmpq_mat_init(outpoly->lrsmtx,inpoly->lrsmtx->r,inpoly->lrsmtx->c);
	fmpq_mat_set(outpoly->lrsmtx,inpoly->lrsmtx);
 	projinfo=chm_preprocess(outpoly->projeqmtx,outpoly->lrsmtx,inpoly->linearities->len,inpoly->linearities->array,inpoly->projdim); 
 	outpoly->linearities->len=0;// since we removed any equalities
 	if(projinfo->len>0)
 	{
		fprintf(stderr,"\nProjection dim reduces by %d", projinfo->len);
        // modify sum<=1 ineq to contain only full-dim proj variables
        fprintf(stderr,"\n bad proj variables:");
        for(i=0;i<projinfo->len;i++)
        fprintf(stderr," %d",projinfo->array[i]);
        fmpq_mat_print(outpoly->projeqmtx);
        fmpq_mat_print(outpoly->lrsmtx);
        for(i=0;i<outpoly->lrsmtx->r;i++)
        {
            if(fmpq_is_one(fmpq_mat_entry(outpoly->lrsmtx,i,0)))
            {   
                // found non-homogeneous ineq
                for(j=0;j<(inpoly->projdim)-(projinfo->len);j++)
                {
                    fmpq_one(fmpq_mat_entry(outpoly->lrsmtx,i,j+1));
                    fmpq_neg( fmpq_mat_entry(outpoly->lrsmtx,i,j+1),fmpq_mat_entry(outpoly->lrsmtx,i,j+1));
                }
                fprintf(stderr,"\n fixed sum to one ineq");
                fmpq_mat_print(outpoly->projeqmtx);
                fmpq_mat_print(outpoly->lrsmtx);
                break;
            }
        } 
    }
 	outpoly->projdim=(inpoly->projdim)-(projinfo->len);
 	if(projinfo->len>0)
 	outpoly->projeqpresent=1;
 	else
 	outpoly->projeqpresent=0;
 	outpoly->projelimvars->array=projinfo->array;
 	outpoly->projelimvars->len=projinfo->len;
}

void chm_unbounded2bounded(polyhedron_t outpoly,polyhedron_t inpoly)
{
	//chm_makebounded2(fmpq_mat_t Y,fmpq_mat_t projeqmtx, char* filename,int* projdimptr,char** forewrdptr)
	//corefunction
	//printf("\nConverting unbounded problem to bounded problem...");
	int fore=0;
	char* prepend=malloc(STR_LEN_INC*sizeof(char));
	prepend[0]='\0';
	struct intarray* projinfo;
	struct intarray* redinfo;
	int projdim=inpoly->projdim;;
	int nlin;
	int* linrows,*newlinrows;
	int rowsize, colsize,newnlin;
	int m,n,i,j;
	mpq_t z;
	fmpq_t q;
	fmpq_init(q);
	fmpq_mat_t X,newY;
	//const char* begin="begin";
	//char buf[4096]; 
	//fp=fopen(filename,"r");
    //while(fp!=NULL && fgets(buf, sizeof(buf), fp)!=NULL)
    //{
        //if (strstr(buf, "begin"))
        //{
		//fore=1;
        //printf("begin found!");
        //gmp_fscanf(fp,"%d%d",&m,&n);
        //printf("size %d %d",m,n);
        //fmpq_mat_init ( X , m, n);
		//fmpq_mat_zero ( X);
		//do// skip empty lines
		//{
		//fgets(buf, sizeof(buf), fp);
		//}
		//while(strlen(buf)==0);
		//mpq_init(z);
		//for(i=0;i<m;i++)
		//{
			//for(j=0;j<n;j++)
			//{
				//gmp_fscanf(fp,"%Qi",z);		
				//fmpq_set_mpq(fmpq_mat_entry(X,i,j),z);
			//} time -p ./chm danZY31.ine unbounded 15
			//do// skip empty lines
			//{
				//fgets(buf, sizeof(buf), fp);
			//}
			//while(strlen(buf)==0);
		//}
		//mpq_clear(z);
		//break;
		//}
		//if(fore==0 && buf[0]!='*')
		//{
			//strcat(prepend,buf);
		//}
    //}
    //if(fp!=NULL) 
    //fclose(fp);
    //fmpq_mat_print(X);
    
	
    rowsize=inpoly->lrsmtx->c+inpoly->lrsmtx->r;
    colsize=inpoly->lrsmtx->r+projdim+1;
    fmpq_mat_init(outpoly->lrsmtx,inpoly->lrsmtx->c+inpoly->lrsmtx->r,inpoly->lrsmtx->r+projdim+1);// Y matrix corresponds to bounded version of X
	fmpq_mat_zero(outpoly->lrsmtx);// initialize Y to all zero
	//X is m by n+1  matrix
	fmpq_one(fmpq_mat_entry(outpoly->lrsmtx,inpoly->lrsmtx->c-1,0));// 1 for \lambda_1+\lambda_2...\lambda_m
	fmpq_mat_t Ip;
	fmpq_eye(Ip,projdim,-1);
	copy_submat2submat(outpoly->lrsmtx,1,2,Ip,1,Ip->r,1,Ip->c);// coefficients of \alpha s in repective equalities
	fmpq_mat_t A,At;
	fmpq_mat_submatrix(A,inpoly->lrsmtx,1,inpoly->lrsmtx->r,2,inpoly->lrsmtx->c);
	fmpq_mat_init(At,A->c,A->r);
	fmpq_mat_transpose(At,A);
	for(i=0;i<At->r;i++)
	{
		for(j=0;j<At->c;j++)
		{
			fmpq_neg(fmpq_mat_entry(At,i,j),fmpq_mat_entry(At,i,j));
		}
	}
	copy_submat2submat(outpoly->lrsmtx,1,projdim+2,At,1,At->r,1,At->c);
	for(i=projdim+2-1;i<outpoly->lrsmtx->c;i++)
	{
		fmpq_one(fmpq_mat_entry(outpoly->lrsmtx,inpoly->lrsmtx->c-1,i));// 1-\sum_i{\lambda_i}=0
		fmpq_neg(fmpq_mat_entry(outpoly->lrsmtx,inpoly->lrsmtx->c-1,i),fmpq_mat_entry(outpoly->lrsmtx,inpoly->lrsmtx->c-1,i));
	}
	fmpq_mat_t Ilambda;
	fmpq_eye(Ilambda,inpoly->lrsmtx->r,1);
	copy_submat2submat(outpoly->lrsmtx,inpoly->lrsmtx->c+1,projdim+2,Ilambda,1,Ilambda->r,1,Ilambda->c);
	/*Y= [I_projdim  A^t ]--->equalities involving \alphas and \lambdas  
	 *   [0          I_m ]---> \lambdas >= 0 constraints */
	 nlin=inpoly->lrsmtx->c;
	 linrows=malloc(nlin*sizeof(int));
	 for(i=1;i<=nlin;i++)
	 {
		 linrows[i-1]=i;
	 }

	redinfo=redund_naive(outpoly->lrsmtx,"h",nlin,linrows);
	fmpq_mat_init(newY,outpoly->lrsmtx->r-redinfo->len,outpoly->lrsmtx->c);
	j=0;
	newnlin=0;
	if(nlin>0)
	{
		newlinrows=malloc(nlin*sizeof(int));
	}
	else
	{
		newlinrows=NULL;
	}
	for(i=0;i<outpoly->lrsmtx->r;i++)
	{
		if(ismember2(redinfo->array,redinfo->len,i)==-1)// i+1(in MATLAB sense) is nonredundant row
		{
			if(ismember2(linrows,nlin,i+1)!=-1)// i+1 is actually equality
			{
				newlinrows[newnlin]=j+1;
				newnlin++;
			}
			//printf("\ncopy row %d to newlrsmtx\n",i);
			copyrow(newY,j,outpoly->lrsmtx,i);
			j++;
		}
	}
	//writelrsfile("b4bproc.ine","h",newY,newnlin,newlinrows,NULL,NULL);
	projinfo=chm_preprocess(outpoly->projeqmtx,newY,newnlin,newlinrows,projdim); 
	outpoly->projdim=projdim-projinfo->len;
	if(projinfo->len>0)
	outpoly->projeqpresent=1;
	else
	outpoly->projeqpresent=0;
	fmpq_mat_clear(Ilambda);
    fmpq_mat_clear(outpoly->lrsmtx);
    fmpq_mat_init(outpoly->lrsmtx,newY->r,newY->c);
    fmpq_mat_set(outpoly->lrsmtx,newY);
    outpoly->linpresent=0;
    outpoly->redpresent=0;
    outpoly->linearities->len=0;
    outpoly->redundancy->len=0;
    free(linrows);
    free(newlinrows);
    return ;

}

struct raylist* chm_bounded(polyhedron_t projpoly,polyhedron_t inpoly)
{//corefunction
	//struct raylist*  chm_bounded4(char* filename,fmpq_mat_t lrsmtx,fmpq_mat_t Vset, int projdimreal)
	//***************************CHM BEGIN******************************************/
	fmpq_mat_t returnVset;// Vset without the ones at the start. For the purpose of returning
	mpq_QSprob prob;
	fmpq_mat_t qslrsmtx;
	fmpq_mat_t fixedoutmat;
	//printf("Passed to qsload fromlrsmatrix: %d",inpoly->linearities->len);
	qsload_fromlrsmatrix(inpoly->lrsmtx,inpoly->linearities->len,inpoly->linearities->array,&prob);
	//qsload_fromlrsfile(qslrsmtx,filename,&prob);
	int projdim=inpoly->projdim+1;
	int * vnum;
	vnum= malloc(sizeof(int));
	vnum[0]=0;
	fmpq_mat_t Hrep,Rset,Hset;
	mpq_QSprob* probptr=&prob;
	fprintf(stderr,"\nFinding Initial Hull...");
	findinitialhull8(inpoly->projVset,inpoly->lrsmtx,inpoly->projdim);
	fprintf(stderr,"\nDone with initial hull");
	fmpq_mat_init(Rset,inpoly->projVset->c,inpoly->projVset->r); // vset is vertices in lrs format. since it already has a 1 in first column, we neednot homogenize seperately
	fmpq_mat_transpose(Rset,inpoly->projVset);// Rset  is transpose of Vset
	initialHrep(Hrep,Rset);
	//create the raylist in the polar space
	struct raylist* head, *currentray,* adjcandidate,*temp_raylist,*tptr;
	struct isfacet_returnset* facetinfo=NULL;
	struct insineqcon_returnset *returnset;
	int* tightset;
	head=create_new_raylist(Hrep,inpoly->projVset); // Note that H and V have been exchanged. Behold! We are in polar space now!
	fmpq_mat_t fcandidate;

	//tptr=head;
	//do
	//{
		//fmpq_mat_print(tptr->ray);
		//tptr=tptr->nextray;
	//}while(tptr!=head);
	int terminal=0;
	int lastindex=inpoly->projdim;
	long itcnt=inpoly->projVset->r;
	while(1)
	{
		fprintf(stderr,"\n\nraylist has %ld rays\n\n",raylist_numrays(head));
		temp_raylist=head;
		while(1)
		{
			
			if(temp_raylist->flag1==0)//potentially nonterminal facet
			{
				facetinfo=isfacet2(temp_raylist->ray,probptr,projdim);
				temp_raylist=temp_raylist->nextray;
				if((facetinfo->isfacet)<0) // found a nonterminal facet
				{
					//{
					//fmpq_mat_print(temp_raylist->prevray->ray);
					//}
					//fprintf(stderr,"\nNew non-terminal facet found");
					break;
				}
				else //new terminal facet found
				{
					//fprintf(stderr,"\nNew terminal facet found");
					temp_raylist->prevray->flag1=1;
					facetinfo=NULL;
				}
			}
			else // terminal facet
			{
				temp_raylist=temp_raylist->nextray;
			}
			if(temp_raylist==head)
			{
				terminal=1;
				break;
			}
			
		}
		if(terminal!=1)
		{
			//print_raylist(head);
			updatehset(inpoly->projVset,facetinfo->ineqcon);
			//fprintf(stderr,"\nUpdated hset");
			returnset= insineqcon_withadj3(facetinfo->ineqcon,head ,lastindex,inpoly->projVset,projdim);
			//fprintf(stderr,"\nInserted new constraint");
			head=returnset->head;
			lastindex=returnset->lastindex;
			free(returnset);
			free(facetinfo);
			itcnt++;
		}
		else
		{
			break;
		}
	}
	//raylist2ineq(outpoly->lrsmtx,head);
	fmpq_mat_init(projpoly->VR,inpoly->projVset->r,inpoly->projVset->c);
	fmpq_mat_set(projpoly->VR,inpoly->projVset);
	raylist2ineq(projpoly->lrsmtx,head);
	fprintf(stderr,"\nDone with bounded chm...\n>>stats: Bounded projection has %ld facets %ld vertices...",projpoly->lrsmtx->r,projpoly->VR->r);
	fmpq_mat_neg(projpoly->lrsmtx,projpoly->lrsmtx);
	if(inpoly->projeqpresent==1)
	{
		projpoly->projeqpresent=1;
		fmpq_mat_init(projpoly->projeqmtx,inpoly->projeqmtx->r,inpoly->projeqmtx->c);
		fmpq_mat_set(projpoly->projeqmtx,inpoly->projeqmtx);
		projeqs2linearities(fixedoutmat,projpoly->linearities,inpoly->projeqmtx,projpoly->lrsmtx);
		fmpq_mat_clear(projpoly->lrsmtx);
		fmpq_mat_init(projpoly->lrsmtx,fixedoutmat->r,fixedoutmat->c);
		fmpq_mat_set(projpoly->lrsmtx,fixedoutmat);
	}
	else
	{
		projpoly->projeqpresent=0;
		projpoly->linearities->len=0;
		projpoly->linearities->array=(int*)NULL;
	}
	projpoly->head=head;
	if(prob)  mpq_QSfree_prob(prob);
		free(vnum);
	return head;

}

void projeqs2linearities(fmpq_mat_t out,arrayofints_t linearities, fmpq_mat_t projeqmtx,fmpq_mat_t in)
{
	int i,j,k,l;
	k=0;
	linearities->array=malloc(projeqmtx->r*sizeof(int));
	int* elimvars=malloc(projeqmtx->r*sizeof(int));
	for(i=0;i<projeqmtx->r;i++)
	{
		for(j=projeqmtx->c-1;j>-1;j--)
		{
			if(fmpq_is_one ( fmpq_mat_entry(projeqmtx,i,j))!=0)
			{
				linearities->array[k]=k+1;
				elimvars[k]=j;
				k=k+1;
				break;
			}
		}
	}
	linearities->len=k;
	fmpq_mat_init(out,projeqmtx->r+in->r,projeqmtx->c);
	copy_submat2submat(out,1,1,projeqmtx,1,projeqmtx->r,1,projeqmtx->c);
	copy_submat2submat(out,projeqmtx->r+1,1,in,1,in->r,1,1);
	for(i=projeqmtx->r;i<out->r;i++)
	{
		l=0;
		for(j=1;j<out->c;j++)
		{
			if(ismember(elimvars,k,j)==1)
			fmpq_zero(fmpq_mat_entry(out,i,j));
			else
			{
				fmpq_set(fmpq_mat_entry(out,i,j),fmpq_mat_entry(in,i-projeqmtx->r,l));
				l++;
			}
		}
	}
}


void chm_adddualprojeqs(polyhedron_t projpoly)
{
	printf("Fixing dual projection equalities");
	int i;
	fmpq_mat_t newVset,tempvtx,row;
	fmpq_mat_init(newVset,projpoly->VR->r,projpoly->projeqmtx->c);
	fmpq_mat_init(row,1,projpoly->projeqmtx->c);
	fmpq_mat_init(tempvtx,1,projpoly->VR->c);
	for(i=0;i<projpoly->VR->r;i++)
	{
		copyrow(tempvtx,0,projpoly->VR,i);
		embedvtx_inhyperplanes(row,tempvtx,projpoly->projeqmtx);
		copyrow(newVset,i,row,0);
	}
	fmpq_mat_clear(projpoly->projeqmtx);
	fmpq_mat_init(projpoly->VR,newVset->r,newVset->c);
	fmpq_mat_set(projpoly->VR,newVset);
	fmpq_mat_clear(newVset);
}

void chm_bounded2unbounded(polyhedron_t projpolyrdd, polyhedron_t dualprojpoly)
{
	int i,j,k=0;
	fmpq_mat_t outmtx,lrsmtx_new;
	fmpq_mat_init(outmtx,dualprojpoly->VR->r,dualprojpoly->VR->c);
	//fmpq_mat_print(dualprojpoly->VR);
	for(i=0;i<outmtx->r;i++)
	{
		for(j=0;j<outmtx->c-1;j++)
		{
			fmpq_set(fmpq_mat_entry(outmtx,i,j+1),fmpq_mat_entry(dualprojpoly->VR,i,j+1));
			fmpq_neg(fmpq_mat_entry(outmtx,i,j+1),fmpq_mat_entry(dualprojpoly->VR,i,j+1));
		}
	}
	polyhedron_t projpoly;
	fmpq_mat_init(projpoly->lrsmtx,outmtx->r,outmtx->c);
	fmpq_mat_set(projpoly->lrsmtx,outmtx);
	projpoly->linearities->len=0;
	//fmpq_mat_print(outmtx);
	redund_polyhedron(projpolyrdd,projpoly);
	lrsmtxcone_scale2integers(lrsmtx_new,projpolyrdd->lrsmtx);
	fmpq_mat_clear(projpolyrdd->lrsmtx);
	fmpq_mat_init(projpolyrdd->lrsmtx,lrsmtx_new->r,lrsmtx_new->c);
	fmpq_mat_set(projpolyrdd->lrsmtx,lrsmtx_new);
	
	//struct intarray* redarr=redund_naive(outmtx,"h",0,NULL);
	//fmpq_mat_init(projpolyrdd->lrsmtx,outmtx->r-redarr->len,outmtx->c);
	//for(i=0;i<outmtx->r;i++)
	//{
		//if(ismember(redarr->array,redarr->len,i)==0)// row is not redundant
		//{
			//copyrow(projpolyrdd->lrsmtx,k,outmtx,i);
			//k++;
		//}
	//}
	projpolyrdd->linpresent=0;
	projpolyrdd->redpresent=0;
	//fprintf(stderr,"\nprimal projpoly has %ld inequalities...",projpolyrdd->lrsmtx->r);
	fmpq_mat_clear(outmtx);
}


void chm_addprojeqs(polyhedron_t childpoly, polyhedron_t parentpoly)
{
	if(parentpoly->projeqmtx->r<=0)
	{
		childpoly->linpresent=0;
		return;
	}
	else
	{
		int i;
		fmpq_mat_t newmtx;
		fmpq_mat_init(newmtx,(childpoly->lrsmtx->r)+parentpoly->projeqmtx->r,parentpoly->projeqmtx->c);
		childpoly->linpresent=1;
		copy_submat2submat(newmtx,1,1,parentpoly->projeqmtx,1,parentpoly->projeqmtx->r,1,parentpoly->projeqmtx->c);
		copy_submat2submat(newmtx,parentpoly->projeqmtx->r+1,1,childpoly->lrsmtx,1,childpoly->lrsmtx->r,1,childpoly->lrsmtx->c);
		childpoly->linearities->len=parentpoly->projeqmtx->r;
		childpoly->linearities->array=(int*)malloc((parentpoly->projeqmtx->r)*sizeof(int));
		for(i=0;i<childpoly->linearities->len;i++)
		childpoly->linearities->array[i]=i+1;
	}
}
