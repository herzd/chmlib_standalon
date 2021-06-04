libpath=$(PWD)/../lib
includepath=$(PWD)/../include

all: chmqsXcore.c core.c setops.c 
	gcc -I$(includepath)  -o chm chmqsXcore.c core.c chmqs.c chmflint.c chmstructs.c setops.c -lm -lflint -L$(libpath) -lqsopt_ex $(libpath)/EGlib.a -lgmp -lz -lbz2 -lm
	
permgroups: permgroups.c
	gcc -I./ -o pgps permgroups.c

hset: hset.c 
	gcc -I./ -o hset hset.c	 

lpsolve: lpsolve.c core.c setops.c chmqs.c
	gcc -I./ -o lpsolve lpsolve.c core.c chmqs.c chmflint.c chmstructs.c setops.c  -lm /usr/local/lib/libflint.a -lQSopt_ex $(egpath) -lEGlib -lgmp -lz -lbz2 -g 
