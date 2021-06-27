include $(TOPDIR)/rules.mk

PKG_NAME:=rtl8723bs-ol
PKG_RELEASE=2

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=

PKG_SOURCE_URL:=https://github.com/openlumi/rtl8723bs.git
PKG_MIRROR_HASH:=91eb42ffbb032f71ca8054700b7f2a215366e1247c9d1a08e8a4512351e737c7
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2020-04-14
PKG_SOURCE_VERSION:=8171031d9dd3d795b73b3fcfc296b810aa791d31

PKG_MAINTAINER:=Ivan Belokobylskiy <belokobylskij@gmail.com>
PKG_BUILD_PARALLEL:=1

STAMP_CONFIGURED_DEPENDS := $(STAGING_DIR)/usr/include/mac80211-backport/backport/autoconf.h

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/rtl8723bs-ol
  SUBMENU:=Wireless Drivers
  TITLE:=Realtek RTL8723BS SDIO WiFi support (out-of-tree)
  DEPENDS:=+kmod-mac80211 +kmod-cfg80211 +@DRIVER_11N_SUPPORT
  FILES:=\
	$(PKG_BUILD_DIR)/rtl8723bs.ko
  AUTOLOAD:=$(call AutoProbe,rtl8723bs)
  PROVIDES:=kmod-rtl8723bs
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

NOSTDINC_FLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT -DCONFIG_CONCURRENT_MODE -DBUILD_OPENWRT

define Build/Compile
	rm -f $(PKG_BUILD_DIR)/include/linux/wireless.h
	+$(MAKE) $(PKG_JOBS) -C "$(LINUX_DIR)" \
		$(KERNEL_MAKE_FLAGS) \
		M="$(PKG_BUILD_DIR)" \
		NOSTDINC_FLAGS="$(NOSTDINC_FLAGS)" \
		CONFIG_RTL8723BS=m \
		USER_MODULE_NAME=rtl8723bs \
		modules
endef

$(eval $(call KernelPackage,rtl8723bs-ol))
