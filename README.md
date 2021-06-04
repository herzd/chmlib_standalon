# chmlib_standalone
development of unpriviledged-user chmlib standalone installers for various operating systems. 

NOTE: this repo is under active development right now. do not expect it to be functional until somehow announced.

# standalone (no-root) installer for chmlib

+ Installation currently developed for Debian-10 and CentOS-7 (blank netinstalls, see below).
+ Recreates the environment used in original research papers of AG Zanghellini.

NOTE: The Debian installer uses a newer version of flint (2.7.0 instead of 2.5.2 as in the original environment), as 2.5.2 fails to build with gcc 8.3.0 at the flags '-r -pie' at compilation time.  Furthermore the resulting binary causes memory access errors at random occurence at about 15% times of the invocations. Nevertheless the results calculated on both systems are the same.  For that reason we recommend to use CentOS-7 and the CentOS-7 installer, as we could not detect these errors with the binaries created by Centos-7's gcc-4.9.

# INSTALLING ON BARE CENTOS-7
This package has been designed for a fully-updated bare-metal Centos-7 net-installation pointing to http://mirror.centos.org/centos/7/os/x86_64. At install time we just chose compatibility libraries and development tools.

## OPTION 1 (~10MIN INSTALL TIME)
  + run 'bash install_no_depcheck_centos7.bash'. Once finished, you will have the files chm.bash and library_loader in your working directory.
  + if you want to time and  log the installation process use something like '(time bash install_no_depcheck_centos7.bash) 2>&1 | tee centos7_no_depcheck_install.log' to write all output to the file 'centos7_no_depcheck_install.log'. Chose a file name of your liking, but make sure to to manually remove it if you plan on compressing and handing out the package, as the uninstall.bash script does  not know about your file-naming.

## OPTION 2 (ALL AVAILABLE CHECKS RUN AT COMPILATION TIME OF THE SOURCES, ~60MIN INSTALL TIME)
  + run 'bash install_centos7.bash'. Once finished, you will have the files chm.bash and library_loader in your working directory.
  + if you want to time and  log the installation process use something like '(time bash install_centos7.bash) 2>&1 | tee centos7_full_depcheck_install.log' to write all output to the file 'centos7_full_depcheck_install.log'. Chose a file name of your liking, but make sure to to manually remove it if you plan on compressing and handing out the package, as the uninstall.bash script does  not know about your file-naming.

# INSTALLING ON BARE DEBIAN-10
This package has been designed for a fully-updated bare-metal Debian-10 net-installation. At install time only system-tools where selected. At post-install the packages 'build-essential', 'm4', 'emacs', and 'tree' where added.
  + NOTE:Using the debian10-install scripts you will chose flint-2.7.0 over flint-2.5.2, as the flint-Makefile produces gcc flags, that cause errors on Debian's gcc-8.3. The whole package compiles without errors and the created binary produces the same results as the respective Centos-7-version.  It is known though, that on random occurences of about 20% of invocations the binary produces memory access errors and fails.

## OPTION 1 (~20MIN INSTALL TIME)
  + run 'bash install_no_depcheck_debian10.bash'. Once finished, you will have the files chm.bash and library_loader in your working directory.
  + if you want to time and  log the installation process use something like '(time bash install_no_depcheck_debian10.bash) 2>&1 | tee debian10_no_depcheck_install.log' to write all output to the file 'debian10_no_depcheck_install.log'. Chose a file name of your liking, but make sure to to manually remove it if you plan on compressing and handing out the package, as the uninstall.bash script does  not know about your file-naming.

## OPTION 2 (ALL AVAILABLE CHECKS RUN AT COMPILATION TIME OF THE SOURCES, ~60MIN INSTALL TIME)
  + run 'bash install_debian10.bash'. Once finished, you will have the files chm.bash and library_loader in your working directory.
  + if you want to time and  log the installation process use something like '(time bash install_debian10.bash) 2>&1 | tee debian10_full_depcheck_install.log 2>&1' to write all output to the file 'debian10_full_depcheck_install.log'. Chose a file name of your liking, but make sure to to manually remove it if you plan on compressing and handing out the package, as the uninstall.bash script does  not know about your file-naming.


# UNINSTALLING
  + run 'bash uninstaller.bash' to remove unpacked sources, their installations, the 'chm.bash' wrapper, and the 'library_loader' file. The script will also remove the file 'solvelp.lp', as this is the standard output name, in case one ran eg 'bash chm.bash simpleNetr1r2r3.ine bounded 2' before.  However it will not automatically remove install-logfiles, that you possible created as described above.
