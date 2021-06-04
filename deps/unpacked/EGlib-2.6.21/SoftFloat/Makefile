# =============================================================================
# This is the Makefile of EGlib
# Revision 2003-4-14
# - 2005-08-17
#						- Update EGlib.so rule
# - 2005-08-01 
#						- Update to generate templates, and compute dependencies on the 
#							fly
# - 2003-11-18 
#						- Update to automatic dependency generation
# =============================================================================

# =============================================================================
# Here we read some configurations, you should edit this file instead 
# of this makefile
include make.conf

# =============================================================================
# Basic programs used, if no in the PATH variable, put the full path to the
# utilites
# =============================================================================
# Compiler 
CC := gcc
#linker
AR := ar
# ranlib utility
RANLIB := ranlib
GREP := grep
CTAGS := ctags
AWK := awk
CUT := cut
UNIQ := uniq

# =============================================================================
# Path for each kind of file
# =============================================================================
SOURCE_DIR := .
INCLUDE_DIR := .
OBJ_DIR := obj
DEP_DIR := dep
LIB_DIR := lib
ALT_DIR := TEmplate
SOURCE_DIR += $(ALT_DIR)
INCLUDE_DIR += $(ALT_DIR)


# =============================================================================
# Here we define the source files (not need to put the prefix) that will
# generate objective files. DO NOT INCLUDE files that don't generate objective
# files here, they will be automagically computed
# =============================================================================
# Main files without template
MAIN_SFILE := eg_bbtree.ex.c eg_dgraph.ex.c eg_edgraph.ex.c eg_elist.ex.c \
	eg_eugraph.ex.c eg_heap.ex.c eg_net.ex.c eg_ugraph.ex.c eg_ebtree.ex.c
# Template main files
LPNUM_MAIN_SFILE := eg_lpnum.ex.c eg_min_cut.ex.c eg_push_relabel.ex.c \
	eg_shrink_graph.ex.c eg_eheap.ex.c eg_memslab.ex.c eg_dmatrix.ex.c
# regular soource files
SOURCE_FILE := eg_aequiset.c eg_bbtree.c eg_bellford.c eg_bit.c eg_col.c \
	eg_compare.c eg_dagsp.c eg_dgraph.c eg_dijkstra_app.c eg_dijkstra.c \
	eg_equiset.c eg_hash.c eg_heap.c eg_io.c eg_list.c eg_lpnum.c \
	eg_mempool.c eg_net.c eg_row.c eg_spanning.c eg_ugraph.c
# Template source files
EGLPNUM_SFILE := eg_push_relabel.c eg_min_cut.c eg_eheap.c eg_shrink_graph.c \
	eg_dmatrix.c eg_numutil.c eg_dbasis_red.c
ifdef HAVE_CPLEX
ifneq ($(HAVE_CPLEX),0)
SOURCE_FILE +=	eg_cplex.c 
endif
endif
# =============================================================================
# Include files without a .c file
INC_FILE := eg_config.h eg_cplexTypes.h eg_edgraph.h eg_elink.h eg_elist.h \
	eg_eset.h eg_eugraph.h eg_fp.h eg_listint.h eg_macros.h eg_timer.h \
	eg_ebtree.h eg_mem.h eg_memslab.h eg_lpnum.mpz.h eg_lpnum.mpq.h \
	eg_lpnum.mpf.h eg_lpnum.dbl.h eg_lpnum.ldbl.h eg_lpnum.int.h \
	eg_lpnum.llint.h 

