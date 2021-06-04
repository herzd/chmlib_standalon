/* EGlib "Efficient General Library" provides some basic structures and
 * algorithms commons in many optimization algorithms.
 *
 * Copyright (C) 2005 Daniel Espinoza and Marcos Goycoolea.
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
/* ========================================================================= */
#include "eg_cplex.h"
#include "eg_io.h"
/** @file
 * @ingroup EGcpx */
/** @{ */
char __EGcpx_errbuf__[4096];
#if HAVE_CPLEX
/* ========================================================================= */
/** @brief macro that change integer params of cplex, and report changes into
 * screen, and test return values, it asumes that params are readed from
 * argv[0], argv[1] and argc, and assumes that env, rval, CLEANUP, verbose are
 * defined and have the usual meanning */
#define EGcpxReadIntparam(__EGCPX_BASE_PARAM__) \
	if(strncmp(argv[0], #__EGCPX_BASE_PARAM__,(size_t)64)==0){\
		int __EGint__=0;\
		TESTG((rval=(argc!=2)),CLEANUP,#__EGCPX_BASE_PARAM__" has not 2 tokens");\
		if(strncmp(argv[1],"ON",(size_t)64)==0) __EGint__= CPX_ON;\
		else if(strncmp(argv[1],"OFF",(size_t)64)==0) __EGint__ = CPX_OFF;\
		else if(strncmp(argv[1],"PRIMAL",(size_t)64)==0) __EGint__ = CPX_ALG_PRIMAL;\
		else if(strncmp(argv[1],"DUAL",(size_t)64)==0) __EGint__ = CPX_ALG_DUAL;\
		else if(strncmp(argv[1],"BARRIER",(size_t)64)==0) __EGint__ = CPX_ALG_BARRIER;\
		else __EGint__ = atoi(argv[1]);\
		EGcpxSetintparam(CPX_PARAM_ ## __EGCPX_BASE_PARAM__,__EGint__);\
		IFMESSAGE(verbose,"Set "#__EGCPX_BASE_PARAM__" to %s (%d), ",argv[1],__EGint__);}
/* ========================================================================= */
/** @brief macro that change double params of cplex, and report changes into
 * screen, and test return values, it asumes that params are readed from
 * argv[0], argv[1] and argc, and assumes that env, rval, CLEANUP, verbose are
 * defined and have the usual meanning */
#define EGcpxReadDblparam(__EGCPX_BASE_PARAM__) \
	if(strncmp(argv[0], #__EGCPX_BASE_PARAM__,(size_t)64)==0){\
		TESTG((rval=(argc!=2)),CLEANUP,#__EGCPX_BASE_PARAM__" has not 2 tokens");\
		EGcpxCallD(CPXsetdblparam(env,CPX_PARAM_ ## __EGCPX_BASE_PARAM__,strtod(argv[1],0)));\
		IFMESSAGE(verbose,"Set "#__EGCPX_BASE_PARAM__" to %s (%lg)",argv[1],strtod(argv[1],0));}
/* ========================================================================= */
int EGcpxReadOptions(CPXENVptr env, FILE * inputfile)
{
	const char delim[3] = " :";
	const char coment[3] = "%#";
	char str[2048], *argv[1024];
	int argc, rval=0, todo = 1, verbose = 1;
	EGcpxSetintparam(CPX_PARAM_CLOCKTYPE,1);
	while(todo)
	{
		EGcallD(EGioReadLine (str, (size_t) 2048, inputfile));
		TESTG((rval=(feof(inputfile)||ferror(inputfile))), CLEANUP, 
				"file ended prematurelly");
		EGioNParse (str, 1024, delim, coment, &argc, argv);
		/* skip empty lines */
		if (!argc) continue;
		/* for the moment, we discard this information */
		if (strncmp (argv[0], "NAME", (size_t) 33) == 0) continue;
		/* for the moment, we discard this information */
		else if (strncmp (argv[0], "COMMENT", (size_t) 33) == 0) continue;
		/* for the moment, check type equal to GEOMIN */
		else if (strncmp (argv[0], "TYPE", (size_t) 33) == 0)
		{
			TESTG((rval=(argc!=2)), CLEANUP, "TYPE has not 2 tokens");
			TESTG((rval=(strncmp (argv[1], "OPTIONFILE", (size_t) 33))), CLEANUP, 
					"File TYPE is not OPTIONFILE, instead %s", argv[1]);
		}
		else EGcpxReadIntparam(AGGFILL)
		else EGcpxReadIntparam(AGGIND)
		else EGcpxReadIntparam(BARDISPLAY)
		else EGcpxReadIntparam(BBINTERVAL)
		else EGcpxReadIntparam(HEURFREQ)
		else EGcpxReadIntparam(INTSOLLIM)
		else EGcpxReadIntparam(LBHEUR)
		else EGcpxReadIntparam(MIPCBREDLP)
		else EGcpxReadIntparam(MIPDISPLAY)
		else EGcpxReadIntparam(MIPEMPHASIS)
		else EGcpxReadIntparam(MIPINTERVAL)
		else EGcpxReadIntparam(NODELIM)
		else EGcpxReadIntparam(PREIND)
		else EGcpxReadIntparam(PROBE)
		else EGcpxReadIntparam(RINSHEUR)
		else EGcpxReadIntparam(SCRIND)
		else EGcpxReadIntparam(STARTALG)
		else EGcpxReadIntparam(SUBALG)
		else EGcpxReadIntparam(THREADS)
		else EGcpxReadIntparam(VARSEL)
		/* pre-processing */
		else EGcpxReadIntparam(REPEATPRESOLVE)
		else EGcpxReadIntparam(RELAXPREIND)
		else EGcpxReadIntparam(PRESLVND)
		else EGcpxReadIntparam(PREPASS)
		else EGcpxReadIntparam(PREDUAL)
		else EGcpxReadIntparam(PREIND)
		else EGcpxReadIntparam(PRELINEAR)
		else EGcpxReadIntparam(REDUCE)
		else EGcpxReadIntparam(SYMMETRY)
		/* cuts */
		else EGcpxReadIntparam(CUTPASS)
		else EGcpxReadIntparam(COVERS)
		else EGcpxReadIntparam(GUBCOVERS)
		else EGcpxReadIntparam(FLOWCOVERS)
		else EGcpxReadIntparam(CLIQUES)
		else EGcpxReadIntparam(FRACCUTS)
		else EGcpxReadIntparam(FRACCAND)
		else EGcpxReadIntparam(FRACPASS)
		else EGcpxReadIntparam(MIRCUTS)
		else EGcpxReadIntparam(FLOWPATHS)
		else EGcpxReadIntparam(DISJCUTS)
		else EGcpxReadIntparam(IMPLBD)
		#ifdef CPX_PARAM_ZEROHALFCUTS
		else EGcpxReadIntparam(ZEROHALFCUTS)
		#else
		else if (strncmp (argv[0], "ZEROHALFCUTS", (size_t)33)==0)
			fprintf(stdout,"WARNING Not setting ZEROHALFCUTS CPLEX VERSION %.2lf < 11.00\n",((double)CPX_VERSION)/100.0);
		#endif
		#ifdef CPX_PARAM_MCFCUTS
		else EGcpxReadIntparam(MCFCUTS)
		#else
		else if(strncmp(argv[0],"MCFCUTS",(size_t)33)==0)
			fprintf(stdout,"WARNING Not setting MCFCUTS CPLEX VERSION %.2lf < 12.00\n", ((double)CPX_VERSION)/100.0);
		#endif
		/* double params */
		else EGcpxReadDblparam(CUTLO)
		else EGcpxReadDblparam(CUTSFACTOR)
		else EGcpxReadDblparam(CUTUP)
		else EGcpxReadDblparam(EPAGAP)
		else EGcpxReadDblparam(EPGAP)
		else EGcpxReadDblparam(EPINT)
		else EGcpxReadDblparam(TILIM)
		else EGcpxReadDblparam(OBJDIF)
		else EGcpxReadDblparam(RELOBJDIF)
		else if (strncmp (argv[0], "EOF", (size_t) 33) == 0)
		{
			todo = 0;
		}
		/* default case */
		else
		{
			rval = 1;
			printf ("Unknown token %s\n", argv[0]);
			goto CLEANUP;
		}
		/* end loop */
	}
	CLEANUP:
	EG_RETURN(rval);
}
/* ========================================================================= */
#endif
/** @} */
