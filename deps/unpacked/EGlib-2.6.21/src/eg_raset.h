/* EGlib "Efficient General Library" provides some basic structures and
 * algorithms commons in many optimization algorithms.
 *
 * Copyright (C) 2010 Daniel Espinoza
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
/** @defgroup EGrSet EGrSet
 *
 * Here we define the basic interface for random-access sets. They provide a
 * mean for adding/deleting elements, checking for inclusion in constant
 * time, and looping through the set in time o(|set|).
 *
 * This comes at a price, the structure uses two arrays of length the
 * ``base set'' size. 
 *
 * @version 0.0.1
 * @par History:
 * - 2010-09-02
 * 						- First Implementation
 * */
/** @file 
 * @ingroup EGrSet */
/** @addtogroup EGrSet */
/** @{ */
/** @example eg_raset.ex.c
 * This is a simple example of the usage of heaps using @ref EGrSet */
/* ========================================================================= */
#ifndef __EG_RASET__
#define __EG_RASET__
#include "eg_config.h"
#include "eg_macros.h"
#include "eg_mem.h"
/* ========================================================================= */
/** @brief Debug level for the heap */
#ifndef EG_RSET_DEBUG
#define EG_RSET_DEBUG 1000
#endif
/* ========================================================================= */
/** @brief set structure */
typedef struct 
{
	int32_t sz;		/**< @brief number of elements in the set */
	int32_t grsz;	/**< @brief number of elements in ground set */
	int32_t asz;	/**< @brief actual length of EGrset_t::set and EGrset_t::inv */
	int32_t*set;	/**< @brief array of length EGrset_t::asz, it contains in 
											positions [0,sz-1] the elements in the set, and in 
											positions [sz,grsz-1] the complement */
	int32_t*inv;	/**< @brief array of length EGrset_t::asz, it contains the
											inverse positions for the complete EGrset_t::set array,
											i.e. set[inv[i]]=i for i in [0,asz-1] */
}
EGrset_t;
/* ========================================================================= */
/** @brief null initializer */
#define EGrsetNull ((EGrset_t){(int32_t)0,(int32_t)0,(int32_t)0,(int32_t*)0,(int32_t*)0})
/* ========================================================================= */
/** @brief display the contents of the set to the specified FILE buffer */
#define EGrsetDisplay(__EGrset__,__file__) do{\
	const EGrset_t*const __EGs = (__EGrset__);\
	const int32_t __EGsz = __EGs->sz, __EGasz = __EGs->asz, __EGgrsz = __EGs->grsz;\
	int __EGi;\
	fprintf(__file__,"rset %p: sz %"PRId32" asz %"PRId32" grsz %"PRId32"\n\tSet:\n\t\tset:",\
					__EGs, __EGsz,__EGasz,__EGgrsz);\
	for(__EGi=0;__EGi<__EGsz;__EGi++){fprintf(__file__," %2"PRId32,__EGs->set[__EGi]);}\
	fprintf(__file__,"\n\t\tinv:");\
	for(__EGi=0;__EGi<__EGsz;__EGi++){fprintf(__file__," %2"PRId32,__EGs->inv[__EGi]);}\
	fprintf(__file__,"\n\tComplement:\n\t\tset:");\
	for(__EGi=__EGsz;__EGi<__EGgrsz;__EGi++){fprintf(__file__," %2"PRId32,__EGs->set[__EGi]);}\
	fprintf(__file__,"\n\t\tinv:");\
	for(__EGi=__EGsz;__EGi<__EGgrsz;__EGi++){fprintf(__file__," %2"PRId32,__EGs->inv[__EGi]);}\
	fprintf(__file__,"\n\tExtra_Space:\n\t\tset:");\
	for(__EGi=__EGgrsz;__EGi<__EGasz;__EGi++){fprintf(__file__," %2"PRId32,__EGs->set[__EGi]);}\
	fprintf(__file__,"\n\t\tinv:");\
	for(__EGi=__EGgrsz;__EGi<__EGasz;__EGi++){fprintf(__file__," %2"PRId32,__EGs->inv[__EGi]);}\
	fprintf(__file__,"\n");}while(0)
/* ========================================================================= */
/** @brief empty a set 
 * @par __EGrset__ pointer to the structure
 * */
#define EGrsetEmpty(__EGrset__) ((__EGrset__)->sz=0)
/* ========================================================================= */
/** @brief fill a set
 * @par __EGrset__ pointer to the structure
 * */
#define EGrsetFill(__EGrset__) ((__EGrset__)->sz=(__EGrset__)->grsz)
/* ========================================================================= */
/** @brief Initialize an EGrset_t with the given size, note that both
 * ground-size and set size will be __grsz__, but the set will be set empty by
 * default.
 * @par __EGrset__ pointer to the structure
 * @par __grsz__ size of the ground set 
 * */
