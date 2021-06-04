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
#ifndef int32___EG_DMATRIX_H__
#define int32___EG_DMATRIX_H__
#include "eg_config.h"
#include "eg_macros.h"
#include "eg_mem.h"
#include "eg_lpnum.h"
#include "int32_eg_numutil.h"

/* ========================================================================= */
/** @defgroup EGdMatrix Dense Matrices
 * Here we define a common interface for dense matrices (i.e. a structure), and
 * some common operations over dense matrices. The definition uses EGlpNum as
 * reference number type, this allow for template initializations.
 * 
 * @par History:
 * Revision 0.0.2
 *  - 2005-10-27
 *  					- First implementation.
 * */
/** @{*/
/** @file
 * @brief This file provide the user interface and function definitions for
 * Dense Matrices.
 * */
/** @example int32_eg_dmatrix.ex.c */
/* ========================================================================= */
/** @brief structure to hold a dense matrix, we choose a row representation
 * of the matrix, and we allow row and column permutations. All actual values 
 * in the matrix are stored in #int32_EGdMatrix_t::matval, and the rows in
 * #int32_EGdMatrix_t::matrow. */
typedef struct int32_EGdMatrix_t
{
	size_t col_sz;		/**< @brief Number of columns in the matrix. */
	size_t row_sz;		/**< @brief Number of rows in the matrix */
	int32_t **matrow;
										/**< @brief Array of size #int32_EGdMatrix_t::row_sz containing 
												 all rows of the matrix */
	int32_t *matval;/**< @brief Values for all entries */
	int *col_ord;			/**< @brief Array of size at least #int32_EGdMatrix_t::col_sz 
												 containing the order ammong all columns i.e. it is a 
												 permutation of {0,....,col_sz-1} which is how the 
												 matrix is treated internally */
	int *row_ord;			/**< @brief Array of size at least #int32_EGdMatrix_t::row_sz 
												 containing the order ammong all rows, i.e. it is a 
												 permutation of {0,...,row_sz-1} which is how the 
												 matrix is treated internally */
}
int32_EGdMatrix_t;

/* ========================================================================= */
/** @brief Initialize (as a dense matrix of dimension 0x0) an #int32_EGdMatrix_t
 * structure.
 * @param __dmatrix dense matrix structure pointer.
 * */
#define int32_EGdMatrixInit(__dmatrix) memset(__dmatrix,0,sizeof(int32_EGdMatrix_t))

/* ========================================================================= */
/** @brief Clear a dense matrix structure, i.e. free all internally allocated
 * data of the structure. Note that no further use of the structure can be made
 * unless it is re-initialized and set to a suitable size.
 * @param __dmatrix dense matrix structure pointer.
 * */
#define int32_EGdMatrixClear(__dmatrix) do{\
	int32_EGdMatrix_t*const __EGdm = (__dmatrix);\
	int32_EGlpNumFreeArray(__EGdm->matval);\
	EGfree(__EGdm->matrow);\
	int_EGlpNumFreeArray(__EGdm->col_ord);\
	int_EGlpNumFreeArray(__EGdm->row_ord);} while(0)

/* ========================================================================= */
/** @brief Set new dimensions for a dense matrix structure.
 * @param __dmatrix dense matrix structure pointer.
 * @param __nnewrows number of rows in the matrix.
 * @param __nnewcols number of columns in the matrix.
 * @note Take care that the values stored in the matrix are not initialized to
 * any particular number. Also the ordering (for both column and row) is reset
 * to the standard ordering 0,....,n.
 * */
#define int32_EGdMatrixSetDimension(__dmatrix,__nnewrows,__nnewcols) do{\
	int32_EGdMatrix_t*const __EGdm = (__dmatrix);\
	register int __EGdmi;\
	__EGdm->col_sz = (__nnewcols);\
	__EGdm->row_sz = (__nnewrows);\
	int32_EGlpNumReallocArray(&(__EGdm->matval),__EGdm->col_sz * __EGdm->row_sz);\
	EGrealloc(__EGdm->matrow,__EGdm->row_sz * sizeof(int32_t*));\
	int_EGlpNumReallocArray(&(__EGdm->col_ord),__EGdm->col_sz);\
	int_EGlpNumReallocArray(&(__EGdm->row_ord),__EGdm->row_sz);\
	__EGdmi = (int)(__EGdm->col_sz);\
	while(__EGdmi--) __EGdm->col_ord[__EGdmi] = __EGdmi;\
	__EGdmi = (int)(__EGdm->row_sz);\
	while(__EGdmi--) \
		__EGdm->matrow[__EGdmi] = __EGdm->matval + ((size_t)(__EGdmi) * __EGdm->col_sz);\
	__EGdmi = (int)(__EGdm->row_sz);\
	while(__EGdmi--) __EGdm->row_ord[__EGdmi] = __EGdmi;} while(0)

