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
/** @mainpage EGlib Home Page
 *
 * @section Introduction
 
 <P>This is a joint project of <A HREF=http://www.dii.uchile.cl/~daespino TARGET=_top>Daniel Espinoza</A> and <A HREF=http://mgoycool.uai.cl TARGET=_top>Marcos Goycoolea</A> where a number of people have contributed, including <A HREF=http://www.isye.gatech.edu/~rgarcia TARGET=_top>Renan Garcia</A>, <A HREF=http://www.isye.gatech.edu/~ddogan TARGET=_top>Deniz Dogan</A>, <A HREF=http://www.isye.gatech.edu/~fenginee TARGET=_top>Faram Engineer</A> and <A HREF=http://www.isye.gatech.edu/~jvielma TARGET=_top>Juan Pablo Vielma</A>.</P>

<P>The problem that this project intend to solve (or at least help to solve) is
that in any computer implementation
there is a lot of overlap in the basic structures and utilities used, such as
graph structures, linked lists, hash tables, heaps among others, as well as a
number of algorithms wich are very useful as sub-routines for more complicated
programs. We have used this library in a number of applied and research projects and have been
in development since 2003. I am sure that it is not a perfect
solution, and that there might be a lot of obscure features in here, but we
think that it might be usefull  for other people, at least as a place where to
learn some of the tricks that are needed while trying to get efficient
algorithms. And how to use some tools like unix sockets and memory managment.
Most of the ideas in this code aren't new, and we have tried to give due credit
where needed, but if you feel that we should aknlowledge something, please send me an e-mail
and I will try to fix it as soon as posible.</P>

<P>Much of the ideas used here come from <A HREF=http://www.tsp.gatech.edu/concorde.html TARGET=_top>Concorde</A>, from the <A HREF=http://www.kernel.org/ TARGET=_top>Linux Kernel</A>, and from a number of books like <A HREF=http://www.amazon.com/exec/obidos/tg/detail/-/0898711878/ref=wl_it_dp/104-1187958-7592743?%5Fencoding=UTF8&coliid=I14K4ED1JB3KTX&v=glance&colid=38CHMLSKMPOQV TARGET=_top>Data Structures and Network Algorithms</A>, <A HREF=http://www.amazon.com/exec/obidos/tg/detail/-/013617549X/ref=wl_it_dp/104-1187958-7592743?%5Fencoding=UTF8&coliid=I3FWMVP8FZAA9F&v=glance&colid=38CHMLSKMPOQV TARGET=_top>Network Flows: Theory, Algorithms, and Applications</A> and <A HREF=http://www.amazon.com/exec/obidos/tg/detail/-/0262032937/qid=1117732253/sr=8-1/ref=pd_csp_1/104-1187958-7592743?v=glance&s=books&n=507846 TARGET=_top>Introduction to Algorithms</A> and <A HREF=http://www-cs-faculty.stanford.edu/~knuth/taocp.html TARGET=_top>The Art of Computer Programming</A> among others.</P>

<P>You can see the <A HREF=EGlib_doc/modules.html TARGET=info>documentation</A>  or download the <A HREF=http://www.dii.uchile.cl/~daespino/SOurce/EGlib.tar.bz2 TARGET=_top>full source code</A> for it.</P>
<P>We also have made available binaries for <A HREF=http://www.dii.uchile.cl/~daespino/SOurce/EGlib-32.tar.bz2 TARGET=_top>linux 32 bit</A> and for <A HREF=http://www.dii.uchile.cl/~daespino/SOurce/EGlib-64.tar.bz2 TARGET=_top>linux 64 bit</A>

<P>Almost all the code is <A HREF=http://www.dii.uchile.cl/~daespino/files/Iso_C_1999_definition.pdf TARGET=_top> ISO-C99 </A> compliant, with some <A HREF=http://www.gnu.org TARGET=_top>GNU</A> <A HREF=http://gcc.gnu.org/onlinedocs TARGET=_top>C-extensions</A>, and thus compile
with any recent version of <A HREF=http://gcc.gnu.org/ TARGET=_top>gcc</A>. (i.e. at least GCC-3.4.1). We haven't tested the code outside the
Linux/Unix realm, and for sure some modules (like timers) won't run on other
plataforms.</P>

 * @section A General Structures
 * 
 * This library provide some of the most common structures and some structures
 * have been implemented in more than one form (depending on the coding 
 * philosophy to be used), here is a lists:
 * - Bynary Trees (@ref EGeBTree).
 * - Directed Graphs (@ref EGeDgraph).
 * - Double Linked Lists (@ref EGeList).
 * - General-Length bitfields (@ref eg_bit.h).
 * - Hash Tables (@ref eg_ehash.h).
 * - Heaps (@ref EGeHeap).
 * - Sets (@ref EGeSet).
 * - Simple Shrinkable graphs (@ref EGsrkGraph).
 * - Simple imput Parser (@ref eg_io.h).
 * - Timers (@ref EGtimer).
 * - Undirected Graphs (@ref EGeUgraph).
 * - Dense Matrices (@ref EGdMatrix).
 *
 * @section B  Parallel Support
 * 
 * We also provide a basic network interface @ref EGnet (with a simple 
 * example) that we use on our parallel algorithms/implementations, it is 
 * simple enough to use, and also provide some details of how it works, We 
 * are not sure regarding portability of this library outside POSIX systems, it
 * has been tested on 32 and 64 bit UNIX and LINUX systems.... but in the worst
 * case it should provide a starting point to work with.
 *
 * @section  C Basic Algorithms
 *
 * Some common algorithms are implemented:
 * - Push-Relabel max-flow (@ref EGalgPushRelabel).
 * - Minimum Global Cut (@ref EGalgMinCut).
 * - Simple Gausian Elimination over Dense Matrices (@ref EGdMatrixGaussianElimination).
 * - LLL Basis Reduction (@ref EGdBsRed).
 *
 * @section  D Memory Managment
 *
 * For different reasons we have needed some type of memory managment, from
 * macros to simplify memory allocation/free procedures, to actual pool
 * mechanism. There is one general set of macros defined in @ref EGmem, and a Slab-Allocator pool
 * implementation that up to now is in testing phase @ref EGmemSlab.
 *
 * @section  E Simple Templates on C
 *
 * Beyond the fact that Templates are implemented on C++, for those that like
 * to stick with C, we developed a general way of programming templates for
 * numbers in such a way as to implement at once different versions of the same
 * code with different underlying number definitions, this arose as a necesity
 * while porting <A HREF=http://www.isye.gatech.edu/~wcook/qsopt TARGET=_top>QSopt</A> to
 * work on exact arithmetic, and also on multiple precision floating point
 * numbers (as provided by <A HREF=http://www.swox.com/gmp TARGET=_top>GNUMP library</A>)
 * so that we have only one code for Minimum Global Cuts, but we get
 * implementations for doubles, fixed point arithmetic, GMP's integers,
 * floating points, rationals, and so on. This is done through the Makefile,
 * utilities lake ctags and awk, but uses the general interface @ref EGlpNum 
 * as a way to work with numbers.
 * 
 * @section  F Final Comments
 *
 * This library hopes to be usefull in general both to teach some programming
 * tricks, structures and algorithms, and also by providing (we think) usable
 * code, in fact we allways test our code with <A HREF=http://valgrind.org/ TARGET=_top>valgrind</A> (one of the best memory
 * debugger available today in my opinion), and check for any
 * memory leaks and illegal access to memory.
 * 
 * */
