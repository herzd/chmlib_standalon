#include "eg_config.h"
#ifndef SVNVERSION
const char svn_EGlib[1024]="release";
#else
const char svn_EGlib[1024]=SVNVERSION;
#endif
const char string_EGlib[1024] = "2.6.21";
const char date_EGlib[1024] = __DATE__"-"__TIME__;
void EGlib_version(void) __attribute__ ((constructor));
static int EGlib_int = 0;
void EGlib_version(void){if(EGlib_int == 0)fprintf(stderr,"Using %s (SVN-version %s:%s, built %s)%s","EGlib",string_EGlib,svn_EGlib,date_EGlib,EG_NEWLINE);EGlib_int=1;}
