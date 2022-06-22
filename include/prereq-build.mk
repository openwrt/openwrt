# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2006-2020 OpenWrt.org

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/prereq.mk

SHELL:=sh
PKG_NAME:=Build dependency

MAC_HOST_PATHS:=/opt/local/bin /usr/local/opt

# an easy way to check for GNU coreutils
$(eval $(call FindHostCommand,*[,Please install GNU coreutils, \
	g[ --version | grep GNU, \
	[ --version | grep GNU))

$(eval $(call FindHostCommand,*true,Please install GNU coreutils, \
	gtrue --version | grep GNU, \
	true --version | grep GNU))

$(eval $(call FindHostCommand,*false,Please install GNU coreutils, \
	gfalse --version | grep GNU, \
	false --version | grep GNU))

# Required for the toolchain
$(eval $(call SetupHostCommand,make, \
	Please install GNU make v4.1 or later., \
	$(MAKE) -v | grep -E 'Make (4\.[1-9]|[5-9]\.)'))

$(eval $(call TestHostCommand,case-sensitive-fs, \
	OpenWrt can only be built on a case-sensitive filesystem, \
	$(RM) $(TMP_DIR)/test.*; touch $(TMP_DIR)/test.fs; \
		test ! -f $(TMP_DIR)/test.FS))

$(eval $(call TestHostCommand,proper-umask, \
	Please build with umask 022 - other values produce broken packages, \
	umask | grep -xE 0?0[012][012]))

ifndef IB
$(eval $(call SetupHostCommand,gcc, \
	Please install the GNU C Compiler (gcc) 6 or later, \
	$(CC) -dumpversion | grep -E '^([6-9]\.?|1[0-9]\.?)', \
	gcc -dumpversion | grep -E '^([6-9]\.?|1[0-9]\.?)', \
	gcc --version | grep -E 'Apple.(LLVM|clang)' ))

$(eval $(call TestHostCommand,working-gcc, \
	Please reinstall the GNU C Compiler (6 or later) - \
	it appears to be broken, \
	echo 'int main(int argc, char **argv) { return 0; }' | \
		gcc -x c -o $(TMP_DIR)/a.out -))

$(eval $(call SetupHostCommand,g++, \
	Please install the GNU C++ Compiler (g++) 6 or later, \
	$(CXX) -dumpversion | grep -E '^([6-9]\.?|1[0-9]\.?)', \
	g++ -dumpversion | grep -E '^([6-9]\.?|1[0-9]\.?)', \
	g++ --version | grep -E 'Apple.(LLVM|clang)' ))

$(eval $(call TestHostCommand,working-g++, \
	Please reinstall the GNU C++ Compiler (6 or later) - \
	it appears to be broken, \
	echo 'int main(int argc, char **argv) { return 0; }' | \
		g++ -x c++ -o $(TMP_DIR)/a.out - -lstdc++ && \
		$(TMP_DIR)/a.out))

$(eval $(call TestHostCommand,ncurses, \
	Please install ncurses. (Missing libncurses.so or ncurses.h), \
	echo 'int main(int argc, char **argv) { initscr(); return 0; }' | \
		gcc -include ncurses.h -x c -o $(TMP_DIR)/a.out - -lncurses))

$(eval $(call SetupHostCommand,git,Please install Git (git-core) >= 2.10, \
	git --version | grep '2.[1-9][0-9].', \
	git --version | grep '3.[0-9][0-9].'))

$(eval $(call TestHostCommand,git-submodule,git-submodule is missing --recursive support, \
	git --exec-path | xargs -I % -- grep -q -- --recursive %/git-submodule, \
	git submodule --help | grep -- --recursive))

$(eval $(call SetupHostCommand,rsync,Please install 'rsync', \
	rsync --version </dev/null))
endif # IB

ifeq ($(HOST_OS),Linux)
  zlib_link_flags := -Wl,-Bstatic -lz -Wl,-Bdynamic
else
  zlib_link_flags := -lz
endif

$(eval $(call TestHostCommand,perl-data-dumper, \
	Please install the Perl Data::Dumper module, \
	perl -MData::Dumper -e 1))

$(eval $(call TestHostCommand,perl-findbin, \
	Please install the Perl FindBin module, \
	perl -MFindBin -e 1))

$(eval $(call TestHostCommand,perl-file-copy, \
	Please install the Perl File::Copy module, \
	perl -MFile::Copy -e 1))

$(eval $(call TestHostCommand,perl-file-compare, \
	Please install the Perl File::Compare module, \
	perl -MFile::Compare -e 1))

$(eval $(call TestHostCommand,perl-thread-queue, \
	Please install the Perl Thread::Queue module, \
	perl -MThread::Queue -e 1))

$(eval $(call SetupHostCommand,tar,Please install GNU 'tar', \
	gtar --version 2>&1 | grep GNU, \
	gnutar --version 2>&1 | grep GNU, \
	tar --version 2>&1 | grep GNU))

$(eval $(call SetupHostCommand,find,Please install GNU 'find', \
	gfind --version 2>&1 | grep GNU, \
	find --version 2>&1 | grep GNU))

$(eval $(call SetupHostCommand,bash,Please install GNU 'bash', \
	bash --version 2>&1 | grep GNU))

$(eval $(call SetupHostCommand,xargs, \
	Please install 'xargs' that supports '-r/--no-run-if-empty', \
	gxargs -r --version, \
	xargs -r --version))

$(eval $(call SetupHostCommand,patch,Please install GNU 'patch', \
	gpatch --version 2>&1 | grep 'Free Software Foundation', \
	patch --version 2>&1 | grep 'Free Software Foundation'))

$(eval $(call SetupHostCommand,diff,Please install GNU diffutils, \
	gdiff --version 2>&1 | grep GNU, \
	diff --version 2>&1 | grep GNU))

$(eval $(call SetupHostCommand,cp,Please install GNU fileutils, \
	gcp --help 2>&1 | grep 'Copy SOURCE', \
	cp --help 2>&1 | grep 'Copy SOURCE'))

$(eval $(call SetupHostCommand,seq,Please install seq, \
	gseq --version, \
	seq --version 2>&1 | grep seq))

$(eval $(call SetupHostCommand,awk,Please install GNU 'awk', \
	gawk --version 2>&1 | grep GNU, \
	awk --version 2>&1 | grep GNU))

$(eval $(call SetupHostCommand,grep,Please install GNU 'grep', \
	ggrep --version 2>&1 | grep GNU, \
	grep --version 2>&1 | grep GNU))

$(eval $(call SetupHostCommand,egrep,Please install GNU 'grep', \
	gegrep --version 2>&1 | grep GNU, \
	egrep --version 2>&1 | grep GNU))

$(eval $(call FindHostCommand,*getopt, \
	Please install an extended getopt version that supports --long, \
	gnugetopt -o t --long test -- --test | grep '^ *--test *--', \
	getopt -o t --long test -- --test | grep '^ *--test *--'))

$(eval $(call SetupHostCommand,realpath,Please install a 'realpath' utility, \
	grealpath /, \
	realpath /))

$(eval $(call SetupHostCommand,stat,Cannot find a file stat utility, \
	gnustat -c%s $(TOPDIR)/Makefile, \
	gstat -c%s $(TOPDIR)/Makefile, \
	stat -c%s $(TOPDIR)/Makefile))

$(eval $(call FindHostCommand,unzip,Please install 'unzip', \
	unzip))

$(eval $(call SetupHostCommand,bzip2,Please install 'bzip2', \
	bzip2 --version </dev/null))

$(eval $(call TestHostCommand,download-util,Please install 'curl' or GNU 'wget', \
	curl --version | grep http, \
	wget --version | grep GNU))

$(eval $(call FindHostCommand,*install,Please install GNU 'install', \
	install --version | grep GNU, \
	ginstall --version | grep GNU))

$(eval $(call FindHostCommand,*readlink,Please install GNU 'readlink', \
	readlink --version | grep GNU, \
	greadlink --version | grep GNU))

$(eval $(call SetupHostCommand,perl,Please install Perl >= 5.20, \
	perl --version | grep 'perl.*v5.[2-9][0-9]'))

$(eval $(call SetupHostCommand,python,Please install Python >= 3.7, \
	python3.10 -V 2>&1 | grep 'Python 3', \
	python3.9 -V 2>&1 | grep 'Python 3', \
	python3.8 -V 2>&1 | grep 'Python 3', \
	python3.7 -V 2>&1 | grep 'Python 3', \
	python3 -V 2>&1 | grep -E 'Python 3\.([7-9]|10)\.?'))

$(eval $(call SetupHostCommand,python-config,Please install python-dev tools >= 3.7, \
	python3.10-config --help | grep config, \
	python3.9-config --help | grep config, \
	python3.8-config --help | grep config, \
	python3.7-config --help | grep config, \
	python3-config --configdir | grep -E '3\.([7-9]|10)\.?'))

$(eval $(call SetupHostCommand,python3,Please install Python >= 3.7, \
	python3.10 -V 2>&1 | grep 'Python 3', \
	python3.9 -V 2>&1 | grep 'Python 3', \
	python3.8 -V 2>&1 | grep 'Python 3', \
	python3.7 -V 2>&1 | grep 'Python 3', \
	python3 -V 2>&1 | grep -E 'Python 3\.([7-9]|10)\.?'))

$(eval $(call SetupHostCommand,python3-config,Please install python-dev tools >= 3.7, \
	python3.10-config --help | grep config, \
	python3.9-config --help | grep config, \
	python3.8-config --help | grep config, \
	python3.7-config --help | grep config, \
	python3-config --configdir | grep -E '3\.([7-9]|10)\.?'))

$(eval $(call TestHostCommand,python3-distutils, \
	Please install the Python3 distutils module, \
	$(STAGING_DIR_HOST)/bin/python3 -c 'import distutils'))

$(eval $(call SetupHostCommand,file,Please install the 'file' package, \
	file --version 2>&1 | grep file))

$(eval $(call FindHostCommand,which,Please install 'which', \
	which which))

$(eval $(call TestHostCommand,ldconfig,Failed to link ldconfig, \
	$(LN) true $(STAGING_DIR_HOST)/bin/ldconfig))