# =============================================================================
# This is a list of tags whose name shoud be changed while compiling
# different versions of template files
# =============================================================================
DO_CHANGE := epsLpNum EGlpNumGetStr EGlpNumReadStr EGlpNumCeil EGlpNumFloor \
	EGlpNumInv zeroLpNum oneLpNum EGlpNumSet EGlpNumIsEqual EGlpNumIsEqqual \
	EGlpNumIsNeq EGlpNumIsNeqq EGlpNumIsNeqZero EGlpNumIsNeqqZero EGlpNumIsLess \
	EGlpNumIsSumLess EGlpNumIsDiffLess EGlpNumIsLessDbl EGlpNnumIsGreaDbl \
	EGlpNumIsLeq EGlpNumCopyDiffRatio EGlpNumCopyDiff EGlpNumCopySum EGlpNumCopy \
	EGlpNumSetToMaxAbs EGlpNumSetToMinAbs EGlpNumCopySqrOver EGlpNumCopyAbs \
	EGlpNumCopyNeg EGlpNumCopyFrac EGlpNumCopyArray EGlpNumSubInnProd \
	EGlpNumAddInnProd EGlpNumSubUiTo EGlpNumAddUiTo EGlpNumAddTo EGlpNumSubTo \
	EGlpNumMultTo EGlpNumDivTo EGlpNumDivUiTo EGlpNumMultUiTo EGlpNumZero \
	EGlpNumOne EGlpNumSign EGlpNumToLf EGlpNumAllocArray EGlpNumInitVar \
	EGlpNumInitVar EGlpNumClearVar EGlpNumReallocArray EGlpNumFreeArray

# =============================================================================
# This is a list of functions whose name shoudn't be changed while compiling
# different versions of templates
# =============================================================================
NO_CHANGE := main sscanf sprintf fprintf fprint ungetc perror parseargs \
	memset listen fscanf fflush fclose TRACE connect bind accept \
	time printf getrusage 

# =============================================================================
# You shoudn't edit bellow this point. Other configuration is found in
# 'make.conf', see that file for more details.
# =============================================================================

# =============================================================================
# This is a trickier make file, because we need to create three .o files
# for each .c file in qsopt, one for doubles, other for mpq and other for mpf,
# what we do is to create three .c files from the original on the fly and change
# all names of functions inside to qsopt to dbl_ mpf_ mpq_ respectivale, as well
# as all structure definitions, we do this using ctags and awk
# =============================================================================
FP20_SFILE := $(addprefix fp20_,$(EGLPNUM_SFILE))
DBL_SFILE := $(addprefix dbl_,$(EGLPNUM_SFILE))
LDBL_SFILE := $(addprefix ldbl_,$(EGLPNUM_SFILE))
MPF_SFILE := $(addprefix mpf_,$(EGLPNUM_SFILE))
MPQ_SFILE := $(addprefix mpq_,$(EGLPNUM_SFILE))
FP20_MAIN_SFILE := $(addprefix fp20_,$(LPNUM_MAIN_SFILE))
DBL_MAIN_SFILE := $(addprefix dbl_,$(LPNUM_MAIN_SFILE))
LDBL_MAIN_SFILE := $(addprefix ldbl_,$(LPNUM_MAIN_SFILE))
MPF_MAIN_SFILE := $(addprefix mpf_,$(LPNUM_MAIN_SFILE))
MPQ_MAIN_SFILE := $(addprefix mpq_,$(LPNUM_MAIN_SFILE))
ALL_MAIN_SFILE := $(MAIN_SFILE) $(DBL_MAIN_SFILE) $(LDBL_MAIN_SFILE) \
	$(MPF_MAIN_SFILE) $(MPQ_MAIN_SFILE) $(FP20_MAIN_SFILE)
ALL_SFILE := $(SOURCE_FILE) $(DBL_SFILE) $(LDBL_SFILE) $(MPF_SFILE) \
	$(MPQ_SFILE) $(FP20_SFILE)
INC_FILE += $(ALL_SFILE:.c=.h)
MAIN_PROG := $(notdir $(ALL_MAIN_SFILE:.ex.c=))

# objective files =============================================================
OBJ_FILE := $(addprefix $(OBJ_DIR)/,$(ALL_SFILE:.c=.o))
MAIN_OFILE := $(addprefix $(OBJ_DIR)/,$(ALL_MAIN_SFILE:.c=.o))

# dependency files ====================================================
DEP_FILE := $(addsuffix .d,$(addprefix $(DEP_DIR)/,$(ALL_SFILE) \
	$(ALL_MAIN_SFILE) $(INC_FILE))) 

