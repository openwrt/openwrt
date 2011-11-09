#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=madwifi

PKG_REV:=3314
PKG_VERSION:=r$(PKG_REV)
PKG_RELEASE:=5

PKG_SOURCE_PROTO:=svn
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=$(if $(PKG_BRANCH),$(PKG_BRANCH),madwifi-trunk)-$(PKG_VERSION)
PKG_SOURCE_URL:=http://madwifi-project.org/svn/madwifi/$(if $(PKG_BRANCH),branches/$(PKG_BRANCH),trunk)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(if $(PKG_BRANCH),$(PKG_BRANCH),madwifi-trunk)-$(PKG_VERSION)

HAL_VERSION:=20090508
HAL_FILE:=ath_hal-$(HAL_VERSION).tgz
HAL_MD5SUM:=4ab7ae8bdb96c0be388c98bf8f92d5ca

PKG_BUILD_DEPENDS:=wprobe

include $(INCLUDE_DIR)/package.mk

ifdef CONFIG_MADWIFI_COMPRESSION
  COMPRESSION:=1
else
  COMPRESSION:=0
endif

define Download/hal
  FILE:=$(HAL_FILE)
  URL:=http://mirror2.openwrt.org/sources
  MD5SUM:=$(HAL_MD5SUM)
endef
$(eval $(call Download,hal))


ifneq ($(CONFIG_TARGET_atheros),)
  BUS:=AHB
else
  ifneq ($(CONFIG_PCI_SUPPORT),)
    BUS:=PCI
  endif
endif

ifneq ($(CONFIG_CPU_MIPS32_R2),)
  ifeq ($(ARCH),mips)
    HAL_TARGET:=mips32r2-be-elf
  endif
  ifeq ($(ARCH),mipsel)
    HAL_TARGET:=mips32r2-le-elf
  endif
else
  ifeq ($(ARCH),mips)
    HAL_TARGET:=mips32-be-elf
  endif
  ifeq ($(ARCH),mipsel)
    HAL_TARGET:=mips32-le-elf
  endif
endif
ifeq ($(ARCH),i386)
  HAL_TARGET:=i386-elf
endif
ifeq ($(ARCH),i686)
  HAL_TARGET:=i386-elf
endif
ifeq ($(ARCH),armeb)
  HAL_TARGET:=xscale-be-elfgnueabi
endif
ifeq ($(ARCH),arm)
  HAL_TARGET:=xscale-le-elfgnueabi
  ifeq ($(BOARD),cns21xx)
    HAL_TARGET:=armv4-le-elfgnueabi
  endif
  ifeq ($(BOARD),cns3xxx)
    HAL_TARGET:=arm11-le-elfgnueabi
  endif
  ifeq ($(BOARD),gemini)
    HAL_TARGET:=armv4-le-elfgnueabi
  endif
endif
ifeq ($(ARCH),powerpc)
  HAL_TARGET:=powerpc-be-elf
endif
ifneq ($(CONFIG_TARGET_atheros),)
  HAL_TARGET:=wisoc
endif

ifdef CONFIG_MADWIFI_RCA_MINSTREL
  RATE_CONTROL:=minstrel
endif

ifdef CONFIG_MADWIFI_RCA_ONOE
  RATE_CONTROL:=onoe
endif

ifdef CONFIG_MADWIFI_RCA_AMRR
  RATE_CONTROL:=amrr
endif

ifdef CONFIG_MADWIFI_RCA_SAMPLERATE
  RATE_CONTROL:=sample
endif

ifneq ($(CONFIG_MADWIFI_SINGLE_MODULE),)
MADWIFI_FILES:= $(PKG_BUILD_DIR)/ath_hal/ath_hal.ko
else
MADWIFI_FILES:= \
	$(PKG_BUILD_DIR)/net80211/wlan.ko \
	$(PKG_BUILD_DIR)/net80211/wlan_scan_ap.ko \
	$(PKG_BUILD_DIR)/net80211/wlan_scan_sta.ko \
	$(PKG_BUILD_DIR)/ath_hal/ath_hal.ko \
	$(PKG_BUILD_DIR)/ath_rate/$(RATE_CONTROL)/ath_rate_$(RATE_CONTROL).ko \
	$(PKG_BUILD_DIR)/net80211/wlan_acl.ko \
	$(PKG_BUILD_DIR)/net80211/wlan_ccmp.ko \
	$(PKG_BUILD_DIR)/net80211/wlan_tkip.ko \
	$(PKG_BUILD_DIR)/net80211/wlan_wep.ko \
	$(PKG_BUILD_DIR)/net80211/wlan_xauth.ko
endif

