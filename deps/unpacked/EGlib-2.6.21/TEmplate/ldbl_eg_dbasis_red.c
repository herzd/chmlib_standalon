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
#include "ldbl_eg_dbasis_red.h"
/** @file
 * @ingroup EGdBasisRed */
/** @addtogroup EGdBasisRed */
/** @{ */
/* ========================================================================= */
uintmax_t ldbl_EGdBsRedStats[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/* ========================================================================= */
/** @brief, build (from scratch) the GM matrix */
static void ldbl_EGdBsRedBuildGM (ldbl_EGdMatrix_t * const GM,
														 const size_t full_dim,
														 const size_t length,
														 long double ** const basis,
														 long double orilen,
														 long double tmpval)
{
	register size_t i, j;
	long double lval;
	ldbl_EGlpNumInitVar(lval);
	/* compute all norms and store them in the first row and in the diagonal. */
	ldbl_EGlpNumOne (orilen);
	for (i = full_dim; i--;)
	{
		ldbl_EGlpNumInnProd (lval, basis[i], basis[i], length);
		ldbl_EGlpNumCopy (GM->matrow[0][i], lval);
		ldbl_EGlpNumCopy (GM->matrow[i][i], lval);
		ldbl_EGlpNumMultTo (orilen, lval);
	}
	/* sort the vectors in increasing order according to norm, we do this because
	 * one of the conditions for the algorithm to stop is that the norms are in
	 * increasing order. */
	ldbl_EGutilPermSort (full_dim, GM->col_ord, (const long double*const)(GM->matrow[0]));
	memcpy (GM->row_ord, GM->col_ord, sizeof (int) * full_dim);
	/* fill-up the GM matrix with the inner products of the vectors */
	for (i = full_dim; i--;)
	{
		for (j = i + 1; j < full_dim; j++)
		{
			ldbl_EGlpNumInnProd (lval, basis[i], basis[j], length);
			ldbl_EGlpNumCopy (GM->matrow[i][j], lval);
			ldbl_EGlpNumCopy (GM->matrow[j][i], lval);
		}
	}
	ldbl_EGlpNumCopy(tmpval,lval);
	ldbl_EGlpNumClearVar(lval);
}

/* ========================================================================= */
int ldbl_EGdBsRed (ldbl_EGdBsRed_t * const bsred,
							unsigned *const ext_dim,
							const long double zero_tol,
							int *const ext_status)
{
	ldbl_EGdMatrix_t *const GM = &(bsred->GM);
	const size_t full_dim = bsred->dim;
	const size_t length = bsred->length;
	long double **const basis = bsred->basis;
	int rval = 0;
	long double tmpval,
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
	MESSAGE (ldbl_EG_DBSRED_VERBOSE, "Entering with dimension %zd length %zd", full_dim,
					 length);
	ldbl_EGdBsRedStats[ldbl_EG_BSRED_CALLS]++;
	ldbl_EGlpNumInitVar (tmpval);
	ldbl_EGlpNumInitVar (tmpval2);
	ldbl_EGlpNumInitVar (orilen);
	ldbl_EGlpNumInitVar (endlen);
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
	ldbl_EGdMatrixSetDimension (GM, full_dim, full_dim);
	ldbl_EGlpNumOne (orilen);
	/* compute GM and sort the matrix according to the norms of the basis vectors
	 * */
	ldbl_EGdBsRedBuildGM (GM, full_dim, length, basis, orilen, tmpval);
#if ldbl_EG_DBSRED_VERBOSE +100<= DEBUG
	fprintf (stderr, "Order ");
	for (i = 0; i < full_dim; i++)
		fprintf (stderr, "%d ", GM->row_ord[i]);
	fprintf (stderr, "\nGM ");
	ldbl_EGdMatrixDisplay (GM, 0, stderr);
#endif
	/* main loop */
	while (dim < full_dim)
	{
		ldbl_EGdBsRedStats[ldbl_EG_BSRED_ITT]++;
		/* do gaussian elimination to compute the Gram-Smith multipliers */
		rval = ldbl_EGdMatrixGaussianElimination (GM, 0, 0, &rank, zero_tol, &ge_status);
		CHECKRVALG (rval, CLEANUP);
#if ldbl_EG_DBSRED_VERBOSE+100 <= DEBUG
		fprintf (stderr, "Order ");
		for (i = 0; i < full_dim; i++)
			fprintf (stderr, "%d ", GM->row_ord[i]);
		fprintf (stderr, "\nafter Gausian Elimination GM ");
		ldbl_EGdMatrixDisplay (GM, 0, stderr);
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
				ldbl_EGlpNumCopy (tmpval, GM->matrow[GM->col_ord[i]][GM->col_ord[j]]);
				ldbl_EGlpNumDivTo (tmpval, GM->matrow[GM->col_ord[i]][GM->col_ord[i]]);
				ldbl_EGlpNumCopyAbs (tmpval2, tmpval);
				/* do the reduction step if needed */
				if (ldbl_EGlpNumIsGreaDbl (tmpval2, 0.5))
				{
					ldbl_EGdBsRedStats[ldbl_EG_BSRED_SZRED]++;
					/* bset tmpval2 = round(tmpval) */
					ldbl_EGlpNumFloor (tmpval2, tmpval);
					ldbl_EGlpNumSubTo (tmpval, tmpval2);
					if (ldbl_EGlpNumIsGreaDbl (tmpval, 0.5))
						ldbl_EGlpNumAddTo (tmpval2, ldbl_oneLpNum);
					MESSAGE (ldbl_EG_DBSRED_VERBOSE + 100, "Doing reduction for mu(%zu,%zu)=%lf,"
									 " multiple %lf", j, i, ldbl_EGlpNumToLf (tmpval),
									 ldbl_EGlpNumToLf (tmpval2));
					/* now we replace b_j by b_j - tmpval2 b_i */
					ldbl_EGdMatrixSubColMultiple (GM, GM->col_ord[j], GM->col_ord[i], tmpval2);
					for (k = (size_t)length; k--;)
						ldbl_EGlpNumSubInnProdTo (basis[GM->col_ord[j]][k],
																 basis[GM->col_ord[i]][k], tmpval2);
				}												/* done with the reduction */
			}													/* end checking column j of GM */
		}
#if ldbl_EG_DBSRED_VERBOSE+100 <= DEBUG
		fprintf (stderr, "Order ");
		for (i = 0; i < full_dim; i++)
			fprintf (stderr, "%d ", GM->row_ord[i]);
		fprintf (stderr, "\nafter Step I GM ");
		ldbl_EGdMatrixDisplay (GM, 0, stderr);
#endif
		/* now we are done with step I of the algorithm, now we check step 2, wich
		 * check for condition 
		 * | b^*_j + \mu_{j,j-1} b^*_{j-1}|^2 \geq \alpha |b^*_{j-1}|^2, where
		 * \alpha must be in (1/4,1). */
		for (i = 1; i < full_dim; i++)
		{
			ldbl_EGlpNumCopy (tmpval, GM->matrow[GM->col_ord[i - 1]][GM->col_ord[i]]);
			ldbl_EGlpNumMultTo (tmpval, tmpval);
			ldbl_EGlpNumDivTo (tmpval, GM->matrow[GM->col_ord[i - 1]][GM->col_ord[i - 1]]);
			ldbl_EGlpNumAddTo (tmpval, GM->matrow[GM->col_ord[i]][GM->col_ord[i]]);
			ldbl_EGlpNumDivTo (tmpval, GM->matrow[GM->col_ord[i - 1]][GM->col_ord[i - 1]]);
			/* check that we have to interchange b_j and b_{j-1}, we use lambda equal
			 * to 1048575/1048576 = (2^20-1)/2^20 */
			if (ldbl_EGlpNumIsLessDbl (tmpval, ldbl_EG_DBSRED_ALPHA))
			{
				ldbl_EGdBsRedStats[ldbl_EG_BSRED_INTR]++;
				MESSAGE (ldbl_EG_DBSRED_VERBOSE + 100, "Doing swap between columns %zu,%zu",
								 i - 1, i);
				ldbl_EGlpNumCopy (tmpval2, GM->matrow[GM->col_ord[i - 1]][GM->col_ord[i]]);
				ldbl_EGlpNumCopy (tmpval, tmpval2);
				ldbl_EGlpNumMultTo (tmpval, tmpval);
				ldbl_EGlpNumDivTo (tmpval2,
											GM->matrow[GM->col_ord[i - 1]][GM->col_ord[i - 1]]);
				ldbl_EGlpNumDivTo (tmpval,
											GM->matrow[GM->col_ord[i - 1]][GM->col_ord[i - 1]]);
				ldbl_EGlpNumAddTo (tmpval, GM->matrow[GM->col_ord[i]][GM->col_ord[i]]);
				ldbl_EGdMatrixAddRowMultiple (GM, GM->col_ord[i], GM->col_ord[i - 1],
																 tmpval2);
				ldbl_EGlpNumCopy (GM->matrow[GM->col_ord[i]][GM->col_ord[i]], tmpval);
				EGswap (GM->col_ord[i], GM->col_ord[i - 1], tmpint);
				EGswap (GM->row_ord[i], GM->row_ord[i - 1], tmpint);
				break;
			}													/* done doing the interchange of rows */
		}
		dim = i;
#if ldbl_EG_DBSRED_VERBOSE+100 <= DEBUG
		fprintf (stderr, "Order ");
		for (i = 0; i < full_dim; i++)
			fprintf (stderr, "%d ", GM->row_ord[i]);
		fprintf (stderr, "\nafter Step II GM ");
		ldbl_EGdMatrixDisplay (GM, 0, stderr);
#endif
		MESSAGE (ldbl_EG_DBSRED_VERBOSE + 100, "Current Dimension %zu", dim);
	}
	/* compute final length */
	ldbl_EGlpNumOne (endlen);
	for (i = (size_t)full_dim; i--;)
		ldbl_EGlpNumMultTo (endlen, GM->matrow[i][i]);
	/* ending */
CLEANUP:
	*ext_dim = (unsigned)dim;
	*ext_status = status;
	MESSAGE (ldbl_EG_DBSRED_VERBOSE, "Ending with dimmension %zu, status %d\nOriginal "
					 "Basis Length %lg, Final Length %lg, gain %lg", dim, status,
					 ldbl_EGlpNumToLf (orilen), ldbl_EGlpNumToLf (endlen),
					 ldbl_EGlpNumToLf (orilen) / ldbl_EGlpNumToLf (endlen));
	ldbl_EGlpNumClearVar (endlen);
	ldbl_EGlpNumClearVar (orilen);
	ldbl_EGlpNumClearVar (tmpval2);
	ldbl_EGlpNumClearVar (tmpval);
	return rval;
}

/* ========================================================================= */
/** @} */
