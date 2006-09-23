# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=openswan
PKG_VERSION:=2.4.6
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.openswan.org/download
PKG_MD5SUM:=b34d71ca49dedad017879b0e912d40dd
PKG_CAT:=zcat

PKG_INIT_PRIO:=60

include $(INCLUDE_DIR)/package.mk

define Package/openswan/Default
  TITLE:=Openswan
  DESCRIPTION:=\
	Openswan is an IPsec implementation for Linux.
  URL:=http://www.openswan.org/
endef

define Package/openswan
  $(call Package/openswan/Default)
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+kmod-openswan +libgmp
  TITLE+= (daemon)
  DESCRIPTION+=\\\
	\\\
	This package contains the Openswan user-land daemon.
  URL:=http://www.openswan.org/
endef

define Package/kmod-openswan
  $(call Package/openswan/Default)
  SECTION:=kernel
  CATEGORY:=Kernel drivers
  TITLE+= (kernel module)
  DESCRIPTION+=\\\
	\\\
	This package contains the Openswan kernel module.
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef

PKG_MAKE_OPTS:= \
		LINUX_RELEASE="$(LINUX_RELEASE)" \
		KERNELSRC="$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		USERCOMPILE="$(TARGET_CFLAGS) -I./linux/include -I$(STAGING_DIR)/usr/include -L$(STAGING_DIR)/usr/lib" \
		IPSECDIR="/usr/lib/ipsec" \
		INC_USRLOCAL="/usr" \

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		$(PKG_MAKE_OPTS) \
		module
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		$(PKG_MAKE_OPTS) \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		programs
endef

define Package/openswan/install
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		$(PKG_MAKE_OPTS) \
		DESTDIR="$(1)" \
		install
	install -d -m0755 $(1)/etc/init.d
	$(CP) $(1)/etc/rc.d/init.d/ipsec $(1)/etc/init.d/S$(PKG_INIT_PRIO)ipsec
	rm -rf $(1)/usr/share
	rm -rf $(1)/usr/man
	rm -rf $(1)/var
	rm -rf $(1)/etc/rc.d
	find $(1) -name \*.old | xargs rm -rf
endef

define Package/kmod-openswan/install
	mkdir -p $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_BUILD_DIR)/modobj*/ipsec.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
endef

$(eval $(call BuildPackage,openswan))
$(eval $(call BuildPackage,kmod-openswan))
