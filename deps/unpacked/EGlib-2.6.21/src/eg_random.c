/***********************************************************************\
 *
 * File:           EGrandState_t*.c for multiple streams of Random Numbers
 * Language:       ANSI C
 * Copyright:      Pierre L'Ecuyer, University of Montreal
 * Notice:         This code can be used freely for personal, academic,
 *                 or non-commercial purposes. For commercial purposes, 
 *                 please contact P. L'Ecuyer at: lecuyer@iro.UMontreal.ca
 * Date:           14 August 2001
 *
\***********************************************************************/
/* Minor modifications by Daniel Espinoza to include it in EGlib 
 * - 07082006 */


#include "eg_random.h"

/** @file
 * @ingroup EGrnd */
/** @addtogroup EGrnd */
/**  @{  */
/* ========================================================================= */
/** @name Constants
 * These are the internal constants */
/* @{ */
#define norm  0x1p-32 						/**< 2^-32 */
#define m1    0xFFFFFF2Fp0				/**<       4294967087.0 */
#define m2    0xFFFFA6BBp0				/**<       4294944443.0 */
#define a12   0x156ABCp0					/**<          1403580.0 */
#define a13n  0xC5EE8p0						/**<           810728.0 */
#define a21   0x80CFCp0						/**<           527612.0 */
#define a23n  0x14E9DDp0					/**<          1370589.0 */
#define two17 0x1p17							/**<           131072.0 */
#define two53 0x1p53							/**< 9007199254740992.0 */
#define fact  0x1p-24    					/**< 2^-24 */

/* ========================================================================= */
/** @brief precomputed @f$A_1^{-1}\textrm{mod} m_1@f$, where @f$A_1@f$ is the transition matrix. */
static double InvA1[3][3] = {          /* Inverse of A1p0 */
          { 184888585.0,   0.0,  1945170933.0 },
          {         1.0,   0.0,           0.0 },
          {         0.0,   1.0,           0.0 }
          };

/* ========================================================================= */
/** @brief precomputed @f$A_2^{-1}\textrm{mod} m_2@f$, where @f$A_2@f$ is the transition matrix. */
static double InvA2[3][3] = {          /* Inverse of A2p0 */
          {      0.0,  360363334.0,  4225571728.0 },
          {      1.0,          0.0,           0.0 },
          {      0.0,          1.0,           0.0 }
          };

/* ========================================================================= */
/** @brief precomputed @f$A_1@f$, where @f$A_1@f$ is the transition matrix. */
static double A1p0[3][3] = {
          {       0.0,        1.0,       0.0 },
          {       0.0,        0.0,       1.0 },
          { -810728.0,  1403580.0,       0.0 }
          };

/* ========================================================================= */
/** @brief precomputed @f$A_2@f$, where @f$A_2@f$ is the transition matrix. */
static double A2p0[3][3] = {
          {        0.0,        1.0,       0.0 },
          {        0.0,        0.0,       1.0 },
          { -1370589.0,        0.0,  527612.0 }
          };

/* ========================================================================= */
/** @brief precomputed @f$A_1^{2^{127}}\textrm{mod} m_1@f$, where @f$A_1@f$ is the transition matrix. */
static double A1p127[3][3] = {
          {    2427906178.0, 3580155704.0,  949770784.0 }, 
          {     226153695.0, 1230515664.0, 3580155704.0 },
          {    1988835001.0,  986791581.0, 1230515664.0 }
          };

/* ========================================================================= */
/** @brief precomputed @f$A_2^{2^{127}}\textrm{mod} m_2@f$, where @f$A_2@f$ is the transition matrix. */
static double A2p127[3][3] = {
          {    1464411153.0,  277697599.0, 1610723613.0 },
          {      32183930.0, 1464411153.0, 1022607788.0 },
          {    2824425944.0,   32183930.0, 2093834863.0 }
          };

/* ========================================================================= */
/**@ brief Default initial seed of the package. Will be updated to become
   the seed of the next created stream. */
