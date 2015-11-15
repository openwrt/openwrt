#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ath10k-firmware
PKG_SOURCE_VERSION:=69f955c3dd95d97c7c30960aa5c9852c80b85cc3
PKG_VERSION:=2014-11-13-$(PKG_SOURCE_VERSION)
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=https://github.com/kvalo/ath10k-firmware.git

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>

include $(INCLUDE_DIR)/package.mk

WMENU:=Wireless Drivers

define Package/ath10k-firmware-default
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=$(WMENU)
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=kmod-ath10k
endef

define Package/ath10k-firmware-qca988x
$(Package/ath10k-firmware-default)
  DEFAULT:=PACKAGE_kmod-ath10k
  TITLE:=ath10k firmware for QCA988x devices
endef

define Package/ath10k-firmware-qca99x0
$(Package/ath10k-firmware-default)
  TITLE:=ath10k firmware for QCA99x0 devices
endef

define Package/ath10k-firmware-qca6174
$(Package/ath10k-firmware-default)
  TITLE:=ath10k firmware for QCA6174 devices
endef

define Build/Compile

endef

define Package/ath10k-firmware-qca988x/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA988X/hw2.0
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/QCA988X/board.bin \
		$(1)/lib/firmware/ath10k/QCA988X/hw2.0/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/QCA988X/10.2.4/firmware-5.bin_10.2.4.70.12-2 \
		$(1)/lib/firmware/ath10k/QCA988X/hw2.0/firmware-5.bin
endef

define Package/ath10k-firmware-qca6174/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k
	$(CP) $(PKG_BUILD_DIR)/ath10k/QCA6174 $(1)/lib/firmware/ath10k/
endef

define Package/ath10k-firmware-qca99x0/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA99X0/hw2.0
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/QCA99X0/hw2.0/boardData_AR900B_CUS239_5G_v2_001.bin \
		$(1)/lib/firmware/ath10k/QCA99X0/hw2.0/board.bin
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/QCA99X0/hw2.0/firmware-5.bin_10.4.1.00030-1 \
		$(1)/lib/firmware/ath10k/QCA99X0/hw2.0/firmware-5.bin
endef

$(eval $(call BuildPackage,ath10k-firmware-qca988x))
$(eval $(call BuildPackage,ath10k-firmware-qca99x0))
$(eval $(call BuildPackage,ath10k-firmware-qca6174))
