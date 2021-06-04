#include "chmflint.h"
#include "chmstructs.h"

void arrayofints_init(arrayofints_t iarray, int len)
{
	iarray->array=(int* )calloc(len,sizeof(int));
	iarray->len=len;
}

void arrayofints_clear(arrayofints_t iarray)
{
	free(iarray->array);
	iarray->len=0;
}




void newray_createadjlist(struct raylist* newray, struct raylist *head,fmpq_mat_t Hset, int dim)
{
	struct raylist* adjcandidate;
	adjcandidate=head->nextray;
	while (1) // we check from next ray to head, assuming that adjacencies with previous ray were added when the outer while loop iterated over that ray
		{	
			if(fmpq_sgn(adjcandidate->score)>=0)
			{
				//printf("Computing adjacencies for ray %d", newray->rayindex);
				if(adjtest_algebraic3(newray,adjcandidate,Hset,dim)!=0)
					{
					//printf("add ray %d to %d's adjlist",adjcandidate->rayindex,newray->rayindex);
					addray2adjlist(newray, adjcandidate);
					}
				else 
					{
						//printf("newray %d and ray  %d are not adjacent",newray->rayindex,adjcandidate->rayindex);
					}
			}
			if (adjcandidate==head)
			break;
			adjcandidate=adjcandidate->nextray;
		}
}






long raylist_numrays(struct raylist* head)
{//corefunction
	long cnt=0;
	struct raylist* tempray=head->nextray;
	while(1)
	{
		cnt++;
		tempray=tempray->nextray;
		if(tempray==head->nextray)
		break;
	}
	return cnt;
}


void remove_nray2(struct raylist* nray)
{//corefunction
	int acnt=0;
	int acnt2=0;
	struct raylist* temp_adjray,*temp_raylist,*newhead;
	nray->prevray->nextray=nray->nextray;
	nray->nextray->prevray=nray->prevray;
	setops_clear(nray->tightset);
	free(nray);
}



void newray_add2raylist(fmpq_mat_t newRay,struct raylist *head,fmpq_mat_t Hset, int lastrayindex,int dim)
{//corefunction
		struct raylist* newray;
		int i=lastrayindex+1;
		newray=malloc(sizeof(struct raylist));
		newray->flag1=0;
		newray->rayindex=i;
		fmpq_mat_init(newray->ray,1,newRay->c);
		fmpq_mat_set(newray->ray,newRay);
		//newray->ray_tightlist=create_new_tightlist(newray->ray,Hset,&(newray->hnum));//ray_tightlist, hnum
		create_new_tightset(newray->tightset,newray->ray,Hset,&(newray->hnum));
		newray->rnum=0;
		newray->score->num=0;
		newray->score->den=1;
		/*Time to fill in adjacency list of the new ray*/
		newray_createadjlist(newray,head,Hset,dim);
		/* set newray pointers */
		newray->nextray=head;
		newray->prevray=head->prevray;
		/*set pointers in prospective neighbours of newray to point to newray*/          //head->prevray <====>  head
		head->prevray->nextray=newray;													//head->prevray(newray->prevray) <===> newray <====> head
		head->prevray=newray;
}



