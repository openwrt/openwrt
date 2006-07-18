# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 2480 2005-11-14 02:07:33Z nbd $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=broadcom-wl
PKG_VERSION:=4.80.9.2
PKG_RELEASE:=1
WLC_VERSION:=0.1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://downloads.openwrt.org/sources
PKG_MD5SUM:=7d9fab2c611369bbe075c3924ea77450
PKG_CAT:=bzcat

include $(INCLUDE_DIR)/package.mk

define Package/kmod-brcm-wl
  SECTION:=drivers
  CATEGORY:=Drivers
  DEPENDS:=@LINUX_2_4_BRCM
  DEFAULT:=y
  SUBMENU:=Proprietary BCM43xx WiFi driver
  TITLE:=Kernel driver (normal version)
  DESCRIPTION:=Proprietary Wireless driver for the Broadcom BCM43xx chipset
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(PKG_RELEASE)
endef

define Package/kmod-brcm-wl-mimo
  $(call Package/kmod-brcm-wl)
  DEFAULT:=m if ALL
  TITLE:=Kernel driver (MIMO version)
  DESCRIPTION:=Proprietary Wireless driver for the Broadcom BCM43xx chipset (MIMO version)
endef

define Package/wlc
  $(call Package/kmod-brcm-wl-mimo)
  DEFAULT:=y
  DEPENDS:=kmod-brcm-wl
  TITLE:=Setup utility
  DESCRIPTION:=Utility for initializing the Broadcom wl driver
  VERSION:=$(WLC_VERSION)-$(PKG_RELEASE)
endef

define Package/wl
  $(call Package/wlc)
  TITLE:=Utility for changing the driver's parameters
  DESCRIPTION:=Utility for changing the Broadcom wl driver's parameters
  VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
endef

define Package/nas
  $(call Package/wl)
  DEPENDS:=+nvram
  DEFAULT:=m
  TITLE:=Proprietary WPA/WPA2 authenticator
  DESCRIPTION:=Proprietary WPA/WPA2 authenticator for the Broadcom wl driver
endef

define Build/Prepare
  $(call Build/Prepare/Default)
	$(CP) src/* $(PKG_BUILD_DIR)/
endef

MAKEFLAGS_KMOD = -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		PATH="$(TARGET_PATH)" \
		SUBDIRS="$(PKG_BUILD_DIR)/kmod"


define Build/Compile
	# Compile the kernel part
	$(MAKE) $(MAKEFLAGS_KMOD) \
		modules
	$(MAKE) $(MAKEFLAGS_KMOD) MOD_NAME="_mimo" \
		modules

	# Compile wlc
	$(MAKE) -C $(PKG_BUILD_DIR)/wlc \
		$(TARGET_CONFIGURE_OPTS) \
		CC="$(TARGET_CC)" \
		CFLAGS="-I$(PKG_BUILD_DIR)/wlc/include $(TARGET_CFLAGS)" \
		all
endef

define wl_template
	install -d -m0755 $(1)/etc/modules.d
	echo "wl$(2)" > $(1)/etc/modules.d/20-wl$(2)
	install -d -m0755 $(1)/lib/modules/$(LINUX_VERSION)
	install -m0644 $(PKG_BUILD_DIR)/kmod/wl$(2).o $(1)/lib/modules/$(LINUX_VERSION)/
endef

define Package/kmod-brcm-wl/install
	$(call wl_template,$(1))
endef

define Package/kmod-brcm-wl-mimo/install
	$(call wl_template,$(1),_mimo)
endef

define Package/wlc/install
	$(CP) ./files/* $(1)/
	install -d -m0755 $(1)/sbin
	install -m0755 $(PKG_BUILD_DIR)/wlc/wlc $(1)/sbin/
endef

define Package/wl/install
	install -d -m0755 $(1)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/wl $(1)/usr/sbin/
endef

define Package/nas/install
	install -d -m0755 $(1)/usr/lib
	install -m0755 $(PKG_BUILD_DIR)/libbcmcrypto.so $(1)/usr/lib/
	install -d -m0755 $(1)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/nas $(1)/usr/sbin/
	ln -sf nas $(1)/usr/sbin/nas4not
	ln -sf nas $(1)/usr/sbin/nas4wds
endef

$(eval $(call BuildPackage,kmod-brcm-wl))
$(eval $(call BuildPackage,kmod-brcm-wl-mimo))
$(eval $(call BuildPackage,wlc))
$(eval $(call BuildPackage,wl))
$(eval $(call BuildPackage,nas))
