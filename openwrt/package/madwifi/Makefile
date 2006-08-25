# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=madwifi
PKG_VERSION:=0.9.0
PKG_RELEASE:=1
PKG_MD5SUM:=9b558d1df84cab2b0006c595feee0fba

PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

ifeq ($(ARCH),mips)
  HAL_TARGET:=mips-be-elf
endif
ifeq ($(ARCH),mipsel)
  HAL_TARGET:=mips-le-elf
endif
ifeq ($(ARCH),i386)
  HAL_TARGET:=i386-elf
endif
ifeq ($(ARCH),armeb)
  HAL_TARGET:=xscale-be-elf
endif
ifeq ($(ARCH),powerpc)
  HAL_TARGET:=powerpc-be-elf
endif

BUS := PCI
ifneq ($(CONFIG_LINUX_2_4_AR531X),)
  BUS := AHB
endif
ifneq ($(CONFIG_LINUX_2_6_ARUBA),)
  BUS := PCI AHB	# no suitable HAL for AHB yet.
endif

BUS_MODULES:=
ifeq ($(findstring AHB,$(BUS)),AHB)
  BUS_MODULES+=$(PKG_BUILD_DIR)/ath/ath_ahb.$(LINUX_KMOD_SUFFIX)
endif
ifeq ($(findstring PCI,$(BUS)),PCI)
  BUS_MODULES+=$(PKG_BUILD_DIR)/ath/ath_pci.$(LINUX_KMOD_SUFFIX)
endif

MADWIFI_MAKEOPTS= -C $(PKG_BUILD_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		CC="$(TARGET_CC)" \
		TARGET="$(HAL_TARGET)" \
		TOOLPREFIX="$(KERNEL_CROSS)" \
		TOOLPATH="$(KERNEL_CROSS)" \
		KERNELPATH="$(LINUX_DIR)" \
		LDOPTS="" \
		DOMULTI=1

include $(INCLUDE_DIR)/package.mk

ifeq ($(findstring AHB,$(BUS)),AHB)
define Build/Compile/ahb
	$(MAKE) $(MADWIFI_MAKEOPTS) BUS="AHB" all
endef
endif

ifeq ($(findstring PCI,$(BUS)),PCI)
define Build/Compile/pci
	$(MAKE) $(MADWIFI_MAKEOPTS) BUS="PCI" all
endef
endif

define Build/Compile
$(call Build/Compile/ahb)
$(call Build/Compile/pci)
endef

define Build/InstallDev
	mkdir -p $(STAGING_DIR)/usr/include/madwifi
	$(CP) $(PKG_BUILD_DIR)/include $(STAGING_DIR)/usr/include/madwifi/
	mkdir -p $(STAGING_DIR)/usr/include/madwifi/net80211
	$(CP) $(PKG_BUILD_DIR)/net80211/*.h $(STAGING_DIR)/usr/include/madwifi/net80211/
endef

define Build/UninstallDev
	rm -rf $(STAGING_DIR)/usr/include/madwifi
endef

define Package/kmod-madwifi
 SECTION:=drivers
 CATEGORY:=Drivers
 DEFAULT:=y if LINUX_2_6_BRCM || LINUX_2_6_ARUBA || LINUX_2_4_AR531X || LINUX_2_6_XSCALE, m if ALL
 TITLE:=Driver for Atheros wireless chipsets
 DESCRIPTION:=Driver for Atheros 802.11a/b/g chipsets
 URL:=http://madwifi.org/
 VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef

define Package/kmod-madwifi/install
	mkdir -p $(1)/etc/modules.d
	mkdir -p $(1)/etc/init.d
	mkdir -p $(1)/lib/modules/$(LINUX_VERSION)
	mkdir -p $(1)/usr/sbin
	install -m0644 ./files/madwifi.modules $(1)/etc/modules.d/20-madwifi
ifeq ($(findstring AHB,$(BUS)),AHB)
	echo ath_ahb >> $(1)/etc/modules.d/20-madwifi
endif
ifeq ($(findstring PCI,$(BUS)),PCI)
	echo ath_pci >> $(1)/etc/modules.d/20-madwifi
endif
	install -m0755 ./files/madwifi.init $(1)/etc/init.d/S20madwifi
	$(CP) \
		$(PKG_BUILD_DIR)/ath_hal/ath_hal.$(LINUX_KMOD_SUFFIX) \
		$(BUS_MODULES) \
		$(PKG_BUILD_DIR)/ath_rate/sample/ath_rate_sample.$(LINUX_KMOD_SUFFIX) \
		$(PKG_BUILD_DIR)/net80211/*.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
	$(CP) $(PKG_BUILD_DIR)/tools/{madwifi_multi,80211debug,80211stats,athchans,athctrl,athdebug,athkey,athstats,wlanconfig} $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,kmod-madwifi))