void remove_nray(struct raylist* nray)
{//corefunction
	int acnt=0;
	int acnt2=0;
	struct tightlist* temp_tightlist;
	struct tightlist* temp_tighthead;
	struct tightlist* next_tightlist;
	struct adjlist* temp_adjlist,*temp_adjray_adjlist,*temp_head;
	temp_adjlist=nray->head_adjlist;
	struct raylist* temp_adjray,*temp_raylist,*newhead;
	nray->prevray->nextray=nray->nextray;
	nray->nextray->prevray=nray->prevray;
	while(1) //loop over entire adjlist of nray
	{
		acnt++;
		temp_adjlist=temp_adjlist->nextadj;
		//if ((temp_adjlist->adjray->flag2==1 || temp_adjlist->adjray->flag2==0)==1)// we only fix +/0 rays' adjlist
		if (temp_adjlist->adjray->flag2>=0)
		{
			//fprintf(stdout,"positive adjray no. %d",acnt);
			temp_adjray=temp_adjlist->adjray; //one of the rays adjacent to nray
			temp_adjray_adjlist=temp_adjray->head_adjlist;// head of adjray's adjlist
			//fprintf(stdout,"finding nray is %d's adjlist",acnt);
			acnt2=0;
			while(temp_adjray_adjlist->adjray!=nray) //find nray in adjray's adjlist
			{
				temp_adjray_adjlist=temp_adjray_adjlist->nextadj;
				if(temp_adjray_adjlist==temp_adjray->head_adjlist)
				{
					fprintf(stdout,"\n\nFATAL ERROR: nray not found in neighbour's adjlist\n\n");
				}
						
			}
			//fprintf(stdout,"found nray is %d's adjlist",acnt);
			temp_adjray_adjlist->prevadj->nextadj=temp_adjray_adjlist->nextadj; //remove nray from adjray's adjlist
			temp_adjray_adjlist->nextadj->prevadj=temp_adjray_adjlist->prevadj;
			//printf("removing %d from %d's adjlist",nray->rayindex,temp_adjray->rayindex);
			if(temp_adjray_adjlist==temp_adjray->head_adjlist)// oops. we are removing head of temp_adjray's adjlist
			{
				//fprintf(stdout,"\n\nNegative head!!!!\n\n");
				temp_head=temp_adjray->head_adjlist;
				while(temp_head->adjray->flag2==-1)// better find temp_adjray another non-ve head
				{
					temp_head=temp_head->nextadj;
					if(temp_head==temp_adjray->head_adjlist)
					{
						//fprintf(stdout,"\n\nFATAL ERROR: No positive head found\n\n");
					}
				}
				temp_adjray->head_adjlist=temp_head;
			}
			free(temp_adjray_adjlist);
			temp_adjray->rnum--;
		}
		if(temp_adjlist==nray->head_adjlist)
		{
			break;
		}
		
	}
	clear_adjlist(nray->head_adjlist);
	setops_clear(nray->tightset);
	free(nray);
}



void clear_adjlist(struct adjlist* head_adjlist)
{//corefunction
	struct adjlist* temp_adjlist;
	struct adjlist* temp_nextadj;
	temp_adjlist=head_adjlist->nextadj;
	while(temp_adjlist!=head_adjlist)
	{
		temp_nextadj=temp_adjlist->nextadj;
		free(temp_adjlist);
		temp_adjlist=temp_nextadj;
	}
	free(head_adjlist);
}


void create_new_tightset(set_t tightset,fmpq_mat_t ray,fmpq_mat_t Hset,int* hnum) // hindex indexes Hset in matlab sense, ie starts from 1
{//corefunction
	int i;
	setops_init(tightset);
	fmpq_mat_t temp_h;
	fmpq_t temp_dot;
	//fmpq_mat_init(temp_h,1,Hset->c);
	hnum[0]=0;// number of tight inequalities
	for (i=0;i<Hset->r;i++)
	{
		fmpq_mat_submatrix(temp_h,Hset,i+1,i+1,1,Hset->c);
		dotrr(temp_dot,temp_h,ray);
		//printf("\ndot for ineq %d is:",i);
		//fmpq_print(temp_dot);
		if(fmpq_sgn(temp_dot)==0)
		{
			hnum[0]++;
			setops_insert_ul(tightset,i+1);
		}
		fmpq_mat_clear(temp_h);
		fmpq_clear(temp_dot);
	}
}


void newray_add2raylist2(fmpq_mat_t newRay,struct raylist *head,fmpq_mat_t Hset, int lastrayindex,int dim)
{//corefunction
	/*This function is to be used inside the adjacency-free chm. It doesnot create an adjacency list for newRay*/
		struct raylist* newray;
		int i=lastrayindex+1;
		newray=malloc(sizeof(struct raylist));
		newray->flag1=0;
		newray->rayindex=i;
		fmpq_mat_init(newray->ray,1,newRay->c);
		fmpq_mat_set(newray->ray,newRay);
		//newray->ray_tightlist=create_new_tightlist(newray->ray,Hset,&(newray->hnum));//ray_tightlist, hnum
		create_new_tightset(newray->tightset,newray->ray,Hset,&(newray->hnum));
		newray->rnum=0;
		newray->score->num=0;
		newray->score->den=1;
		/*Time to fill in adjacency list of the new ray*/
		/* set newray pointers */
		newray->nextray=head;
		newray->prevray=head->prevray;
		/*set pointers in prospective neighbours of newray to point to newray*/          //head->prevray <====>  head
		head->prevray->nextray=newray;													//head->prevray(newray->prevray) <===> newray <====> head
		head->prevray=newray;
}



void clear_raylist2(struct raylist* head)
{//corefunction
	struct raylist* temp_raylist;
	struct raylist* temp_nextray;
	temp_raylist=head->nextray;
	while(temp_raylist!=head)
	{
		
		temp_nextray=temp_raylist->nextray;
		setops_clear(temp_raylist->tightset);
		free(temp_raylist);
		temp_raylist=temp_nextray;
	}
	free(head);
}