#==============================================================================
# Default search paths for each type of file
#==============================================================================
vpath %.c $(SOURCE_DIR)
vpath %.h $(INCLUDE_DIR) $(CPLEXINCLUDE)
vpath %.o $(OBJ_DIR)
vpath %.d $(DEP_DIR)
vpath %.da $(OBJ_DIR)
vpath %.so $(LIB_DIR) $(LIB_PATH)

# =============================================================================
# definitions of OS types
LINUX := 0
SUN := 1

# =============================================================================
#architecture dependent configuration
ARCH = $(shell arch)
HOST = $(shell uname)
ifeq ($(ARCH),x86_64)
ARCH := k8
endif
ifeq ($(ARCH),sun4)
ARCH := v9
endif
ifeq ($(HOST),Linux)
OS := $(LINUX)
endif
ifeq ($(HOST),SunOS)
OS := $(SUN)
endif

# =============================================================================
# main configuration options, the definition of _XOPEN_SOURCE is needed to use
# posix_memalign
MAINOPT += -DDEBUG=$(DEBUG) -DOS=$(OS) -D_XOPEN_SOURCE=600
ifdef HAVE_CPLEX
ifneq ($(HAVE_CPLEX),0)
MAINOPT += -DHAVE_CPLEX
endif
endif

# =============================================================================
# Optimization Flags 
SW := -std=c99 -g -Wall -Winline -Wshadow -Wwrite-strings -Wpointer-arith \
      -Wundef -Wcast-qual -Wbad-function-cast -Wnested-externs \
      -Wstrict-prototypes -Wunused-function -Wunused-label -Wunused-variable \
      -Wunused-value -Wunused-parameter -Wfloat-equal -Wconversion \
			-Wsign-compare -Wwrite-strings
ifdef OPTIMIZE
ifneq ($(OPTIMIZE),0)
SW += -O3 -fforce-addr -funroll-loops -frerun-cse-after-loop \
      -finline-limit=1000000 -frerun-loop-opt -fno-trapping-math \
			-funsafe-math-optimizations -ffast-math -fno-math-errno \
			-march=$(ARCH) -Wuninitialized 
else
SW += -fno-inline
ifdef PROFILE
ifeq ($(PROFILE),0)
SW += -fomit-frame-pointer 
endif
endif
endif
endif
ifdef PROFILE
ifneq ($(PROFILE),0)
SW += -pg -ftest-coverage -fprofile-arcs
ifdef BRANCHPF
ifneq ($(BRANCHPF),0)
SW += -fbranch-probabilities -fprofile-arcs
endif
endif
endif
endif
ifeq ($(HOST),SunOS)
SW += -m64
endif

# =============================================================================
# Libraries flags 
LIB_FLAGS := -lm -lpthread -lresolv -lnsl
ifeq ($(HOST),SunOS)
LIB_FLAGS += -lsocket
endif
ifdef HAVE_CPLEX
ifneq ($(HAVE_CPLEX),0)
LIB_FLAGS += -lcplex
LIB_PATH += $(CPLEXLIBRARY)
endif
endif
ifdef HAVE_GNU_MP
ifneq ($(HAVE_GNU_MP),0)
LIB_FLAGS += -lgmp 
LIB_PATH += $(GNUMPPATH)/lib
endif
endif
#==============================================================================
# Path rules for CC
#==============================================================================
CCFLAGS += $(patsubst %,-I%,$(subst :, ,$(INCLUDE_DIR) $(CPLEXINCLUDE) $(GNUMPPATH)/include ))
CCFLAGS += $(patsubst %,-L%,$(subst :, ,$(LIB_DIR) $(LIB_PATH)))

#==============================================================================
# Default targets to do
#==============================================================================
.PHONY: indent doc clean selftest default 
DEFAULT := selftest tags $(MAIN_PROG) 
ifeq ($(HOST),Linux)
DEFAULT += $(LIB_DIR)/EGlib.so 
endif
default: $(DEFAULT) 
make.conf: ;
Makefile : ;

#==============================================================================
# Some extra dependencies
#==============================================================================
$(OBJ_FILE): make.conf
$(MAIN_OFILE): make.conf

