set -e
ORIG_BASE_DIR=$PWD

# create installation directories
for DESTDIR in eglib_install gmp_install mpfr_install \
qsopt_install flint_install bzip2_install zlib_install; \
do mkdir -p $ORIG_BASE_DIR/deps/installations/$DESTDIR; done
 
# gmp install
printf "entering gmp source directory.\n"
cd $(find $ORIG_BASE_DIR/deps/unpacked -mindepth 1 -maxdepth 1 -type d -name "gmp*")
printf "$PWD\n"
./configure --prefix=$ORIG_BASE_DIR/deps/installations/gmp_install
make clean
make
#make check
make install
printf "gmp compiled, checked and installed.\n"
cd $ORIG_BASE_DIR

# mpfr install
printf "entering mpfr source directory.\n"
cd $(find $ORIG_BASE_DIR/deps/unpacked -mindepth 1 -maxdepth 1 -type d -name "mpfr*")
printf "$PWD\n"
./configure --prefix=$ORIG_BASE_DIR/deps/installations/mpfr_install --with-gmp=$ORIG_BASE_DIR/deps/installations/gmp_install
make clean
make
#make check
make install
printf "mpfr compiled, checked and installed.\n"
cd $ORIG_BASE_DIR

# zlib install
printf "entering zlib source directory.\n"
cd $(find $ORIG_BASE_DIR/deps/unpacked -mindepth 1 -maxdepth 1 -type d -name "zlib*")
./configure --prefix=$ORIG_BASE_DIR/deps/installations/zlib_install
make clean
make
#make check
make install
printf "zlib compiled, checked and installed.\n"
cd $ORIG_BASE_DIR

# bzip2 install
printf "entering bzip2 source directory.\n"
cd $(find $ORIG_BASE_DIR/deps/unpacked -mindepth 1 -maxdepth 1 -type d -name "bzip*")
make clean
make -f Makefile-libbz2_so
make
#make check
make install PREFIX=$ORIG_BASE_DIR/deps/installations/bzip2_install
cp libbz2.so.1.0.6 $ORIG_BASE_DIR/deps/installations/bzip2_install/lib/
cp bzlib_private.h $ORIG_BASE_DIR/deps/installations/bzip2_install/include/
printf "bzip2 compiled, checked and installed.\n"
cd $ORIG_BASE_DIR

# qsopt-ex install
printf "entering qsopt-ex source directory.\n"
cd $(find $ORIG_BASE_DIR/deps/unpacked -mindepth 1 -maxdepth 1 -type d -name "qsopt*")
# create build directory, delete potentially old one
if [ ! -d build ]
then
    mkdir build && cd build
else
    rm -rf build && mkdir build && cd build
fi
# bring libraries and headers in place for compile time
export LD_LIBRARY_PATH=$ORIG_BASE_DIR/deps/installations/gmp_install/lib:$ORIG_BASE_DIR/deps/installations/zlib_install/lib:$ORIG_BASE_DIR/deps/installations/bzip2_install/lib:
export LIBRARY_PATH=$ORIG_BASE_DIR/deps/installations/gmp_install/lib:$ORIG_BASE_DIR/deps/installations/zlib_install/lib:$ORIG_BASE_DIR/deps/installations/bzip2_install/lib:
export C_INCLUDE_PATH=$ORIG_BASE_DIR/deps/installations/bzip2_install/include:$ORIG_BASE_DIR/deps/installations/zlib_install/include:$ORIG_BASE_DIR/deps/installations/gmp_install/include:
# configure and compile
../configure --prefix=$ORIG_BASE_DIR/deps/installations/qsopt_install/ \
LDFLAGS="-L$ORIG_BASE_DIR/deps/installations/gmp_install/lib -L$ORIG_BASE_DIR/deps/installations/zlib_install/lib -L$ORIG_BASE_DIR/deps/installations/bzip2_install/lib" \
CPPFLAGS="-I$ORIG_BASE_DIR/deps/installations/bzip2_install/include -I$ORIG_BASE_DIR/deps/installations/zlib_install/include -I$ORIG_BASE_DIR/deps/installations/gmp_install/include"
make clean
#make check
make install
printf "qsopt-ex compiled, checked and installed.\n"
cd $ORIG_BASE_DIR

# flint installation
printf "entering flint source directory.\n"
# hardcoding, as there is flint-2.5.2 for centos-7 in this repo as well
cd $(find $ORIG_BASE_DIR/deps/unpacked -mindepth 1 -maxdepth 1 -type d -name "flint-2.7.0")
./configure --prefix=$ORIG_BASE_DIR/deps/installations/flint_install --with-gmp=$ORIG_BASE_DIR/deps/installations/gmp_install --with-mpfr=$ORIG_BASE_DIR/deps/installations/mpfr_install
make
#make check
make install
printf "flint compiled, checked and installed.\n"
cd $ORIG_BASE_DIR