void addray2adjlist(struct raylist* currentray, struct raylist* adjray)
{
	struct adjlist* temp_head_adjlist; 
	struct adjlist* new_adjray;
	new_adjray=malloc(sizeof(struct adjlist));
	if(currentray->rnum==0)//first adjacent ray is being added
	{
		currentray->head_adjlist=new_adjray;// update head of adjlist inside the raylist of currentray
	}
	temp_head_adjlist=currentray->head_adjlist; // so that we don't need to use currentray->head_adjlist everywhere
	new_adjray->rayindex=adjray->rayindex; // set the index of newly added ray to same as that of adjray
	//printf("\nAdding rayindex %d to %d's adjlist",adjray->rayindex,currentray->rayindex);
	/*stick the new adjacent ray behind th head of adjlist*/
	new_adjray->nextadj=temp_head_adjlist; 
	if(currentray->rnum==0)
	{
		new_adjray->prevadj=new_adjray;
	}
	else
	{
		new_adjray->prevadj=temp_head_adjlist->prevadj;
		temp_head_adjlist->prevadj->nextadj=new_adjray;
		temp_head_adjlist->prevadj=new_adjray;
	}
	/*Finally, point to the raylist structure adjray*/
	new_adjray->adjray=adjray; 
	new_adjray->parent=currentray;
	currentray->rnum++; // increment the adjacent ray counter in currentray
	/************************************************************************************/
	/*Now add currentray to adjcandidate's adjlist!*/
	/************************************************************************************/
	new_adjray=malloc(sizeof(struct adjlist)); // make a new adjlist node
	if(adjray->rnum==0)//first adjacent ray is being added
	{
		adjray->head_adjlist=new_adjray;// update head of adjlist inside the raylist of currentray
	}
	temp_head_adjlist=adjray->head_adjlist; // so that we don't need to use currentray->head_adjlist everywhere
	new_adjray->rayindex=currentray->rayindex; // set the index of newly added ray to same as that of currentray(note that currentray is being added to adjray's adjlist)
	//printf("\nAdding rayindex %d to %d's adjlist",currentray->rayindex,adjray->rayindex);
	/*stick the new adjacent ray behind th head of adjlist*/
	new_adjray->nextadj=temp_head_adjlist; 
	if(adjray->rnum==0)
	{
		new_adjray->prevadj=new_adjray;
	}
	else
	{
	new_adjray->prevadj=temp_head_adjlist->prevadj;
	temp_head_adjlist->prevadj->nextadj=new_adjray;
	temp_head_adjlist->prevadj=new_adjray;
	}
	/*Finally, point to the raylist structure adjray*/
	new_adjray->adjray=currentray; 
	new_adjray->parent=adjray;
	adjray->rnum++; // increment the adjacent ray counter in currentray	
}



void populate_adjlist_algebraic(struct raylist* head, fmpq_mat_t Hset,int dim)
{
	struct raylist* adjcandidate;
	struct raylist* currentray;
	currentray=head;
	adjcandidate=currentray->nextray;// start testing adjacency from the ray next to current ray
	//printf("Computing adjacencies for ray %d", currentray->rayindex);
	while (adjcandidate!=head) // we check from next ray to head, assuming that adjacencies with previous ray were added when the outer while loop iterated over that ray
		{	
			if(adjtest_algebraic3(currentray,adjcandidate,Hset,dim)!=0)
			{
				//printf("add ray %d to %d's adjlist",adjcandidate->rayindex,currentray->rayindex);
				addray2adjlist(currentray, adjcandidate);
			}
			adjcandidate=adjcandidate->nextray;
		}
	currentray=currentray->nextray;
	while(currentray!=head->prevray) // loop over all rays in the raylist filling in each adjlist
	{
		adjcandidate=currentray->nextray;
		while (adjcandidate!=head) // we check from next ray to head, assuming that adjacencies with previous ray were added when the outer while loop iterated over that ray
		{	//printf("Computing adjacencies for ray %d", currentray->rayindex);
			if(adjtest_algebraic3(currentray,adjcandidate,Hset,dim)!=0)
			{
				//printf("add ray %d to %d's adjlist",adjcandidate->rayindex,currentray->rayindex);
				addray2adjlist(currentray, adjcandidate);
			}
			adjcandidate=adjcandidate->nextray;
		}
		currentray=currentray->nextray;
	}
}