static double nextSeed[6] = { 12345, 12345, 12345, 12345, 12345, 12345 };
#if HAVE_EG_THREAD
pthread_mutex_t egrm = PTHREAD_MUTEX_INITIALIZER;
#endif
/* @} */

/* ========================================================================= */
/** @name Internal_Functions
 * These are the internal functions that perform the actual work to generate
 * pseudo-random sequences */
/* @{ */


/* ========================================================================= */
/** @brief Compute @f$ (a\cdot s + c) \textrm{mod}  m@f$.
 * @param m must be less than @f$ 2^{35} @f$.
 * @param s can be less than zero.
 * @param c can be less than zero.
 * @param a value.
 * @return  @f$ (a\cdot s + c) \textrm{mod}  m@f$ */
static double MultModM (
		double a,
		double s,
		double c,
		double m)
{
   double v,a2;
   long a1;
   v = (a2=a) * s + c;
   if ((v >= two53) || (v <= -two53)) {
      a1 = (long) (a / two17);
      a2 -= ((double)a1) * two17;
      v = ((double)a1) * s;
      a1 = (long) (v / m);
      v -= ((double)a1) * m;
      v = v * two17 + a2 * s + c;
   }
   a1 = (long) (v / m);
   if ((v -= ((double)a1) * m) < 0.0)
      return v += m;
   else
      return v;
}

/* ========================================================================= */
/** @brief Computes @f$v = A\cdot s \textrm{mod} m@f$, assumes that @f$-m < s[i] < m@f$,  Works even if @f$v = s@f$.
 * @param A matrix of dimension @f$3\times 3@f$ 
 * @param s vector of dimension @f$3@f$
 * @param v vector of dimension @f$3@f$
 * @param m module
 * */
static void MatVecModM (
	double A[3][3],
	double s[3],
	double v[3],
	double m)
{
   int i;
   double x[3];
   for (i = 0; i < 3; ++i) {
      x[i] = MultModM (A[i][0], s[0], 0.0, m);
      x[i] = MultModM (A[i][1], s[1], x[i], m);
      x[i] = MultModM (A[i][2], s[2], x[i], m);
   }
   for (i = 0; i < 3; ++i)
      v[i] = x[i];
}

/* ========================================================================= */
/** @brief Returns @f$C = A\cdot B \textrm{mod} m@f$. Work even if @f$A = C@f$ or @f$B = C@f$ or @f$A = B = C@f$.
 * @param A matrix of @f$3\times3@f$
 * @param B matrix of @f$3\times3@f$
 * @param C matrix of @f$3\times3@f$
 * @param m module.
 * */
static void MatMatModM (
		double A[3][3],
		double B[3][3],
		double C[3][3],
		double m)
{
   int i, j;
   double V[3], W[3][3];
   for (i = 0; i < 3; ++i) {
      for (j = 0; j < 3; ++j)
         V[j] = B[j][i];
      MatVecModM (A, V, V, m);
      for (j = 0; j < 3; ++j)
         W[j][i] = V[j];
   }
   for (i = 0; i < 3; ++i) {
      for (j = 0; j < 3; ++j)
         C[i][j] = W[i][j];
   }
}

/* ========================================================================= */
/** @brief  Compute matrix @f$B = (A^{(2^e)} \textrm{mod} m)@f$, works even if @f$A = B@f$
 * @param A matrix of @f$3\times3@f$
 * @param B matrix of @f$3\times3@f$
 * @param m module.
 * @param e double exponent
 * */
static void MatTwoPowModM (
		double A[3][3],
		double B[3][3],
		double m,
		long e)
{
	int i, j;

	/* initialize: B = A */
	for (i = 0; i < 3; i++) 
	{
		for (j = 0; j < 3; ++j)
			B[i][j] = A[i][j];
	}
	/* Compute B = A^{2^e} */
	for (i = 0; i < e; i++)
		MatMatModM (B, B, B, m);
}