#define EGrsetInit(__EGrset__,__grsz__) do{\
	EGrset_t*const __EGs__ = (__EGrset__);\
	int __EGi__ = (__EGs__->asz = __EGs__->grsz = (__grsz__));\
	__EGs__->sz = 0;\
	__EGs__->set = EGsMalloc(int32_t,((size_t)__EGi__)*sizeof(int32_t));\
	__EGs__->inv = EGsMalloc(int32_t,((size_t)__EGi__)*sizeof(int32_t));\
	for(;__EGi__--;) __EGs__->inv[__EGi__] = __EGs__->set[__EGi__] = __EGi__;}while(0)
/* ========================================================================= */
/** @brief Clear an EGrset_t and leave it as a null rset
 * @par __EGrset__ pointer to the structure
 * */
#define EGrsetClear(__EGrset__) do{\
	EGrset_t*const __EGs__ = (__EGrset__);\
	EGfree(__EGs__->inv);\
	EGfree(__EGs__->set);\
	__EGs__->asz = __EGs__->sz = __EGs__->grsz = 0;}while(0)
/* ========================================================================= */
/** @brief helper macro to swap elements one and two in both inv and set
 * arrays
 * @par __EGs pointer to the structure (we assume it is internal and safe
 * to use it directly)
 * @par __EGe1 first element (idem, it can be a constant value).
 * @par __EGe2 second element (idem, it can be a constant value).
 * */
#define __EGrsetSwap(__EGs,__EGe1,__EGe2) do{\
	const int32_t __EGt = __EGs->inv[__EGe1];\
	__EGs->inv[__EGe1] = __EGs->inv[__EGe2];\
	__EGs->inv[__EGe2] = __EGt;\
	__EGs->set[__EGt] = __EGe2;\
	__EGs->set[__EGs->inv[__EGe1]] = __EGe1;}while(0)
/* ========================================================================= */
/** @brief Add element __i__
 * @par __EGrset__ pointer to the structure
 * @par __i__ element to add
 * @note if the element was in the set, do nothing
 * */
#define EGrsetAdd(__EGrset__,__i__) do{\
	EGrset_t*const __EGs__ = (__EGrset__);\
	const int32_t __EGi__ = (__i__),__EGp__=__EGs__->inv[__EGi__],__EGr__=__EGs__->set[__EGs__->sz];\
	if(__EGp__>=__EGs__->sz){\
		__EGrsetSwap(__EGs__,__EGi__,__EGr__);\
		__EGs__->sz++;}}while(0)
/* ========================================================================= */
/** @brief Del element __i__
 * @par __EGrset__ pointer to the structure
 * @par __i__ element to del
 * @note if the element was not in the set, do nothing
 * */
#define EGrsetDel(__EGrset__,__i__) do{\
	EGrset_t*const __EGs2__ = (__EGrset__);\
	const int32_t __EGi2__ = (__i__);\
	const int32_t __EGp2__=__EGs2__->inv[__EGi2__],__EGr2__=__EGs2__->set[__EGs2__->sz-1];\
	if(__EGp2__<__EGs2__->sz){\
		__EGrsetSwap(__EGs2__,__EGi2__,__EGr2__);\
		__EGs2__->sz--;}}while(0)
/* ========================================================================= */
/** @brief test if an element is in the set
 * @par __EGrset__ pointer to the structure
 * @par __i__ element to test
 * */
#define EGrsetPeek(__EGrset__,__i__) ((__EGrset__)->sz > (__EGrset__)->inv[__i__])
/* ========================================================================= */
/** @brief Re-set the groud set size.
 * if smaller than  previous value, it implements S = S \ {new_sz,...,old_sz}. 
 * if bigger than previous value, it implements S = S
 * @par EGs pointer to the structure
 * @par grsz new ground set */
void EGrsetSetgrnd(
	EGrset_t*const EGs,
	const int32_t grsz);
/* ========================================================================= */
/** @brief test structure consistency */
#define EGrsetCheck(__EGrset__) ({\
	const EGrset_t*const __EGs__ = (__EGrset__);\
	int32_t __EGi__;\
	int __rval__=0;\
	for(__EGi__=__EGs__->asz ; __EGi__--;){\
		__rval__=+(__EGi__!=__EGs__->inv[__EGs__->set[__EGi__]]);}\
	for(__EGi__=__EGs__->grsz; __EGi__<__EGs__->asz;__EGi__++){\
		__rval__+=(__EGs__->inv[__EGi__]<__EGs__->grsz);}\
	if(__rval__) fprintf(stdout,"ERROR\n");\
	__rval__;})
/* ========================================================================= */
/** @} */
/* end of eg_raset.h */
#endif
