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
/* ========================================================================= */
/** @defgroup EGslin_kern Simple Lin-Kernigham 
 *
 * Here we define a simple implementation of the lin-kernighan k-opt heuristic
 * for the TSP. The ideas were taken from ``Finding Tours in the TSP'' of
 * Applegate et al.
 *
 * For simplicity we choose to implement the array structure for managing
 * <code>flips</code>, with the reverse bit version.
 *
 * @version 0.0.1
 * @par History:
 * - 2006-09-06
 * 						- First implementation.
 * */
/** @file
 * @ingroup EGslin_kern */
/** @addtogroup EGslin_kern */
/** @{ 
 * @example eg_slk.ex.c
 * This is a simple example of the usage of this simulator
 * @ref EGsimula */
/* ========================================================================= */
#ifndef __EG_SLK_H__
#define __EG_SLK_H__
#include "eg_config.h"
#include "eg_macros.h"
#include "eg_random.h"
#include "eg_edgraph.h"
#include "dbl_eg_eheap.h"
#include "eg_memslab.h"
#include "eg_io.h"
/* ========================================================================= */
/** @brief structure to hold a (sub-)tour that allow flip operations. */
typedef struct EGflipper_t
{
	int* tour;	/**< hold the order (or reverse order) of the elements in this 
									 tour, from zero to #EGflipper_t::sz - 1  */
	int* inv;		/**< hold the inverse of the ordering, the values range from zero 
									 to #EGflipper_t::sz */
	int* map;		/**< map the internal id (from zero to #EGflipper_t::sz - 1) to 
									 their external numbering. */ 
	size_t sz;	/**< number of elements in this sub-tour */
	unsigned reverse;/**< if set to zero, then the tour has the real ordering, otherwise, it has the reverse ordering of the tour. */ 
	size_t max_sz;	/**< size of the arrays #EGflipper_t::tour, #EGflipper_t::inv and #EGflipper_t::map */
} EGflipper_t;

/* ========================================================================= */
/** @brief debug level */
#define EG_SLK_DEBUG 100

/* ========================================================================= */
/** @brief verbosity level */
#define EG_SLK_VERB 100

/* ========================================================================= */
/** @brief if non-zero, then do a full neighbour search, otherwise, do a first
 * improvement search. */
#define EG_SLK_FULL_SEARCH 0
/* ========================================================================= */
/** @brief structure holding the data related to our instance. It mainly store
 * the cost function. */
typedef struct EGslkData_t
{
	size_t dim; 		/**< Number of cities in the problem */
	int **cost;			/**< double array containing costs of all edges. */
	int *best_tour;	/**< best tour seen so-far */
	int *curr_tour;	/**< Tour that we are using to work */
	int best_val;		/**< value of the best tour */
	int capacity;		/**< Capacity of the trucks */
	int depot;			/**< Depot for the problem */
	int *demand;		/**< Demand on each node */
	double* x;			/**< X-position of the nodes */
	double* y;			/**< Y-position of the nodes */
} EGslkData_t;

/* ========================================================================= */
/** @brief initialize an #EGslkData_t structure.
 * @param data pointer to the structure to initialize */
#define EGslkDataInit(data) (memset(data,0,sizeof(EGslkData_t)))

/* ========================================================================= */
/** @brief clear an #EGslkData_t structure, and free all internal memory.
 * @param data pointer to the structure to initialize */
void EGslkDataClear(EGslkData_t*const data);

/* ========================================================================= */
/** @brief load a TSPLIB95 file format into a EGslkData_t structure
 * @param data pointer to the structure to initialize 
 * @param input pointer from where we will read the data.
 * @return zero on success, non-zero otherwise. */
int EGslkDataLoadTSPLIB95(EGslkData_t*const data,EGioFile_t*input);

/* ========================================================================= */
/** @brief Given two nodes (in #EGslkData_t numbering), and a data structure,
 * return the cost of the edge between the two nodes.
 * @param a tail of the edge.
 * @param b head of the edge.
 * @param data pointer to the data.
 * @return cost of the requested edge.
 * */
int EGslkEdgeCost(EGslkData_t*const data,const int a,const int b);

/* ========================================================================= */
/** @brief Given an initialized flipper structure, and a global data structure,
 * perform a 2-opt run.
 * @param data pointer to the problem data.
 * @param flip pointer to the flipper structure holding the tour.
 * @return zero on success, non-zero otherwise.
 * @note Note that a local-2-opt-optimum is returned in the given flipper, and
 * the original tour stored in the flipper is lost.
 * */
int EGslk2opt(EGslkData_t*const data,EGflipper_t*const flip);

/* ========================================================================= */
/** @brief, given a tour, compute its value 
 * @param tour array containing the tour.
 * @param sz length of the tour.
 * @param data pointer to the problem data.
 * @return the value of the given tour.
 * */
int EGslkTourValue(EGslkData_t*const data,const int*const tour,const size_t sz);

