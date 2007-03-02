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

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)
PKG_FILE_DEPEND:=$(PLATFORM_DIR)

REV:=$(shell LANG=C svn info | awk '/^Revision:/ { print$$2 }' )
ifeq ($(REV),)
  REV:=0
endif

include $(INCLUDE_DIR)/package.mk

ifneq ($(DUMP),1)
  TARGET:=-$(BOARD)-$(KERNEL)
  UCLIBC_VERSION:=${shell cat $(STAGING_DIR)/uclibc_version 2>/dev/null}
  LIBGCC_VERSION:=${shell cat $(STAGING_DIR)/gcc_version 2>/dev/null}
else
  UCLIBC_VERSION:=<UCLIBC_VERSION>
  LIBGCC_VERSION:=<LIBGCC_VERSION>
endif

CONFIG_PACKAGE_base-files$(TARGET):=$(CONFIG_PACKAGE_base-files)

define Package/base-files$(TARGET)
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=Base filesystem for OpenWrt
  DESCRIPTION:=\
	This package contains a base filesystem and system scripts for OpenWrt.
  URL:=http://openwrt.org/
  VERSION:=$(PKG_RELEASE)-$(REV)
  $(call Config,network.lan.proto,string,static,LAN Protocol)
  $(call Config,network.lan.ipaddr,ip,192.168.1.1,LAN IP Address)
  $(call Config,network.lan.netmask,netmask,255.255.0.0,LAN Network Mask)
  $(call Config,network.lan.gateway,ip,,LAN Gateway)
  $(call Config,network.lan.dns,ip,,LAN DNS server)
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
  CATEGORY:=Base system
  DEPENDS:=@!NATIVE_TOOLCHAIN
  TITLE:=GCC support library
  URL:=http://gcc.gnu.org/
  VERSION:=$(LIBGCC_VERSION)-$(PKG_RELEASE)
endef

define Package/libpthread
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=@!NATIVE_TOOLCHAIN
  TITLE:=POSIX thread library
  URL:=http://uclibc.org/
  VERSION:=$(UCLIBC_VERSION)-$(PKG_RELEASE)
endef

define Package/uclibc
  SECTION:=libs
  CATEGORY:=Base system
  DEPENDS:=@!NATIVE_TOOLCHAIN
  TITLE:=C library embedded systems
  URL:=http://uclibc.org/
  VERSION:=$(UCLIBC_VERSION)-$(PKG_RELEASE)
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile/Default
endef

define Build/Compile
	$(call Build/Compile/Default)
endef

define Package/base-files$(TARGET)/install
	$(CP) ./files/* $(1)/
	if [ -d $(PLATFORM_DIR)/base-files/default/. ]; then \
		$(CP) $(PLATFORM_DIR)/base-files/default/* $(1)/; \
	fi
	if [ "$(PROFILE)" != "Default" ]; then \
		if [ -d $(PLATFORM_DIR)/base-files/profile-$(PROFILE)/. ]; then \
			$(CP) $(PLATFORM_DIR)/base-files/profile-$(PROFILE)/* $(1)/; \
		fi; \
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
	ln -sf /proc/mounts $(1)/etc/mtab
	rm -f $(1)/var
	ln -sf /tmp $(1)/var
	mkdir -p $(1)/etc
	ln -sf /tmp/resolv.conf $(1)/etc/resolv.conf
	$(call Package/base-files/install-target,$(1))
	for conffile in $(1)/etc/config/*; do \
		if [ -f "$$$$conffile" ]; then \
			grep "$$$$conffile" $(1)/CONTROL/conffiles || \
				echo "$$$${conffile##$(1)}" >> $(1)/CONTROL/conffiles; \
		fi \
	done
endef

define Package/libgcc/install
	$(INSTALL_DIR) $(1)/lib
	$(CP) $(STAGING_DIR)/lib/libgcc_s.so.* $(1)/lib/
endef

define Package/libpthread/install
	$(INSTALL_DIR) $(1)/lib
	$(CP) $(STAGING_DIR)/lib/libpthread.so.* $(1)/lib/
	$(CP) $(STAGING_DIR)/lib/libpthread-$(UCLIBC_VERSION).so $(1)/lib/
endef

define Package/uclibc/install
	$(INSTALL_DIR) $(1)/lib
	for file in ld-uClibc libc libcrypt libdl libm libnsl libresolv librt libuClibc libutil; do \
		$(CP) $(STAGING_DIR)/lib/$$$$file.so.* $(1)/lib/; \
		$(CP) $(STAGING_DIR)/lib/$$$$file-$(UCLIBC_VERSION).so $(1)/lib/; \
	done
endef

ifneq ($(DUMP),1)
  -include $(PLATFORM_DIR)/base-files.mk
endif

$(eval $(call BuildPackage,base-files$(TARGET)))
$(eval $(call BuildPackage,libgcc))
$(eval $(call BuildPackage,libpthread))
$(eval $(call BuildPackage,uclibc))


