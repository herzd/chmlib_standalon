/* EGlib "Efficient General Library" provides some basic structures and
 * algorithms commons in many optimization algorithms.
 *
 * Copyright (C) 2005-2008 Daniel Espinoza.
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public 
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA 
 * */
/** @file 
 * @ingroup EGsymtab */
/** @addtogroup EGsymtab */
/** @{ */
/* ========================================================================= */
/** @brief This program reads a file given from the input and try to put all
 * words within the symbol table. 
 * */
/* ========================================================================= */
#include "EGlib.h"
/* ========================================================================= */
static char file_name[1024]=""; /**< @brief where we store input file name */
/* ========================================================================= */
/** @brief parsing arguments */
void symtab_usage(char*program)
{
	fprintf(stdout,"Usage: %s [options]\n",program);
	fprintf(stdout,"Options:\n\t-f f file name to read\n");
}
/* ========================================================================= */
/** @brief parse input */
int symtab_parse(int argc,char**argv)
{
	int c;
	if(argc<3) 
	{
		symtab_usage(argv[0]);
		return 1;
	}
	while((c = getopt(argc,argv,"f:")) != EOF)
	{
		switch(c)
		{
			case 'f':
				snprintf(file_name,(size_t)1023,"%s",optarg);
				break;
			default:
				symtab_usage(argv[0]);
				return 1;
		}
	}
	return 0;
}
/* ========================================================================= */
/** @brief main function */
int main (int argc,char**argv)
{
	const char delim[2] = " ";
	const char coment[3] = "%#";
	char str[4096],*tok[1024];
	int rval=0,ntok,i;
	FILE*fin;
	EGsymtab_t symtab;
	EGlib_info();
	EGlib_version();
	EGsymtabInit(&symtab);
	rval = symtab_parse(argc,argv);
	CHECKRVALG(rval,CLEANUP);
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	/* now we open the file */
	fin = EGsfopen(file_name,"r");
	/* now we read line by line, and add each new word to the table */
	while(1)
	{
		rval = EGioReadLine(str,(size_t)4095,fin);
		if(feof(fin)||ferror(fin)) break;
		CHECKRVALG(rval,CLEANUP);
		EGioNParse(str,1024,delim,coment,&ntok,tok);
		for( i = 0 ; i < ntok ; i++)
		{
			if(!EGsymtabLookUp(&symtab,tok[i],0))
				EGsymtabAdd(&symtab,tok[i]);
		}
	}
	fclose(fin);
	/* now we display the contents in the table */
	fprintf(stdout,"Found %zu symbols in file %s\n",symtab.nsym,file_name);
	for( i = ((int)symtab.nsym) ; i-- ; )
		fprintf(stdout,"%s\n",EGsymtabSymbol(&symtab,i));
	CLEANUP:
	EGsymtabClear(&symtab);
	return rval;
}
/* ========================================================================= */
/** @} */
