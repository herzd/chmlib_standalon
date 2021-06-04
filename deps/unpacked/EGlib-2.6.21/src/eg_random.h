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

/* ========================================================================= */
/** @defgroup EGrnd EGrnd
 *
 * Here we define an interface for pseudo-random number generators based in
 * Pierre L'Ecuyer code. It provides a random state structure and a
 * pseudorandom generator with 32 bits of meaningfull data. 
 * The idea is to also provide a common interface to pseudo-random sequences
 * and to allow to use native pseudo-random codes.
 *
 * @version 1.0.1
 * @par History:
 * - 2013-04-18
 * 						- Add static initializer
 * - 2011-03-01
 * 						- Make the global-seed thread-safe, note that individual random
 * 							states are not thread-safe.
 * */ 
/** @file
 * @ingroup EGrnd */
/** @addtogroup EGrnd */
/** @{
 * @example eg_random.ex.c
 * This is a simple example of the usage of this random number generator 
 * @ref EGrnd */
/* ========================================================================= */
#ifndef __EGRNGSTREAM_H__
#define __EGRNGSTREAM_H__
#include "eg_config.h"
#include "eg_macros.h"

/* ========================================================================= */
/** @brief random state structure */
typedef struct {
   double Cg[6];	/**< Current state of the stream */
	 double Ig[6];	/**< Initial state of the stream */
} EGrandState_t;
/* ========================================================================= */
/** @brief null initializer (uses fixed zero seed) */
#define EGrandStateZero {.Cg={0.0,0.0,0.0,0.0,0.0,0.0},.Ig={0.0,0.0,0.0,0.0,0.0,0.0}}
/* ========================================================================= */
/** @brief Initialize a stream to the default initial seed.
 * It also advances the global seed @f$2^{127}@f$ positions.
 * @return zero on success, non-zero otherwise */
int EGrandInit(EGrandState_t*const state);

/* ========================================================================= */
/** @brief Set to the given values the global default seed 
 * @return zero on success, non-zero otherwise */
int EGrandSetGlobalSeed(const unsigned long* seed);

/* ========================================================================= */
/** @brief return 32 (pseudo) random bits from the given random state (and 
 * advance the random seed) 
 * @param g random seed to use.
 * @return 32 (pseudo) random bits, i.e. a number between zero and @f$2^{32}-1@f$ */
uint32_t EGrand(EGrandState_t*const g);

/* ========================================================================= */
/** @brief use 32 bits (i.e. 1 call to #EGrand) to create a (pseudo) random double number between 0 and 1.0.
 * @param g random seed to use.
 * @return a double (pseudo) random number between 0 and 1. */
double EGrandU01(EGrandState_t*const g);

/* ========================================================================= */
/** @brief use 64 bits (i.e. 2 calls to #EGrand) to create a (pseudo) random double number between \f$a\f$ and \f$b\f$.
 * @param g random seed to use.
 * @param a lower bound for the value.
 * @param b upper bound for the value.
 * @return a double (pseudo) random number between \f$a\f$ and \f$b\f$. */
#define EGrandUab(g,a,b) ({\
	const double __egrnd_A = (a), __egrnd_B = (b);\
	__egrnd_A + (__egrnd_B - __egrnd_A)*EGrandU01(g);})

/* ========================================================================= */
/** @brief generate a (pseudo) normal random number with mean \f$\mu\f$ and standard deviation \f$\sigma\f$ using 64 bits of psudorandom numbers (i.e. two calls to #EGrand).
 * @param g random seed to use.
 * @param mu mean for the normal distribution \f$\mu\f$.
 * @param sigma standard deviation of the normal distribution. \f$\sigma\f$.
 * @return a double (pseudo) normal random number.
 * @note We use the so-called Box-Muller transform in normal form to generate
 * the normal distribution, we took this form instead of the polar form because it seems to generate better normal sequences. for more details on this see <A HREF=http://en.wikipedia.org/wiki/Box-Muller_transformation TARGET=_top>this link</A> */
double EGrandNormal(EGrandState_t*const g,const double mu, const double sigma);

/* ========================================================================= */
/** @brief return a (pseudo) random integer between i and j.
 * @param i minimum value for the random number.
 * @param j maximum value for the random number.
 * @param g random seed to use.
 * @return a (pseudo) random number between i and j.
 * @note It uses the high order bits og #EGrand to generate the random 
 * integer.
 * */
int EGrandInt (EGrandState_t*const g,const int i,const int j);

/* ========================================================================= */
/** @brief Reset the random stream to its original state
 * @param g random seed to use.
 * @note This call allows to re-generate a sequence of calls to the random
 * number generator by setting the internal seed or state to its original value
 * (i.e. to the value when it was created). */
void EGrandReset (EGrandState_t*const g);

/* ========================================================================= */
/** @brief Set the (current and original) seed of the given random state to the
 * given seed.
 * @param g random seed to use.
 * @param seed new values to use as seed.
 * @return zero on success, non-zero otherwise.
 * */
int EGrandSetSeed(EGrandState_t*const g, const unsigned long* seed);

/* ========================================================================= */
/** @brief store the curren  state of the given random state.
 * @param g random seed to use.
 * @param seed where to store the current random state of g.
 * */
void EGrandGetState (EGrandState_t*const g, unsigned long *seed);

/* ========================================================================= */
/** @brief advances the (current) state of the random state by \f$k\f$ steps.
 * @param g random seed to use.
 * @param e part of the formula to compute \f$k\f$, can be positive or
 * negative.
 * @param c part of the formula to compute \f$k\f$, can be positive or
 * negative.
 * @note \f$k\f$ is given by the following formula: \f[k = c + \textrm{sign}(e)2^{|e|} \f]
 * where we assume that sign(0) = 0.
 * */
void EGrandAdvanceState (EGrandState_t*const g, const long e,const long c);

/* ========================================================================= */
/** @brief return a (pseudo) random number following an exponential
 * distribution with parameter \f$\lambda\f$ using 32 bits of random numbers
 * (i.e. one call to #EGrand).
 * @param g random seed to use.
 * @param lambda is the \f$\lambda\f$ parameter, it should be positive.
 * @return a (psaudo) random number following from an exponential distribution.
 * */
double EGrandExp(EGrandState_t*const g,const double lambda);

#endif
/* ========================================================================= */
/** @} */
