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
#include "eg_bit.h"
/* ========================================================================= */
int EGbitRightShift (EGbitset_t * dst,
										 const EGbitset_t * src,
										 const unsigned int shift,
										 const unsigned int from,
										 const unsigned int to)
{
	/* local variables */
	EGbitset_t carry_on = 0;
	register unsigned int i;
	unsigned int start = from >> __EGBIT_SHIFT__;

	/* doing the left shift */
	i = (to >> __EGBIT_SHIFT__);
	carry_on = ((EGbitset_t)(src[i] << (__EGBIT_WORD_SIZE__ - shift)));
	dst[i] = ((EGbitset_t)(src[i] >> shift));
	for (i--; i >= start; i--)
	{
		dst[i] = (EGbitset_t)(src[i] >> shift) | carry_on;
		carry_on = ((EGbitset_t)(src[i] << (__EGBIT_WORD_SIZE__ - shift)));
	}

	/* ending */
	return 0;
}

/* ========================================================================= */
int EGbitPlus (EGbitset_t * dst,
							 const EGbitset_t * src,
							 const unsigned int from,
							 const unsigned int to)
{
	/* local variables */
	EGbitset_t carry_on = 0,
	  carry_on3 = 0,
	  carry_on2 = 0;
	register unsigned int i;
	unsigned int end = to >> __EGBIT_SHIFT__;

	/* doung the sum */
	for (i = (from >> __EGBIT_SHIFT__); i <= end; i++)
	{
		carry_on2 = (EGbitset_t)(((dst[i]) >> (__EGBIT_WORD_SIZE__ - 1)) | ((src[i]) >> (__EGBIT_WORD_SIZE__ - 1)));
		carry_on3 = (EGbitset_t)(((dst[i]) >> (__EGBIT_WORD_SIZE__ - 1)) & ((src[i]) >> (__EGBIT_WORD_SIZE__ - 1)));
		dst[i] = dst[i] + src[i] + carry_on;
		carry_on2 &= (((~dst[i]) & (1ULL << (__EGBIT_WORD_SIZE__ - 1)))) >> (__EGBIT_WORD_SIZE__ - 1);
		carry_on = carry_on2 | carry_on3;
	}
	/* ending */
	return 0;
}

/* ========================================================================= */
/* copy one bitset to another within some range [from,to] */
/* ========================================================================= */
int EGbitCopy (EGbitset_t * const a,
							 const EGbitset_t * const b,
							 const unsigned int from,
							 const unsigned int to)
{
	/* copying memory */
	memcpy (a + (from >> __EGBIT_SHIFT__),
					b + (from >> __EGBIT_SHIFT__),
					sizeof (EGbitset_t) * ((to - from) >> __EGBIT_SHIFT__));

	/* ending */
	return 0;
}

/* ========================================================================= */
/* tell us if a <= b bit within some range [from,to] in a vector sense. */
/* ========================================================================= */
int EGbitIsLeq (const EGbitset_t * a,
								const EGbitset_t * b,
								const unsigned int from,
								const unsigned int to)
{
	/* local variables */
	register unsigned int i;
	unsigned int start = from >> __EGBIT_SHIFT__;

	/* doing the left shift */
	i = (to >> __EGBIT_SHIFT__) - start;
	do
	{
		if (a[i] < b[i])
			return 1;
		if (a[i] > b[i])
			return 0;
	} while (i--);

	/* ending */
	return 1;
}

/* ========================================================================= */
/* tell us if these bit fields are equal within some range [from,to[. */
/* ========================================================================= */
int EGbitIsEqual (const EGbitset_t * a,
									const EGbitset_t * b,
									const unsigned int from,
									const unsigned int to)
{
	/* local variables */
	register unsigned int i;
	unsigned int end = to >> __EGBIT_SHIFT__;

	/* doing the left shift */
	for (i = (from >> __EGBIT_SHIFT__); i <= end; i++)
	{
		if (a[i] != b[i])
			return 0;
	}

	/* ending */
	return 1;
}

/* ========================================================================= */
int EGbitLeftShift (EGbitset_t * dst,
										const EGbitset_t * src,
										const unsigned int shift,
										const unsigned int from,
										const unsigned int to)
{
	/* local variables */
	EGbitset_t carry_on = 0;
	register unsigned int i;
	unsigned int end = to >> __EGBIT_SHIFT__;

	/* doing the left shift */
	i = (from >> __EGBIT_SHIFT__);
	carry_on = (EGbitset_t)(src[i] >> (__EGBIT_WORD_SIZE__ - shift));
	dst[i] = (EGbitset_t)(src[i] << shift);
	for (i++; i <= end; i++)
	{
		dst[i] = (EGbitset_t)((src[i] << shift) | carry_on);
		carry_on = (EGbitset_t)(src[i] >> (__EGBIT_WORD_SIZE__ - shift));
	}

	/* ending */
	return 0;
}