/* ========================================================================= */
/** @brief Compute matrix @f$B = A^n \textrm{mod} m @f$; works even if @f$A = B@f$ 
 * @param A matrix of @f$3\times3@f$
 * @param B matrix of @f$3\times3@f$
 * @param m module.
 * @param n exponent
 * */
static void MatPowModM (double A[3][3], double B[3][3], double m, long n)
{
   int i, j;
   double W[3][3];

   /* initialize: W = A; B = I */
   for (i = 0; i < 3; i++) {
      for (j = 0; j < 3; ++j) {
         W[i][j] = A[i][j];
         B[i][j] = 0.0;
      }
   }
   for (j = 0; j < 3; ++j)
      B[j][j] = 1.0;

   /* Compute B = A^n % m using the binary decomposition of n */
   while (n > 0) {
      if (n % 2)
         MatMatModM (W, B, B, m);
      MatMatModM (W, W, W, m);
      n /= 2;
   }
}

/* ========================================================================= */
/** @brief return a (pesudo) random number uniformily distributed between 0 and
 * 1 */
static double U01 (EGrandState_t*const g)
{
	uint32_t res = EGrand(g);
	return ((double)res)*norm;
}


/* ========================================================================= */
/** @brief Check that the seeds are legitimate values.
 * @return 0 if legal seeds, 1 otherwise */
static int CheckSeed (const unsigned long* seed)
{
	int i;
	for (i = 0; i < 3; ++i) 
	{
		if (seed[i] >= m1) 
		{
			TEST( 1, "ERROR: %ld = Seed[%1d] >= m1 = %ld, Seed is not set.\n", 
						seed[i], i, (long)m1);
		}
	}
	for (i = 3; i < 6; ++i) 
	{
		if (seed[i] >= m2)
		{
			TEST( 1, "ERROR: %ld = Seed[%1d] >= m2 = %ld, Seed is not set.\n", 
						seed[i], i, (long)m2);
		}
	}
	if (seed[0] == 0 && seed[1] == 0 && seed[2] == 0) 
	{
		TEST(1,"ERROR: First 3 seeds = 0");
	}
	if (seed[3] == 0 && seed[4] == 0 && seed[5] == 0) 
	{
		TEST(1,"ERROR: Last 3 seeds = 0");
	}
   return 0;
}

/* @} */

/* ========================================================================= */
int EGrandInit(EGrandState_t*const state)
{
	int i = 6;
	#if HAVE_EG_THREAD
	pthread_mutex_lock(&egrm);
	#endif
	for( ; i-- ; )
	{
		state->Ig[i] = state->Cg[i] = nextSeed[i];
	}
	/* move the next global seed to a suitable position */
	MatVecModM (A1p127, nextSeed, nextSeed, m1);
	MatVecModM (A2p127, &(nextSeed[3]), &(nextSeed[3]), m2);
	#if HAVE_EG_THREAD
	pthread_mutex_unlock(&egrm);
	#endif
	return 0;
}

/* ========================================================================= */
void EGrandReset (EGrandState_t*const g)
{
   int i;
   for (i = 0; i < 6; ++i)
      g->Cg[i] = g->Ig[i];
}

/* ========================================================================= */
int EGrandSetGlobalSeed(const unsigned long* seed)
{
	int i;
	if (CheckSeed (seed))
		return -1;                    /* FAILURE */
	#if HAVE_EG_THREAD
	pthread_mutex_lock(&egrm);
	#endif
	for (i = 0; i < 6; ++i)
		nextSeed[i] = ((double)seed[i]);
	#if HAVE_EG_THREAD
	pthread_mutex_unlock(&egrm);
	#endif
	return 0;                       /* SUCCESS */
}

/* ========================================================================= */
int EGrandSetSeed (EGrandState_t*const g, const unsigned long* seed)
{
   int i;
   if (CheckSeed (seed))
      return -1;                    /* FAILURE */
   for (i = 0; i < 6; ++i)
      g->Cg[i] = g->Ig[i] = ((double)seed[i]);
   return 0;                       /* SUCCESS */ 
}

