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
#include "eg_ehash.h"
/** @file
 * @ingroup EGeHash */
/** @addtogroup EGeHash */
/** @{ */
/* ========================================================================= */
/* this is for the hash function */
/* prime number, this is one of the 1024 biggest prime for unsigned int */
const EGkey_t EGhashPrime = 4294959751LLU;
/* other prime number */
const EGkey_t EGhashB = 109180541LLU;
/* and yet other prime number */
const EGkey_t EGhashA = 179422291LLU;


/** @} */
