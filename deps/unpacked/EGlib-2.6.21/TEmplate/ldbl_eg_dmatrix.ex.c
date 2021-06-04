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
/** @file 
 * @ingroup EGdMatrix
 * @brief This file shows some examples of the use of #ldbl_EGdMatrix_t structure 
 * and functions. as a test-set we use the Hilber Matrix wich is a matrix
 * with coefficients \f$H^n_{i,j} = \frac1{i+j-1}\f$. It is well known that the
 * Hilbert matrix is extremmaly ill conditioned, and is very hard to invert,
 * here are some values for it's determinant for different dimmensions \f$n\f$.
 * - \f$\mathrm{det}(H_1) = 1 \f$. 
 * - \f$\mathrm{det}(H_2) = \frac1{12} \f$. 
 * - \f$\mathrm{det}(H_3) = \frac1{2160} \f$. 
 * - \f$\mathrm{det}(H_4) = \frac1{604800} \f$. 
 * - \f$\mathrm{det}(H_5) = \frac1{266716800000} \f$. 
 * Since a lot is known about the Hilber Matrix, some accuracy tests are
 * possible. For more details see <A HREF=http://mathworld.wolfram.com/HilbertMatrix.html TARGET=_top>MathWorld</A>
 * This test program takes as input the number of columns and rows for the
 * hilbert matrix, and so some operations on it and display it to the standard
 * output.
 * */
/** @addtogroup EGdMatrix */
/** @{ */
/* ========================================================================= */
#include "EGlib.h"

/* ========================================================================= */
/** @brief size of the table of eigenvalues of the hilbert matrix */
#define ldbl_HILBERT_TABLE_SIZE 9U

/* ========================================================================= */
/** @brief table containing 1/eigenvalues of the hilbert matrix. */
static unsigned ldbl_dmatrix_hilbert_eigenvalues[ldbl_HILBERT_TABLE_SIZE] = {
	1U,
	12U,
	180U,
	2800U,
	44100U,
	698544U,
	11099088U,
	176679360U,
	2815827300U
};

/* ========================================================================= */
/** @brief display the usage message for this program */
void ldbl_dmatrix_usage (char *program)
{
	fprintf (stdout, "Usage: %s [options]\n", program);
	fprintf (stdout, "Options:\n");
	fprintf (stdout, "     -n n   number of rows in the hilbert matrix.\n");
	fprintf (stdout, "     -m n   number of columns in the hilbert matrixt.\n");

}

/* ========================================================================= */
/** @brief parse the input argumenbts for the program */
int ldbl_dmatrix_parseargs (int argc,
											 char **argv,
											 size_t * n,
											 size_t * m)
{
	int c;
	while ((c = getopt (argc, argv, "n:m:")) != EOF)
	{
		switch (c)
		{
		case 'n':
			*n = atoi (optarg);
			break;
		case 'm':
			*m = atoi (optarg);
			break;
		default:
			ldbl_dmatrix_usage (argv[0]);
			return 1;
		}
	}
	/* reporting the options */
	if (!(*m) || !(*n))
	{
		ldbl_dmatrix_usage (argv[0]);
		return 1;
	}
	fprintf (stdout, "Parsed Options:\n");
	fprintf (stdout, "n : %zu\n", *n);
	fprintf (stdout, "m : %zu\n", *m);
	return 0;
}

/* ========================================================================= */
/** @brief main function */
int main (int argc,
					char **argv)
{

	int rval = 0,
	  status;
	size_t n = 1,
	  m = 1;
	register unsigned i = 0,
	  j = 0;
	unsigned rank;
	ldbl_EGdMatrix_t dmatrix;
	ldbl_EGdBsRed_t bsred;
	long double error;
	EGlib_info();
	EGlib_version();
	/* start */
	EGlpNumStart();
	ldbl_EGlpNumInitVar (error);
	ldbl_EGdMatrixInit (&dmatrix);
	ldbl_EGdBsRedInit (&bsred);
	/* parse args */
	rval = ldbl_dmatrix_parseargs (argc, argv, &n, &m);
	CHECKRVALG (rval, CLEANUP);
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	/* set the matrix size */
	ldbl_EGdMatrixSetDimension (&dmatrix, n, m);
	/* set the coefficients of the hilbert matrix */
	for (i = n; i--;)
		for (j = m; j--;)
		{
			ldbl_EGlpNumOne (dmatrix.matrow[i][j]);
			ldbl_EGlpNumDivUiTo (dmatrix.matrow[i][j], i + j + 1);
		}
	/* Display the hilbert matrix */
	fprintf (stdout, "Hilber ");
	ldbl_EGdMatrixDisplay (&dmatrix, 1, stdout);
	fflush (stdout);
	/* now we call gaussian elimination */
	rval =
		ldbl_EGdMatrixGaussianElimination (&dmatrix, 0, 0, &rank, ldbl_epsLpNum, &status);
	CHECKRVALG (rval, CLEANUP);
	fprintf (stdout, "Gaussian Elimination ended with status %d, rank %u and ",
					 status, rank);
	ldbl_EGdMatrixDisplay (&dmatrix, 0, stdout);
	fflush (stdout);
	/* now we compute the worst error in the diagonal up to
	 * EGmin(ldbl_HILBERT_TABLE_SIZE,rank) */
	fprintf (stdout, "Relative errors in eigenvalues:\n");
	for (i = EGmin (ldbl_HILBERT_TABLE_SIZE, rank); i--;)
	{
		ldbl_EGlpNumCopy (error, dmatrix.matrow[dmatrix.row_ord[i]][dmatrix.col_ord[i]]);
		fprintf (stdout, "Lambda_%u = %lg", i + 1, ldbl_EGlpNumToLf (error));
		ldbl_EGlpNumMultUiTo (error, ldbl_dmatrix_hilbert_eigenvalues[i]);
		ldbl_EGlpNumSubTo (error, ldbl_oneLpNum);
		fprintf (stdout, " error = %lg\n", ldbl_EGlpNumToLf (error));
	}
	/* set the coefficients of the hilbert matrix */
	for (i = n; i--;)
		for (j = m; j--;)
		{
			ldbl_EGlpNumOne (dmatrix.matrow[i][j]);
			ldbl_EGlpNumDivUiTo (dmatrix.matrow[i][j], i + j + 1);
		}
	fprintf (stdout, "Using Basis Reduction on ");
	ldbl_EGdMatrixDisplay (&dmatrix, 1, stdout);
	/* now we do basis reduction over the rows of the matrix */
	/* ending */
	ldbl_EGdBsRedSetLength (&bsred, m);
	for (i = 0; i < rank; i++)
		ldbl_EGdBsRedAddElement (&bsred, dmatrix.matrow[dmatrix.row_ord[i]]);
	/* now call the Basis Reduction Algorithm and print the reduced basis */
	rval = ldbl_EGdBsRed (&bsred, &rank, ldbl_epsLpNum, &status);
	CHECKRVALG (rval, CLEANUP);
	fprintf (stdout, "Basis Reduction ended with status %d, rank %u and ",
					 status, rank);
	ldbl_EGdMatrixDisplay (&dmatrix, 1, stdout);
	fflush (stdout);
CLEANUP:
	ldbl_EGdBsRedProfile (stderr);
	ldbl_EGlpNumClearVar (error);
	ldbl_EGdMatrixClear (&dmatrix);
	ldbl_EGdBsRedClear (&bsred);
	MESSAGE(0,"done");
	EGlpNumClear();
	return rval;
}

/* ========================================================================= */
/** @} */
