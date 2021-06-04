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
 * @ingroup EGaList
 * */
/** @addtogroup EGaList */
/** @{ */
#include "EGlib.h"
/* ========================================================================= */
/**@brief example of integer number lists structure based on embeded lists. */
typedef struct
{
	unsigned n;				/**< actual information contained in the list */
	EGaList_t cn;	/**< structure to put this together in a list */
}
integer_list_t;

/* ========================================================================= */
/** @brief A simple example of using embeded lists
 * @return zero on success, non-zero otherwise.
 * @par Description:
 * Show how to use embeded lists in a dynamic way and in a static fashion. */
int main (void)
{
	integer_list_t number[20];
	char*const base=((char*)(&(number[0].cn)));
	const size_t stsz=sizeof(integer_list_t);
	int rval = 0;
	EGrandState_t g;
	int head=-1,back_up=-1,it;
	int i;
	EGlib_info();
	EGlib_version();

	/* initialize the list */
	EGrandInit(&g);
	/* init list wih element zero in it */
	head=0;
	number[0].n=EGrand(&g)%500;
	EGaListInit (base,stsz,head);
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	/* add all elements to the list and set some value */
	for (i = 1; i < 20; i++)
	{
		number[i].n = EGrand (&g) % 500;
		EGaListAddBefore (base,stsz,i,head);
	}
	/* display all elements in the list */
	fprintf (stderr, "Numbers in the list:\n");
	it=head;
	do
	{
		fprintf (stderr, "(%d,%u) ", it,number[it].n);
		it=number[it].cn.next;
	}while(it!=head);
	fprintf (stderr, "\n");
	/* now we eliminate all even elements in the list, and display the
	 * deleted element */
	fprintf (stderr, "Eliminating values:\n");
	for (i = 1; i < 20; i++)
	{
		EGaListDel (base,stsz,i);
		fprintf (stderr, "(%d,%u) ",i,number[i].n);
	}
	fprintf (stderr, "\n");
	/* and display the remaining list */
	fprintf (stderr, "Numbers in the list:\n");
	it=head;
	do
	{
		fprintf (stderr, "(%d,%u) ", it,number[it].n);
		it=number[it].cn.next;
	}while(it!=head);
	fprintf (stderr, "\n");
	/* now we set back_up as the odd members, and head to even members */
	for(i=2;i<20;i+=2)
	{
		EGaListAddBefore(base,stsz,i,head);
	}
	back_up=1;
	EGaListInit(base,stsz,back_up);
	for(i=3;i<20;i+=2)
	{
		EGaListAddBefore(base,stsz,i,back_up);
	}
	/* now we display both lists */
	/* and display the remaining list */
	fprintf (stderr, "Numbers in head:\n");
	it=head;
	do
	{
		fprintf (stderr, "(%d,%u) ", it,number[it].n);
		it=number[it].cn.next;
	}while(it!=head);
	fprintf (stderr, "\n");
	fprintf (stderr, "Numbers in back_up:\n");
	it=back_up;
	do
	{
		fprintf (stderr, "(%d,%u) ", it,number[it].n);
		it=number[it].cn.next;
	}while(it!=back_up);
	fprintf (stderr, "\n");
	/* move test */
	fprintf(stderr,"Move elements 3 and 18 betwen lists:\n");
	EGaListMoveAfter(base,stsz,3,head);
	EGaListMoveBefore(base,stsz,18,back_up);
	fprintf (stderr, "Numbers in head:\n");
	it=head;
	do
	{
		fprintf (stderr, "(%d,%u) ", it,number[it].n);
		it=number[it].cn.next;
	}while(it!=head);
	fprintf (stderr, "\n");
	fprintf (stderr, "Numbers in back_up:\n");
	it=back_up;
	do
	{
		fprintf (stderr, "(%d,%u) ", it,number[it].n);
		it=number[it].cn.next;
	}while(it!=back_up);
	fprintf (stderr, "\n");
	/* now we splice both lists into back_up list, and reset the head list */
	fprintf(stderr,"Splicing head and back-up:\n");
	EGaListSplice (base,stsz,head,back_up);
	/* now we display both lists */
	fprintf (stderr, "Numbers in head:\n");
	it=head;
	do
	{
		fprintf (stderr, "(%d,%u) ", it,number[it].n);
		it=number[it].cn.next;
	}while(it!=head);
	fprintf (stderr, "\n");
	fprintf (stderr, "Numbers in back_up:\n");
	it=back_up;
	do
	{
		fprintf (stderr, "(%d,%u) ", it,number[it].n);
		it=number[it].cn.next;
	}while(it!=back_up);
	fprintf (stderr, "\n");
	/* now we replace one member in back_up list with one of the deleted ones
	 * from the complete original list and print the resulting list */
	EGaListDel(base,stsz,10);
	EGaListReplace(base,stsz,11,10);
	fprintf(stderr,"Eliminate 10, and then replace 11 by 10");
	fprintf (stderr, "Numbers in head:\n");
	it=head;
	do
	{
		fprintf (stderr, "(%d,%u) ", it,number[it].n);
		it=number[it].cn.next;
	}while(it!=head);
	fprintf (stderr, "\n");
	CLEANUP:
	return rval;
}

/* ========================================================================= */
/** @} */