/* ========================================================================= */
int EGbitUnset (EGbitset_t * dst,
								const unsigned int pos)
{
	MESSAGE (__EGBIT_DEBUG_LEVEL__,
					 "in EGbitUnset, set bit %u (chunk %u position %u)", pos,
					 pos >> __EGBIT_SHIFT__, pos & __EGBIT_MASK__);

	dst[(pos >> __EGBIT_SHIFT__)] &= (~(1 << (pos & __EGBIT_MASK__)));

	return 0;
}

/* ========================================================================= */
int EGbitOr (EGbitset_t * dst,
						 const EGbitset_t * src,
						 const unsigned int from,
						 const unsigned int to)
{

	/* local variables */
	unsigned int i;
	unsigned int end = to >> __EGBIT_SHIFT__;

	/* doing or */
	for (i = (from >> __EGBIT_SHIFT__); i <= end; i++)
		dst[i] = dst[i] | src[i];

	/* ending */
	return 0;
}

/* ========================================================================= */
int EGbitAnd (EGbitset_t * dst,
							const EGbitset_t * src,
							const unsigned int from,
							const unsigned int to)
{

	/* local variables */
	unsigned int i;
	unsigned int end = to >> __EGBIT_SHIFT__;

	/* doing and */
	for (i = (from >> __EGBIT_SHIFT__); i <= end; i++)
		dst[i] &= src[i];

	/* ending */
	return 0;
}

/* ========================================================================= */
int EGbitXor (EGbitset_t * dst,
							const EGbitset_t * src,
							const unsigned int from,
							const unsigned int to)
{

	/* local variables */
	unsigned int i;
	unsigned int end = to >> __EGBIT_SHIFT__;

	/* doing Xor */
	for (i = (from >> __EGBIT_SHIFT__); i <= end; i++)
		dst[i] ^= src[i];

	/* ending */
	return 0;
}

/* ========================================================================= */
int EGbitReset (EGbitset_t * dst,
								const unsigned int from,
								const unsigned int to)
{

	/* local variables */
	unsigned int lo;
	unsigned int up;

	/* doing reset */
	lo = from >> __EGBIT_SHIFT__;
	up = to >> __EGBIT_SHIFT__;
	memset (dst + lo, 0, (up + 1 - lo) * sizeof (EGbitset_t));

	/* ending */
	return 0;
}

/* ========================================================================= */
int EGbitTest (EGbitset_t const *const dst,
							 const unsigned int pos)
{
	MESSAGE (__EGBIT_DEBUG_LEVEL__,
					 "in EGbitTest, test bit %u (chunk %u position %u)", pos,
					 pos >> __EGBIT_SHIFT__, pos & __EGBIT_MASK__);

	return (int)(dst[(pos >> __EGBIT_SHIFT__)] & (1U << (pos & __EGBIT_MASK__)));
}

/* ========================================================================= */
int EGbitSet (EGbitset_t * const dst,
							const unsigned int pos)
{
	MESSAGE (__EGBIT_DEBUG_LEVEL__,
					 "in EGbitSet, set bit %u (chunk %u position %u)", pos,
					 pos >> __EGBIT_SHIFT__, pos & __EGBIT_MASK__);

	dst[(pos >> __EGBIT_SHIFT__)] |= (1 << (pos & __EGBIT_MASK__));

	return 0;
}

/* ========================================================================= */
unsigned int EGbitPrev (const EGbitset_t * dst,
												const unsigned int pos)
{

	/* local variables */
	register unsigned int curChunk = pos >> __EGBIT_SHIFT__,
	  curPos = pos;
	EGbitset_t curMask = (EGbitset_t)(1U << (pos & __EGBIT_MASK__));

	/* first we check the first chunk bit by bit */
	while (curMask)
	{
		if (dst[curChunk] & curMask)
			return curPos;
		curPos--;
		curMask = curMask >> 1;
	}

	/* now we look chunk by chunk */
	curChunk--;
	while ((curChunk < pos) && (!dst[curChunk]))
	{
		curChunk--;
		curPos -= __EGBIT_WORD_SIZE__;
	}

	/* if we run out of space we _exit */
	if (curChunk > pos)
		return UINT_MAX;

	/* if we are here is becouse there is a one bit in the range, we are only
	 * looking for it */
	curMask = (EGbitset_t)(1U << __EGBIT_MASK__);
	while (curMask)
	{
		if (dst[curChunk] & curMask)
			return curPos;
		curPos--;
		curMask = curMask >> 1;
	}

	/* ending */
	/* if we reach this line then the code is wrong */
	EXIT (1, "This is due to a programming error or some memory overwrite");
	exit(EXIT_FAILURE);
}

