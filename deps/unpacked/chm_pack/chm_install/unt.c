//if(strcmp(argv[2],"unbounded")==0)// its a general cone
		//{
			//argv[1]=chm_makebounded2(lrsmtx,projeqmtx2,argv[1],projdimptr,&forewrd);
		//}
		//printf("\nprojection equalities after bounding");
		//fmpq_mat_print(projeqmtx2);
		//argc2=2;
		//argv2=argv;
		//printf("lrsmtx dimension is %ld while projection dimension is %d",lrsmtx->c-1,projdimptr[0]);
		//start = clock();
		//if(lrsmtx->c-1>=projdimptr[0]) //We still have more dimensions than projection dims 
		//{
			//printf("\nbounded chm run on: %s\n",argv[1]);
			//head= chm_bounded4(argv2[1],lrsmtx,Vset,projdimptr[0]);
		//}
		
		////fmpq_mat_print(Vset);
		////printf("\nprojeqs1:::");
		////fmpq_mat_print(projeqmtx2);
		//if(projdim1>projdimptr[0])// fix for projection equalities in bounded polytope projection
		//{
			//fmpq_mat_init(newVset,Vset->r,projeqmtx2->c);
			//fmpq_mat_init(row,1,projeqmtx2->c);
			//fmpq_mat_init(tempvtx,1,Vset->c);
			//for(i=0;i<Vset->r;i++)
			//{
				//copyrow(tempvtx,0,Vset,i);
				//embedvtx_inhyperplanes(row,tempvtx,projeqmtx2);
				//copyrow(newVset,i,row,0);
			//}
			
		//}
		//else
		//{
			//fmpq_mat_init(newVset,Vset->r,Vset->c);
			//fmpq_mat_set(newVset,Vset);
		//}
		////printf("newVset::");
		////fmpq_mat_print(newVset);
		////printf("\nInequalities(polar rays)\n");
		////print_raylist(head);
		//fmpq_mat_init(outmtx,newVset->r,newVset->c);
		//for(i=0;i<outmtx->r;i++)
		//{
			//for(j=0;j<outmtx->c-1;j++)
			//{
				//fmpq_set(fmpq_mat_entry(outmtx,i,j+1),fmpq_mat_entry(newVset,i,j+1));
				//fmpq_neg(fmpq_mat_entry(outmtx,i,j+1),fmpq_mat_entry(outmtx,i,j+1));
			//}
		//}
		
		////printf("\nprojeqmtx1 is ");
		////fmpq_mat_print(projeqmtx1);
		////printf("\nprojeqmtx2 is ");
		////fmpq_mat_print(projeqmtx2);
		//writelrsfile((char*)(NULL),"h",outmtx,0,(int*)(NULL),(char*)(NULL),(char*)(NULL));
		//redarr=redund_naive(outmtx,"h",0,NULL);
	////printf("\n%d rows redundant\n",redarr->len);
	//if(redarr->len>0)
	//{
	//prepend=malloc(50*(redarr->len)*sizeof(char));
	//prepend[0]='\0';
	//}
	//else
	//{
		//prepend=malloc(1*sizeof(char));
		//prepend[0]='\0';
	//}
	////printf("\nprepend len %d prepend size %d foreword len %d foreword size %d",strlen(prepend),sizeof(prepend),strlen(forewrd),sizeof(forewrd));
	//append=malloc(130*sizeof(char));
	//strrow=malloc(70*sizeof(char));
	//strrow[0]='\0';

	//append[0]='\0';
	//for(i=0;i<redarr->len;i++)
	//{
			//sprintf(strrow,"*row %d was redundant and removed\n",redarr->array[i]+1);
			//strcat(prepend,strrow);
	//}
	//fmpq_mat_init(lrsmtx_new,outmtx->r-redarr->len,outmtx->c);// new lrs mtx
	//k=0;
	//for(i=0;i<outmtx->r;i++)
	//{
		//if(ismember(redarr->array,redarr->len,i)==0)// row is not redundant
		//{
			//copyrow(lrsmtx_new,k,outmtx,i);
			//k++;
		//}
	//}
	//times(&end_tms);// get end time
	
	//sprintf(append,"*chm output had %ld rows and %ld columns: %d row(s) redundant\n*chm:chmlib v.0.3 2013.7.23 Jayant Apte\n*Time Taken: %f",outmtx->r,outmtx->c,redarr->len,((double)clock() - start) / CLOCKS_PER_SEC);
	////puts(prepend);
	////puts(append);
	//prepend=strcat(forewrd,prepend);
	//if(argc<5)
	//{
		//lrsmtxcone_scale2integers(intlrsmtx_new,lrsmtx_new);
		//writelrsfile(NULL, "h" ,intlrsmtx_new, 0,NULL, prepend,append);
		
	//}
	//else if(argv[4]!=NULL)
	//{
		//lrsmtxcone_scale2integers(intlrsmtx_new,lrsmtx_new);
		//writelrsfile(NULL, "h" ,intlrsmtx_new, 0,NULL, prepend,append);
		//writelrsfile(argv[4], "h" ,intlrsmtx_new, 0,NULL, prepend,append);
	//}
	//clear_raylist2(head);
	//free(forewrd);
	//printf("Time elapsed: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);
	

	//free(nlinptr);
	//free(projdimptr);
	//free(linrowsptr);
