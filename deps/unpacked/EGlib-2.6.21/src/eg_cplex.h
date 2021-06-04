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
/** @defgroup EGcpx EGcpx
 *
 * Here we define common error-handling functions and utilities to work with
 * CPLEX 
 *
 * @version 1.0.0
 * @par History:
 * - 2012-06-08
 * 		- First port of the implementation
 * */
/** @example example/cplex.opt
 * this is an example of a configuration file for CPLEX
 * */
/** @file 
 * @ingroup EGcpx */
/** @addtogroup EGcpx */
/** @{ */
/* ========================================================================= */
#ifndef __EG_CPLEX_H__
#define __EG_CPLEX_H__
#include "eg_config.h"
#include "eg_macros.h"
/* ========================================================================= */
/** @brief internal error string for cplex */
extern char __EGcpx_errbuf__[4096];
/* ========================================================================= */
/** @brief print error-messages when a CPLEX routine fails
 * @param __rval cplex return value (zero means success)
 * @param __env environment as returned by CPXopenCPLEX
 * @param __where point where the execution should jump if an error is
 * encountered */
#define EGcpxCHECKRVALG(__env,__rval,__where) do{\
	if(__rval)\
	{\
		fflush(stdout);\
		CPXgeterrorstring(__env,__rval,__EGcpx_errbuf__);\
		fprintf(stderr,"\n");/* to ensure correct aignment of error message */\
		fprintf(stderr,__EGcpx_errbuf__);\
		__EG_PRINTLOC__;\
		goto __where;\
	}}\
while(0)
/* ========================================================================= */
/** @brief equivalent of EGcall for CPXxxx functions */
#define EGcpxCall(__env__,__rval__,__where__,__cpxfunc__) {\
	const int __EGrval__=__cpxfunc__;\
	EGcpxCHECKRVALG(__env__,((__rval__)=__EGrval__),__where__);}
/* ========================================================================= */
/** @brief equivalent of EGcallD for CPXxxx functions, we assume env, rval, and
 * CLEANUP as default values */
#define EGcpxCallD(__cpxfunc2__) EGcpxCall(env,rval,CLEANUP,__cpxfunc2__)
/* ========================================================================= */
/** @brief macro for calling with return value checks CPXsetintparam, it
 * assumes that env, rval, CLEANUP are defined and have usual meaning */
#define EGcpxSetintparam(__EGCPX_PARAM__,__EGCPX_VAL__) \
	EGcpxCallD(CPXsetintparam(env,(__EGCPX_PARAM__),(__EGCPX_VAL__)))
/* ========================================================================= */
/* now we define functions that require de types definitions of CPLEX */
#if HAVE_CPLEX
/* ========================================================================= */
/** @brief read optionfile and set parameters in the given environment.
 * @param env pointer to the CPLEX envitronment
 * @param inputfile source file
 * @return zero on success.
 * */
int EGcpxReadOptions(CPXENVptr env, FILE*inputfile);
#endif
#endif
/* ========================================================================= */
/** @} */
/* end of eg_cplex.h */