/* ========================================================================= */
void EGrandAdvanceState (EGrandState_t*const g,const long e,const long c)
{
   double B1[3][3], C1[3][3], B2[3][3], C2[3][3];

   if (e > 0) {
      MatTwoPowModM (A1p0, B1, m1, e);
      MatTwoPowModM (A2p0, B2, m2, e);
   } else if (e < 0) {
      MatTwoPowModM (InvA1, B1, m1, -e);
      MatTwoPowModM (InvA2, B2, m2, -e);
   }

   if (c >= 0) {
      MatPowModM (A1p0, C1, m1, c);
      MatPowModM (A2p0, C2, m2, c);
   } else {
      MatPowModM (InvA1, C1, m1, -c);
      MatPowModM (InvA2, C2, m2, -c);
   }

   if (e) {
      MatMatModM (B1, C1, C1, m1);
      MatMatModM (B2, C2, C2, m2);
   }

   MatVecModM (C1, g->Cg, g->Cg, m1);
   MatVecModM (C2, &g->Cg[3], &g->Cg[3], m2);
}

/* ========================================================================= */
void EGrandGetState (EGrandState_t*const g, unsigned long* seed)
{
   int i;
   for (i = 0; i < 6; ++i)
      seed[i] = ((unsigned long)(g->Cg[i]));
}

/* ========================================================================= */
int EGrandInt(EGrandState_t*const g,const int i,const int j)
{
	double rnd = EGrand(g);
	return i + ((int)(rnd*norm*(j-i+1)));
}

/* ========================================================================= */
uint32_t EGrand(EGrandState_t*const g)
{
   long k;
   double p1, p2, u;

   /* Component 1 */
   p1 = a12 * g->Cg[1] - a13n * g->Cg[0];
   k = (long)(p1 / m1);
   p1 -= ((double)k) * m1;
   if (p1 < 0.0)
      p1 += m1;
   /*
	 memmove(g->Cg,g->Cg+1,sizeof(double)*2);
	 */
	 g->Cg[0] = g->Cg[1];
   g->Cg[1] = g->Cg[2];
   g->Cg[2] = p1;

   /* Component 2 */
   p2 = a21 * g->Cg[5] - a23n * g->Cg[3];
   k = ((long)(p2 / m2));
   p2 -= ((double)k) * m2;
   if (p2 < 0.0)
      p2 += m2;
	 /*
	 memmove(g->Cg+3,g->Cg+4,sizeof(double)*2);
	 */
   g->Cg[3] = g->Cg[4];
   g->Cg[4] = g->Cg[5];
   g->Cg[5] = p2;

   /* Combination */
   u = ((p1 > p2) ? (p1 - p2) : (p1 - p2 + m1) );
   return (uint32_t)(u);
}

/* ========================================================================= */
double EGrandU01 (EGrandState_t* g)
{
	return U01(g);
}

/* ========================================================================= */
double EGrandNormal(EGrandState_t*const g,const double mu, const double sigma)
{
	/* this is the polar form, it has better kurtosis than the orthogonal form */
	double phi = EGrand(g);
	double r = EGrand(g);
	r *= norm;
	phi *= norm;
	return mu + sigma*cos(2*((double)EG_M_PI)*phi)*sqrt(-2*log(r));
	/* this is the othogonal form, it is faster than the polar form, but less
	 * acurate, we keep it so that the user may choose which form to use */
	/*
	double x = EGrand(g);
	double y = EGrand(g);
	double r;
	x = 0x1p-31*x - 1;
	y = 0x1p-31*y - 1;
	while((r = hypot(x,y))>=1.0)
	{
		x = EGrand(g);
		y = EGrand(g);
		x = 0x1p-31*x - 1;
		y = 0x1p-31*y - 1;
	}
	return mu+sigma*x*sqrt(-2*log(r));
	*/
}

/* ========================================================================= */
double EGrandExp(EGrandState_t*const g,const double lambda)
{
	double u = EGrand(g);
	u *= norm;
	return -log(u)/lambda;
}

/* ========================================================================= */
/**  @}  */
