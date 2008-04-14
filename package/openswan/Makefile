#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=openswan
PKG_VERSION:=2.4.10
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.openswan.org/download
PKG_MD5SUM:=2b36785342c74d524d8d86bde89a445f

include $(INCLUDE_DIR)/package.mk

define Package/openswan/Default
  TITLE:=Openswan
  URL:=http://www.openswan.org/
  DEPENDS:=@BROKEN
endef

define Package/openswan/Default/description
 Openswan is an IPsec implementation for Linux.
endef

define Package/openswan
$(call Package/openswan/Default)
  SECTION:=net
  CATEGORY:=Network
  DEPENDS+= +kmod-openswan +libgmp +ip
  TITLE+= (daemon)
  URL:=http://www.openswan.org/
endef

define Package/openswan/description
$(call Package/openswan/Default/description)
 This package contains the Openswan user-land daemon.
endef

define KernelPackage/openswan
$(call Package/openswan/Default)
  SUBMENU:=Network Support
  TITLE+= (kernel module)
  FILES:=$(PKG_BUILD_DIR)/modobj*/ipsec.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/openswan/description
$(call Package/openswan/Default/description)
 This package contains the Openswan kernel module.
endef

TARGET_CPPFLAGS = \
	-I$(STAGING_DIR)/usr/include \
	-I$(LINUX_DIR)/include

TARGET_LDFLAGS = \
	-L$(STAGING_DIR)/usr/lib

OPENSWAN_MAKE := $(MAKE) -C $(PKG_BUILD_DIR) \
	$(TARGET_CONFIGURE_OPTS) \
	LINUX_RELEASE="$(LINUX_RELEASE)" \
	KERNELSRC="$(LINUX_DIR)" \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	USERCOMPILE="$(TARGET_CFLAGS) -I$(PKG_BUILD_DIR)/linux/include $(TARGET_CPPFLAGS) $(TARGET_LDFLAGS)" \
	IPSECDIR="/usr/lib/ipsec" \
	INC_USRLOCAL="/usr" \
	INC_RCDEFAULT="/etc/init.d" \
	MODPROBE="/sbin/insmod" \
	LDFLAGS="$(TARGET_LDFLAGS)" \
	DESTDIR="$(PKG_INSTALL_DIR)"

define Build/Compile
	$(OPENSWAN_MAKE) \
		programs module install
endef

define Package/openswan/install
	$(CP) $(PKG_INSTALL_DIR)/* $(1)
	$(INSTALL_DIR) $(1)/etc/init.d
	$(CP) ./files/ipsec.init $(1)/etc/init.d/ipsec
	rm -rf $(1)/usr/share
	rm -rf $(1)/usr/man
	rm -rf $(1)/var
	rm -rf $(1)/etc/rc.d
	find $(1) -name \*.old | xargs rm -rf
endef

$(eval $(call BuildPackage,openswan))
$(eval $(call KernelPackage,openswan))
