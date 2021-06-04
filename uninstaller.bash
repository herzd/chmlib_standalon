ORIG_BASE_DIR=$PWD

# remove possibly unpacked sources
printf "removing previously unpacked sources...\n"
find $ORIG_BASE_DIR/deps/unpacked -mindepth 1 -maxdepth 1 -type d \
-exec rm -rf {} \;
printf "done.\n"

# remove possibly created install directories
for DESTDIR in eglib_install gmp_install mpfr_install \
qsopt_install flint_install bzip2_install zlib_install chm_install; \
do rm -rf $ORIG_BASE_DIR/deps/installations/$DESTDIR; done

# remove library_loader, chm wrapper and possible result file
rm chm.bash library_loader solvelp.lp
