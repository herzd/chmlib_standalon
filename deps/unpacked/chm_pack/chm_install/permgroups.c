#include "permgroups.h"
#include "chmutils.h"
int main(int argc, char *argv[])
{
	int i,j,num=0;
	int**gens=readgens(argv[1],&num);
	fprintf(stderr,"Done with readgens\n");
	for(i=0;i<num;i++)
	{
		for(j=1;j<gens[i][0]+1;j++)
		fprintf(stderr,"%d  ",gens[i][j]);
		fprintf(stderr,"\n");
	}
	return 0;
}

int** readgens(char* filename,int* num)
{
	FILE *fp; 
	int m,n,i,j,firsttime,skip,genlen;
	char tempstr[15];
	const char* gentag="gens";
	char buf[4096]; 
	char* linptr;
	fp=fopen(filename,"r");
	firsttime=FALSE;
	int** gens;
	while(fp!=NULL && fgets(buf, sizeof(buf), fp)!=NULL)
	{
		linptr=strstr(buf, "gens");
		if (linptr !=NULL && firsttime==FALSE)
		{
			linptr=linptr+4;
			sscanf ( linptr, "%d", num);
			sprintf(tempstr, "%d", num[0]);
			linptr=strstr(buf,tempstr)+strlen(tempstr);
			while(linptr[0]==9 || linptr[0]==32)
			{
				linptr=linptr+1;
			}
			gens=(int**)malloc(num[0]*sizeof(int*));
			firsttime=TRUE;
			break;
		}
	}
	for (i=0;i<num[0];i++)
	{
		fprintf(stderr,"gen %d",i+1);
		fscanf(fp,"%d",&genlen);
		gens[i]=(int*)malloc((genlen+1)*sizeof(int));
		gens[i][0]=genlen;
		for(j=1;j<genlen+1;j++)
		{
			fscanf(fp,"%d",&gens[i][j]);
		}
	}
	fprintf(stderr,"Found %d gens\n",num[0]);
	return gens;
}
