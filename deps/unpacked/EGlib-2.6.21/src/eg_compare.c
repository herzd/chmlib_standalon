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
#include "eg_compare.h"
/** @file
 * @ingroup EGcompare */
/** @{ */
/* ========================================================================= */
/** @brief Lexicographical order of strings.
 * @param str1 pointer to a stream of stirngs.
 * @param str2 pointer to a stream of stirngs.
 * @par Description:
 * This function compare two strings of (\\0 terminated) chars in
 * lexicographical order. */
int EGstringCompare (const void *str1,
										 const void *str2)
{
	return strcmp ((const char *) str1, (const char *) str2);
}

/* ========================================================================= */
/** @brief Normal order of doubles.
 * @par Description:
 * This function compare two double. */
int EGlfCompare (const void *str1,
								 const void *str2)
{
	if (*((const double *) str1) < *((const double *) str2))
		return -1;
	if (*((const double *) str1) > *((const double *) str2))
		return 1;
	return 0;
}

/* ========================================================================= */
/** @brief Normal order of integers.
 * @par Description:
 * This function compare integers. */
int EGdCompare (const void *str1,
								const void *str2)
{
	if (*((const int *) str1) < *((const int *) str2))
		return -1;
	if (*((const int *) str1) > *((const int *) str2))
		return 1;
	return 0;
}

/* ========================================================================= */
/** @brief Normal order of unsigned integers.
 * @par Description:
 * This function compare two unsigned integers. */
int EGudCompare (const void *str1,
								 const void *str2)
{
	return memcmp (str1, str2, sizeof (unsigned int));
}

/* ========================================================================= */
/** @brief Normal order of pointers.
 * @par Description:
 * This function compare pointers in 'memory' order. */
int EGptCompare (const void *str1,
								 const void *str2)
{
	if (str1 < str2)
		return -1;
	if (str1 > str2)
		return 1;
	return 0;
}

/* ========================================================================= */
/** @} */
/* end of eg_compare.c */