/* ========================================================================= */
/** @brief Display a given #int32_EGdMatrix_t structure contents.
 * @param __dmatrix dense matrix structure pointer.
 * @param __nat_order if set to one, display the matrix using the natural 
 * internal order, i.e. we discard the order of columns and rows as defined in
 * #int32_EGdMatrix_t::col_ord and #int32_EGdMatrix_t::row_ord. Otherwise, use such orders.
 * @param __ofile pointer to a FILE structure where we want the output to be
 * printed.
 * */
#define int32_EGdMatrixDisplay(__dmatrix,__nat_order,__ofile) do{\
	int32_EGdMatrix_t*const __EGdm = (__dmatrix);\
	char* __EGdmstr = 0;\
	size_t __EGdmi, __EGdmj;\
	fprintf(__ofile,"Matrix %p\nDimensions: %zd rows, %zd columns\n", (void*)__EGdm, __EGdm->row_sz, __EGdm->col_sz);\
	if(__nat_order){\
		for(__EGdmi = 0 ; __EGdmi < __EGdm->row_sz ; __EGdmi++){\
			for(__EGdmj = 0 ; __EGdmj < __EGdm->col_sz ; __EGdmj++){\
				__EGdmstr = int32_EGlpNumGetStr(__EGdm->matrow[__EGdmi][__EGdmj]);\
				fprintf(__ofile,"%10s ", __EGdmstr);\
				EGfree(__EGdmstr);\
			}\
			fprintf(__ofile,"\n");}\
	} else {\
		for(__EGdmi = 0 ; __EGdmi < __EGdm->row_sz ; __EGdmi++){\
			for(__EGdmj = 0 ; __EGdmj < __EGdm->col_sz ; __EGdmj++){\
				__EGdmstr = int32_EGlpNumGetStr(__EGdm->matrow[__EGdm->row_ord[__EGdmi]][__EGdm->col_ord[__EGdmj]]);\
				fprintf(__ofile,"%10s ", __EGdmstr);\
				EGfree(__EGdmstr);\
			}\
			fprintf(__ofile,"\n");}\
	}} while(0)

/* ========================================================================= */
/** @brief Given a number '__num' and a two rows '__orig', '__dest', set rows 
 * '__dest' to '__dest' + '__orig' * '__num'. Note that the number MUST_NOT be stored 
 * in row '__dest', and note that rows '__orig' and '__dest' should be different.
 * This is needed because of the way GNU_MP interface works.
 * @param __dmatrix dense matrix structure pointer.
 * @param '__orig' index of the row whose multiple will be added to the '__dest'
 * row.
 * @param '__dest' row to be replaced by '__dest' + '__orig' * '__num'.
 * @param '__num' constant to be multiply to the '__orig' and be added to the 
 * '__dest' row.
 * @note The index of the row are taken as internal index, i.e. if we give row
 * 'k' we will use the row stored in #int32_EGdMatrix_t::matrow[k], wich does not
 * mean that we will access the k-th row in the matrix (wich would need to use
 * as index the value #int32_EGdMatrix_t::row_ord[k] instead). Note that we don't
 * test wether the given multiple is zero or not. we always perform the
 * operation.
 * */
#define int32_EGdMatrixAddRowMultiple(__dmatrix,__dest,__orig,__num) do{\
	int32_EGdMatrix_t*const __EGdm = (__dmatrix);\
	const size_t __EGdest = (size_t)(__dest);\
	const size_t __EGori = (size_t)(__orig);\
	size_t __EGdmj = __EGdm->col_sz;\
	while(__EGdmj--) \
		int32_EGlpNumAddInnProdTo(__EGdm->matrow[__EGdest][__EGdmj],\
												__EGdm->matrow[__EGori][__EGdmj],__num);\
	} while(0)

