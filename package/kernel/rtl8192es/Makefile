include $(TOPDIR)/rules.mk

PKG_NAME:=rtl8192es
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/Mange/rtl8192eu-linux-driver.git
PKG_SOURCE_DATE:=2026-07-18
PKG_SOURCE_VERSION:=da21d44a40690fe0df21f5603a972abd3cbb311d
PKG_MIRROR_HASH:=528d99e65e1a87f7e6c0e50d2159ef17f172443d03931ba5e216f192035d2d95

PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=COPYING
PKG_MAINTAINER:=Dmitry Sinyavin <sinyavin@gmail.com>
PKG_BUILD_PARALLEL:=1

STAMP_CONFIGURED_DEPENDS:=$(STAGING_DIR)/usr/include/mac80211-backport/backport/autoconf.h

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/rtl8192es
  SUBMENU:=Wireless Drivers
  TITLE:=Realtek RTL8192ES SDIO full-MAC driver
  DEPENDS:=@TARGET_mdm9607 +kmod-mmc +kmod-cfg80211 +mf283v-wlan-data
  FILES:=$(PKG_BUILD_DIR)/8192es.ko
endef

define KernelPackage/rtl8192es/description
  GPL Realtek full-MAC driver for the RTL8192ES SDIO WLAN device found in the
  ZTE MF283V. The maintained RTL8192E/cfg80211 core is combined with the GPL
  RTL8192ES SDIO transport from Realtek's v5.3.5 driver family.
endef

NOSTDINC_FLAGS:= \
	$(KERNEL_NOSTDINC_FLAGS) \
	-I$(STAGING_DIR)/usr/include/mac80211-backport \
	-I$(STAGING_DIR)/usr/include/mac80211-backport/uapi \
	-I$(STAGING_DIR)/usr/include/mac80211 \
	-I$(STAGING_DIR)/usr/include/mac80211/uapi \
	-include backport/backport.h \
	-DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT -DBUILD_OPENWRT \
	-DCONFIG_RTW_IOCTL_SET_COUNTRY \
	-DCONFIG_RTW_MF283V_CFG80211_REGULATORY \
	-DCONFIG_RTW_MF283V_PRIMARY_WDEV \
	-DCONFIG_RTW_MF283V_CALIBRATION \
	-DMF283V_CAL_PATH=\\\"/tmp/mf283v-wlan/calibration\\\" \
	-DCONFIG_RTW_DEBUG -DRTW_LOG_LEVEL=2 \
	-DHAL_MAC_ENABLE=1 -DHAL_BB_ENABLE=1 -DHAL_RF_ENABLE=1

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) ./src/. $(PKG_BUILD_DIR)/
endef

define Build/Compile
	+$(KERNEL_MAKE) $(PKG_JOBS) \
		M="$(PKG_BUILD_DIR)" \
		NOSTDINC_FLAGS="$(NOSTDINC_FLAGS)" \
		USER_EFUSE_MAP_PATH="/tmp/mf283v-wlan/efuse.map" \
		USER_WIFIMAC_PATH="/tmp/mf283v-wlan/macaddr" \
		modules
endef

$(eval $(call KernelPackage,rtl8192es))