#==============================================================================
# Default rules for each type of file
#==============================================================================
# rule to make special template files
$(addprefix $(ALT_DIR)/,$(filter fp20_%,$(ALL_SFILE) $(INC_FILE) $(ALL_MAIN_SFILE))): $(ALT_DIR)/fp20_% : % tag.fp20
	@echo Building $@
	@$(AWK) -f tag.fp20 $< > $@

$(addprefix $(ALT_DIR)/,$(filter ldbl_%,$(ALL_SFILE) $(INC_FILE) $(ALL_MAIN_SFILE))): $(ALT_DIR)/ldbl_% : % tag.ldbl
$(addprefix $(ALT_DIR)/,$(filter dbl_%,$(ALL_SFILE) $(INC_FILE) $(ALL_MAIN_SFILE))): $(ALT_DIR)/dbl_% : % tag.dbl
	@echo Building $@
	@$(AWK) -f tag.dbl $< > $@

$(addprefix $(ALT_DIR)/,$(filter ldbl_%,$(ALL_SFILE) $(INC_FILE) $(ALL_MAIN_SFILE))): $(ALT_DIR)/ldbl_% : % tag.ldbl
	@echo Building $@
	@$(AWK) -f tag.ldbl $< > $@

$(addprefix $(ALT_DIR)/,$(filter mpf_%,$(ALL_SFILE) $(INC_FILE) $(ALL_MAIN_SFILE))): $(ALT_DIR)/mpf_% : % tag.mpf
	@echo Building $@
	@$(AWK) -f tag.mpf $< > $@

$(addprefix $(ALT_DIR)/,$(filter mpq_%,$(ALL_SFILE) $(INC_FILE) $(ALL_MAIN_SFILE))): $(ALT_DIR)/mpq_% : % tag.mpq
	@echo Building $@
	@$(AWK) -f tag.mpq $< > $@

# rule to make .o files
$(filter $(OBJ_DIR)/eg_%,$(OBJ_FILE) $(MAIN_OFILE)): $(OBJ_DIR)/%.o : %.c
	@echo Compiling $<
	@$(CC) $(SW) $(CCFLAGS) $(MAINOPT) -c $< -o $@ 

$(filter $(OBJ_DIR)/fp20_%,$(OBJ_FILE) $(MAIN_OFILE)): $(OBJ_DIR)/%.o : %.c
	@echo Compiling $<
	@$(CC) $(SW) $(CCFLAGS) $(MAINOPT) -DEGLPNUM_TYPE=4 -c $< -o $@ 

$(filter $(OBJ_DIR)/dbl_%,$(OBJ_FILE) $(MAIN_OFILE)): $(OBJ_DIR)/%.o : %.c
	@echo Compiling $<
	@$(CC) $(SW) $(CCFLAGS) $(MAINOPT) -DEGLPNUM_TYPE=0 -c $< -o $@ 

$(filter $(OBJ_DIR)/ldbl_%,$(OBJ_FILE) $(MAIN_OFILE)): $(OBJ_DIR)/%.o : %.c
	@echo Compiling $<
	@$(CC) $(SW) $(CCFLAGS) $(MAINOPT) -DEGLPNUM_TYPE=11 -c $< -o $@ 

$(filter $(OBJ_DIR)/mpq_%,$(OBJ_FILE) $(MAIN_OFILE)): $(OBJ_DIR)/%.o : %.c
	@echo Compiling $<
	@$(CC) $(SW) $(CCFLAGS) $(MAINOPT) -DEGLPNUM_TYPE=9 -c $< -o $@ 

$(filter $(OBJ_DIR)/mpf_%,$(OBJ_FILE) $(MAIN_OFILE)): $(OBJ_DIR)/%.o : %.c
	@echo Compiling $<
	@$(CC) $(SW) $(CCFLAGS) $(MAINOPT) -DEGLPNUM_TYPE=10 -c $< -o $@ 

