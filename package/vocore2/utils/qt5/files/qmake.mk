#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

QMAKE_SPECFILE:=$(STAGING_DIR)/usr/mkspecs/linux-openwrt-g++

TARGET_INCDIRS +=\
	$(STAGING_DIR)/include \
	$(STAGING_DIR)/usr/include

TARGET_LIBDIRS += \
	$(STAGING_DIR)/lib \
	$(STAGING_DIR)/usr/lib 

define Build/Configure/Qmake
	TARGET_CC="$(TARGET_CROSS)gcc" \
	TARGET_CXX="$(TARGET_CROSS)g++" \
	TARGET_AR="$(TARGET_CROSS)ar cqs" \
	TARGET_OBJCOPY="$(TARGET_CROSS)objcopy" \
	TARGET_RANLIB="$(TARGET_CROSS)ranlib" \
	TARGET_CFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
	TARGET_CXXFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
	TARGET_LDFLAGS="$(TARGET_LDFLAGS) $(EXTRA_LDFLAGS)" \
	TARGET_INCDIRS="$(TARGET_INCDIRS)" \
	TARGET_LIBDIRS="$(TARGET_LIBDIRS)" \
	TARGET_LIBS="$(TARGET_LIBS)" \
	STAGING_DIR_HOST="$(STAGING_DIR)/../host" \
	STAGING_DIR="$(STAGING_DIR)" \
	qmake \
		-spec $(QMAKE_SPECFILE) \
		-o $(PKG_BUILD_DIR)/Makefile \
		$(2) \
		$(PKG_BUILD_DIR)/$(1).pro
endef

# we need to pass everything to $(MAKE) as well, as Makefiles may invoke qmake once again for creating further Makefiles
define Build/Compile/Default
	TARGET_CC="$(TARGET_CROSS)gcc" \
	TARGET_CXX="$(TARGET_CROSS)g++" \
	TARGET_AR="$(TARGET_CROSS)ar cqs" \
	TARGET_OBJCOPY="$(TARGET_CROSS)objcopy" \
	TARGET_RANLIB="$(TARGET_CROSS)ranlib" \
	TARGET_CFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
	TARGET_CXXFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
	TARGET_LDFLAGS="$(TARGET_LDFLAGS) $(EXTRA_LDFLAGS)" \
	TARGET_INCDIRS="$(TARGET_INCDIRS)" \
	TARGET_LIBDIRS="$(TARGET_LIBDIRS)" \
	TARGET_LIBS="$(TARGET_LIBS)" \
	STAGING_DIR_HOST="$(STAGING_DIR)/../host" \
	STAGING_DIR="$(STAGING_DIR)" \
	$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR)/$(MAKE_PATH) \
		$(1);
endef
