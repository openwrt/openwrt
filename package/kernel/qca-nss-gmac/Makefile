#
# Copyright (c) 2015 The Linux Foundation. All rights reserved.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=qca-nss-gmac
PKG_VERSION:=20150210
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=git://codeaurora.org/quic/qsdk/oss/lklm/nss-gmac
PKG_SOURCE_PROTO:=git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=969aab0aff40a3a5afe70d0f00e783b1c57aaea6

PKG_LICENSE:=ISC

include $(INCLUDE_DIR)/package.mk

define KernelPackage/qca-nss-gmac
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=@TARGET_ipq806x
  TITLE:=Kernel driver for NSS gmac
  FILES:=$(PKG_BUILD_DIR)/ipq806x/qca-nss-gmac.ko
  AUTOLOAD:=$(call AutoProbe,qca-nss-gmac)
endef

define KernelPackage/qca-nss-gmac/Description
This package contains a NSS driver for QCA chipset
endef

EXTRA_CFLAGS+= \
	-DCONFIG_NSS_DEBUG_LEVEL=4 \
	-I$(PKG_BUILD_DIR)/nss_hal/include \
	-I$(PKG_BUILD_DIR)/nss_hal/$(BOARD)

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		modules
endef

$(eval $(call KernelPackage,qca-nss-gmac))
