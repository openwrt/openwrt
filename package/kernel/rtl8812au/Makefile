include $(TOPDIR)/rules.mk

PKG_NAME:=rtl8812au-aircrack
PKG_RELEASE=1

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=

PKG_SOURCE_URL:=https://github.com/aircrack-ng/rtl8812au.git
PKG_MIRROR_HASH:=7c595fafa1330a9fe7526062b29a09ddef18f92540301f4427b29f8db0a13f52
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2021-11-10
PKG_SOURCE_VERSION:=307d694076b056588c652c2bdaa543a89eb255d9

PKG_MAINTAINER:=Janpieter Sollie <janpieter.sollie@edpnet.be>
PKG_BUILD_PARALLEL:=1

STAMP_CONFIGURED_DEPENDS := $(STAGING_DIR)/usr/include/mac80211-backport/backport/autoconf.h

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/rtl8812au-aircrack
  SUBMENU:=Wireless Drivers
  TITLE:=Aircrack Driver for Realtek 8812 AU USB devices
  DEPENDS:=+kmod-cfg80211 +kmod-usb-core +@DRIVER_11N_SUPPORT +@DRIVER_11AC_SUPPORT
  FILES:=\
	$(PKG_BUILD_DIR)/88XXau.ko
  AUTOLOAD:=$(call AutoProbe,rtl8812au-aircrack)
  PROVIDES:=kmod-rtl8812au-aircrack
endef
NOSTDINC_FLAGS := \
        $(KERNEL_NOSTDINC_FLAGS) \
        -I$(PKG_BUILD_DIR) \
        -I$(PKG_BUILD_DIR)/include \
        -I$(STAGING_DIR)/usr/include/mac80211-backport \
        -I$(STAGING_DIR)/usr/include/mac80211-backport/uapi \
        -I$(STAGING_DIR)/usr/include/mac80211 \
        -I$(STAGING_DIR)/usr/include/mac80211/uapi \
        -include backport/backport.h

NOSTDINC_FLAGS+=-DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT -DBUILD_OPENWRT

define Build/Compile
        +$(MAKE) $(PKG_JOBS) -C "$(LINUX_DIR)" \
                $(KERNEL_MAKE_FLAGS) \
                M="$(PKG_BUILD_DIR)" \
                NOSTDINC_FLAGS="$(NOSTDINC_FLAGS)" \
                modules
endef

$(eval $(call KernelPackage,rtl8812au-aircrack))
