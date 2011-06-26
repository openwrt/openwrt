#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=eglibc
PKG_VERSION:=$(call qstrip,$(CONFIG_EGLIBC_VERSION))
PKG_REVISION:=$(call qstrip,$(CONFIG_EGLIBC_REVISION))

PKG_SOURCE_PROTO:=svn
PKG_SOURCE_VERSION:=$(PKG_REVISION)
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)-r$(PKG_REVISION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.bz2

ifneq ($(CONFIG_EGLIBC_VERSION_2_6),)
  PKG_SOURCE_URL:=svn://svn.eglibc.org/branches/eglibc-2_6
endif
ifneq ($(CONFIG_EGLIBC_VERSION_2_7),)
  PKG_SOURCE_URL:=svn://svn.eglibc.org/branches/eglibc-2_7
endif
ifneq ($(CONFIG_EGLIBC_VERSION_2_8),)
  PKG_SOURCE_URL:=svn://svn.eglibc.org/branches/eglibc-2_8
endif
ifneq ($(CONFIG_EGLIBC_VERSION_2_9),)
  PKG_SOURCE_URL:=svn://svn.eglibc.org/branches/eglibc-2_9
endif
ifneq ($(CONFIG_EGLIBC_VERSION_2_10),)
  PKG_SOURCE_URL:=svn://svn.eglibc.org/branches/eglibc-2_10
endif
ifneq ($(CONFIG_EGLIBC_VERSION_2_11),)
  PKG_SOURCE_URL:=svn://svn.eglibc.org/branches/eglibc-2_11
endif
ifneq ($(CONFIG_EGLIBC_VERSION_2_12),)
  PKG_SOURCE_URL:=svn://svn.eglibc.org/branches/eglibc-2_12
endif
ifneq ($(CONFIG_EGLIBC_VERSION_2_13),)
  PKG_SOURCE_URL:=svn://svn.eglibc.org/branches/eglibc-2_13
endif
ifneq ($(CONFIG_EGLIBC_VERSION_TRUNK),)
  PKG_SOURCE_URL:=svn://svn.eglibc.org/trunk
endif

PATCH_DIR:=./patches/$(PKG_VERSION)

HOST_BUILD_DIR:=$(BUILD_DIR_TOOLCHAIN)/$(PKG_SOURCE_SUBDIR)

include $(INCLUDE_DIR)/toolchain-build.mk

HOST_STAMP_BUILT:=$(TOOLCHAIN_DIR)/stamp/.eglibc_built
HOST_STAMP_INSTALLED:=$(TOOLCHAIN_DIR)/stamp/.eglibc_installed

HOST_BUILD_DIR1:=$(HOST_BUILD_DIR)-initial
HOST_BUILD_DIR2:=$(HOST_BUILD_DIR)-final

EGLIBC_CFLAGS:= \
	$(TARGET_CFLAGS)

EGLIBC_CONFIGURE:= \
	BUILD_CC="$(HOSTCC)" \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(EGLIBC_CFLAGS)" \
	libc_cv_slibdir="/lib" \
	use_ldconfig=no \
	$(HOST_BUILD_DIR)/libc/configure \
		--prefix= \
		--build=$(GNU_HOST_NAME) \
		--host=$(REAL_GNU_TARGET_NAME) \
		--with-headers=$(TOOLCHAIN_DIR)/include \
		--disable-profile \
		--without-gd \
		--without-cvs \
		--enable-add-ons \

ifeq ($(CONFIG_SOFT_FLOAT),)
  EGLIBC_CONFIGURE+= \
	--with-fp
else
  EGLIBC_CONFIGURE+= \
	--without-fp
endif

EGLIBC_MAKE:= \
	$(MAKE) \

export libc_cv_ssp=no

define Host/SetToolchainInfo
	$(SED) 's,^\(LIBC_TYPE\)=.*,\1=$(PKG_NAME),' $(TOOLCHAIN_DIR)/info.mk
	$(SED) 's,^\(LIBC_URL\)=.*,\1=http://www.eglibc.org/,' $(TOOLCHAIN_DIR)/info.mk
	$(SED) 's,^\(LIBC_VERSION\)=.*,\1=$(PKG_VERSION),' $(TOOLCHAIN_DIR)/info.mk
	$(SED) 's,^\(LIBC_SO_VERSION\)=.*,\1=$(PKG_VERSION),' $(TOOLCHAIN_DIR)/info.mk
endef

define Stage1/Configure
	mkdir -p $(HOST_BUILD_DIR1)
	$(CP) $(HOST_BUILD_DIR)/libc/option-groups.config $(HOST_BUILD_DIR1)/
	( cd $(HOST_BUILD_DIR1); rm -f config.cache; \
		$(EGLIBC_CONFIGURE) \
	);
endef

define Stage1/Compile
endef

define Stage1/Install
	mkdir -p $(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev/{include,lib}
	$(EGLIBC_MAKE) -C $(HOST_BUILD_DIR1) \
		install_root="$(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev" \
		install-bootstrap-headers=yes \
		install-headers 
	$(EGLIBC_MAKE) -C $(HOST_BUILD_DIR1) \
		csu/subdir_lib
	( cd $(HOST_BUILD_DIR1); \
		$(CP) csu/crt1.o csu/crti.o csu/crtn.o $(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev/lib/ \
	)
	$(TARGET_CC) -nostdlib -nostartfiles -shared -x c /dev/null \
		-o $(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev/lib/libc.so
endef

define Stage2/Configure
	mkdir -p $(HOST_BUILD_DIR2)
	$(CP) $(HOST_BUILD_DIR)/libc/option-groups.config $(HOST_BUILD_DIR2)/
	( cd $(HOST_BUILD_DIR2); rm -f config.cache; \
		$(EGLIBC_CONFIGURE) \
	);
endef

define Stage2/Compile
	$(EGLIBC_MAKE) -C $(HOST_BUILD_DIR2) all
endef

define Stage2/Install
	$(EGLIBC_MAKE) -C $(HOST_BUILD_DIR2) \
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
	ln -snf $(PKG_SOURCE_SUBDIR) $(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)
	$(SED) 's,y,n,' $(HOST_BUILD_DIR)/libc/option-groups.defaults
	grep 'CONFIG_EGLIBC_OPTION_' $(TOPDIR)/.config | sed -e "s,\\(# \)\\?CONFIG_EGLIBC_\\(.*\\),\\1\\2,g" > $(HOST_BUILD_DIR)/libc/option-groups.config
	ln -sf ../ports $(HOST_BUILD_DIR)/libc/
	( cd $(HOST_BUILD_DIR)/libc; autoconf --force )
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
	rm -rf $(HOST_BUILD_DIR) $(HOST_BUILD_DIR1) $(HOST_BUILD_DIR2) \
		$(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev \
		$(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)
endef

$(eval $(call HostBuild))
