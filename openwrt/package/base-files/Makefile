# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=base-files
PKG_RELEASE:=8
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/base-files

REV:=$(shell LANG=C svn info | awk '/^Revision:/ { print$$2 }' )
ifeq ($(REV),)
REV:=0
endif

include $(INCLUDE_DIR)/package.mk

ifneq ($(DUMP),1)
TARGET:=-$(BOARD)-$(KERNEL)
UCLIBC_VERSION:=${shell cat $(STAGING_DIR)/uclibc_version}
LIBGCC_VERSION:=${shell cat $(STAGING_DIR)/gcc_version}
else
UCLIBC_VERSION:=<UCLIBC_VERSION>
LIBGCC_VERSION:=<LIBGCC_VERSION>
endif

CONFIG_PACKAGE_base-files$(TARGET):=$(CONFIG_PACKAGE_base-files)

define Package/base-files$(TARGET)
  SECTION:=base
  CATEGORY:=Base system
  DEFAULT:=y
  TITLE:=OpenWrt system scripts
  DESCRIPTION:=Base filesystem for OpenWrt
  VERSION:=$(PKG_RELEASE)
  FORCEREBUILD:=y
endef

define -ar7-2.4/conffiles
/etc/config/network
endef

define -aruba-2.6/conffiles
/etc/config/network
endef

define -au1000-2.6/conffiles
/etc/config/network
endef

define -rb532-2.6/conffiles
/etc/config/network
endef

define -sibyte-2.6/conffiles
/etc/config/network
endef

define -x86-2.6/conffiles
/etc/config/network
endef

define -xscale-2.6/conffiles
/etc/config/network
endef


define Package/base-files$(TARGET)/conffiles
/etc/banner
/etc/hosts
/etc/inittab
/etc/group
/etc/passwd
/etc/profile
/etc/shells
/etc/ipkg.conf
/etc/sysctl.conf
$(call $(TARGET)/conffiles)
endef

define Package/libgcc
  SECTION:=libs
  CATEGORY:=Libraries
  DEFAULT:=y
  TITLE:=GCC support library
  VERSION:=$(LIBGCC_VERSION)-$(PKG_RELEASE)
  DESCRIPTION:=$(TITLE)
  FORCEREBUILD:=n
endef

define Package/libpthread
  $(call Package/base-files$(TARGET))
  DEFAULT:=n
  VERSION:=$(UCLIBC_VERSION)-$(PKG_RELEASE)
  TITLE:=POSIX thread library
  DESCRIPTION:=POSIX thread library
  FORCEREBUILD:=n
endef


define Package/uclibc
  $(call Package/base-files$(TARGET))
  VERSION:=$(UCLIBC_VERSION)-$(PKG_RELEASE)
  TITLE:=C library
  DESCRIPTION:=C library for embedded systems
  FORCEREBUILD:=n
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef


define Build/Compile/ar7
	$(TARGET_CC) -o $(PKG_BUILD_DIR)/adam2patcher src/adam2patcher.c
endef

define Build/Compile/brcm
	$(TARGET_CC) -o $(PKG_BUILD_DIR)/jffs2root src/jffs2root.c
endef

define Build/Compile
  $(call Build/Compile/$(BOARD))
endef


define Package/base-files$(TARGET)/install-ar7
	mkdir -p $(1)/sbin
	$(CP) $(PKG_BUILD_DIR)/adam2patcher $(1)/sbin
endef
define Package/base-files$(TARGET)/install-brcm
	mkdir -p $(1)/sbin
	$(CP) $(PKG_BUILD_DIR)/jffs2root $(1)/sbin
endef

define Package/base-files$(TARGET)/install
  $(call Package/base-files$(TARGET)/install-$(BOARD),$(1))
	$(CP) ./default/* $(1)/
	if [ -d $(BOARD)-$(KERNEL) ]; then \
		$(CP) $(BOARD)-$(KERNEL)/* $(1)/; \
	fi
	$(SED) 's,$$$$R,r$(REV),g' $(1)/etc/banner
	$(SED) 's,$$$$S,$(BOARD)-$(KERNEL),g' $(1)/etc/ipkg.conf
	mkdir -p $(1)/dev
	mkdir -p $(1)/etc/crontabs
	mkdir -p $(1)/jffs
	mkdir -p $(1)/lib
	mkdir -p $(1)/mnt
	mkdir -p $(1)/proc
	mkdir -p $(1)/tmp
	mkdir -p $(1)/usr/lib
	mkdir -p $(1)/usr/bin
	mkdir -p $(1)/sys
	mkdir -p $(1)/www
	ln -sf /tmp/resolv.conf $(1)/etc/resolv.conf
	ln -sf /proc/mounts $(1)/etc/mtab
	rm -f $(1)/var
	ln -sf /tmp $(1)/var
	mkdir -p $(1)/etc
endef

define Package/libgcc/install
	install -m0755 -d $(1)/lib
	$(CP) $(STAGING_DIR)/lib/libgcc_s.so.* $(1)/lib/
endef

define Package/libpthread/install
	install -m0755 -d $(1)/lib
	$(CP) $(STAGING_DIR)/lib/libpthread.so.* $(1)/lib/
	$(CP) $(STAGING_DIR)/lib/libpthread-$(UCLIBC_VERSION).so $(1)/lib/
endef

define Package/uclibc/install
	install -m0755 -d $(1)/lib
	for file in ld-uClibc libc libcrypt libdl libm libnsl libresolv librt libuClibc libutil; do \
		$(CP) $(STAGING_DIR)/lib/$$$$file.so.* $(1)/lib/; \
		$(CP) $(STAGING_DIR)/lib/$$$$file-$(UCLIBC_VERSION).so $(1)/lib/; \
	done
endef

$(eval $(call BuildPackage,base-files$(TARGET)))
$(eval $(call BuildPackage,libgcc))
$(eval $(call BuildPackage,libpthread))
$(eval $(call BuildPackage,uclibc))
