/* EGlib "Efficient General Library" provides some basic structures and
 * algorithms commons in many optimization algorithms.
 *
 * Copyright (C) 2013 Daniel Espinoza.
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
#ifndef __EG_IDXNUM_H__
#define __EG_IDXNUM_H__
/* ========================================================================= */
/** @defgroup EGidxNum EGidxNum
 *
 * Here we define a common interface to handle integer numbers in general,
 * the idea is to provide a common interface to index-numbers, based on the
 * unsigned flavor of numbers, starting from uint8_t until uint128_t. This
 * interface will provide a basis for array-based structures.
 *
 * The basic assumption is that index-variables are given as parameters (not
 * pointers) and that the function using them return complete structures (just
 * as using built-in types); thus declarations as 
 * 
 * EGidx_t f(EGidxc_t,EGidxc_t);
 * EGidx_t a,b;
 * ....
 * EGidx_t c=f(a,b);
 *
 * are assumed to be completelly legal.
 *
 * @par History:
 * Revision 0.9.0
 * 	- 2013-05-27
 * 						- First implementation
 * */

/** @file
 * @ingroup EGidxNum */
/** @addtogroup EGidxNum */
/** @{ */
/* ========================================================================= */
#include "eg_config.h"
#include "eg_macros.h"
#include "eg_mem.h"
/* ========================================================================= */
#include "eg_idx.uint8.h"
#include "eg_idx.uint16.h"
#include "eg_idx.uint32.h"
#include "eg_idx.uint64.h"
/* ========================================================================= */
/** @name Common EGidxNum Interface functions:
 * Here we define the basic functions needed to declare when implementing a
 * number template. */
/** @{ */
#ifdef UNUSED_INTERFACE
/* ========================================================================= */
/** extern definitions of constants for different set-ups */
#define zeroEGidx 	/**<@brief zero-valued constant */
#define oneEGidx 		/**<@brief one-valued constant */
#define MaxEGidx 		/**<@brief maximum representable value */
#define EGidx_t			/**<@brief typedef for modifiable l-value */
#define EGidxc_t		/**<@brief typedef for constant values */
/* ========================================================================= */
/** @brief following strtoui, strtoimax, etc. this function converts a string
 * to the required precision.
 * @param __nptr__ source string pointer
 * @param __endptr__ (if provided) end-pointer while reading
 * @param __base__ base used for conversion
 * @return EGidx_t converted value
 * */
#define EGstrtoIdx(__nptr__,__endptr__,__base__) 
/* ========================================================================= */
/** @brief given an unsigned long, return the equivalent constant as EGidx_t,
 * note that while doing this, we might truncate values.
 * @param __EGidxul__ unsigned long value to convert
 * @return EGidx_t
 * detail. */
#define EG_UL2idx(__EGidxul__)
/* ========================================================================= */
/** @brief Compare if two numbers are equal.
 * @param __EGPa__ EGidx_t first operand
 * @param __EGPb__ EGidx_t second operand
 * @return int one in true, zero oterwise.
 * */
#define EGidxIsEqqual(__EGPa__,__EGPb__)
/* ========================================================================= */
/** @brief test if the first number smaller than the second one
 * @param __EGPa__ EGidx_t the first number.
 * @param __EGPb__ EGidx_t the second number
 * @return int one if true, zero otherwise.
 * */
#define EGidxIsLess(__EGPa__,__EGPb__)
/* ========================================================================= */
/** @brief test if the first number is less than or equal to the second number
 * @param __EGPa__ EGidx_t the first number.
 * @param __EGPb__ EGidx_t the second number
 * @return one if true, zero otherwise.
 * */
#define EGidxIsLeq(__EGPa__,__EGPb__)
/* ========================================================================= */
/** @brief return the adition of the given two values
 * @param __EGPa__ EGidx_t first operand
 * @param __EGPb__ EGidx_t second operand
 * @par Description:
 * This function returns __EGPa__ +  __EGPb__ 
 * */
#define EGidxAdd(__EGPa__,__EGPb__)
/* ========================================================================= */
/** @brief Substract the value of the second operand to the first operand, and
 * return the resulting value
 * @param __EGPa__ EGidx_t first operand
 * @param __EGPb__ EGidx_t sedond operand
 * @return  __EGPa__ - __EGPb__
 * */
#define EGidxSub(__EGPa__,__EGPb__)
/* ========================================================================= */
/** @brief Given two numbers, return its multiplication
 * @param __EGPa__ EGidx_t first operand
 * @param __EGPb__ EGidx_t sedond operand
 * @return  __EGPa__ * __EGPb__
 * */
#define EGidxMult(__EGPa__,__EGPb__)
/* ========================================================================= */
/** @brief Given two numbers, return the (integer part) of their divition.
 * @param __EGPa__ EGidx_t first operand
 * @param __EGPb__ EGidx_t sedond operand
 * @return  floor(__EGPa__ / __EGPb__)
 * */
#define EGidxDiv(__EGPa__,__EGPb__)
/* ========================================================================= */
/** @brief set the value of the variable to zero */
#define EGidxZero(__EGPa__)
/* ========================================================================= */
/** @brief Reset the value of the pointed number to one.
 * @param __EGPa__ EGidx_t value to be set to one.
 * */
#define EGidxOne(__EGPa__)
/* ========================================================================= */
/** @brief unary plus plus operator */
#define EGidxPP(__EGPa__)
/* ========================================================================= */
/** @brief String format for printing a variable of type EGidx_t and conversion
 * to fullfill such format (from a pointer to such a structure)
 * */
#define PRIuidxFmt
#define PRIuidxArg(__EGPa__)
#endif
 /** @} */
/* ========================================================================= */
#endif