ifneq ($(CONFIG_MADWIFI_SINGLE_MODULE),)
  MADWIFI_AUTOLOAD:= ath_hal
else
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
	wlan_xauth
endif

ifeq ($(findstring AHB,$(BUS)),AHB)
  MADWIFI_FILES+= $(PKG_BUILD_DIR)/ath/ath_ahb.ko
  MADWIFI_AUTOLOAD+= ath_ahb
endif
ifeq ($(findstring PCI,$(BUS)),PCI)
  MADWIFI_FILES+= $(PKG_BUILD_DIR)/ath/ath_pci.ko
  MADWIFI_AUTOLOAD+= ath_pci
endif

MADWIFI_APPLETS:=80211stats athchans athkey athstats wlanconfig ath_info madwifi_multi
ifdef CONFIG_MADWIFI_DEBUG
  MADWIFI_APPLETS += athdebug 80211debug
endif

define KernelPackage/madwifi
  SUBMENU:=Wireless Drivers
  TITLE:=Driver for Atheros wireless chipsets
  URL:=http://madwifi-project.org/
  MAINTAINER:=Felix Fietkau <nbd@openwrt.org>
  DEPENDS:=+wireless-tools @PCI_SUPPORT @(!(TARGET_avr32||TARGET_cobalt||TARGET_ep93xx||TARGET_etrax||TARGET_octeon||TARGET_pxcab||TARGET_sibyte||LINUX_3_1||LINUX_3_2)||BROKEN) +@DRIVER_WEXT_SUPPORT
  FILES:=$(MADWIFI_FILES)
  AUTOLOAD:=$(call AutoLoad,50,$(MADWIFI_AUTOLOAD))
  MENU:=1
endef

define KernelPackage/madwifi/description
 This package contains a driver for Atheros 802.11a/b/g chipsets.
endef

define KernelPackage/madwifi/config
	source "$(SOURCE)/Config.in"
endef

MADWIFI_INC = \
	-I$(PKG_BUILD_DIR) \
	-I$(PKG_BUILD_DIR)/include \
	-I$(PKG_BUILD_DIR)/hal \
	-I$(PKG_BUILD_DIR)/ath \
	-I$(PKG_BUILD_DIR)/ath_hal \
	-I$(PKG_BUILD_DIR)/net80211 \
	-I$(STAGING_DIR)/usr/include/wprobe \
	-include $(PKG_BUILD_DIR)/include/compat.h

MAKE_ARGS:= \
	PATH="$(TARGET_PATH)" \
	ARCH="$(LINUX_KARCH)" \
	ARCH-y="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	TARGET="$(HAL_TARGET)" \
	TOOLPREFIX="$(KERNEL_CROSS)" \
	TOOLPATH="$(KERNEL_CROSS)" \
	KERNELPATH="$(LINUX_DIR)" \
	LDOPTS="--no-warn-mismatch " \
	ATH_RATE="$(RATE_CONTROL)" \
	ATH_CAP_SUPERG_COMP="$(COMPRESSION)" \
	DO_MULTI=1 \
	SINGLE_MODULE=$(if $(CONFIG_MADWIFI_SINGLE_MODULE),1) \
	INCS="$(MADWIFI_INC)" \
	$(if $(CONFIG_MADWIFI_DEBUG),,DEBUG=) WARNINGS="-Wno-unused"

MAKE_VARS:= \
	COPTS="-DATH_REVERSE_ENGINEERING=1" \

define Build/Prepare/HAL
	rm -rf $(PKG_BUILD_DIR)/tmp
	mkdir -p $(PKG_BUILD_DIR)/tmp
	tar xvzf $(DL_DIR)/$(HAL_FILE) -C $(PKG_BUILD_DIR)/tmp
	$(CP) $(PKG_BUILD_DIR)/tmp/ath_hal*/* $(PKG_BUILD_DIR)/hal/
	rm -rf $(PKG_BUILD_DIR)/tmp
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(call Build/Prepare/HAL)
	# patch cflags
	$(SED) 's, -E[LB],,' \
		-e 's, -mips2,,' \
		-e 's, -mapcs-32,,' \
		-e 's, -mlong-calls,,' \
		$(PKG_BUILD_DIR)/hal/public/*.inc
	$(SED) 's,march=armv4,march=armv5te,' \
		$(PKG_BUILD_DIR)/hal/public/xscale*.inc
endef

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
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) ./files/* $(1)/
	$(CP) $(foreach applet,$(MADWIFI_APPLETS),$(PKG_BUILD_DIR)/tools/$(applet)) $(1)/usr/sbin/
endef

$(eval $(call KernelPackage,madwifi))
