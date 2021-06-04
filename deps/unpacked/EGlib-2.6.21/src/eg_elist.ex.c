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
 * @ingroup EGeList
 * */
/** @addtogroup EGeList */
/** @{ */
#include "EGlib.h"
/* ========================================================================= */
/**@brief example of integer number lists structure based on embeded lists. */
typedef struct
{
	uint32_t n;				/**< actual information contained in the list */
	EGeList_t cn;	/**< structure to put this together in a list */
}
integer_list_t;

/* ========================================================================= */
/** @brief A simple example of using embeded lists
 * @return zero on success, non-zero otherwise.
 * @par Description:
 * Show how to use embeded lists in a dynamic way and in a static fashion. */
int main (void)
{
	int rval = 0;
	integer_list_t number[20],
	 *n_ptr;
	EGrandState_t g;
	EGeList_t head=EGeListNull,
	  back_up=EGeListNull,
	 *it,
	 *itn;
	int i;
	EGlib_info();
	EGlib_version();

	/* initialize the list */
	EGrandInit(&g);
	EGeListInit (&head);
	EGeListInit (&back_up);
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	/* check that the list is non empty */
	if (EGeListIsEmpty (&head))
		fprintf (stderr, "List is empty at beginning\n");
	/* add all elements to the list and set some value */
	for (i = 0; i < 20; i++)
	{
		number[i].n = EGrand (&g) % 500;
		EGeListAddBefore (&(number[i].cn), &head);
	}
	/* display all elements in the list */
	fprintf (stderr, "Numbers in the list:\n");
	for (it = head.next, i = 0; it != &head; it = it->next, i++)
	{
		n_ptr = EGcontainerOf (it, integer_list_t, cn);
		fprintf (stderr, "(%u,%u) ", n_ptr->n, number[i].n);
	}
	fprintf (stderr, "\n");
	/* now we eliminate all even elements in the list, and display the
	 * deleted element */
	fprintf (stderr, "Eliminating values:\n");
	for (i = 0; i < 10; i++)
	{
		it = EGeListDel (&(number[i << 1].cn));
		n_ptr = EGcontainerOf (it, integer_list_t, cn);
		fprintf (stderr, "%d ", n_ptr->n);
	}
	fprintf (stderr, "\n");
	/* and display the remaining list */
	fprintf (stderr, "Numbers in the list:\n");
	for (it = head.next, i = 0; it != &head; it = it->next, i++)
	{
		n_ptr = EGcontainerOf (it, integer_list_t, cn);
		fprintf (stderr, "(%d,%d) ", n_ptr->n, number[(i << 1) + 1].n);
	}
	fprintf (stderr, "\n");
	/* now we move halve of the members in the list to back_up list, note
	 * however that as it is we can't move tyhe current element and then ask for
	 * the next one, because after we move the entry, the next in the for loop
	 * is the head of the back_up list */
	for (it = head.next, i = 0; it != &head; i++)
	{
		itn = it->next;
		if (i % 2 == 0)
			EGeListMoveBefore (it, &back_up);
		it = itn;
	}
	/* now we display both lists */
	fprintf (stderr, "head: ");
	for (it = head.next; it != &head; it = it->next)
	{
		n_ptr = EGcontainerOf (it, integer_list_t, cn);
		fprintf (stderr, "%d ", n_ptr->n);
	}
	fprintf (stderr, "\nback_up: ");
	for (it = back_up.next; it != &back_up; it = it->next)
	{
		n_ptr = EGcontainerOf (it, integer_list_t, cn);
		fprintf (stderr, "%d ", n_ptr->n);
	}
	fprintf (stderr, "\n");
	/* now we splice both lists into back_up list, and reset the head list */
	EGeListSplice (&head, &back_up);
	EGeListInit (&head);
	/* now we display both lists */
	fprintf (stderr, "head: ");
	for (it = head.next; it != &head; it = it->next)
	{
		n_ptr = EGcontainerOf (it, integer_list_t, cn);
		fprintf (stderr, "%d ", n_ptr->n);
	}
	fprintf (stderr, "\nback_up: ");
	for (it = back_up.next; it != &back_up; it = it->next)
	{
		n_ptr = EGcontainerOf (it, integer_list_t, cn);
		fprintf (stderr, "%d ", n_ptr->n);
	}
	fprintf (stderr, "\n");
	/* now we replace one member in back_up list with one of the deleted ones
	 * from the complete original list and print the resulting list */
	EGeListReplace (back_up.next, &(number[0].cn));
	fprintf (stderr, "back_up: ");
	for (it = back_up.next; it != &back_up; it = it->next)
	{
		n_ptr = EGcontainerOf (it, integer_list_t, cn);
		fprintf (stderr, "%d ", n_ptr->n);
	}
	fprintf (stderr, "\n");
	CLEANUP:
	return rval;
}

/* ========================================================================= */
/** @} */
