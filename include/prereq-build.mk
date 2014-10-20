# 
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/prereq.mk
include $(INCLUDE_DIR)/host.mk
include $(INCLUDE_DIR)/host-build.mk

PKG_NAME:=Build dependency

define Require/non-root
	[ "$$(shell whoami)" != "root" ]
endef
$(eval $(call Require,non-root, \
	Please do not compile as root. \
))

# Required for the toolchain
define Require/working-make
	$(MAKE) -v | awk '($$$$1 == "GNU") && ($$$$2 == "Make") && ($$$$3 >= "3.81") { print "ok" }' | grep ok > /dev/null
endef

$(eval $(call Require,working-make, \
	Please install GNU make v3.81 or later. (This version has bugs) \
))

define Require/case-sensitive-fs
	rm -f $(TMP_DIR)/test.*
	touch $(TMP_DIR)/test.fs
	[ \! -f $(TMP_DIR)/test.FS ]
endef

$(eval $(call Require,case-sensitive-fs, \
	OpenWrt can only be built on a case-sensitive filesystem \
))

define Require/getopt
	gnugetopt --help 2>&1 | grep long >/dev/null || \
	/usr/local/bin/getopt --help 2>&1 | grep long >/dev/null || \
	getopt --help 2>&1 | grep long >/dev/null
endef
$(eval $(call Require,getopt, \
	Please install GNU getopt \
))

define Require/fileutils
	gcp --help || cp --help
endef
$(eval $(call Require,fileutils, \
	Please install GNU fileutils \
))

define Require/working-gcc
	echo 'int main(int argc, char **argv) { return 0; }' | \
		gcc -x c -o $(TMP_DIR)/a.out -
endef

$(eval $(call Require,working-gcc, \
	Please install the GNU C Compiler (gcc). \
))

define Require/working-g++
	echo 'int main(int argc, char **argv) { return 0; }' | \
		g++ -x c++ -o $(TMP_DIR)/a.out - -lstdc++ && \
		$(TMP_DIR)/a.out
endef

$(eval $(call Require,working-g++, \
	Please install the GNU C++ Compiler (g++). \
))

ifneq ($(HOST_STATIC_LINKING),)
  define Require/working-gcc-static
	echo 'int main(int argc, char **argv) { return 0; }' | \
		gcc -x c $(HOST_STATIC_LINKING) -o $(TMP_DIR)/a.out -
  endef

  $(eval $(call Require,working-gcc-static, \
    Please install the static libc development package (glibc-static on CentOS/Fedora/RHEL). \
  ))

  define Require/working-g++-static
	echo 'int main(int argc, char **argv) { return 0; }' | \
		g++ -x c++ $(HOST_STATIC_LINKING) -o $(TMP_DIR)/a.out - -lstdc++ && \
		$(TMP_DIR)/a.out
  endef

  $(eval $(call Require,working-g++-static, \
	Please install the static libstdc++ development package (libstdc++-static on CentOS/Fedora/RHEL). \
  ))
endif

define Require/ncurses
	echo 'int main(int argc, char **argv) { initscr(); return 0; }' | \
		gcc -include ncurses.h -x c -o $(TMP_DIR)/a.out - -lncurses
endef

$(eval $(call Require,ncurses, \
	Please install ncurses. (Missing libncurses.so or ncurses.h) \
))


define Require/zlib
	echo 'int main(int argc, char **argv) { gzdopen(0, "rb"); return 0; }' | \
		gcc -include zlib.h -x c -o $(TMP_DIR)/a.out - -lz
endef

$(eval $(call Require,zlib, \
	Please install zlib. (Missing libz.so or zlib.h) \
))

ifneq ($(HOST_STATIC_LINKING),)
  define Require/zlib-static
	echo 'int main(int argc, char **argv) { gzdopen(0, "rb"); return 0; }' | \
		gcc -include zlib.h -x c $(HOST_STATIC_LINKING) -o $(TMP_DIR)/a.out - -lz
  endef

  $(eval $(call Require,zlib-static, \
	Please install a static zlib. (zlib-static on CentOS/Fedora/RHEL). \
  ))
endif

$(eval $(call RequireCommand,gawk, \
	Please install GNU awk. \
))

$(eval $(call RequireCommand,unzip, \
	Please install unzip. \
))

$(eval $(call RequireCommand,bzip2, \
	Please install bzip2. \
))

$(eval $(call RequireCommand,perl, \
	Please install perl. \
))

$(eval $(call RequireCommand,$(PYTHON), \
	Please install python. \
))

$(eval $(call RequireCommand,wget, \
	Please install wget. \
))

define Require/git
	git --version | awk '($$$$1 == "git") && ($$$$2 == "version") && ($$$$3 >= "1.6.5") { print "ok" }' | grep ok > /dev/null
endef

$(eval $(call Require,git, \
	Please install git (git-core) v1.6.5 or later. \
))

define Require/gnutar
	$(TAR) --version 2>&1 | grep GNU > /dev/null
endef

$(eval $(call Require,gnutar, \
	Please install GNU tar. \
))

$(eval $(call RequireCommand,svn, \
	Please install the subversion client. \
))

define Require/libssl
	echo 'int main(int argc, char **argv) { SSL_library_init(); return 0; }' | \
		gcc -include openssl/ssl.h -x c -o $(TMP_DIR)/a.out - -lcrypto -lssl
endef

$(eval $(call Require,libssl, \
	Please install the openssl library (with development headers) \
))

$(eval $(call RequireCommand,openssl, \
       Please install openssl. \
))

define Require/gnu-find
	$(FIND) --version 2>/dev/null
endef

$(eval $(call Require,gnu-find, \
	Please install GNU find \
))

define Require/getopt-extended
	getopt --long - - >/dev/null
endef

$(eval $(call Require,getopt-extended, \
	Please install an extended getopt version that supports --long \
))

$(eval $(call RequireCommand,file, \
	Please install the 'file' package. \
))
