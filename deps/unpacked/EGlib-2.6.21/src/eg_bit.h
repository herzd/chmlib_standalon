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
/* bit operations over arbitrary memory segnemts 
 *
 * Version 0.0.1 2003-04-11
 *
 * - 2006-01-30
 * 		- Change some type definitions so that to make its behavior independent
 * 		of the plataform, we choose to use bit words of size 32 bits.
 * - 2005-08-01
 * 		- Fix some types to size_t for alloc/free functions
 * - 2004-11-05 
 * 		- Add EGbitsetSize macro to compute the wright size of static
 * 			bitsets.
 * - 2003-05-14 
 * 		- Fix problem in EGbitTest
 * - 2003-04-29 
 * 		- Change the parameters that are not changed to const.
 * 		- Change the definition of the function to 'extern inline int' to 
 * 			speed-up the running time for all funcions but EGbitNext.
 *
 * */
/* ========================================================================= */

#ifndef __EG_BIT_H__
#define __EG_BIT_H__
#include "eg_config.h"
#include "eg_macros.h"
#include "eg_mem.h"
/* ============================================================================
   Bitset definitions and functions                                                                
   ==========================================================================*/


/* ========================================================================= */
/* EG_BIT options */
/* ========================================================================= */
/* the debug level indicates what is the minimum debug level to start generating
 * messages of the operation for this suit, by defoult is 10, note that the
 * lower the level, the more messages that we will generate. */
#ifndef __EGBIT_DEBUG_LEVEL__
#define __EGBIT_DEBUG_LEVEL__ 10
#endif

/* ========================================================================== */
/** @name Word lengths and related macros */
/* @{ */
#define __EGBIT_WORD_SIZE__ 32U
#define __EGBIT_SHIFT__ 5U
#define __EGBIT_MASK__ 31U
/* @} */

/* ========================================================================== */
/** @brief Macro to compute the right size of a bitset given the desired 
 * number of bits in the bitset */
#define EGbitsetSize(__Bsize) (((__Bsize)>>__EGBIT_SHIFT__)+(((__Bsize)&__EGBIT_MASK__)?1:0))

/* ============================================================================   
   all positions are in bits not in bytes   
   the ranged operations are performed between low_wordsize(from)   
   upper_wordsize(to) where low and upper are points in the wordsize generated   
   latice, for example, if wordsize is 32, and from is 46 and to is 90, then   
   the real range will be [32,95] so, be careful and remember that the range   
   start at zero.   
      
   this function is to check if the parameter are well set up   
   ========================================================================= */
/*typedef uint32_t bit_int_t;*/
#define bit_int_t uint32_t
#define EGbitset_t bit_int_t

/* ========================================================================= */
/* this function only check if the internal parameters of this utilities were
 * well set at compile time */
/* ========================================================================= */
int EGbitSanity (void);

/* ========================================================================= */
/* this function implements dst |= src in the range [from,to[, where from and to
 * are roneded down to the nearest complete word size. */
/* ========================================================================= */
int EGbitOr (bit_int_t * dst,
						 const bit_int_t * src,
						 const unsigned int from,
						 const unsigned int to);

/* ========================================================================= */
/* this function implements dst &= src in the range [from,to[, where from and to
 * are rounded down to the nearest complete word size. */
/* ========================================================================= */
int EGbitAnd (bit_int_t * dst,
							const bit_int_t * src,
							const unsigned int from,
							const unsigned int to);

/* ========================================================================= */
/* this function implement right shifts in the range [from,to[, where
 * from and to are rounded down to the nearest complete word size */
/* ========================================================================= */
int EGbitRightShift (bit_int_t * dst,
										 const bit_int_t * src,
										 const unsigned int shift,
										 const unsigned int from,
										 const unsigned int to);

/* ========================================================================= */
/* this function implement left shifts in the range [from,to[, where
 * from and to are rounded down to the nearest complete word size */
