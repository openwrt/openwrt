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
PKG_VERSION:=2.4.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.openswan.org/download
PKG_MD5SUM:=70f22e8adc39e07a165f75eccb7cd079

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

define KernelPackage/openswan
  SUBMENU:=Network Support
  $(call Package/openswan/Default)
  TITLE+= (kernel module)
  DESCRIPTION+=\\\
	\\\
	This package contains the Openswan kernel module.
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
  FILES:=$(PKG_BUILD_DIR)/modobj*/ipsec.$(LINUX_KMOD_SUFFIX)
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
