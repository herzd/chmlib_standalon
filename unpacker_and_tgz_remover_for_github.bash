# this is how the archived dependency source-codes were originally put into the github project herzd/chm_standalone for better editability

set -e
ORIG_BASE_DIR=$PWD

# unpack sources
printf "unpacking dependency sources...\n"
find $ORIG_BASE_DIR/deps/sources -type f \
-exec tar xvf {} --directory $PWD/deps/unpacked \; 
printf "unpacking done.\n"

# remove archived sources for leaner github project
printf "removing dependency sources-archives...\n"
find $ORIG_BASE_DIR/deps/sources -type f \
     -exec rm -v {} \;
printf "removed source-archives, code ready for github.\n"

