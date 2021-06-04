#include "core.h"
/****************************************************************/
//**This file contains the main function.**/
int main (int argc, char *argv[])
{
	/*****************variable declarations*********************/
	int i,j,k;
	if(argc<4)
	{
		fprintf(stderr,"\nInvalid input...\nUsage: %s <inputfile> <bounded/unbounded> <projection dimension> <H-outputfile(optional)> <V-outputfile(optional)>\n",argv[0]);
		return 0;
	}
	else if(argc==4)
	{
	argv[4]=(char*)NULL;
	argv[5]=(char*)NULL;
	}
	else if(argc==5)
	argv[5]=(char*)NULL;
	int projdim=strtol(argv[3],NULL,10);
	int projdim1;
	fmpq_mat_t lrsmtx,col,row,newVset,tempvtx,lrsmtx1,lrsmtx_new,intlrsmtx_new,projeqmtx,newlrsmtx,Vset,projeqmtx1,projeqmtx2,outmtx;
	clock_t start,end;
	float rddtime, ubpreproctime,xformtime,chmtime,optime;
	QSexactStart();
    QSexact_set_precision (128);
	polyhedron_t inpoly,inpolyrdd,inpolyrddproc,xformedrddproc, projpoly,dualprojpoly,projpolyrdd;
	struct raylist* head;
	/***********read input file********************************/
	readlrsfile2poly(inpoly, argv[1]);
	inpoly->projdim=projdim;
	if(argv[2]!=NULL && strcmp(argv[2],"unbounded")==0)
	{
		/***********Redundancy removal*****************************/
		start = clock();
		fprintf(stderr,"\nRemoving input redundancies...");
		
		redund_polyhedron(inpolyrdd,inpoly);
		end = clock();
		rddtime=(float)(end - start) / CLOCKS_PER_SEC;
		fprintf(stderr,"%f s.", rddtime);
		/*************Dimension finder******************************/
		fprintf(stderr,"\nFininding dimension of input...");
		start = clock();
		ubpoly_preproc( inpolyrddproc,  inpolyrdd);
		end = clock();
		ubpreproctime=(float)(end - start) / CLOCKS_PER_SEC;
		fprintf(stderr,"%f s.",ubpreproctime);
		fprintf(stderr,"\nDimension of affine hull of %s is %ld...", argv[1],inpolyrddproc->lrsmtx->c-1);
		projdim1=inpolyrddproc->projdim;
		/************boundedness transformation + Dimension finder*******************/
		start=clock();
		fprintf(stderr,"\nTransforming unbounded polyhedron to a bounded polyhedron...");
		chm_unbounded2bounded(xformedrddproc,inpolyrddproc);
		end=clock();
		xformtime=(float)(end - start) / CLOCKS_PER_SEC;
		fprintf(stderr,"%f s.",xformtime);
		chmtime=0;
		if(xformedrddproc->projdim < xformedrddproc->lrsmtx->c-1)
		{
			
			start=clock();
			fprintf(stderr,"\nPerforming bounded projection...");
			/*************Bounded chm***************/
			chm_bounded(dualprojpoly,xformedrddproc);
			fmpq_mat_neg(dualprojpoly->lrsmtx,dualprojpoly->lrsmtx);
			end=clock();
			chmtime=(float)(end - start) / CLOCKS_PER_SEC;
			fprintf(stderr,"%f s.",chmtime);
			start=clock();
			if(xformedrddproc->projeqpresent==1)
			{
				chm_adddualprojeqs(dualprojpoly);
				fprintf(stderr,"\nFixed dual projectoin equalities, there are %ld %ld-dimensional vertices",dualprojpoly->VR->r,dualprojpoly->VR->c-1);
			}
			chm_bounded2unbounded( projpolyrdd, dualprojpoly);
			/******************write out the result*******************/
			writepoly2lrsfile(argv[4], projpolyrdd,0);
			end=clock();
			optime=(float)(end - start) / CLOCKS_PER_SEC;
			fprintf(stderr,"\nTotal time required: %f s.\n",rddtime+ubpreproctime+xformtime+chmtime+optime);
		}
		else
		{
			fprintf(stderr,"\nProjection dimension is greater than input polyhedron dimension, aborting...");
		}
		
	}
	else if(argv[2]!=NULL && strcmp(argv[2],"bounded")==0)
	{
		/***********Redundancy removal*****************************/
		start = clock();
		fprintf(stderr,"\nRemoving input redundancies...: %ld X %ld input matrix",inpoly->lrsmtx->r,inpoly->lrsmtx->c);
		
		redund_polyhedron(inpolyrdd,inpoly);
		end = clock();
		rddtime=(float)(end - start) / CLOCKS_PER_SEC;
		fprintf(stderr,"\n%f s.", rddtime);
		/*************Dimension finder******************************/
		fprintf(stderr,"\nFininding dimension of input...");
		start = clock();
		ubpoly_preproc( inpolyrddproc,  inpolyrdd);
		end = clock();
		ubpreproctime=(float)(end - start) / CLOCKS_PER_SEC;
		fprintf(stderr,"\n%f s.",ubpreproctime);
		fprintf(stderr,"\nDimension of affine hull of %s is %ld...", argv[1],inpolyrddproc->lrsmtx->c-1);
		projdim1=inpolyrddproc->projdim;
		//fmpq_mat_print(inpolyrddproc->projeqmtx);
		start=clock();
		fprintf(stderr,"\nPerforming bounded projection...");
		/*************Bounded chm***************/
		chm_bounded(projpolyrdd,inpolyrddproc);
		end=clock();
		chmtime=(float)(end - start) / CLOCKS_PER_SEC;
		fprintf(stderr,"\nchm time = %f s.",chmtime);
		start=clock();
		//fmpq_mat_neg(projpolyrdd->lrsmtx,projpolyrdd->lrsmtx);
		fprintf(stderr,"\nWriting output file");
		writepoly2lrsfile(argv[4], projpolyrdd,0);// write h-rep
		writepoly2lrsfile(argv[5], projpolyrdd,1);// write v-rep
		end=clock();
		optime=(float)(end - start) / CLOCKS_PER_SEC;
		fprintf(stderr,"\nTotal time required: %f s.\n",rddtime+ubpreproctime+chmtime+optime);
		//fprintf(stderr,"\n Eliminated projection variables: ");
		//for(i=0;i<inpolyrddproc->projelimvars->len;i++)
		//fprintf(stderr,"{%d}",inpolyrddproc->projelimvars->array[i]);
		//fprintf(stderr,"\n");
		
	}
    else if(argv[2]!=NULL && strcmp(argv[2],"boundedcone")==0)
	{
		/***********Redundancy removal*****************************/
		start = clock();
		fprintf(stderr,"\nRemoving input redundancies...: %ld X %ld input matrix",inpoly->lrsmtx->r,inpoly->lrsmtx->c);
		
		redund_polyhedron(inpolyrdd,inpoly);
		end = clock();
		rddtime=(float)(end - start) / CLOCKS_PER_SEC;
		fprintf(stderr,"\n%f s.", rddtime);
		/*************Dimension finder******************************/
		fprintf(stderr,"\nFininding dimension of input...");
		start = clock();
		ubpoly_preproc_boundedcone( inpolyrddproc,  inpolyrdd);
		end = clock();
		ubpreproctime=(float)(end - start) / CLOCKS_PER_SEC;
		fprintf(stderr,"\n%f s.",ubpreproctime);
		fprintf(stderr,"\nDimension of affine hull of %s is %ld...", argv[1],inpolyrddproc->lrsmtx->c-1);
		projdim1=inpolyrddproc->projdim;
		//fmpq_mat_print(inpolyrddproc->projeqmtx);
		start=clock();
		fprintf(stderr,"\nPerforming bounded projection...");
		/*************Bounded chm***************/
		chm_bounded(projpolyrdd,inpolyrddproc);
		end=clock();
		chmtime=(float)(end - start) / CLOCKS_PER_SEC;
		fprintf(stderr,"\nchm time = %f s.",chmtime);
		start=clock();
		//fmpq_mat_neg(projpolyrdd->lrsmtx,projpolyrdd->lrsmtx);
		fprintf(stderr,"\nWriting output file");
		writepoly2lrsfile(argv[4], projpolyrdd,0);// write h-rep
		writepoly2lrsfile(argv[5], projpolyrdd,1);// write v-rep
		end=clock();
		optime=(float)(end - start) / CLOCKS_PER_SEC;
		fprintf(stderr,"\nTotal time required: %f s.\n",rddtime+ubpreproctime+chmtime+optime);
		//fprintf(stderr,"\n Eliminated projection variables: ");
		//for(i=0;i<inpolyrddproc->projelimvars->len;i++)
		//fprintf(stderr,"{%d}",inpolyrddproc->projelimvars->array[i]);
		//fprintf(stderr,"\n");
		
	}
	else
	fprintf(stderr,"\nInvalid input...\nUsage: ./%s <inputfile> <projection dimension> <outputfile(optional)>",argv[0]);
	//QSexactClear();
	return 0; 
}