/* ========================================================================= */
/** @brief given an element in the tour (in internal numbering) it return the 
 * next element in the tour (in internal numbering).
 * @param node id of the node from which we want its successor.
 * @param flip structure to initialize 
 * @return the next element in the tour from node 
 * */
int EGslkGetNext(EGflipper_t*const flip,const int node);

/* ========================================================================= */
/** @brief given an element in the tour (in internal numbering) it return the 
 * previous element in the tour (in internal numbering).
 * @param node id of the node from which we want its predecesor.
 * @param flip structure to initialize
 * @return the previous element in the tour from node
 * */
int EGslkGetPrev(EGflipper_t*const flip,const int node);

/* ========================================================================= */
/** @brief given a #EGflipper_t structure, and two nodes a,b in the structure,
 * flip the segment a,b in the tour.
 * @param a id of the begin of the segment to flip.
 * @param b id of the end of the segment to flip.
 * @param flip structure holding the tour.
 * @return zero on success, non-zero otherwise.
 * @note that EGslkFlip(flip,a,b) is different from EGslkFlip(flip,b,a) .
 * */
int EGslkFlip(EGflipper_t*const flip,const int a, const int b);

/* ========================================================================= */
/** @brief Load a tour into a #EGflipper_t structure.
 * @param sz length of the tour.
 * @param tour array of length sz that we want to load.
 * @param flip strucutre where we will load the tour.
 * @return zero on success, non-zero otherwise.
 * @note this function will resize automatically the #EGflipper_t structure if
 * necesary.
 * */
int EGflipperLoadTour(EGflipper_t*const flip,const int*const tour,size_t const sz);

/* ========================================================================= */
/** @brief Load from a #EGflipper_t structure to an array the implicit tour.
 * @param tour array where we will load the tour.
 * @param flip strucutre holding the tour.
 * @return zero on success, non-zero otherwise.
 * */
int EGflipperGetTour(EGflipper_t*const flip,int*const tour);

/* ========================================================================= */
/** @brief Return the cost of the tour stored in the flipper structure.
 * @param flip strucutre holding the tour.
 * @param data pointer structure. 
 * @return zero on success, non-zero otherwise.
 * */
int EGflipperGetCost(EGslkData_t*const data,EGflipper_t*const flip);

/* ========================================================================= */
/** @brief Display a flipper structure to the given file.
 * @param flip strucutre holding the tour.
 * @param out where to print the structure
 * @return zero on success, non-zero otherwise.
 * */
int EGflipperDisplay(EGflipper_t*const flip,EGioFile_t* out);

/* ========================================================================= */
/** @brief Given a flip, kick it, to obtain a different starting point for
 * lin-kernighan. This idea, and the special kick we use is due to  Martin,
 * Otto and Felten (1991,1992). The kick that we are using is a so-called
 * double bridge.
 * @param flip strucutre holding the tour.
 * @return zero on success, non-zero otherwise.
 * */
int EGslkKick(EGflipper_t*const flip);

/* ========================================================================= */
/** @brief Initialize a #EGflipper_t structure (with no space for storing
 * tours). 
 * @param flip structure to initialize */
#define EGflipperInit(flip) (memset((flip),0,sizeof(EGflipper_t)))

/* ========================================================================= */
/** @brief Leave a #EGflipper_t structure as returned by #EGflipperInit
 * function, but freeing any internal memory if needed 
 * @param flip structure to initialize */
#define EGflipperClear(flip) ({\
	EGflipper_t*const __EGslkflip = (flip);\
	if(__EGslkflip->tour) EGfree(__EGslkflip->tour);\
	if(__EGslkflip->inv) EGfree(__EGslkflip->inv);\
	if(__EGslkflip->map) EGfree(__EGslkflip->map);\
	memset(__EGslkflip,0,sizeof(EGflipper_t));})

/* ========================================================================= */
/** @brief resize the maximum capacity of a #EGflipper_t structure 
 * @note Note that if the new size is smaller than the current size of the
 * stored tour, then the stored tour is lost, and the internal size will be
 * reset to zero.
 * @param new_sz new size for the internal arrays.
 * @param flip structure to initialize */
void EGflipperResize(EGflipper_t*const flip,const size_t new_sz);

/* ========================================================================= */
/** @brief remove an element from the flipper structure.
 * @param node internal ID of the element to remove.
 * @param flip structure containing a route.
 * @return zero on success, non-zero otherwise. */
int EGflipperRemoveNode(EGflipper_t*const flip,const int node);

/* ========================================================================= */
/** @brief add an element from the flipper structure.
 * @param node external ID of the element to add.
 * @param flip structure containing a route.
 * @return zero on success, non-zero otherwise. */
int EGflipperAddNode(EGflipper_t*const flip,const int node);

/* ========================================================================= */
/** @brief Copy a flipper structure to another */
void EGflipperCopy(const EGflipper_t*const src,EGflipper_t*const dst);

/* ========================================================================= */
/** @} */
/* end of eg_slk.h */
#endif