/* ========================================================================= */
unsigned int EGbitNext (const EGbitset_t * dst,
												const unsigned int pos,
												const unsigned int size)
{

	/* local variables */
	register unsigned int curChunk = pos >> __EGBIT_SHIFT__,
	  curPos = pos;
	EGbitset_t curMask = (EGbitset_t)(1U << (pos & __EGBIT_MASK__));

	/* first we check the first chunk bit by bit */
	while (curMask)
	{
		if (dst[curChunk] & curMask)
			return curPos;
		curPos++;
		curMask = (EGbitset_t)(curMask << 1U);
	}

	/* now we look chunk by chunk */
	curChunk++;
	curMask = 1;
	while ((curPos < size) && (!dst[curChunk]))
	{
		curChunk++;
		curPos += __EGBIT_WORD_SIZE__;
	}

	/* if we run out of space we _exit */
	if (curPos >= size)
		return (UINT32_MAX);

	/* if we are here is because there is a one bit in 
	 * the range, we are only looking for it */
	while (curMask)
	{
		if (dst[curChunk] & curMask)
			return curPos;
		curPos++;
		curMask = curMask << 1;
	}

	/* ending */
	/* if we reach this line then the code is wrong */
	EXIT (1, "This is due to a programming error or some memory overwrite");
	exit(EXIT_FAILURE);
}

