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
#include "mpf_eg_dbasis_red.h"
/** @file
 * @ingroup EGdBasisRed */
/** @addtogroup EGdBasisRed */
/** @{ */
/* ========================================================================= */
uintmax_t mpf_EGdBsRedStats[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/* ========================================================================= */
/** @brief, build (from scratch) the GM matrix */
static void mpf_EGdBsRedBuildGM (mpf_EGdMatrix_t * const GM,
														 const size_t full_dim,
														 const size_t length,
														 mpf_t ** const basis,
														 mpf_t orilen,
														 mpf_t tmpval)
{
	register size_t i, j;
	mpf_t lval;
	mpf_EGlpNumInitVar(lval);
	/* compute all norms and store them in the first row and in the diagonal. */
	mpf_EGlpNumOne (orilen);
	for (i = full_dim; i--;)
	{
		mpf_EGlpNumInnProd (lval, basis[i], basis[i], length);
		mpf_EGlpNumCopy (GM->matrow[0][i], lval);
		mpf_EGlpNumCopy (GM->matrow[i][i], lval);
		mpf_EGlpNumMultTo (orilen, lval);
	}
	/* sort the vectors in increasing order according to norm, we do this because
	 * one of the conditions for the algorithm to stop is that the norms are in
	 * increasing order. */
	mpf_EGutilPermSort (full_dim, GM->col_ord, (const mpf_t*const)(GM->matrow[0]));
	memcpy (GM->row_ord, GM->col_ord, sizeof (int) * full_dim);
	/* fill-up the GM matrix with the inner products of the vectors */
	for (i = full_dim; i--;)
	{
		for (j = i + 1; j < full_dim; j++)
		{
			mpf_EGlpNumInnProd (lval, basis[i], basis[j], length);
			mpf_EGlpNumCopy (GM->matrow[i][j], lval);
			mpf_EGlpNumCopy (GM->matrow[j][i], lval);
		}
	}
	mpf_EGlpNumCopy(tmpval,lval);
	mpf_EGlpNumClearVar(lval);
}

/* ========================================================================= */
int mpf_EGdBsRed (mpf_EGdBsRed_t * const bsred,
							unsigned *const ext_dim,
							const mpf_t zero_tol,
							int *const ext_status)
{
	mpf_EGdMatrix_t *const GM = &(bsred->GM);
	const size_t full_dim = bsred->dim;
	const size_t length = bsred->length;
	mpf_t **const basis = bsred->basis;
	int rval = 0;
	mpf_t tmpval,
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
	MESSAGE (mpf_EG_DBSRED_VERBOSE, "Entering with dimension %zd length %zd", full_dim,
					 length);
	mpf_EGdBsRedStats[mpf_EG_BSRED_CALLS]++;
	mpf_EGlpNumInitVar (tmpval);
	mpf_EGlpNumInitVar (tmpval2);
	mpf_EGlpNumInitVar (orilen);
	mpf_EGlpNumInitVar (endlen);
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
	mpf_EGdMatrixSetDimension (GM, full_dim, full_dim);
	mpf_EGlpNumOne (orilen);
	/* compute GM and sort the matrix according to the norms of the basis vectors
	 * */
	mpf_EGdBsRedBuildGM (GM, full_dim, length, basis, orilen, tmpval);
#if mpf_EG_DBSRED_VERBOSE +100<= DEBUG
	fprintf (stderr, "Order ");
	for (i = 0; i < full_dim; i++)
		fprintf (stderr, "%d ", GM->row_ord[i]);
	fprintf (stderr, "\nGM ");
	mpf_EGdMatrixDisplay (GM, 0, stderr);
#endif
	/* main loop */
	while (dim < full_dim)
	{
		mpf_EGdBsRedStats[mpf_EG_BSRED_ITT]++;
		/* do gaussian elimination to compute the Gram-Smith multipliers */
		rval = mpf_EGdMatrixGaussianElimination (GM, 0, 0, &rank, zero_tol, &ge_status);
		CHECKRVALG (rval, CLEANUP);
#if mpf_EG_DBSRED_VERBOSE+100 <= DEBUG
		fprintf (stderr, "Order ");
		for (i = 0; i < full_dim; i++)
			fprintf (stderr, "%d ", GM->row_ord[i]);
		fprintf (stderr, "\nafter Gausian Elimination GM ");
		mpf_EGdMatrixDisplay (GM, 0, stderr);
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
				mpf_EGlpNumCopy (tmpval, GM->matrow[GM->col_ord[i]][GM->col_ord[j]]);
				mpf_EGlpNumDivTo (tmpval, GM->matrow[GM->col_ord[i]][GM->col_ord[i]]);
				mpf_EGlpNumCopyAbs (tmpval2, tmpval);
				/* do the reduction step if needed */
				if (mpf_EGlpNumIsGreaDbl (tmpval2, 0.5))
				{
					mpf_EGdBsRedStats[mpf_EG_BSRED_SZRED]++;
					/* bset tmpval2 = round(tmpval) */
					mpf_EGlpNumFloor (tmpval2, tmpval);
					mpf_EGlpNumSubTo (tmpval, tmpval2);
					if (mpf_EGlpNumIsGreaDbl (tmpval, 0.5))
						mpf_EGlpNumAddTo (tmpval2, mpf_oneLpNum);
					MESSAGE (mpf_EG_DBSRED_VERBOSE + 100, "Doing reduction for mu(%zu,%zu)=%lf,"
									 " multiple %lf", j, i, mpf_EGlpNumToLf (tmpval),
									 mpf_EGlpNumToLf (tmpval2));
					/* now we replace b_j by b_j - tmpval2 b_i */
					mpf_EGdMatrixSubColMultiple (GM, GM->col_ord[j], GM->col_ord[i], tmpval2);
					for (k = (size_t)length; k--;)
						mpf_EGlpNumSubInnProdTo (basis[GM->col_ord[j]][k],
																 basis[GM->col_ord[i]][k], tmpval2);
				}												/* done with the reduction */
			}													/* end checking column j of GM */
		}
#if mpf_EG_DBSRED_VERBOSE+100 <= DEBUG
		fprintf (stderr, "Order ");
		for (i = 0; i < full_dim; i++)
			fprintf (stderr, "%d ", GM->row_ord[i]);
		fprintf (stderr, "\nafter Step I GM ");
		mpf_EGdMatrixDisplay (GM, 0, stderr);
#endif
		/* now we are done with step I of the algorithm, now we check step 2, wich
		 * check for condition 
		 * | b^*_j + \mu_{j,j-1} b^*_{j-1}|^2 \geq \alpha |b^*_{j-1}|^2, where
		 * \alpha must be in (1/4,1). */
		for (i = 1; i < full_dim; i++)
		{
			mpf_EGlpNumCopy (tmpval, GM->matrow[GM->col_ord[i - 1]][GM->col_ord[i]]);
			mpf_EGlpNumMultTo (tmpval, tmpval);
			mpf_EGlpNumDivTo (tmpval, GM->matrow[GM->col_ord[i - 1]][GM->col_ord[i - 1]]);
			mpf_EGlpNumAddTo (tmpval, GM->matrow[GM->col_ord[i]][GM->col_ord[i]]);
			mpf_EGlpNumDivTo (tmpval, GM->matrow[GM->col_ord[i - 1]][GM->col_ord[i - 1]]);
			/* check that we have to interchange b_j and b_{j-1}, we use lambda equal
			 * to 1048575/1048576 = (2^20-1)/2^20 */
			if (mpf_EGlpNumIsLessDbl (tmpval, mpf_EG_DBSRED_ALPHA))
			{
				mpf_EGdBsRedStats[mpf_EG_BSRED_INTR]++;
				MESSAGE (mpf_EG_DBSRED_VERBOSE + 100, "Doing swap between columns %zu,%zu",
								 i - 1, i);
				mpf_EGlpNumCopy (tmpval2, GM->matrow[GM->col_ord[i - 1]][GM->col_ord[i]]);
				mpf_EGlpNumCopy (tmpval, tmpval2);
				mpf_EGlpNumMultTo (tmpval, tmpval);
				mpf_EGlpNumDivTo (tmpval2,
											GM->matrow[GM->col_ord[i - 1]][GM->col_ord[i - 1]]);
				mpf_EGlpNumDivTo (tmpval,
											GM->matrow[GM->col_ord[i - 1]][GM->col_ord[i - 1]]);
				mpf_EGlpNumAddTo (tmpval, GM->matrow[GM->col_ord[i]][GM->col_ord[i]]);
				mpf_EGdMatrixAddRowMultiple (GM, GM->col_ord[i], GM->col_ord[i - 1],
																 tmpval2);
				mpf_EGlpNumCopy (GM->matrow[GM->col_ord[i]][GM->col_ord[i]], tmpval);
				EGswap (GM->col_ord[i], GM->col_ord[i - 1], tmpint);
				EGswap (GM->row_ord[i], GM->row_ord[i - 1], tmpint);
				break;
			}													/* done doing the interchange of rows */
		}
		dim = i;
#if mpf_EG_DBSRED_VERBOSE+100 <= DEBUG
		fprintf (stderr, "Order ");
		for (i = 0; i < full_dim; i++)
			fprintf (stderr, "%d ", GM->row_ord[i]);
		fprintf (stderr, "\nafter Step II GM ");
		mpf_EGdMatrixDisplay (GM, 0, stderr);
#endif
		MESSAGE (mpf_EG_DBSRED_VERBOSE + 100, "Current Dimension %zu", dim);
	}
	/* compute final length */
	mpf_EGlpNumOne (endlen);
	for (i = (size_t)full_dim; i--;)
		mpf_EGlpNumMultTo (endlen, GM->matrow[i][i]);
	/* ending */
CLEANUP:
	*ext_dim = (unsigned)dim;
	*ext_status = status;
	MESSAGE (mpf_EG_DBSRED_VERBOSE, "Ending with dimmension %zu, status %d\nOriginal "
					 "Basis Length %lg, Final Length %lg, gain %lg", dim, status,
					 mpf_EGlpNumToLf (orilen), mpf_EGlpNumToLf (endlen),
					 mpf_EGlpNumToLf (orilen) / mpf_EGlpNumToLf (endlen));
	mpf_EGlpNumClearVar (endlen);
	mpf_EGlpNumClearVar (orilen);
	mpf_EGlpNumClearVar (tmpval2);
	mpf_EGlpNumClearVar (tmpval);
	return rval;
}

/* ========================================================================= */
/** @} */
