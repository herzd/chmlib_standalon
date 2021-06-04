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
#include "int32_eg_dbasis_red.h"
/** @file
 * @ingroup EGdBasisRed */
/** @addtogroup EGdBasisRed */
/** @{ */
/* ========================================================================= */
uintmax_t int32_EGdBsRedStats[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/* ========================================================================= */
/** @brief, build (from scratch) the GM matrix */
static void int32_EGdBsRedBuildGM (int32_EGdMatrix_t * const GM,
														 const size_t full_dim,
														 const size_t length,
														 int32_t ** const basis,
														 int32_t orilen,
														 int32_t tmpval)
{
	register size_t i, j;
	int32_t lval;
	int32_EGlpNumInitVar(lval);
	/* compute all norms and store them in the first row and in the diagonal. */
	int32_EGlpNumOne (orilen);
	for (i = full_dim; i--;)
	{
		int32_EGlpNumInnProd (lval, basis[i], basis[i], length);
		int32_EGlpNumCopy (GM->matrow[0][i], lval);
		int32_EGlpNumCopy (GM->matrow[i][i], lval);
		int32_EGlpNumMultTo (orilen, lval);
	}
	/* sort the vectors in increasing order according to norm, we do this because
	 * one of the conditions for the algorithm to stop is that the norms are in
	 * increasing order. */
	int32_EGutilPermSort (full_dim, GM->col_ord, (const int32_t*const)(GM->matrow[0]));
	memcpy (GM->row_ord, GM->col_ord, sizeof (int) * full_dim);
	/* fill-up the GM matrix with the inner products of the vectors */
	for (i = full_dim; i--;)
	{
		for (j = i + 1; j < full_dim; j++)
		{
			int32_EGlpNumInnProd (lval, basis[i], basis[j], length);
			int32_EGlpNumCopy (GM->matrow[i][j], lval);
			int32_EGlpNumCopy (GM->matrow[j][i], lval);
		}
	}
	int32_EGlpNumCopy(tmpval,lval);
	int32_EGlpNumClearVar(lval);
}

/* ========================================================================= */
int int32_EGdBsRed (int32_EGdBsRed_t * const bsred,
							unsigned *const ext_dim,
							const int32_t zero_tol,
							int *const ext_status)
{
	int32_EGdMatrix_t *const GM = &(bsred->GM);
	const size_t full_dim = bsred->dim;
	const size_t length = bsred->length;
	int32_t **const basis = bsred->basis;
	int rval = 0;
	int32_t tmpval,
	  tmpval2,
	  orilen,
	  endlen;
	size_t dim = 1;
	unsigned rank = 0;
	int status = EG_ALGSTAT_SUCCESS;
	int ge_status = 0;
	int tmpint;
	register size_t i,
	  j,
	  k;
	MESSAGE (int32_EG_DBSRED_VERBOSE, "Entering with dimension %zd length %zd", full_dim,
					 length);
	int32_EGdBsRedStats[int32_EG_BSRED_CALLS]++;
	int32_EGlpNumInitVar (tmpval);
	int32_EGlpNumInitVar (tmpval2);
	int32_EGlpNumInitVar (orilen);
	int32_EGlpNumInitVar (endlen);
	TESTG ((rval = (full_dim > length)), CLEANUP, "the length of the vectors "
				 "(%zd) is smaller than the number of vectors (%zd) being considered!",
				 length, full_dim);
	/* check that we do have at least two vectors in the basis, otherwise we are
	 * done. */
	if (full_dim < 2)
	{
		dim = full_dim;
		goto CLEANUP;
	}
	/* otherwise we have a non-trivial problem in our hands and we have to re-set
	 * everithing */
	int32_EGdMatrixSetDimension (GM, full_dim, full_dim);
	int32_EGlpNumOne (orilen);
	/* compute GM and sort the matrix according to the norms of the basis vectors
	 * */
	int32_EGdBsRedBuildGM (GM, full_dim, length, basis, orilen, tmpval);
#if int32_EG_DBSRED_VERBOSE +100<= DEBUG
	fprintf (stderr, "Order ");
	for (i = 0; i < full_dim; i++)
		fprintf (stderr, "%d ", GM->row_ord[i]);
	fprintf (stderr, "\nGM ");
	int32_EGdMatrixDisplay (GM, 0, stderr);
#endif
	/* main loop */
	while (dim < full_dim)
	{
		int32_EGdBsRedStats[int32_EG_BSRED_ITT]++;
		/* do gaussian elimination to compute the Gram-Smith multipliers */
		rval = int32_EGdMatrixGaussianElimination (GM, 0, 0, &rank, zero_tol, &ge_status);
		CHECKRVALG (rval, CLEANUP);
#if int32_EG_DBSRED_VERBOSE+100 <= DEBUG
		fprintf (stderr, "Order ");
		for (i = 0; i < full_dim; i++)
			fprintf (stderr, "%d ", GM->row_ord[i]);
		fprintf (stderr, "\nafter Gausian Elimination GM ");
		int32_EGdMatrixDisplay (GM, 0, stderr);
#endif
		/* if the gaussian elimination procedure does not fully finish, then we
		 * stop right now with status EG_ALGSTAT_PARTIAL or EG_ALGSTAT_NUMERROR */
		if (ge_status != EG_ALGSTAT_SUCCESS)
		{
			status = ge_status;
			break;
		}
		/* now we first try to find a \mu_ij > 1/2, if we do find such an element,
		 * we must do the reduction step of the algorithm */
		j = (size_t)dim;
		for (; j < (size_t)full_dim; j++)
		{
			for (i = j; i--;)
			{
				int32_EGlpNumCopy (tmpval, GM->matrow[GM->col_ord[i]][GM->col_ord[j]]);
				int32_EGlpNumDivTo (tmpval, GM->matrow[GM->col_ord[i]][GM->col_ord[i]]);
				int32_EGlpNumCopyAbs (tmpval2, tmpval);
				/* do the reduction step if needed */
				if (int32_EGlpNumIsGreaDbl (tmpval2, 0.5))
				{
					int32_EGdBsRedStats[int32_EG_BSRED_SZRED]++;
					/* bset tmpval2 = round(tmpval) */
					int32_EGlpNumFloor (tmpval2, tmpval);
					int32_EGlpNumSubTo (tmpval, tmpval2);
					if (int32_EGlpNumIsGreaDbl (tmpval, 0.5))
						int32_EGlpNumAddTo (tmpval2, int32_oneLpNum);
					MESSAGE (int32_EG_DBSRED_VERBOSE + 100, "Doing reduction for mu(%zu,%zu)=%lf,"
									 " multiple %lf", j, i, int32_EGlpNumToLf (tmpval),
									 int32_EGlpNumToLf (tmpval2));
					/* now we replace b_j by b_j - tmpval2 b_i */
					int32_EGdMatrixSubColMultiple (GM, GM->col_ord[j], GM->col_ord[i], tmpval2);
					for (k = (size_t)length; k--;)
						int32_EGlpNumSubInnProdTo (basis[GM->col_ord[j]][k],
																 basis[GM->col_ord[i]][k], tmpval2);
				}												/* done with the reduction */
			}													/* end checking column j of GM */
		}
#if int32_EG_DBSRED_VERBOSE+100 <= DEBUG
		fprintf (stderr, "Order ");
		for (i = 0; i < full_dim; i++)
			fprintf (stderr, "%d ", GM->row_ord[i]);
		fprintf (stderr, "\nafter Step I GM ");
		int32_EGdMatrixDisplay (GM, 0, stderr);
#endif
		/* now we are done with step I of the algorithm, now we check step 2, wich
		 * check for condition 
		 * | b^*_j + \mu_{j,j-1} b^*_{j-1}|^2 \geq \alpha |b^*_{j-1}|^2, where
		 * \alpha must be in (1/4,1). */
		for (i = 1; i < full_dim; i++)
		{
			int32_EGlpNumCopy (tmpval, GM->matrow[GM->col_ord[i - 1]][GM->col_ord[i]]);
			int32_EGlpNumMultTo (tmpval, tmpval);
			int32_EGlpNumDivTo (tmpval, GM->matrow[GM->col_ord[i - 1]][GM->col_ord[i - 1]]);
			int32_EGlpNumAddTo (tmpval, GM->matrow[GM->col_ord[i]][GM->col_ord[i]]);
			int32_EGlpNumDivTo (tmpval, GM->matrow[GM->col_ord[i - 1]][GM->col_ord[i - 1]]);
			/* check that we have to interchange b_j and b_{j-1}, we use lambda equal
			 * to 1048575/1048576 = (2^20-1)/2^20 */
			if (int32_EGlpNumIsLessDbl (tmpval, int32_EG_DBSRED_ALPHA))
			{
				int32_EGdBsRedStats[int32_EG_BSRED_INTR]++;
				MESSAGE (int32_EG_DBSRED_VERBOSE + 100, "Doing swap between columns %zu,%zu",
								 i - 1, i);
				int32_EGlpNumCopy (tmpval2, GM->matrow[GM->col_ord[i - 1]][GM->col_ord[i]]);
				int32_EGlpNumCopy (tmpval, tmpval2);
				int32_EGlpNumMultTo (tmpval, tmpval);
				int32_EGlpNumDivTo (tmpval2,
											GM->matrow[GM->col_ord[i - 1]][GM->col_ord[i - 1]]);
				int32_EGlpNumDivTo (tmpval,
											GM->matrow[GM->col_ord[i - 1]][GM->col_ord[i - 1]]);
				int32_EGlpNumAddTo (tmpval, GM->matrow[GM->col_ord[i]][GM->col_ord[i]]);
				int32_EGdMatrixAddRowMultiple (GM, GM->col_ord[i], GM->col_ord[i - 1],
																 tmpval2);
				int32_EGlpNumCopy (GM->matrow[GM->col_ord[i]][GM->col_ord[i]], tmpval);
				EGswap (GM->col_ord[i], GM->col_ord[i - 1], tmpint);
				EGswap (GM->row_ord[i], GM->row_ord[i - 1], tmpint);
				break;
			}													/* done doing the interchange of rows */
		}
		dim = i;
#if int32_EG_DBSRED_VERBOSE+100 <= DEBUG
		fprintf (stderr, "Order ");
		for (i = 0; i < full_dim; i++)
			fprintf (stderr, "%d ", GM->row_ord[i]);
		fprintf (stderr, "\nafter Step II GM ");
		int32_EGdMatrixDisplay (GM, 0, stderr);
#endif
		MESSAGE (int32_EG_DBSRED_VERBOSE + 100, "Current Dimension %zu", dim);
	}
	/* compute final length */
	int32_EGlpNumOne (endlen);
	for (i = (size_t)full_dim; i--;)
		int32_EGlpNumMultTo (endlen, GM->matrow[i][i]);
	/* ending */
CLEANUP:
	*ext_dim = (unsigned)dim;
	*ext_status = status;
	MESSAGE (int32_EG_DBSRED_VERBOSE, "Ending with dimmension %zu, status %d\nOriginal "
					 "Basis Length %lg, Final Length %lg, gain %lg", dim, status,
					 int32_EGlpNumToLf (orilen), int32_EGlpNumToLf (endlen),
					 int32_EGlpNumToLf (orilen) / int32_EGlpNumToLf (endlen));
	int32_EGlpNumClearVar (endlen);
	int32_EGlpNumClearVar (orilen);
	int32_EGlpNumClearVar (tmpval2);
	int32_EGlpNumClearVar (tmpval);
	return rval;
}

/* ========================================================================= */
/** @} */