/* ========================================================================= */
/** @brief Given a number '__num' and a two rows '__orig', '__dest', set rows 
 * '__dest' to '__dest' - '__orig' * '__num'. Note that the number MUST_NOT be stored 
 * in row '__dest', and note that rows '__orig' and '__dest' should be different.
 * This is needed because of the way GNU_MP interface works.
 * @param __dmatrix dense matrix structure pointer.
 * @param '__orig' index of the row whose multiple will be added to the '__dest'
 * row.
 * @param '__dest' row to be replaced by '__dest' - '__orig' * '__num'.
 * @param '__num' constant to be multiply to the '__orig' and be added to the 
 * '__dest' row.
 * @note The index of the row are taken as internal index, i.e. if we give row
 * 'k' we will use the row stored in #int32_EGdMatrix_t::matrow[k], wich does not
 * mean that we will access the k-th row in the matrix (wich would need to use
 * as index the value #int32_EGdMatrix_t::row_ord[k] instead). Note that we don't
 * test wether the given multiple is zero or not. we always perform the
 * operation.
 * */
#define int32_EGdMatrixSubRowMultiple(__dmatrix,__dest,__orig,__num) do{\
	int32_EGdMatrix_t*const __EGdm = (__dmatrix);\
	const size_t __EGdest = (__dest);\
	const size_t __EGori = (__orig);\
	size_t __EGdmj = __EGdm->col_sz;\
	while(__EGdmj--) \
		int32_EGlpNumSubInnProdTo(__EGdm->matrow[__EGdest][__EGdmj],\
												__EGdm->matrow[__EGori][__EGdmj],__num);\
	} while(0)
/* ========================================================================= */
/** @brief Given a number and a row, multiply the complete row by the given
 * number. Note that the number MUST_NOT be stored in the row being multiplied,
 * this is because of the way GNU_MP interface works.
 * @param __dmatrix dense matrix structure pointer.
 * @param row_ind index of the row being multiplied, note that we will multiply
 * the row stored in #int32_EGdMatrix_t::matrow[row_ind], wich is different to say
 * that we multiply the row in the row_ind-th position in the row ordering (to
 * do that, then row_ind should be #int32_EGdMatrix_t::row_ord[k]).
 * @param multiple constant to be multiply to the row.
 * */
#define int32_EGdMatrixMultiplyRow(__dmatrix,row_ind,multiple) do{\
	int32_EGdMatrix_t*const __EGdm = (__dmatrix);\
	const size_t __EGdmi = (row_ind);\
	size_t __EGdmj = __EGdm->col_sz;\
	while(__EGdmj--) int32_EGlpNumMultTo(__EGdm->matrow[__EGdmi][__EGdmj],multiple);\
	} while(0)

/* ========================================================================= */
/** @brief Given a number '__num' and a two rows '__orig', '__dest', set columns 
 * '__dest' to '__dest' + '__orig' * '__num'. Note that the number MUST_NOT be stored 
 * in column '__dest', and note that columns '__orig' and '__dest' should be 
 * different. This is needed because of the way GNU_MP interface works.
 * @param __dmatrix dense matrix structure pointer.
 * @param '__orig' index of the column whose multiple will be added to the '__dest'
 * column.
 * @param '__dest' column to be replaced by '__dest' + '__orig' * '__num'.
 * @param '__num' constant to be multiply to the '__orig' and be added to the 
 * '__dest' column.
 * @note The index of the column are taken as internal index, i.e. if we give 
 * column 'k' we will use the column stored in #int32_EGdMatrix_t::matrow[*][k], 
 * wich does not mean that we will access the k-th column in the matrix (wich
 * would need to use as index the value #int32_EGdMatrix_t::row_ord[k] instead). 
 * Note that we don't test wether the given multiple is zero or not. we 
 * always perform the operation.
 * */
#define int32_EGdMatrixAddColMultiple(__dmatrix,__dest,__orig,__num) do{\
	int32_EGdMatrix_t*const __EGdm = (__dmatrix);\
	const size_t __EGdest = (__dest);\
	const size_t __EGori = (__orig);\
	size_t __EGdmj = __EGdm->row_sz;\
	while(__EGdmj--) \
		int32_EGlpNumAddInnProdTo(__EGdm->matrow[__EGdmj][__EGdest],\
												__EGdm->matrow[__EGdmj][__EGori],__num);\
	} while(0)