# eglib install
printf "entering eglib source directory.\n"
cd $(find $ORIG_BASE_DIR/deps/unpacked -mindepth 1 -maxdepth 1 -type d -name "EGlib*")
./configure --prefix=$ORIG_BASE_DIR/deps/installations/eglib_install LDFLAGS="-L/$ORIG_BASE_DIR/deps/installations/gmp_install/lib" --with-gmp=$ORIG_BASE_DIR/deps/installations/gmp_install --with-gmp-lib-dir=$ORIG_BASE_DIR/deps/installations/gmp_install/lib --with-gmp-include-dir=$ORIG_BASE_DIR/deps/installations/gmp_install/include
make clean
make SVNVERSION=""
cp -r include $ORIG_BASE_DIR/deps/installations/eglib_install/
cp -r lib $ORIG_BASE_DIR/deps/installations/eglib_install/
printf "eglib compiled and installed.\n"
cd $ORIG_BASE_DIR

# get libraries and headers ready for compilation time
export LD_LIBRARY_PATH=$ORIG_BASE_DIR/deps/installations/gmp_install/lib:$ORIG_BASE_DIR/deps/installations/qsopt_install/lib:$ORIG_BASE_DIR/deps/installations/flint_install/lib:$ORIG_BASE_DIR/deps/installations/eglib_install/lib:$ORIG_BASE_DIR/deps/installations/zlib_install/lib:$ORIG_BASE_DIR/deps/installations/bzip2_install/lib:$ORIG_BASE_DIR/deps/installations/mpfr_install/lib:
export LIBRARY_PATH=$ORIG_BASE_DIR/deps/installations/gmp_install/lib:$ORIG_BASE_DIR/deps/installations/qsopt_install/lib:$ORIG_BASE_DIR/deps/installations/flint_install/lib:$ORIG_BASE_DIR/deps/installations/eglib_install/lib:$ORIG_BASE_DIR/deps/installations/zlib_install/lib:$ORIG_BASE_DIR/deps/installations/bzip2_install/lib:$ORIG_BASE_DIR/deps/installations/mpfr_install/lib:
export C_INCLUDE_PATH=$ORIG_BASE_DIR/deps/installations/gmp_install/include:$ORIG_BASE_DIR/deps/installations/gmp_install/include:$ORIG_BASE_DIR/deps/installations/qsopt_install/include/qsopt_ex:$ORIG_BASE_DIR/deps/installations/flint_install/include/flint:$ORIG_BASE_DIR/deps/installations/zlib_install/include:$ORIG_BASE_DIR/deps/installations/bzip2_install/include:$ORIG_BASE_DIR/deps/installations/mpfr_install/include:
# place chm source into final destination
cp -r $ORIG_BASE_DIR/deps/unpacked/chm_pack/chm_install $ORIG_BASE_DIR/deps/installations/
printf "entering chm source directory.\n"
cd $(find $ORIG_BASE_DIR/deps/installations -mindepth 1 -maxdepth 1 -type d -name "chm*")
gcc -o chm chmqsXcore.c core.c chmqs.c chmflint.c chmstructs.c setops.c -lm -lflint -lqsopt_ex -lgmp -lEGlib -lz -lbz2
printf "chm compiled and ready for use.\n"
printf "creating library-loadfile and wrapper for better usability.\n"
cat > $ORIG_BASE_DIR/library_loader << 'EOF'
ORIG_BASE_DIR=$PWD
export LD_LIBRARY_PATH=$ORIG_BASE_DIR/deps/installations/gmp_install/lib:$ORIG_BASE_DIR/deps/installations/qsopt_install/lib:$ORIG_BASE_DIR/deps/installations/flint_install/lib:$ORIG_BASE_DIR/deps/installations/eglib_install/lib:$ORIG_BASE_DIR/deps/installations/zlib_install/lib:$ORIG_BASE_DIR/deps/installations/bzip2_install/lib:$ORIG_BASE_DIR/deps/installations/mpfr_install/lib:
export LIBRARY_PATH=$ORIG_BASE_DIR/deps/installations/gmp_install/lib:$ORIG_BASE_DIR/deps/installations/qsopt_install/lib:$ORIG_BASE_DIR/deps/installations/flint_install/lib:$ORIG_BASE_DIR/deps/installations/eglib_install/lib:$ORIG_BASE_DIR/deps/installations/zlib_install/lib:$ORIG_BASE_DIR/deps/installations/bzip2_install/lib:$ORIG_BASE_DIR/deps/installations/mpfr_install/lib:
export C_INCLUDE_PATH=$ORIG_BASE_DIR/deps/installations/gmp_install/include:$ORIG_BASE_DIR/deps/installations/gmp_install/include:$ORIG_BASE_DIR/deps/installations/qsopt_install/include/qsopt_ex:$ORIG_BASE_DIR/deps/installations/flint_install/include/flint:$ORIG_BASE_DIR/deps/installations/zlib_install/include:$ORIG_BASE_DIR/deps/installations/bzip2_install/include:$ORIG_BASE_DIR/deps/installations/mpfr_install/include:
EOF
cat > $ORIG_BASE_DIR/chm.bash << 'EOF'
source library_loader
$ORIG_BASE_DIR/deps/installations/chm_install/chm "$@"
EOF

printf "preparation done. run 'bash chm.bash FILE.ine un/bounded NDIM'\n"
