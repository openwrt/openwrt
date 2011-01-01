#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=crda
PKG_RELEASE:=1
PKG_VERSION:=1.1.1
PKG_SOURCE_URL:=http://wireless.kernel.org/download/crda
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_MD5SUM:=5fc77af68b3e21736b8ef2f8b061c810
PKG_BUILD_DEPENDS:=mac80211

PKG_REGULATORY_NAME:=regulatory
PKG_REGULATORY_VERSION:=2010.11.24
PKG_REGULATORY_SOURCE_URL:=http://wireless.kernel.org/download/wireless-regdb/regulatory.bins
PKG_REGULATORY_SOURCE:=$(PKG_REGULATORY_VERSION)-$(PKG_REGULATORY_NAME).bin
PKG_REGULATORY_MD5SUM:=001b69bba19486439031377e09229f9c

include $(INCLUDE_DIR)/package.mk

define Package/crda
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Central Regulatory Domain Agent (CRDA)
  DEPENDS:=+libnl-tiny
  URL:=http://wireless.kernel.org/en/developers/Regulatory/CRDA
endef

define Download/wireless-regdb
  FILE:=$(PKG_REGULATORY_SOURCE)
  URL:=$(PKG_REGULATORY_SOURCE_URL)
  VERSION:=$(PKG_REGULATORY_VERSION)
  MD5SUM:=$(PKG_REGULATORY_MD5SUM)
endef
$(eval $(call Download,wireless-regdb))

define Package/crda/description
 This is the Central Regulatory Domain Agent for Linux. It serves one
 purpose: tell Linux kernel what to enforce. In essence it is a udev
 helper for communication between the kernel and userspace. You only
 need to run this manually for debugging purposes. For manual changing
 of regulatory domains use iw (iw reg set) or wpa_supplicant (feature
 yet to be added).
endef

TARGET_CPPFLAGS := \
	-I$(STAGING_DIR)/usr/include/libnl-tiny \
	-D_GNU_SOURCE \
	$(TARGET_CPPFLAGS)

MAKE_FLAGS += \
	NL1FOUND="" NL2FOUND=Y \
	NLLIBNAME="libnl-tiny" \
	NLLIBS="-lnl-tiny -lm" \
	REG_BIN="$(DL_DIR)/$(PKG_REGULATORY_SOURCE)" \
	crda

define Package/crda/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_DIR) $(1)/etc/hotplug.d
	$(INSTALL_DIR) $(1)/etc/hotplug.d/platform
	$(INSTALL_DIR) $(1)/usr/lib/crda
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/crda $(1)/sbin/
	$(INSTALL_DATA) ./files/hotplug.rule $(1)/etc/hotplug.d/platform/10-regulatory
	$(INSTALL_DATA) $(DL_DIR)/$(PKG_REGULATORY_SOURCE) $(1)/usr/lib/crda/regulatory.bin
endef

$(eval $(call BuildPackage,crda))