/* ========================================================================= */
/** @brief Given a number '__num' and a two rows '__orig', '__dest', set columns 
 * '__dest' to '__dest' - '__orig' * '__num'. Note that the number MUST_NOT be stored 
 * in column '__dest', and note that columns '__orig' and '__dest' should be 
 * different. This is needed because of the way GNU_MP interface works.
 * @param __dmatrix dense matrix structure pointer.
 * @param '__orig' index of the column whose multiple will be added to the '__dest'
 * column.
 * @param '__dest' column to be replaced by '__dest' - '__orig' * '__num'.
 * @param '__num' constant to be multiply to the '__orig' and be added to the 
 * '__dest' column.
 * @note The index of the column are taken as internal index, i.e. if we give 
 * column 'k' we will use the column stored in #int32_EGdMatrix_t::matrow[*][k], 
 * wich does not mean that we will access the k-th column in the matrix (wich 
 * would need to use as index the value #int32_EGdMatrix_t::col_ord[k] instead). 
 * Note that we don't test wether the given multiple is zero or not. we 
 * always perform the operation.
 * */
#define int32_EGdMatrixSubColMultiple(__dmatrix,__dest,__orig,__num) do{\
	int32_EGdMatrix_t*const __EGdm = (__dmatrix);\
	const size_t __EGdest = (size_t)(__dest);\
	const size_t __EGori = (size_t)(__orig);\
	size_t __EGdmj = __EGdm->row_sz;\
	while(__EGdmj--) \
		int32_EGlpNumSubInnProdTo(__EGdm->matrow[__EGdmj][__EGdest],\
												__EGdm->matrow[__EGdmj][__EGori],__num);\
	} while(0)
/* ========================================================================= */
/** @brief Given a number and a column, multiply the complete column by the 
 * given number. Note that the number MUST_NOT be stored in the column being 
 * multiplied, this is because of the way GNU_MP interface works.
 * @param __dmatrix dense matrix structure pointer.
 * @param __colind index of the column being multiplied, note that we will 
 * multiply the column stored in #int32_EGdMatrix_t::matrow[*][__colind], wich is 
 * different to say that we multiply the column in the __colind-th position in
 * the column ordering (to do that, then __colind should be 
 * #int32_EGdMatrix_t::col_ord[k]).
 * @param __mult constant to be multiply to the column.
 * */
#define int32_EGdMatrixMultiplyCol(__dmatrix,__colind,__mult) do{\
	int32_EGdMatrix_t*const __EGdm = (__dmatrix);\
	const size_t __EGdmi = (__colind);\
	size_t __EGdmj = __EGdm->row_sz;\
	while(__EGdmj--) int32_EGlpNumMultTo(__EGdm->matrow[__EGdmj][__EGdmi],__mult);\
	} while(0)


/* ========================================================================= */
/** @brief This function performs gaussian elimination to the given matrix,
 * depending on the given options it may do row/columns permutations allong the
 * way to improve numerical stabillity.
 * @param __dmatrix dense matrix structure pointer.
 * @param do_col_perm if set to one, the try columns permutation to improve
 * numericall stabillity, otherwise, not do column permutations at all.
 * @param do_row_perm if set to one, try row permutations to improve numericall
 * stabillity, otherwise, not do row permutations at all.
 * @param status pointer to where return an status, if the procedure finish all
 * the way (i.e. the matrix is full rank), then we return #EG_ALGSTAT_SUCCESS,
 * if the matrix is found to be partial rank, the status is
 * #EG_ALGSTAT_PARTIAL, otherwise, we return #EG_ALGSTAT_NUMERROR, wich means
 * that we stoped because a zero pivot was found (after checking for allowed
 * row/collumns permmutations).
 * @param rank where to return the (proven) rank of the matrix. This number is
 * accurate if the status is #EG_ALGSTAT_SUCCESS, or #EG_ALGSTAT_PARTIAL, but
 * is just a lower bound if the status is #EG_ALGSTAT_NUMERROR
 * @param zero_tol What is the threshold for a value to be considered zero.
 * @return if no error happen, we return zero, otherwise a non-zero valued is
 * returned. Note that the algorithm status is independent of the return value,
 * non zero values araise only if an error happen during execution, wich is
 * different to say that the algorithm didn't finish correctly. */
int int32_EGdMatrixGaussianElimination (int32_EGdMatrix_t * const __dmatrix,
																	const unsigned do_col_perm,
																	const unsigned do_row_perm,
																	unsigned *const rank,
																	const int32_t zero_tol,
																	int *const status);

/* ========================================================================= */
/** @}*/
#endif
