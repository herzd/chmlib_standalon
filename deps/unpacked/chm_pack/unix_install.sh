# get all dependecy tarballs
#wget https://github.com/jonls/qsopt-ex/archive/v2.5.10.3.tar.gz
wget http://zlib.net/zlib-1.2.8.tar.gz
wget https://sites.google.com/site/jayantapteshomepage/home/qsopt.tar.gz
wget https://sites.google.com/site/jayantapteshomepage/home/bzip2.tar.gz
wget http://www.dii.uchile.cl/~daespino/SOurce/EGlib.tar.bz2
wget https://gmplib.org/download/gmp/gmp-6.1.0.tar.xz
wget http://www.mpfr.org/mpfr-current/mpfr-3.1.3.tar.gz
wget http://www.flintlib.org/flint-2.5.2.tar.gz
tar -xf EGlib.tar.bz2
tar -xf gmp-6.1.0.tar.xz
tar -xf mpfr-3.1.3.tar.gz
tar -xf qsopt.tar.gz
tar -xf flint-2.5.2.tar.gz
tar -xf bzip2.tar.gz
tar -xf zlib-1.2.8.tar.gz
if ( test ! -d eglib_install ) ; then mkdir -p eglib_install ; fi
if ( test ! -d gmp_install ) ; then mkdir -p gmp_install ; fi
if ( test ! -d mpfr_install ) ; then mkdir -p mpfr_install ; fi
if ( test ! -d qsopt_install ) ; then mkdir -p qsopt_install ; fi
if ( test ! -d flint_install ) ; then mkdir -p flint_install ; fi
if ( test ! -d bzip2_install ) ; then mkdir -p bzip2_install ; fi
if ( test ! -d zlib_install ) ; then mkdir -p zlib_install ; fi
if ( test ! -d include ) ; then mkdir -p include ; fi
if ( test ! -d lib ) ; then mkdir -p lib ; fi
# gmp install
cd gmp-6.1.0
./configure --prefix=$PWD/../gmp_install
make -j12
make install
cd ../
# mpfr install
cd mpfr-3.1.3
./configure --prefix=$PWD/../mpfr_install --with-gmp=$PWD/../gmp_install
make -j12
make install
cd ../
#zlib install
cd zlib-1.2.8
./configure --prefix=$PWD/../zlib_install
make -j12
make install
cd ../
#bzip2 install
cd bzip2-1.0.6
make clean
make -f Makefile-libbz2_so
make
make install
if ( test ! -d $PWD/../bzip2_install/lib ) ; then mkdir -p  $PWD/../bzip2_install/lib ; fi
if ( test ! -d $PWD/../bzip2_install/include ) ; then mkdir -p  $PWD/../bzip2_install/include ; fi
cp libbz2.so.1.0.6 $PWD/../bzip2_install/lib/
cp *.h $PWD/../bzip2_install/include/
cd ../ 
cd qsopt-ex-2.5.10.3
mkdir build && cd build
../configure --prefix=$PWD/../../qsopt_install LDFLAGS="-L$PWD/../../gmp_install/lib -L$PWD/../../zlib_install/lib -L$PWD/../../bzip2_install/lib"
make -j12
make install
cd ../../
cd flint-2.5.2
./configure --prefix=$PWD/../flint_install --with-gmp=$PWD/../gmp_install --with-mpfr=$PWD/../mpfr_install
make -j12
make install
cd ../
cd EGlib-2.6.21
./configure --with-gmp=$PWD/../gmp_install
make 
make library
cd ../

cp -r EGlib-2.6.21/include eglib_install/
cp -r EGlib-2.6.21/lib eglib_install/
cp gmp_install/lib/* lib/
cp qsopt_install/lib/* lib/
cp flint_install/lib/* lib/
cp eglib_install/lib/* lib/
cp zlib_install/lib/* lib/
cp bzip2_install/lib/* lib/
cp mpfr_install/lib/* lib/
cp eglib_install/include/* include/
cp gmp_install/include/* include/
cp qsopt_install/include/qsopt_ex/* include/
cp flint_install/include/flint/* include/
cp zlib_install/include/* include/
cp bzip2_install/include/* include/
cp mpfr_install/include/* include/
export LD_LIBRARY_PATH=$PWD/lib:$LD_LIBRARY_PATH
C_INCLUDE_PATH=$PWD/include
cd chm_install
make all
