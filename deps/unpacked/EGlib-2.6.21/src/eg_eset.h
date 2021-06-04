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
/** @defgroup EGeSet EGeSet 
 * This is an implementation of the set's defined at 'Data Structures and
 * Network Algorithms' of Robert Endre Tarjan. This structures allow to work
 * with disjoint setswith a representant (thus an equivalence class) and ask the
 * basic question of, given an elment, who is the representant if his class,
 * make a set, link two sets (i.e. union if them) and given a representant link
 * an element to that class.
 *
 * @version 0.0.1
 * @par History:
 * - 2005-06-20
 * 					- First Implementation.
 * */
/** @file 
 * @ingroup EGeSet */
/** @addtogroup EGeSet */
/** @{ */
/* ========================================================================= */
#ifndef __EG_ESET_H__
#define __EG_ESET_H__
#include "eg_config.h"
#include "eg_macros.h"
#ifndef EG_ESET_DLEVEL
#define EG_ESET_DLEVEL 0
#endif

/* ========================================================================= */
/** @brief this structure holds an element of a set */
typedef struct EGes_t
{
	struct EGes_t *father;/**> pointer to the representing element on the 
													 	 class where this element bellong. */
	unsigned int rank;		/**> This is a lower bound on the number of 
														 elements for wich this is the represenntant */
}
EGes_t;

/* ========================================================================= */
/** @brief Initialize a set element as a set containing only itsself */
#define EGesInit(__elem) ({\
	EGes_t*const __EGesElm = (__elem);\
	*__EGesElm = (EGes_t){__EGesElm,0};})

/* ========================================================================= */
/** @brief this function find the representant of this element in his 
 * equivalence set.
 * @param __elem pointer to the element to wich we want to find the
 * representant.
 * @return the representant for this set (EGes_t*). */
#define EGesFind(__elem) ({\
	EGes_t *_EGesFc = __elem, *_EGesFn;\
	while(_EGesFc != _EGesFc->father)\
	{\
		_EGesFn = _EGesFc->father;\
		_EGesFc->father = _EGesFn->father;\
		_EGesFc = _EGesFn;\
	}\
	_EGesFc;})

/* ========================================================================= */
/**@brief Given two representing elements for two clases, joint them into a
 * single class.
 * @param __u first representative.
 * @param __v second representative.
 * @return the representing element of the new class.
 * @note This implementation always let the first element to be the 
 * representative for the new class. */
#define EGesLink(__u,__v) ({\
	EGes_t *_EGes_lu = (__u), *_EGes_lv = (__v);\
	EXITL(EG_ESET_DLEVEL, _EGes_lu != _EGes_lu->father, #__u\
				" is not a representant for its class");\
	EXITL(EG_ESET_DLEVEL, _EGes_lv != _EGes_lv->father, #__v\
				" is not a representant for its class");\
	EXITL(EG_ESET_DLEVEL, _EGes_lu == _EGes_lv, "same representant v == u");\
	if(_EGes_lu->rank <= _EGes_lv->rank) _EGes_lu->rank = _EGes_lv->rank+1;\
	_EGes_lv->father = _EGes_lu;\
	_EGes_lu;})

/* ========================================================================= */
/** @} end eg_eset.h */
#endif
