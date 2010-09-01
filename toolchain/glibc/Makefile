#
# Copyright (C) 2006-2009 OpenWrt.org
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
ifeq ($(PKG_VERSION),2.4)
  PKG_MD5SUM:=7e9a88dcd41fbc53801dbe5bdacaf245
endif
ifeq ($(PKG_VERSION),2.6.1)
  PKG_MD5SUM:=11cf6d3fc86dbe0890b8d00372eb6286
endif
ifeq ($(PKG_VERSION),2.7)
  PKG_MD5SUM:=065c5952b439deba40083ccd67bcc8f7
endif

PKG_SOURCE_URL:=@GNU/glibc/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2

PATCH_DIR:=./patches/$(PKG_VERSION)

include $(INCLUDE_DIR)/toolchain-build.mk

HOST_STAMP_BUILT:=$(TOOLCHAIN_DIR)/stamp/.glibc_built
HOST_STAMP_INSTALLED:=$(TOOLCHAIN_DIR)/stamp/.glibc_installed

HOST_BUILD_DIR1:=$(HOST_BUILD_DIR)-initial
HOST_BUILD_DIR2:=$(HOST_BUILD_DIR)-final

GLIBC_ADD_ONS+=nptl,

ifneq ($(CONFIG_GLIBC_PORTS),)
  GLIBC_ADD_ONS+=ports,
  define Host/Prepare/ports
	ln -snf ../glibc-ports $(HOST_BUILD_DIR)/ports
  endef
endif

# XXX: {e,}glibc does not build w/ -Os
# http://sourceware.org/bugzilla/show_bug.cgi?id=5203
GLIBC_CFLAGS:=$(subst -Os,-O2,$(TARGET_CFLAGS))

GLIBC_CONFIGURE:= \
	BUILD_CC="$(HOSTCC)" \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(GLIBC_CFLAGS)" \
	libc_cv_forced_unwind=yes \
	libc_cv_c_cleanup=yes \
	libc_cv_386_tls=yes \
	libc_cv_slibdir="/lib" \
	$(HOST_BUILD_DIR)/configure \
		--prefix= \
		--build=$(GNU_HOST_NAME) \
		--host=$(REAL_GNU_TARGET_NAME) \
		--with-headers="$(TOOLCHAIN_DIR)/include" \
		$(if $(CONFIG_mips64)$(CONFIG_mips64el), --enable-kernel="2.6.0") \
		--disable-debug \
		--disable-profile \
		--enable-add-ons="$(GLIBC_ADD_ONS)" \
		--without-gd \
		--without-cvs \

ifeq ($(CONFIG_SOFT_FLOAT),)
  GLIBC_CONFIGURE+= \
		--with-fp
else
  GLIBC_CONFIGURE+= \
		--without-fp
endif

GLIBC_CONFIGURE_STAGE1:= \
	$(GLIBC_CONFIGURE) \
		--disable-sanity-checks \
		--enable-hacker-mode \
	
GLIBC_CONFIGURE_STAGE2:= \
	$(GLIBC_CONFIGURE) \

GLIBC_MAKE:= \
	$(MAKE) \


define Host/SetToolchainInfo
	$(SED) 's,^\(LIBC_TYPE\)=.*,\1=$(PKG_NAME),' $(TOOLCHAIN_DIR)/info.mk
	$(SED) 's,^\(LIBC_URL\)=.*,\1=http://www.gnu.org/software/libc/,' $(TOOLCHAIN_DIR)/info.mk
	$(SED) 's,^\(LIBC_VERSION\)=.*,\1=$(PKG_VERSION),' $(TOOLCHAIN_DIR)/info.mk
	$(SED) 's,^\(LIBC_SO_VERSION\)=.*,\1=$(PKG_VERSION),' $(TOOLCHAIN_DIR)/info.mk
endef

define Stage1/Configure
	mkdir -p $(HOST_BUILD_DIR1)
	( cd $(HOST_BUILD_DIR1); rm -f config.cache; \
		$(GLIBC_CONFIGURE_STAGE1) \
	);
endef

define Stage1/Compile
endef

define Stage1/Install
	$(GLIBC_MAKE) -C $(HOST_BUILD_DIR1) \
		CFLAGS="-DBOOTSTRAP_GCC" \
		cross-compiling=yes \
		install_root="$(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev" \
		install-headers
	[ -f $(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev/include/bits/stdio_lim.h ] || \
		$(CP) $(HOST_BUILD_DIR1)/bits/stdio_lim.h \
			$(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev/include/bits/stdio_lim.h
	[ -f $(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev/include/gnu/stubs.h ] || \
		touch $(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev/include/gnu/stubs.h
endef

define Stage2/Configure
	mkdir -p $(HOST_BUILD_DIR2)
	( cd $(HOST_BUILD_DIR2); rm -f config.cache; \
		$(GLIBC_CONFIGURE_STAGE2) \
	);
endef

define Stage2/Compile
	$(GLIBC_MAKE) -C $(HOST_BUILD_DIR2) all
endef

define Stage2/Install
	$(GLIBC_MAKE) -C $(HOST_BUILD_DIR2) \
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

define Host/Prepare
	$(call Host/SetToolchainInfo)
	$(call Host/Prepare/Default)
	ln -snf $(PKG_NAME)-$(PKG_VERSION) $(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)
	$(call Host/Prepare/ports)
	$(call Stage1/Configure)
	$(call Stage1/Compile)
	$(call Stage1/Install)
endef

define Host/Configure
endef

define Host/Compile
	$(call Stage2/Configure)
	$(call Stage2/Compile)
	$(call Stage2/Install)
endef

define Host/Install
endef

define Host/Clean
	rm -rf \
		$(HOST_BUILD_DIR) \
		$(HOST_BUILD_DIR1) \
		$(HOST_BUILD_DIR2) \
		$(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev \
		$(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)
endef

$(eval $(call HostBuild))
