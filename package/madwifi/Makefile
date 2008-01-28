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
PKG_REV:=3280
PKG_VERSION:=r$(PKG_REV)
# PKG_BRANCH:=madwifi-dfs
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=svn
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=$(if $(PKG_BRANCH),$(PKG_BRANCH),madwifi-trunk)-$(PKG_VERSION)
PKG_SOURCE_URL:=http://svn.madwifi.org/madwifi/$(if $(PKG_BRANCH),branches/$(PKG_BRANCH),trunk)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(if $(PKG_BRANCH),$(PKG_BRANCH),madwifi-trunk)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

# MADWIFI_DEBUG=1

ifeq ($(ARCH),mips)
  HAL_TARGET:=mips-be-elf
endif
ifeq ($(ARCH),mipsel)
  HAL_TARGET:=mips-le-elf
endif
ifeq ($(ARCH),i386)
  HAL_TARGET:=i386-elf
endif
ifeq ($(ARCH),i686)
  HAL_TARGET:=i386-elf
endif
ifeq ($(ARCH),armeb)
  HAL_TARGET:=xscale-be-elf
endif
ifeq ($(ARCH),arm)
  HAL_TARGET:=xscale-le-elf
endif
ifeq ($(ARCH),powerpc)
  HAL_TARGET:=powerpc-be-elf
endif

ifneq ($(CONFIG_TARGET_atheros),)
  BUS:=AHB
  HAL_TARGET:=ap51
else
  ifneq ($(CONFIG_PCI_SUPPORT),)
    BUS:=PCI
  endif
endif

# XXX: remove this check later when we have PCI support properly detected on all targets
ifneq ($(CONFIG_TARGET_uml),)
 BUS:=
endif

RATE_CONTROL:=minstrel

MADWIFI_FILES:= \
	$(PKG_BUILD_DIR)/net80211/wlan.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/net80211/wlan_scan_ap.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/net80211/wlan_scan_sta.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/ath_hal/ath_hal.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/ath_rate/$(RATE_CONTROL)/ath_rate_$(RATE_CONTROL).$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/net80211/wlan_acl.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/net80211/wlan_ccmp.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/net80211/wlan_tkip.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/net80211/wlan_wep.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/net80211/wlan_xauth.$(LINUX_KMOD_SUFFIX) \

MADWIFI_AUTOLOAD:= \
	wlan \
	wlan_scan_ap \
	wlan_scan_sta \
	ath_hal \
	ath_rate_$(RATE_CONTROL) \
	wlan_acl \
	wlan_ccmp \
	wlan_tkip \
	wlan_wep \
	wlan_xauth \

ifeq ($(findstring AHB,$(BUS)),AHB)
  MADWIFI_FILES+= $(PKG_BUILD_DIR)/ath/ath_ahb.$(LINUX_KMOD_SUFFIX)
  MADWIFI_AUTOLOAD+= ath_ahb
endif
ifeq ($(findstring PCI,$(BUS)),PCI)
  MADWIFI_FILES+= $(PKG_BUILD_DIR)/ath/ath_pci.$(LINUX_KMOD_SUFFIX)
  MADWIFI_AUTOLOAD+= ath_pci
endif

MADWIFI_APPLETS:=80211stats,athchans,athctrl,athkey,athstats,wlanconfig
ifneq ($(MADWIFI_DEBUG),)
  MADWIFI_APPLETS:=$(strip $(MADWIFI_APPLETS)),athdebug,80211debug
endif


define KernelPackage/madwifi
  SUBMENU:=Wireless Drivers
  TITLE:=Driver for Atheros wireless chipsets
  URL:=http://madwifi.org/
  DEPENDS:=+wireless-tools @PCI_SUPPORT||TARGET_atheros @!TARGET_avr32 @!TARGET_etrax @!TARGET_uml
  FILES:=$(MADWIFI_FILES)
  AUTOLOAD:=$(call AutoLoad,50,$(MADWIFI_AUTOLOAD))
endef

define KernelPackage/madwifi/description
 This package contains a driver for Atheros 802.11a/b/g chipsets.
endef

MAKE_ARGS:= \
	PATH="$(TARGET_PATH)" \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	TARGET="$(HAL_TARGET)" \
	TOOLPREFIX="$(KERNEL_CROSS)" \
	TOOLPATH="$(KERNEL_CROSS)" \
	KERNELPATH="$(LINUX_DIR)" \
	LDOPTS="--no-warn-mismatch " \
	ATH_RATE="ath_rate/$(RATE_CONTROL)" \
	WARNINGS="-Wno-unused" \
	$(if $(MADWIFI_DEBUG),,DEBUG="") \
	DOMULTI=1

MAKE_VARS:= \
	COPTS="-DCONFIG_ATHEROS_RATE_DEFAULT='\"$(RATE_CONTROL)\"'" \

ifeq ($(findstring AHB,$(BUS)),AHB)
  define Build/Compile/ahb
	$(MAKE_VARS) $(MAKE) -C $(PKG_BUILD_DIR) $(MAKE_ARGS) BUS="AHB" modules
	$(MAKE) -C $(PKG_BUILD_DIR) $(MAKE_ARGS) CFLAGS="$(TARGET_CFLAGS)" tools
  endef
endif

ifeq ($(findstring PCI,$(BUS)),PCI)
  define Build/Compile/pci
	$(MAKE_VARS) $(MAKE) -C $(PKG_BUILD_DIR) $(MAKE_ARGS) BUS="PCI" modules
	$(MAKE) -C $(PKG_BUILD_DIR) $(MAKE_ARGS) CFLAGS="$(TARGET_CFLAGS)" tools
  endef
endif

define Build/Configure
	$(SED) 's,-E[LB] ,,g' $(PKG_BUILD_DIR)/hal/public/*.inc
endef

define Build/Compile
	$(call Build/Compile/ahb)
	$(call Build/Compile/pci)
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include/madwifi
	$(CP) $(PKG_BUILD_DIR)/include $(1)/usr/include/madwifi/
	mkdir -p $(1)/usr/include/madwifi/net80211
	$(CP) $(PKG_BUILD_DIR)/net80211/*.h $(1)/usr/include/madwifi/net80211/
endef

define KernelPackage/madwifi/install
	$(INSTALL_DIR) $(1)/lib/wifi
	$(INSTALL_DATA) ./files/lib/wifi/madwifi.sh $(1)/lib/wifi
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/tools/{madwifi_multi,$(MADWIFI_APPLETS)} $(1)/usr/sbin/
endef

$(eval $(call KernelPackage,madwifi))
