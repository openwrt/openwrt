include $(TOPDIR)/rules.mk

PKG_NAME:=mt76
PKG_VERSION:=2016-03-04
PKG_RELEASE=1

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=

PKG_SOURCE_URL:=https://github.com/openwrt/mt76
PKG_SOURCE_PROTO:=git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=7059cc27df894a4b845b6deb7c51425ffb4b7c5c
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.xz

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/mt76
  SUBMENU:=Wireless Drivers
  TITLE:=MediaTek MT76x2/MT7603 wireless driver
  DEPENDS:=+kmod-mac80211 +@DRIVER_11N_SUPPORT @PCI_SUPPORT
  FILES:=\
	$(PKG_BUILD_DIR)/mt76.ko \
	$(PKG_BUILD_DIR)/mt76x2e.ko \
	$(PKG_BUILD_DIR)/mt7603e.ko
  AUTOLOAD:=$(call AutoLoad,50,mac80211 mt76 mt76x2e mt7603e)
endef

NOSTDINC_FLAGS = \
	-I$(PKG_BUILD_DIR) \
	-I$(STAGING_DIR)/usr/include/mac80211-backport/uapi \
	-I$(STAGING_DIR)/usr/include/mac80211-backport \
	-I$(STAGING_DIR)/usr/include/mac80211/uapi \
	-I$(STAGING_DIR)/usr/include/mac80211 \
	-include backport/autoconf.h \
	-include backport/backport.h

ifdef CONFIG_PACKAGE_MAC80211_MESH
  NOSTDINC_FLAGS += -DCONFIG_MAC80211_MESH
endif

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C "$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		NOSTDINC_FLAGS="$(NOSTDINC_FLAGS)" \
		modules
endef

define KernelPackage/mt76/install
	$(INSTALL_DIR) $(1)/lib/firmware
	cp \
		$(if $(CONFIG_TARGET_ramips_mt7628), \
			$(PKG_BUILD_DIR)/firmware/mt7628_e1.bin \
			$(PKG_BUILD_DIR)/firmware/mt7628_e2.bin \
		) \
		$(PKG_BUILD_DIR)/firmware/mt7603_e1.bin \
		$(PKG_BUILD_DIR)/firmware/mt7603_e2.bin \
		$(PKG_BUILD_DIR)/firmware/mt7662_rom_patch.bin \
		$(PKG_BUILD_DIR)/firmware/mt7662.bin \
		$(1)/lib/firmware
endef

$(eval $(call KernelPackage,mt76))
