include $(TOPDIR)/rules.mk

PKG_NAME:=rtl8852cu
PKG_RELEASE=1

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=Rudy Andram <rmandrad@gmail.com>
PKG_BUILD_PARALLEL:=1

STAMP_CONFIGURED_DEPENDS := $(STAGING_DIR)/usr/include/mac80211-backport/backport/autoconf.h
PKG_EXTMOD_SUBDIRS:=rtl8852cu

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/rtl8852cu
  SUBMENU:=Wireless Drivers
  TITLE:=Driver for Realtek 8852 USB devices 
  DEPENDS:=+kmod-cfg80211 +kmod-usb-core +@DRIVER_11AC_SUPPORT +@DRIVER_11AX_SUPPORT
  FILES:=\
	$(PKG_BUILD_DIR)/8852cu.ko
  AUTOLOAD:=$(call AutoProbe,8852cu)
  PROVIDES:=kmod-rtl8852cu
endef


NOSTDINC_FLAGS1 :=-DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT -DBUILD_OPENWRT \
	-I$(PKG_BUILD_DIR)/include \
	-I$(PKG_BUILD_DIR)/platform

define Build/Compile
	$(MAKE) ARCH=arm64 CROSS_COMPILE=/home/rmandrad/rmandradopenwrt-nss-ax9000-6.1/staging_dir/toolchain-aarch64_cortex-a53_gcc-12.3.0_musl/bin/aarch64-openwrt-linux-musl- -C /home/rmandrad/rmandradopenwrt-nss-ax9000-6.1/build_dir/target-aarch64_cortex-a53_musl/linux-qualcommax_ipq807x/linux-6.1.46/ M=/home/rmandrad/rmandradopenwrt-nss-ax9000-6.1/build_dir/target-aarch64_cortex-a53_musl/linux-qualcommax_ipq807x/rtl8852cu/ NOSTDINC_FLAGS="$(NOSTDINC_FLAGS1)" modules
endef


$(eval $(call KernelPackage,rtl8852cu))

