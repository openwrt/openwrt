#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=glibc
PKG_VERSION:=$(call qstrip,$(CONFIG_GLIBC_VERSION))
PKG_REVISION:=$(call qstrip,$(CONFIG_GLIBC_REVISION))

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://sourceware.org/git/glibc.git
PKG_SOURCE_VERSION:=$(PKG_REVISION)
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)-r$(PKG_REVISION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.bz2

GLIBC_PATH:=
ifneq ($(CONFIG_EGLIBC_VERSION_2_15),)
  GLIBC_PATH:=libc/
  PKG_SOURCE_PROTO:=svn
  PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.bz2
  PKG_SOURCE_URL:=svn://svn.eglibc.org/branches/eglibc-2_15
endif
ifneq ($(CONFIG_EGLIBC_VERSION_2_19),)
  GLIBC_PATH:=libc/
  PKG_SOURCE_PROTO:=svn
  PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.bz2
  PKG_SOURCE_URL:=svn://svn.eglibc.org/branches/eglibc-2_19
endif

PATCH_DIR:=$(PATH_PREFIX)/patches/$(PKG_VERSION)

HOST_BUILD_DIR:=$(BUILD_DIR_TOOLCHAIN)/$(PKG_SOURCE_SUBDIR)
CUR_BUILD_DIR:=$(HOST_BUILD_DIR)-$(VARIANT)

include $(INCLUDE_DIR)/toolchain-build.mk

HOST_STAMP_PREPARED:=$(HOST_BUILD_DIR)/.prepared
HOST_STAMP_CONFIGURED:=$(CUR_BUILD_DIR)/.configured
HOST_STAMP_BUILT:=$(CUR_BUILD_DIR)/.built
HOST_STAMP_INSTALLED:=$(TOOLCHAIN_DIR)/stamp/.glibc_$(VARIANT)_installed

ifeq ($(ARCH),mips64)
  ifdef CONFIG_MIPS64_ABI_N64
    TARGET_CFLAGS += -mabi=64
  endif
  ifdef CONFIG_MIPS64_ABI_N32
    TARGET_CFLAGS += -mabi=n32
  endif
  ifdef CONFIG_MIPS64_ABI_O32
    TARGET_CFLAGS += -mabi=32
  endif
endif

GLIBC_CONFIGURE:= \
	BUILD_CC="$(HOSTCC)" \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS)" \
	libc_cv_slibdir="/lib" \
	use_ldconfig=no \
	$(HOST_BUILD_DIR)/$(GLIBC_PATH)configure \
		--prefix= \
		--build=$(GNU_HOST_NAME) \
		--host=$(REAL_GNU_TARGET_NAME) \
		--with-headers=$(TOOLCHAIN_DIR)/include \
		--disable-profile \
		--disable-werror \
		--without-gd \
		--without-cvs \
		--enable-add-ons \
		--$(if $(CONFIG_SOFT_FLOAT),without,with)-fp

export libc_cv_ssp=no
export ac_cv_header_cpuid_h=yes
export HOST_CFLAGS := $(HOST_CFLAGS) -idirafter $(CURDIR)/$(PATH_PREFIX)/include

define Host/SetToolchainInfo
	$(SED) 's,^\(LIBC_TYPE\)=.*,\1=$(PKG_NAME),' $(TOOLCHAIN_DIR)/info.mk
ifneq ($(CONFIG_GLIBC_VERSION_2_21),)
	$(SED) 's,^\(LIBC_URL\)=.*,\1=http://www.gnu.org/software/libc/,' $(TOOLCHAIN_DIR)/info.mk
else
	$(SED) 's,^\(LIBC_URL\)=.*,\1=http://www.eglibc.org/,' $(TOOLCHAIN_DIR)/info.mk
endif
	$(SED) 's,^\(LIBC_VERSION\)=.*,\1=$(PKG_VERSION),' $(TOOLCHAIN_DIR)/info.mk
	$(SED) 's,^\(LIBC_SO_VERSION\)=.*,\1=$(PKG_VERSION),' $(TOOLCHAIN_DIR)/info.mk
endef

define Host/Configure
	[ -f $(HOST_BUILD_DIR)/.autoconf ] || { \
		cd $(HOST_BUILD_DIR)/$(GLIBC_PATH); \
		autoconf --force && \
		touch $(HOST_BUILD_DIR)/.autoconf; \
	}
	mkdir -p $(CUR_BUILD_DIR)
	grep 'CONFIG_EGLIBC_OPTION_' $(TOPDIR)/.config | sed -e "s,\\(# \)\\?CONFIG_EGLIBC_\\(.*\\),\\1\\2,g" > $(CUR_BUILD_DIR)/option-groups.config
	( cd $(CUR_BUILD_DIR); rm -f config.cache; \
		$(GLIBC_CONFIGURE) \
	);
endef

define Host/Prepare
	$(call Host/Prepare/Default)
	ln -snf $(PKG_SOURCE_SUBDIR) $(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)
ifeq ($(CONFIG_GLIBC_VERSION_2_21),)
	$(SED) 's,y,n,' $(HOST_BUILD_DIR)/libc/option-groups.defaults
ifneq ($(CONFIG_EGLIBC_VERSION_2_15),)
	ln -sf ../ports $(HOST_BUILD_DIR)/libc/
endif
endif
endef

define Host/Clean
	rm -rf $(CUR_BUILD_DIR)* \
		$(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev \
		$(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)
endef
