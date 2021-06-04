/* EGlib "Efficient General Library" provides some basic structures and
 * algorithms commons in many optimization algorithms.
 *
 * Copyright (C) 2005-2008 Daniel Espinoza
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
#include "eg_symtab.h"
#include "eg_nummacros.h"
/** @file
 * @ingroup EGsymtab */
/** @addtogroup EGsymtab */
/** @{ */
/* ========================================================================= */
/** @brief permutation sort for strings */
void EGcharPermSort (const size_t sz,
										 int *const perm,
										 const int* const elem,
										 const char*const all_elem)
{
	size_t i, j;
	int temp;
	const char* t;
	if (sz <= 1)
		return;
	EGswap (perm[0], perm[(sz - 1) / 2], temp);
	i = 0;
	j = sz;
	t = all_elem+elem[perm[0]];
	for (;;)
	{
		do
			i++;
		while (i < sz && (strncmp(all_elem+elem[perm[i]],t,(size_t)1024) < 0));
		do
			j--;
		while (strncmp(t,all_elem+elem[perm[j]],(size_t)1024) < 0);
		if (j < i)
			break;
		EGswap (perm[i], perm[j], temp);
	}
	EGswap (perm[0], perm[j], temp);
	EGcharPermSort (j, perm, elem, all_elem);
	EGcharPermSort (sz - i, perm + i, elem, all_elem);
}
/* ========================================================================= */
/** @brief internal function used to sort our symbol table */
void EGsymtabSort(EGsymtab_t*const symtab)
{
	register int i;
	for( i = ((int)(symtab->nsym)) ; i-- ; ) symtab->sort_perm[i] = i;
	EGcharPermSort( symtab->nsym, symtab->sort_perm,
									symtab->symbols,symtab->all_sym);
}
/* ========================================================================= */
int EGsymtabLookUp( EGsymtab_t*const symtab,
										const char*const symbol,
										int*const pos)
{
	int*const perm = symtab->sort_perm;
	const char*const all_sym = symtab->all_sym;
	const int*const symbols = symtab->symbols;
	int lo = 0, up = (int)(symtab->nsym-1), mid;
	int res, found=0;
	if(symtab->must_sort)
		EGsymtabSort(symtab);
	symtab->must_sort = 0;
	/* now we do binary search */
	while(lo <= up)
	{
		mid = (lo+up)/2;
		res = strncmp(symbol,all_sym+symbols[perm[mid]],(size_t)1024);
		if(res < 0) up = mid-1;
		else if( res > 0) lo = mid+1;
		else
		{
			found = 1; 
			break;
		}
	}
	if(pos && found) *pos = perm[mid];
	return found;
}
/* ========================================================================= */
int EGsymtabAdd(EGsymtab_t*const symtab,const char*const symbol)
{
	const size_t len = strlen(symbol)+1;
	int pos = 0;
	int rval = EGsymtabLookUp(symtab,symbol,&pos);
	FTESTG(rval,CLEANUP,"Symbol %s is already in table, in pos %d",symbol,pos);
	FTESTG((rval=(len>(size_t)4096)),CLEANUP,"symbol to add is too long %zu",len);
	/* now we add the symbol */
	symtab->must_sort = 1;
	/* check if we must re-size */
	if(symtab->nsym + 1 > symtab->sz)
	{
		symtab->sz = ((size_t)(((symtab->sz>>7)+1)*3))<<6;
		symtab->symbols = EGrealloc(symtab->symbols, sizeof(int)*symtab->sz);
		symtab->sort_perm = EGrealloc(symtab->sort_perm, sizeof(int)*symtab->sz);
	}
	if(symtab->all_sz < symtab->use_sz+len)
	{
		symtab->all_sz = ((size_t)(((symtab->all_sz>>12)+1)*3))<<11;
		symtab->all_sym = EGrealloc(symtab->all_sym,sizeof(char)*symtab->all_sz);
	}
	/* we just add it to the end of the table */
	strncpy(symtab->all_sym+symtab->use_sz,symbol,len);
	symtab->symbols[symtab->nsym++] = (int)(symtab->use_sz);
	symtab->use_sz += len;
	CLEANUP:
	return rval;
}
/* ========================================================================= */
/** @} */