# rule to make .d files
$(DEP_FILE): $(DEP_DIR)/%.d : % 
	@echo Making Dependencies for $< 
	@$(CC) -MG -MM -MF $@ -MT $@ -MT $(OBJ_DIR)/$(patsubst %.c.d,%.o,$(notdir $@)) \
	-MT tags $(SW) $(CCFLAGS) $(MAINOPT) $< 

tags: $(EGLPNUM_SFILE) $(EGLPNUM_SFILE:.c=.h) $(LPNUM_MAIN_SFILE)
	@echo Building vi $@
	@$(CTAGS) $^

tag: $(TEMPLPATE_MAIN_SFILE) $(EGLPNUM_SFILE) $(EGLPNUM_SFILE:.c=.h)
	@echo Making $@
	@$(CTAGS) -x --c-kinds=+xp-m --c++-kinds=+xp-m $^ | $(CUT) -d \  -f 1 | \
	$(UNIQ) > $@2; for j in $(NO_CHANGE); do $(GREP) -v -w $$j $@2 > $@3; \
	mv $@3 $@2; done; if [ ! -f $@ ]; then mv $@2 $@; else \
	val=`diff tag2 tag|wc -l`; if [ $$val -ne 0 ]; then mv $@2 $@; else rm $@2; \
	fi; fi

tag.fp20: tag
	@echo Making Awk preprocessor $@
	@rm -f $@; for i in `cat $<`; do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"fp20_$$i\")};" >> $@; done;\
	for i in $(DO_CHANGE); do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"fp20_$$i\")};" >> $@; done; \
	for i in $(LPNUM_MAIN_SFILE); do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"fp20_$$i\")};" >> $@; done; \
	for i in $(EGLPNUM_SFILE); do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"fp20_$$i\")};" >> $@; done; \
	for i in $(EGLPNUM_SFILE:.c=.h); do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"fp20_$$i\")};" >> $@; done; \
	echo "/\<EGlpNum_t\>/ {gsub(/\<EGlpNum_t\>/,\"EGfp20_t\")};">> $@; \
	echo "{print};" >> $@

tag.mpf: tag
	@echo Making Awk preprocessor $@
	@rm -f $@; for i in `cat $<`; do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"mpf_$$i\")};" >> $@; done;\
	for i in $(DO_CHANGE); do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"mpf_$$i\")};" >> $@; done; \
	for i in $(LPNUM_MAIN_SFILE); do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"mpf_$$i\")};" >> $@; done; \
	for i in $(EGLPNUM_SFILE); do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"mpf_$$i\")};" >> $@; done; \
	for i in $(EGLPNUM_SFILE:.c=.h); do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"mpf_$$i\")};" >> $@; done; \
	echo "/\<EGlpNum_t\>/ {gsub(/\<EGlpNum_t\>/,\"mpf_t\")};">> $@; \
	echo "{print};" >> $@

tag.mpq: tag
	@echo Making Awk preprocessor $@
	@rm -f $@; for i in `cat $<`; do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"mpq_$$i\")};" >> $@; done;\
	for i in $(DO_CHANGE); do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"mpq_$$i\")};" >> $@; done; \
	for i in $(LPNUM_MAIN_SFILE); do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"mpq_$$i\")};" >> $@; done; \
	for i in $(EGLPNUM_SFILE); do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"mpq_$$i\")};" >> $@; done; \
	for i in $(EGLPNUM_SFILE:.c=.h); do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"mpq_$$i\")};" >> $@; done; \
	echo "/\<EGlpNum_t\>/ {gsub(/\<EGlpNum_t\>/,\"mpq_t\")};">> $@; \
	echo "{print};" >> $@

tag.dbl: tag
	@echo Making Awk preprocessor $@
	@rm -f $@; for i in `cat $<`; do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"dbl_$$i\")};" >> $@; done;\
	for i in $(DO_CHANGE); do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"dbl_$$i\")};" >> $@; done; \
	for i in $(LPNUM_MAIN_SFILE); do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"dbl_$$i\")};" >> $@; done; \
	for i in $(EGLPNUM_SFILE); do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"dbl_$$i\")};" >> $@; done; \
	for i in $(EGLPNUM_SFILE:.c=.h); do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"dbl_$$i\")};" >> $@; done; \
	echo "/\<EGlpNum_t\>/ {gsub(/\<EGlpNum_t\>/,\"double\")};">> $@; \
	echo "{print};" >> $@

