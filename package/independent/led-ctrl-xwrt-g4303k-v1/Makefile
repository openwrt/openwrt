#
# Copyright (C) 2017-2019 Chen Minqiang <ptpt52@gmail.com>
# Copyright (C) 2019 X-WRT <dev@x-wrt.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=led-ctrl-xwrt-g4303k-v1
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_MAINTAINER:=X-WRT <dev@x-wrt.com>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/led-ctrl-xwrt-g4303k-v1/Default
  SECTION:=Applications
  CATEGORY:=Independent
  TITLE:=led-ctrl for xwrt_g4303k-v1 board
  DEPENDS:=@(TARGET_ramips_mt76x8_DEVICE_xwrt_g4303k-v1||TARGET_DEVICE_ramips_mt76x8_DEVICE_xwrt_g4303k-v1)
endef

define Package/led-ctrl-xwrt-g4303k-v1
  $(call Package/led-ctrl-xwrt-g4303k-v1/Default)
endef

define Package/led-ctrl-xwrt-g4303k-v1/description
  Standalone packages for xwrt_g4303k-v1 board
endef

define Package/led-ctrl-xwrt-g4303k-v1/conffiles
endef

define Package/led-ctrl-xwrt-g4303k-v1/postinst
endef

define Package/led-ctrl-xwrt-g4303k-v1/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/led-ctrl.init $(1)/etc/init.d/led-ctrl
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) ./files/led-ctrl.sh $(1)/sbin/led-ctrl
endef

define Build/Configure
  true
endef

define Build/Prepare
  true
endef

define Build/Compile
  true
endef

$(eval $(call BuildPackage,led-ctrl-xwrt-g4303k-v1))