/* ========================================================================= */
int EGbitLeftShift (bit_int_t * dst,
										const bit_int_t * src,
										const unsigned int shift,
										const unsigned int from,
										const unsigned int to);

/* ========================================================================= */
/* this function set to zero the bit dst:pos (i.e. the bit in the position 'pos'
 * */
/* ========================================================================= */
int EGbitUnset (bit_int_t * dst,
								const unsigned int pos);

/* ========================================================================= */
/* this function set to one the bit dst:pos (i.e. the bit in the position 'pos'
 * */
/* ========================================================================= */
int EGbitSet (bit_int_t * const dst,
							const unsigned int pos);

/* ========================================================================= */
/* this function implement x+=y function in the range [from,to[, where
 * from and to are rounded down to the nearest complete word size */
/* ========================================================================= */
int EGbitPlus (bit_int_t * dst,
							 const bit_int_t * src,
							 const unsigned int from,
							 const unsigned int to);

/* ========================================================================= */
/* copy one bitset to another within some range [from,to] */
/* ========================================================================= */
int EGbitCopy (bit_int_t * const a,
							 const bit_int_t * const b,
							 const unsigned int from,
							 const unsigned int to);

/* ========================================================================= */
/* tell us if a <= b bit fields are equal within some range [from,to[. */
/* ========================================================================= */
int EGbitIsLeq (const bit_int_t * a,
								const bit_int_t * b,
								const unsigned int from,
								const unsigned int to);

/* ========================================================================= */
/* tell us if these bit fields are equal within some range [from,to[. */
/* ========================================================================= */
int EGbitIsEqual (const bit_int_t * a,
									const bit_int_t * b,
									const unsigned int from,
									const unsigned int to);

/* ========================================================================= */
/* this function implements dst ^= src in the range [from,to[, where from and to
 * are rounded down to the nearest complete word size. */
/* ========================================================================= */
int EGbitXor (bit_int_t * dst,
							const bit_int_t * src,
							const unsigned int from,
							const unsigned int to);

/* ========================================================================= */
/* this function implements dst = 0 in the range [from,to[, where from and to
 * are rounded down to the nearest complete word size. */
/* ========================================================================= */
int EGbitReset (bit_int_t * dst,
								const unsigned int from,
								const unsigned int to);

/* ========================================================================= */
/* this function return the value of dst:pos (i.e. the value of the bit in the
 * position 'pos' */
/* ========================================================================= */
int EGbitTest (bit_int_t const *const dst,
							const unsigned int pos);

/* ========================================================================= */
/* return the next one position in the bitset from 'pos' (inclusive), if no
 * next one position is found it return a number bigger than size */
/* ========================================================================= */
unsigned int EGbitNext (const bit_int_t * dst,
												const unsigned int pos,
												const unsigned int size);

/* ========================================================================= */
/* return the previous one position in the bitset from 'pos' (inclusive), if no
 * previous one position is found it return a number bigger than pos */
/* ========================================================================= */
unsigned int EGbitPrev (const EGbitset_t * dst,
												const unsigned int pos);

/** @brief Count number of on bits on 32-bit integers.
 * @return Number of on bits in the given 32-bit integer
 * @par Description:
 * Parallel Count carries out bit counting in a parallel fashion. Consider n
 * after the first line has finished executing. Imagine splitting n into pairs
 * of bits. Each pair contains the number of ones in those two bit positions in
 * the original n. After the second line has finished executing, each nibble
 * contains the number of ones in those four bits positions in the original n.
 * Continuing this for five iterations, the 64 bits contain the number of ones
 * among these sixty-four bit positions in the original n. That is what we
 * wanted to compute.*/
EGbitset_t EGbitElemBitCount (EGbitset_t n);

 /** @brief Count the number of on-bits in a bit-map */
EGbitset_t EGbitCount (bit_int_t * bitset,
											const unsigned int from,
											const unsigned int to);
#endif
