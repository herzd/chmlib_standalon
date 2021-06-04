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
/** @file
 * @ingroup EGidxLst
 * */
/** @addtogroup EGidxLst */
/** @{ */
#include "EGlib.h"
/* ========================================================================= */
/**@brief example of integer number lists structure based on embeded lists. */
typedef struct
{
	uint64_EGidx_t n;			/**< actual information contained in the list */
	uint64_EGidxLst_t cn;	/**< structure to put this together in a list */
}
uint64_integer_list_t;
/* ========================================================================= */
#if uint64_MaxEGidx < 134217728ULL
#define uint64_EGLMAXSZ ((uint64_EGidx_t)(uint64_MaxEGidx-4))
#else
#define uint64_EGLMAXSZ ((uint64_EGidx_t)(134217728ULL))
#endif
#if uint64_EG_IDXLIST_DEBUG < DEBUG
#define uint64_EGIDXLPRINT(__i__) fprintf (stderr, "(%"uint64_PRIuidxFmt",%"uint64_PRIuidxFmt") ", uint64_PRIuidxArg(__i__),uint64_PRIuidxArg(number[__i__].n));
#else
#define uint64_EGIDXLPRINT(__i__) 
#endif

/* ========================================================================= */
/** @brief A simple example of using embeded lists
 * @return zero on success, non-zero otherwise.
 * @par Description:
 * Show how to use embeded lists in a dynamic way and in a static fashion. */
int main (void)
{
	uint64_integer_list_t*const number=EGsMalloc(uint64_integer_list_t,uint64_EGLMAXSZ);
	char*const base=((char*)(&(number[0].cn)));
	const size_t stsz=sizeof(uint64_integer_list_t);
	uint64_EGidxc_t back_up=uint64_oneEGidx, head=uint64_zeroEGidx;
	int rval = 0;
	EGrandState_t g;
	uint64_EGidx_t i=uint64_zeroEGidx, it=uint64_zeroEGidx;
	EGlib_info();
	EGlib_version();

	/* initialize the list */
	EGrandInit(&g);
	/* init list wih element zero in it */
	number[head].n=((uint64_EGidx_t)(EGrand(&g)%uint64_EGLMAXSZ));
	uint64_EGidxLstInit (base,stsz,head);
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	/* add all elements to the list and set some value */
	for (i = uint64_oneEGidx; uint64_EGidxIsLess(i,uint64_EGLMAXSZ); uint64_EGidxPP(i))
	{
		number[i].n = ((uint64_EGidx_t)(EGrand (&g) % uint64_EGLMAXSZ));
		uint64_EGidxLstAddBefore (base,stsz,i,head);
	}
	/* display all elements in the list */
	fprintf (stderr, "Numbers in the list:\n");
	it=head;
	do
	{
		uint64_EGIDXLPRINT(it);
		it=number[it].cn.next;
	}while(!uint64_EGidxIsEqqual(it,head));
	fprintf (stderr, "\n");
	/* now we eliminate all even elements in the list, and display the
	 * deleted element */
	fprintf (stderr, "Eliminating values:\n");
	for (i = uint64_oneEGidx; uint64_EGidxIsLess(i,uint64_EGLMAXSZ); uint64_EGidxPP(i))
	{
		uint64_EGidxLstDel (base,stsz,i);
		uint64_EGIDXLPRINT(i);
	}
	fprintf (stderr, "\n");
	/* and display the remaining list */
	fprintf (stderr, "Numbers in the list:\n");
	it=head;
	do
	{
		uint64_EGIDXLPRINT(it);
		it=number[it].cn.next;
	}while(it!=head);
	fprintf (stderr, "\n");
	/* now we set back_up as the odd members, and head to even members */
	uint64_EGidxLstInit(base,stsz,uint64_zeroEGidx);
	for(i=uint64_EG_UL2idx(2U);
			uint64_EGidxIsLess(i,uint64_EGLMAXSZ);
			i=uint64_EGidxAdd(i,uint64_EG_UL2idx(2U)) )
	{
		uint64_EGidxLstAddBefore(base,stsz,i,head);
	}
	uint64_EGidxLstInit(base,stsz,uint64_oneEGidx);
	for(i=uint64_EG_UL2idx(3U);
			uint64_EGidxIsLess(i,uint64_EGLMAXSZ);
			i=uint64_EGidxAdd(i,uint64_EG_UL2idx(2U)) )
	{
		uint64_EGidxLstAddBefore(base,stsz,i,back_up);
	}
	/* now we display both lists */
	/* and display the remaining list */
	fprintf (stderr, "Numbers in head:\n");
	it=head;
	do
	{
		uint64_EGIDXLPRINT(it);
		it=number[it].cn.next;
	}while(it!=head);
	fprintf (stderr, "\n");
	fprintf (stderr, "Numbers in back_up:\n");
	it=back_up;
	do
	{
		uint64_EGIDXLPRINT(it);
		it=number[it].cn.next;
	}while(it!=back_up);
	fprintf (stderr, "\n");
	/* move test */
	fprintf(stderr,"Move elements 3 and 18 betwen lists:\n");
	uint64_EGidxLstMoveAfter(base,stsz,uint64_EG_UL2idx(3U),head);
	uint64_EGidxLstMoveBefore(base,stsz,uint64_EG_UL2idx(18U),back_up);
	fprintf (stderr, "Numbers in head:\n");
	it=head;
	do
	{
		uint64_EGIDXLPRINT(it);
		it=number[it].cn.next;
	}while(it!=head);
	fprintf (stderr, "\n");
	fprintf (stderr, "Numbers in back_up:\n");
	it=back_up;
	do
	{
		uint64_EGIDXLPRINT(it);
		it=number[it].cn.next;
	}while(it!=back_up);
	fprintf (stderr, "\n");
	/* now we splice both lists into back_up list, and reset the head list */
	fprintf(stderr,"Splicing head and back-up:\n");
	uint64_EGidxLstSplice (base,stsz,head,back_up);
	/* now we display both lists */
	fprintf (stderr, "Numbers in head:\n");
	it=head;
	do
	{
		uint64_EGIDXLPRINT(it);
		it=number[it].cn.next;
	}while(it!=head);
	fprintf (stderr, "\n");
	fprintf (stderr, "Numbers in back_up:\n");
	it=back_up;
	do
	{
		uint64_EGIDXLPRINT(it);
		it=number[it].cn.next;
	}while(it!=back_up);
	fprintf (stderr, "\n");
	/* now we replace one member in back_up list with one of the deleted ones
	 * from the complete original list and print the resulting list */
	uint64_EGidxLstDel(base,stsz,uint64_EG_UL2idx(10U));
	uint64_EGidxLstReplace(base,stsz,uint64_EG_UL2idx(11U),uint64_EG_UL2idx(10U));
	fprintf(stderr,"Eliminate 10, and then replace 11 by 10");
	fprintf (stderr, "Numbers in head:\n");
	it=head;
	do
	{
		uint64_EGIDXLPRINT(it);
		it=number[it].cn.next;
	}while(it!=head);
	fprintf (stderr, "\n");
	CLEANUP:
	free(number);
	return rval;
}

/* ========================================================================= */
/** @} */
