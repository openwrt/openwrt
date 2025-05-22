include $(TOPDIR)/rules.mk

PKG_NAME:=rtl8723bs-ol
PKG_RELEASE=1

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=

PKG_SOURCE_URL:=https://github.com/openlumi/rtl8723bs.git
PKG_MIRROR_HASH:=0c9f707eee0c4a15042111c41d9c98da74923373e7187489d5019022702a090b
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2025-05-02
PKG_SOURCE_VERSION:=d500e58db9191b6e4054f37563e9111c7ac0c1e8

PKG_MAINTAINER:=Ivan Belokobylskiy <belokobylskij@gmail.com>
PKG_BUILD_PARALLEL:=1

STAMP_CONFIGURED_DEPENDS := $(STAGING_DIR)/usr/include/mac80211-backport/backport/autoconf.h

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/rtl8723bs-ol
  SUBMENU:=Wireless Drivers
  TITLE:=Realtek RTL8723BS SDIO WiFi support (out-of-tree)
  DEPENDS:=+kmod-mac80211 +kmod-cfg80211 +kmod-mmc +@DRIVER_11N_SUPPORT
  FILES:=\
	$(PKG_BUILD_DIR)/rtl8723bs.ko
  AUTOLOAD:=$(call AutoProbe,rtl8723bs)
  PROVIDES:=kmod-rtl8723bs
endef
define KernelPackage/rtl8723bs-ol/config
  if PACKAGE_kmod-rtl8723bs-ol

	config PACKAGE_RTL8723BS-OL_CONCURRENT
		bool "Enable concurrent mode"
		default y
  endif
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

NOSTDINC_FLAGS += \
		-DCONFIG_TRAFFIC_PROTECT \
		-DCONFIG_IOCTL_CFG80211 \
		-DRTW_USE_CFG80211_STA_EVENT \
		-DBUILD_OPENWRT \
  $(if $(CONFIG_PACKAGE_RTL8723BS-OL_CONCURRENT),-DCONFIG_CONCURRENT_MODE,-DRTW_P2P_GROUP_INTERFACE=1 -DRTW_DEDICATED_P2P_DEVICE=0)

ifeq ($(CONFIG_BIG_ENDIAN),y)
NOSTDINC_FLAGS+= -DCONFIG_BIG_ENDIAN
endif
ifneq ($(CONFIG_BIG_ENDIAN),y)
NOSTDINC_FLAGS+= -DCONFIG_LITTLE_ENDIAN
endif

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
