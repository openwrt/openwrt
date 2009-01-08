#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=glibc
PKG_VERSION:=$(call qstrip,$(CONFIG_GLIBC_VERSION))

ifeq ($(PKG_VERSION),2.3.6)
  PKG_MD5SUM:=bfdce99f82d6dbcb64b7f11c05d6bc96
endif
ifeq ($(PKG_VERSION),2.6.1)
  PKG_MD5SUM:=11cf6d3fc86dbe0890b8d00372eb6286
endif
ifeq ($(PKG_VERSION),2.7)
  PKG_MD5SUM:=065c5952b439deba40083ccd67bcc8f7
endif

PKG_SOURCE_URL:=@GNU/glibc/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PATCH_DIR:=./patches/$(PKG_VERSION)

STAGING_DIR_HOST:=$(TOOLCHAIN_DIR)
BUILD_DIR_HOST:=$(BUILD_DIR_TOOLCHAIN)

override CONFIG_AUTOREBUILD=

include $(INCLUDE_DIR)/host-build.mk

STAMP_BUILT:=$(TOOLCHAIN_DIR)/stamp/.glibc_built
STAMP_INSTALLED:=$(TOOLCHAIN_DIR)/stamp/.glibc_installed

PKG_BUILD_DIR1:=$(PKG_BUILD_DIR)-initial
PKG_BUILD_DIR2:=$(PKG_BUILD_DIR)-final

GLIBC_ADD_ONS+=nptl,

ifneq ($(CONFIG_GLIBC_PORTS),)
  GLIBC_ADD_ONS+=ports,
  define Build/Prepare/ports
	ln -snf ../glibc-ports $(PKG_BUILD_DIR)/ports
  endef
endif

# XXX: {e,}glibc does not build w/ -Os
# http://sourceware.org/bugzilla/show_bug.cgi?id=5203
GLIBC_CFLAGS:=$(subst -Os,-O2,$(TARGET_CFLAGS))

GLIBC_CONFIGURE_COMMON:= \
	BUILD_CC="$(HOSTCC)" \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(GLIBC_CFLAGS)" \
	libc_cv_forced_unwind=yes \
	libc_cv_c_cleanup=yes \
	libc_cv_386_tls=yes \
	$(PKG_BUILD_DIR)/configure \
		--prefix=/usr \
		--build=$(GNU_HOST_NAME) \
		--host=$(REAL_GNU_TARGET_NAME) \
		--with-headers="$(TOOLCHAIN_DIR)/usr/include" \
		--disable-debug \
		--disable-profile \
		--enable-add-ons="$(GLIBC_ADD_ONS)" \
		--without-gd \
		--without-cvs \

ifeq ($(CONFIG_SOFT_FLOAT),)
  GLIBC_CONFIGURE_COMMON+= \
		--with-fp
else
  GLIBC_CONFIGURE_COMMON+= \
		--without-fp
endif

GLIBC_CONFIGURE_STAGE1:= \
	$(GLIBC_CONFIGURE_COMMON) \
		--disable-sanity-checks \
		--enable-hacker-mode \
	
GLIBC_CONFIGURE_STAGE2:= \
	$(GLIBC_CONFIGURE_COMMON) \

GLIBC_MAKE:= \
	$(MAKE) \


define Build/SetToolchainInfo
	$(SED) 's,^\(LIBC_TYPE\)=.*,\1=$(PKG_NAME),' $(TOOLCHAIN_DIR)/info.mk
	$(SED) 's,^\(LIBC_URL\)=.*,\1=http://www.gnu.org/software/libc/,' $(TOOLCHAIN_DIR)/info.mk
	$(SED) 's,^\(LIBC_VERSION\)=.*,\1=$(PKG_VERSION),' $(TOOLCHAIN_DIR)/info.mk
	$(SED) 's,^\(LIBC_PATCHVER\)=.*,\1=,' $(TOOLCHAIN_DIR)/info.mk
endef

define Stage1/Configure
	mkdir -p $(PKG_BUILD_DIR1)
	( cd $(PKG_BUILD_DIR1); rm -f config.cache; \
		$(GLIBC_CONFIGURE_STAGE1) \
	);
endef

define Stage1/Compile
endef

define Stage1/Install
	$(GLIBC_MAKE) -C $(PKG_BUILD_DIR1) \
		CFLAGS="-DBOOTSTRAP_GCC" \
		cross-compiling=yes \
		install_root="$(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev" \
		install-headers
	[ -f $(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev/usr/include/bits/stdio_lim.h ] || \
		$(CP) $(PKG_BUILD_DIR1)/bits/stdio_lim.h \
			$(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev/usr/include/bits/stdio_lim.h
	[ -f $(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev/usr/include/gnu/stubs.h ] || \
		touch $(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev/usr/include/gnu/stubs.h
endef

define Stage2/Configure
	mkdir -p $(PKG_BUILD_DIR2)
	( cd $(PKG_BUILD_DIR2); rm -f config.cache; \
		$(GLIBC_CONFIGURE_STAGE2) \
	);
endef

define Stage2/Compile
	$(GLIBC_MAKE) -C $(PKG_BUILD_DIR2) all
endef

define Stage2/Install
	$(GLIBC_MAKE) -C $(PKG_BUILD_DIR2) \
		install_root="$(TOOLCHAIN_DIR)" \
		install
	( cd $(TOOLCHAIN_DIR) ; \
		for d in lib usr/lib ; do \
		  for f in libc.so libpthread.so libgcc_s.so ; do \
		    if [ -f $$$$d/$$$$f -a ! -L $$$$d/$$$$f ] ; then \
		      $(SED) 's,/usr/lib/,,g;s,/lib/,,g' $$$$d/$$$$f ; \
		    fi \
		  done \
		done \
	)
endef

define Build/Prepare
	$(call Build/SetToolchainInfo)
	$(call Build/Prepare/Default)
	ln -snf $(PKG_NAME)-$(PKG_VERSION) $(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)
	$(call Build/Prepare/ports)
	$(call Stage1/Configure)
	$(call Stage1/Compile)
	$(call Stage1/Install)
endef

define Build/Configure
endef

define Build/Compile
	$(call Stage2/Configure)
	$(call Stage2/Compile)
	$(call Stage2/Install)
endef

define Build/Install
endef

define Build/Clean
	rm -rf \
		$(PKG_BUILD_DIR) \
		$(PKG_BUILD_DIR1) \
		$(PKG_BUILD_DIR2) \
		$(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev \
		$(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)
endef

$(eval $(call HostBuild))
