include $(TOPDIR)/rules.mk

PKG_NAME:=mt76
PKG_VERSION:=2015-03-11
PKG_RELEASE=1

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=

PKG_SOURCE_URL:=https://github.com/openwrt/mt76
PKG_SOURCE_PROTO:=git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=a94e01211aed8eac39331bc030fd3474357f05e0
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.xz

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/mt76
  SUBMENU:=Wireless Drivers
  TITLE:=MediaTek MT76x2 wireless driver
  DEPENDS:=+kmod-mac80211 +@DRIVER_11N_SUPPORT @PCI_SUPPORT
  FILES:=$(PKG_BUILD_DIR)/mt76pci.ko
  AUTOLOAD:=$(call AutoLoad,50,mac80211 mt76pci)
endef

NOSTDINC_FLAGS = \
	-I$(PKG_BUILD_DIR) \
	-I$(STAGING_DIR)/usr/include/mac80211-backport/uapi \
	-I$(STAGING_DIR)/usr/include/mac80211-backport \
	-I$(STAGING_DIR)/usr/include/mac80211/uapi \
	-I$(STAGING_DIR)/usr/include/mac80211 \
	-include backport/backport.h

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
	$(CP) $(PKG_BUILD_DIR)/firmware/mt7662_patch_e3_hdr_v0.0.2_P48.bin $(1)/lib/firmware/mt7662_rom_patch.bin
	$(CP) $(PKG_BUILD_DIR)/firmware/mt7662_firmware_e3_v1.5.bin $(1)/lib/firmware/mt7662.bin
endef

$(eval $(call KernelPackage,mt76))
