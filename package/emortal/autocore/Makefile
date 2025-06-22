# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2020 Lean <coolsnowwolf@gmail.com>
# Copyright (C) 2021-2023 ImmortalWrt.org

include $(TOPDIR)/rules.mk

PKG_NAME:=autocore
PKG_FLAGS:=nonshared
PKG_RELEASE:=42

PKG_CONFIG_DEPENDS:= \
	CONFIG_TARGET_bcm27xx \
	CONFIG_TARGET_bcm53xx \
	CONFIG_TARGET_x86

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/target.mk

define Package/autocore
  TITLE:=auto core loadbalance script.
  DEPENDS:=@(aarch64||arm||i386||i686||x86_64) \
    +TARGET_bcm27xx:bcm27xx-utils \
    +TARGET_bcm53xx:nvram \
    +TARGET_x86:ethtool \
    +TARGET_x86:lm-sensors
endef

define Build/Compile
endef

define Package/autocore/install
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/60-autocore-reload-rpcd $(1)/etc/uci-defaults/

ifneq ($(filter i386 i686 x86_64, $(ARCH)),)
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/autocore $(1)/etc/init.d/
endif

	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) ./files/cpuinfo $(1)/sbin/
ifneq ($(filter ipq% mediatek% qualcommax%, $(TARGETID)),)
	$(INSTALL_BIN) ./files/tempinfo $(1)/sbin/
endif

	$(INSTALL_DIR) $(1)/usr/share/rpcd/acl.d
	$(CP) ./files/luci-mod-status-autocore.json $(1)/usr/share/rpcd/acl.d/
endef

$(eval $(call BuildPackage,autocore))
