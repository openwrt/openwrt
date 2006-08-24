# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/prereq.mk

define Require/non-root
	[ "$$(shell whoami)" != "root" ]
endef
$(eval $(call Require,non-root, \
	Building OpenWrt as root is not supported! \
))

# Required for the toolchain
define Require/working-make
	echo 'all: test' > $(TMP_DIR)/check.mk
	echo 'e0 = $$$$(foreach s,foobar,$$$$(eval $$$$s:))' >> $(TMP_DIR)/check.mk
	echo 'e1 = $$$$(foreach s,foobar, $$$$(eval $$$$s:))' >> $(TMP_DIR)/check.mk
	echo 'test: $$$$(strip $$$$(e0)) $$$$(strip $$$$(e1))' >> $(TMP_DIR)/check.mk
	$(NO_TRACE_MAKE) -f $(TMP_DIR)/check.mk
endef

$(eval $(call Require,working-make, \
	Your make version is buggy. Please install GNU make v3.81 or later. \
))

define Require/working-gcc
	echo 'int main(int argc, char **argv) { return 0; }' | \
		gcc -x c -o $(TMP_DIR)/a.out -
endef

$(eval $(call Require,working-gcc, \
	No working GNU C Compiler (gcc) was found on your system. \
))

define Require/working-g++
	echo 'int main(int argc, char **argv) { return 0; }' | \
		g++ -x c++ -o $(TMP_DIR)/a.out -lstdc++ -
endef

$(eval $(call Require,working-g++, \
	No working GNU C++ Compiler (g++) was found on your system. \
))

define Require/ncurses
	echo 'int main(int argc, char **argv) { initscr(); return 0; }' | \
		gcc -include ncurses.h -x c -o $(TMP_DIR)/a.out -lncurses -
endef

$(eval $(call Require,ncurses, \
	No ncurses development files were not found on your system. \
))


define Require/zlib
	echo 'int main(int argc, char **argv) { gzdopen(0, "rb"); return 0; }' | \
		gcc -include zlib.h -x c -o $(TMP_DIR)/a.out -lz -
endef

$(eval $(call Require,zlib, \
	No zlib development files were not found on your system. \
))


$(eval $(call RequireCommand,bison, \
	Please install GNU bison. \
))

$(eval $(call RequireCommand,flex, \
	Please install flex. \
))

$(eval $(call RequireCommand,python, \
	Please install python. \
))

$(eval $(call RequireCommand,unzip, \
	Please install unzip. \
))

$(eval $(call RequireCommand,bzip2, \
	Please install bzip2. \
))

$(eval $(call RequireCommand,patch, \
	Please install patch. \
))

$(eval $(call RequireCommand,perl, \
	Please install perl. \
))