tag.ldbl: tag
	@echo Making Awk preprocessor $@
	@rm -f $@; for i in `cat $<`; do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"ldbl_$$i\")};" >> $@; done;\
	for i in $(DO_CHANGE); do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"ldbl_$$i\")};" >> $@; done; \
	for i in $(LPNUM_MAIN_SFILE); do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"ldbl_$$i\")};" >> $@; done; \
	for i in $(EGLPNUM_SFILE); do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"ldbl_$$i\")};" >> $@; done; \
	for i in $(EGLPNUM_SFILE:.c=.h); do \
	echo "/\<$$i\>/ {gsub(/\<$$i\>/,\"ldbl_$$i\")};" >> $@; done; \
	echo "/\<EGlpNum_t\>/ {gsub(/\<EGlpNum_t\>/,\"long double\")};">> $@; \
	echo "{print};" >> $@

# check some stuff in here
selftest: 
ifndef OS
	@echo "Unknown OS system, this may cause errors at compile time"
endif
	@if [ ! -d $(LIB_DIR) ]; then mkdir -p $(LIB_DIR); fi
	@if [ ! -d $(OBJ_DIR) ]; then mkdir -p $(OBJ_DIR); fi
	@if [ ! -d $(DEP_DIR) ]; then mkdir -p $(DEP_DIR); fi
	@if [ ! -d $(ALT_DIR) ]; then mkdir -p $(ALT_DIR); fi

# rule to force an action
FORCE:
	

#==============================================================================
# include files for dependencies
#==============================================================================
include $(DEP_FILE)

#==============================================================================
# Targets to be done
#==============================================================================

doc: $(INCLUDE_FILE) $(SOURCE_FILE) $(DEP_FILE) Doxyfile
	@echo "Generating Documentation..." && doxygen

$(LIB_DIR)/EGlib.so: $(OBJ_FILE)
	@echo Linking $@
	@rm -f $@
	@$(CC) $(SW) -o $@ -Wl,-z,initfirst -shared $(OBJ_FILE) $(CCFLAGS) $(LIB_FLAGS)

$(LIB_DIR)/EGlib.a: $(OBJ_FILE)
	@echo Linking $@
	@rm -f $@
	@$(AR) cq $@ $(OBJ_FILE)
	@if ( test -f $(RANLIB) -o -f /usr/bin/ranlib -o \
		-f /bin/ranlib -o -f /usr/ccs/bin/ranlib ) ; then \
		echo $(RANLIB) $@ ; \
		$(RANLIB) $@ ; \
	fi

clean:
	-rm -f *.lp *.log *.a *.so test_* doc/*/* gmon.* tags tag tag.*
	-rm -f $(OBJ_DIR)/* $(LIB_DIR)/* $(ALT_DIR)/* $(DEP_DIR)/* $(DEFAULT)

IDENTFLAGS := -bli0 -i2 -l79 -lc79 -lps -sc -cdw -bfda -psl -nbc -nbc -npsl \
							-ut -ts2 -bc

indent:
	-@for j in $(SOURCE_DIR); do for i in $$j/*.c; do indent $$i -o _ind_ $(IDENTFLAGS) && mv _ind_ $$i && echo "Indenting $$i"; done ; done
	-@for j in $(INCLUDE_DIR); do for i in $$j/*.h; do indent $$i -o _ind_ $(IDENTFLAGS) && mv _ind_ $$i && echo "Indenting $$i"; done; done

$(MAIN_PROG): % : $(OBJ_DIR)/%.ex.o $(LIB_DIR)/EGlib.a
	@echo Compiling $@
	@$(CC) $(SW) $(INCLUDEDIR) $(MAINOPT) $< $(LIB_DIR)/EGlib.a $(CCFLAGS) $(LIB_FLAGS) -o $@ 


# =============================================================================
# end of Makefile
# =============================================================================