/* ========================================================================= */
int EGbitSanity (void)
{

	/* local variables */
	EGbitset_t mem1[10],
	  mem2[10],
	  mem3[10];
	unsigned int pos;

	/* sanity tests */
	if (sizeof (EGbitset_t) != sizeof (void *))
	{
		fprintf (stderr,
						 "sizeof(EGbitset_t) = %zu does not equal sizeof(void*) = %zu, in file %s, line %d\n",
						 sizeof (EGbitset_t), sizeof (void *), __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	if ((size_t)(__EGBIT_WORD_SIZE__) != 8 * sizeof (EGbitset_t))
	{
		fprintf (stderr,
						 "__EGBIT_WORD_SIZE__ = %u does not equal 8*sizeof(EGbitset_t) = %zu, in file %s, line %d\n",
						 __EGBIT_WORD_SIZE__, 8 * sizeof (EGbitset_t), __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	if ((1 << __EGBIT_SHIFT__) != __EGBIT_WORD_SIZE__)
	{
		fprintf (stderr,
						 "__EGBIT_SHIFT__ = %u is not the log_2(__EGBIT_WORD_SIZE__ = %u), in file %s, line %d\n",
						 __EGBIT_SHIFT__, __EGBIT_WORD_SIZE__, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	if ((__EGBIT_WORD_SIZE__ - 1) != __EGBIT_MASK__)
	{
		fprintf (stderr,
						 "__EGBIT_WORD_SIZE__ -1 = %u, does not equal __EGBIT_MASK__ = %u, in file %s, line %d\n",
						 __EGBIT_WORD_SIZE__ - 1, __EGBIT_MASK__, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}


	/* === now we build three memory sets and dom some 
	 * operations we reset them all =====================*/
	EGbitReset (mem1 , 0, 10 * __EGBIT_WORD_SIZE__ - 1);
	EGbitReset (mem2, 0, 10 * __EGBIT_WORD_SIZE__ - 1);
	EGbitReset (mem3, 0, 10 * __EGBIT_WORD_SIZE__ - 1);

	/* === try to find if any bit is set to one in mem1 = */
	pos = EGbitNext (mem1, 0, 10 * __EGBIT_WORD_SIZE__ - 1);
	if (pos < 10 * __EGBIT_WORD_SIZE__)
	{
		fprintf (stderr,
						 "find in position = %u a bit set to one in a zeroed memory lot, in file %s, line %d\n",
						 pos, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	/* == now we set some things and see what happend === */
	EGbitSet (mem2, 0);
	EGbitSet (mem2, 10 * __EGBIT_WORD_SIZE__ - 1);
	EGbitSet (mem2, 13);
	pos = EGbitNext (mem2, 0, 10 * __EGBIT_WORD_SIZE__ - 1);
	if (pos != 0)
	{
		fprintf (stderr,
						 "next one find in position = %u while it should be 0, in file %s, line %d\n",
						 pos, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	pos = EGbitNext (mem2, pos + 1, 10 * __EGBIT_WORD_SIZE__ - 1);
	if (pos != 13)
	{
		fprintf (stderr,
						 "next one find in position = %u while it should be 13, in file %s, line %d\n",
						 pos, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	pos = EGbitNext (mem2, pos + 1, 10 * __EGBIT_WORD_SIZE__ - 1);
	if (pos != 10 * __EGBIT_WORD_SIZE__ - 1)
	{
		fprintf (stderr,
						 "next one find in position = %u while it should be %d, in file %s, line %d\n",
						 pos, 10 * __EGBIT_WORD_SIZE__ - 1, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	EGbitOr (mem1, mem2, 0, 10 * __EGBIT_WORD_SIZE__ - 1);
	pos = EGbitNext (mem1, 0, 10 * __EGBIT_WORD_SIZE__ - 1);
	if (pos != 0)
	{
		fprintf (stderr,
						 "next one find in position = %u while it should be 0, in file %s, line %d\n",
						 pos, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	pos = EGbitNext (mem1, pos + 1, 10 * __EGBIT_WORD_SIZE__ - 1);
	if (pos != 13)
	{
		fprintf (stderr,
						 "next one find in position = %u while it should be 13, in file %s, line %d\n",
						 pos, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	pos = EGbitNext (mem1, pos + 1, 10 * __EGBIT_WORD_SIZE__ - 1);
	if (pos != 10 * __EGBIT_WORD_SIZE__ - 1)
	{
		fprintf (stderr,
						 "next one find in position = %u while it should be %d, in file %s, line %d\n",
						 pos, 10 * __EGBIT_WORD_SIZE__ - 1, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	EGbitXor (mem3, mem2, 0, 10 * __EGBIT_WORD_SIZE__ - 1);
	pos = EGbitNext (mem3, 0, 10 * __EGBIT_WORD_SIZE__ - 1);
	if (pos != 0)
	{
		fprintf (stderr,
						 "next one find in position = %u while it should be 0, in file %s, line %d\n",
						 pos, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	pos = EGbitNext (mem3, pos + 1, 10 * __EGBIT_WORD_SIZE__ - 1);
	if (pos != 13)
	{
		fprintf (stderr,
						 "next one find in position = %u while it should be 13, in file %s, line %d\n",
						 pos, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	pos = EGbitNext (mem3, pos + 1, 10 * __EGBIT_WORD_SIZE__ - 1);
	if (pos != 10 * __EGBIT_WORD_SIZE__ - 1)
	{
		fprintf (stderr,
						 "next one find in position = %u while it should be %d, in file %s, line %d\n",
						 pos, 10 * __EGBIT_WORD_SIZE__ - 1, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	pos = EGbitNext (mem1, 0, 10 * __EGBIT_WORD_SIZE__ - 1);
	if (pos != 0)
	{
		fprintf (stderr,
						 "next one find in position = %u while it should be 0, in file %s, line %d\n",
						 pos, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	pos = EGbitNext (mem1, pos + 1, 10 * __EGBIT_WORD_SIZE__ - 1);
	if (pos != 13)
	{
		fprintf (stderr,
						 "next one find in position = %u while it should be 13, in file %s, line %d\n",
						 pos, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	pos = EGbitNext (mem1, pos + 1, 10 * __EGBIT_WORD_SIZE__ - 1);
	if (pos != 10 * __EGBIT_WORD_SIZE__ - 1)
	{
		fprintf (stderr,
						 "next one find in position = %u while it should be %d, in file %s, line %d\n",
						 pos, 10 * __EGBIT_WORD_SIZE__ - 1, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	EGbitAnd (mem1, mem2, 0, 10 * __EGBIT_WORD_SIZE__ - 1);
	/* === normal ending =================================== */
	return 0;
}

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
EGbitset_t EGbitElemBitCount (EGbitset_t n)
{
#define EGBIT_TWO(c)     (0x1u << (c))
#define EGBIT_MASK(c)    (((unsigned int)(-1)) / (EGBIT_TWO(EGBIT_TWO(c)) + 1u))
#define EGBIT_COUNT(x,c) (EGbitset_t)(((x) & EGBIT_MASK(c)) + (((x) >> (EGBIT_TWO(c))) & EGBIT_MASK(c)))
	n = EGBIT_COUNT (n, 0);
	n = EGBIT_COUNT (n, 1);
	n = EGBIT_COUNT (n, 2);
	n = EGBIT_COUNT (n, 3);
	n = EGBIT_COUNT (n, 4);
#if __EGBIT_WORD_SIZE__ == 64U
	n = EGBIT_COUNT (n, 5);
#endif
#undef EGBIT_TWO
#undef EGBIT_MASK
#undef EGBIT_COUNT
	return n;
}

 /** @brief Count the number of on-bits in a bit-map */
EGbitset_t EGbitCount (EGbitset_t * bitset,
											 const unsigned int from,
											 const unsigned int to)
{
	/* local variables */
	unsigned int i = from >> __EGBIT_SHIFT__;
	unsigned int end = to >> __EGBIT_SHIFT__;
	EGbitset_t res = 0;
	for (; i <= end; i++)
		res += EGbitElemBitCount (bitset[i]);
	return res;
}
