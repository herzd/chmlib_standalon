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
#ifndef __EG_ELINK_H__
#define __EG_ELINK_H__
/* ========================================================================= */
/** @defgroup EGeLink EGeLink
 * This header contains the definition of an embeded link, this simple
 * structure can be used to build trees that only have the information of the
 * parent, and in many other basic structures.
 * @version 0.0.1
 * @par History:
 * 	- 2005-05-25
 * 						- First Implementation
 * */
/** @{ */
/** @file 
 * */
/* ========================================================================= */

/* ========================================================================= */
/** @brief Define a simple link structure. */
typedef struct EGeLink_t
{
	struct EGeLink_t *link;
}
EGeLink_t;

/* ========================================================================= */
/** @brief Set to null the given link.
 * @param __lpt pointer to the structure to set to NULL.
 * @return the given link pointer. */
#define EGeLinkReset(__lpt) ({\
	EGeLink_t*const __lnk__ = (__lpt);\
	__lnk__->link = 0;\
	__lnk__;})

/* ========================================================================= */
/** @brief Set the link to point to itself.
 * @param __lpt pointer to the structure to set.
 * @return the given pointer. */
#define EGeLinkSetSelf(__lpt) ({\
	EGeLink_t*const __lnk__ = (__lpt);\
	__lnk__->link = __lnk__;})

/* ========================================================================= */
/** @brief test wether the given link point to itself.
 * @param __lpt pointer to the link to test.
 * @return one if the link point to itself, zero otherwise. */
#define EGeLinkIsSelf(__lpt) ({\
	EGeLink_t*const __lnk__ = (__lpt);\
	__lnk__->link == __lnk__ ? 1 : 0;})

/* ========================================================================= */
/** @brief test wether the given link is null.
 * @param __lpt pointer to the link to test.
 * @return one if the pointer is null, zero otherwise. */
#define EGeLinkIsNull(__lpt) ({\
	(__lpt)->link ? 0 : 1;})

/* ========================================================================= */
/** @brief Set the given pointer to the given value.
 * @param __lpt pointer to the link to set.
 * @param __inf information to store.
 * @return pointer to the given link structure.
 * @note We don't require the given information to be of type (EGeLink_t)*,
 * but to use that information as a link, it should. */
#define EGeLinkSet(__lpt,__inf) ({\
	EGeLink_t*const __lnk__ = (__lpt);\
	__lnk__->link = (EGeLink_t*)(__inf);\
	__lnk__;})

/* ========================================================================= */
/** @}
 * end of eg_elink.h */
#endif
