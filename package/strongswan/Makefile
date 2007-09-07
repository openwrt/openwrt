# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=strongswan
PKG_VERSION:=2.8.2
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://download.strongswan.org/
PKG_MD5SUM:=57427f5b48123851a73b10d78dd4f8d6

include $(INCLUDE_DIR)/package.mk

define Package/strongswan/Default
  TITLE:=strongSwan
  DEPENDS:=@LINUX_2_4
  URL:=http://www.strongswan.org/
endef

define Package/strongswan/Default/description
 strongSwan is an IPsec implementation for Linux.
endef

define Package/strongswan
$(call Package/strongswan/Default)
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+kmod-strongswan +libgmp @LINUX_2_4
  TITLE+= (daemon)
endef

define Package/strongswan/description
$(call Package/strongswan/Default/description)
 This package contains the strongSwan user-land daemon.
endef

define KernelPackage/strongswan
$(call Package/strongswan/Default)
  SUBMENU:=Network Support
  TITLE+= (kernel module)
  DEPENDS:=@LINUX_2_4
  FILES:=$(PKG_BUILD_DIR)/linux/net/ipsec/ipsec.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,ipsec)
endef

define KernelPackage/strongswan/description
$(call Package/strongswan/Default/description)
 This package contains the strongSwan kernel module.
endef

PKG_MAKE_OPTS:= \
		LINUX_RELEASE="$(LINUX_RELEASE)" \
		KERNELSRC="$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		USERCOMPILE="$(TARGET_CFLAGS) -I./linux/include $(TARGET_CPPFLAGS) $(TARGET_LDFLAGS)" \
		IPSECDIR="/usr/lib/ipsec" \
		INC_USRLOCAL="/usr" \

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		$(PKG_MAKE_OPTS) \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		programs module install
endef

define Package/strongswan/install
	$(CP) $(PKG_INSTALL_DIR)/* $(1)
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/cron.tick
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DIR) $(1)/etc/hotplug.d/button
	$(INSTALL_BIN) ./files/ipsec.init   $(1)/etc/init.d/ipsec
	$(INSTALL_BIN) ./files/ipsec.cron   $(1)/etc/cron.tick/ipsec-wakeup
	$(INSTALL_BIN) ./files/ipsec.iface  $(1)/etc/hotplug.d/iface/65-ipsec
	$(INSTALL_BIN) ./files/ipsec.button $(1)/etc/hotplug.d/button/65-ipsec
	$(INSTALL_DATA) ./files/ipsec.config $(1)/etc/config/ipsec
	$(INSTALL_DATA) ./files/ipsec.conf $(1)/etc/ipsec.conf
	rm -rf $(1)/usr/share
	rm -rf $(1)/usr/man
	rm -rf $(1)/var
	rm -rf $(1)/etc/rc.d
	find $(1) -name \*.old | xargs rm -rf
endef

$(eval $(call BuildPackage,strongswan))
$(eval $(call KernelPackage,strongswan))
