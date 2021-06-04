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
#ifndef uint16__EG_IDXNUM_H__
#define uint16__EG_IDXNUM_H__
/** @file
 * @ingroup EGidxNum */
/** @addtogroup EGidxNum */
/** @{ */
/* ========================================================================= */
#include "eg_idx.h"
/* ========================================================================= */
/** extern definitions of constants for different set-ups */
#define uint16_zeroEGidx ((uint16_t)0U)	/**<@brief zero-valued constant */
#define uint16_oneEGidx 	((uint16_t)1U)	/**<@brief one-valued constant */
#define uint16_MaxEGidx 	UINT16_MAX			/**<@brief maximum representable value */
#define uint16_EGidx_t uint16_t					/**<@brief typedef for modifiable l-value */
#define uint16_EGidxc_t const uint16_t	/**<@brief typedef for constant values */
/* ========================================================================= */
/** @brief following strtoui, strtoimax, etc. this function converts a string
 * to the required precision.
 * @param __nptr__ source string pointer
 * @param __endptr__ (if provided) end-pointer while reading
 * @param __base__ base used for conversion
 * @return EGidx_t converted value
 * */
#define uint16_EGstrtoIdx(__nptr__,__endptr__,__base__) strtoul(__nptr__,__endptr,__base__)
/* ========================================================================= */
/** @brief given an unsigned long, return the equivalent constant as EGidx_t,
 * note that while doing this, we might truncate values.
 * @param __EGidxul__ unsigned long value to convert
 * @return EGidx_t
 * detail. */
#define uint16_EG_UL2idx(__EGidxul__)	((uint16_t)(__EGidxul__))
/* ========================================================================= */
/** @brief Compare if two numbers are equal.
 * @param __EGPa__ EGidx_t first operand
 * @param __EGPb__ EGidx_t second operand
 * @return int one in true, zero oterwise.
 * */
#define uint16_EGidxIsEqqual(__EGPa__,__EGPb__) ((__EGPa__)==(__EGPb__))
/* ========================================================================= */
/** @brief test if the first number smaller than the second one
 * @param __EGPa__ EGidx_t the first number.
 * @param __EGPb__ EGidx_t the second number
 * @return int one if true, zero otherwise.
 * */
#define uint16_EGidxIsLess(__EGPa__,__EGPb__) ((__EGPa__)<(__EGPb__))
/* ========================================================================= */
/** @brief test if the first number is less than or equal to the second number
 * @param __EGPa__ EGidx_t the first number.
 * @param __EGPb__ EGidx_t the second number
 * @return one if true, zero otherwise.
 * */
#define uint16_EGidxIsLeq(__EGPa__,__EGPb__) ((__EGPa__)<=(__EGPb__))
/* ========================================================================= */
/** @brief return the adition of the given two values
 * @param __EGPa__ EGidx_t first operand
 * @param __EGPb__ EGidx_t second operand
 * @par Description:
 * This function returns __EGPa__ +  __EGPb__ 
 * */
#define uint16_EGidxAdd(__EGPa__,__EGPb__)	((uint16_t)((__EGPa__)+(__EGPb__)))
/* ========================================================================= */
/** @brief Substract the value of the second operand to the first operand, and
 * return the resulting value
 * @param __EGPa__ EGidx_t first operand
 * @param __EGPb__ EGidx_t sedond operand
 * @return  __EGPa__ - __EGPb__
 * */
#define uint16_EGidxSub(__EGPa__,__EGPb__) ((uint16_t)((__EGPa__)-(__EGPb__)))
/* ========================================================================= */
/** @brief Given two numbers, return its multiplication
 * @param __EGPa__ EGidx_t first operand
 * @param __EGPb__ EGidx_t sedond operand
 * @return  __EGPa__ * __EGPb__
 * */
#define uint16_EGidxMult(__EGPa__,__EGPb__) ((uint16_t)((__EGPa__)*(__EGPb__)))
/* ========================================================================= */
/** @brief Given two numbers, return the (integer part) of their divition.
 * @param __EGPa__ EGidx_t first operand
 * @param __EGPb__ EGidx_t sedond operand
 * @return  floor(__EGPa__ / __EGPb__)
 * */
#define uint16_EGidxDiv(__EGPa__,__EGPb__) ((uint16_t)((__EGPa__)/(__EGPb__)))
/* ========================================================================= */
/** @brief set the value of the variable to zero */
#define uint16_EGidxZero(__EGPa__) uint16_zeroEGidx
/* ========================================================================= */
/** @brief Reset the value of the pointed number to one.
 * @param __EGPa__ EGidx_t value to be set to one.
 * */
#define uint16_EGidxOne(__EGPa__) uint16_oneEGidx
/* ========================================================================= */
/** @brief unary plus plus operator */
#define uint16_EGidxPP(__EGPa__) ((__EGPa__)++)
/* ========================================================================= */
/** @brief String format for printing a variable of type EGidx_t and conversion
 * to fullfill such format (from a pointer to such a structure)
 * */
#define uint16_PRIuidxFmt PRIu16
#define uint16_PRIuidxArg(__EGPa__) ((uint16_t)(__EGPa__))
#endif
 /** @} */
/* ========================================================================= */
