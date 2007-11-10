#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
# 
# $Id: $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=p54
PKG_RELEASE:=1

PKG_FWP54_NAME:=2.7.0.0.arm
PKG_FWP54_URL:=http://prism54.org/firmware
PKG_FWP54_MD5SUM:=09f9da7ea757173c9de1a0322a1f9782

include $(INCLUDE_DIR)/package.mk

define KernelPackage/p54/Default
  SUBMENU:=Wireless Drivers
  TITLE:=Prism Wireless Support (mac80211)
  DEPENDS:=+kmod-mac80211
endef

define KernelPackage/p54-common
$(call KernelPackage/p54/Default)
  TITLE+= (COMMON)
  DEPENDS+= @PCI_SUPPORT||USB_SUPPORT
  FILES:=$(PKG_BUILD_DIR)/p54common.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,p54common)
endef

define KernelPackage/p54-pci
$(call KernelPackage/p54/Default)
  TITLE+= (PCI)
  DEPENDS+= @PCI_SUPPORT +kmod-p54-common
  FILES:=$(PKG_BUILD_DIR)/p54pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,31,p54pci)
endef

define KernelPackage/p54-usb
$(call KernelPackage/p54/Default)
  TITLE+= (USB)
  DEPENDS+= @USB_SUPPORT +kmod-usb-core +kmod-p54-common
  FILES:=$(PKG_BUILD_DIR)/p54usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,31,p54usb)
endef

define KernelPackage/p54/description
  Kernel module for Prism54 chipsets (mac80211)
endef

$(STAMP_PREPARED): $(DL_DIR)/$(PKG_FWP54_NAME)

$(DL_DIR)/$(PKG_FWP54_NAME):
	$(SCRIPT_DIR)/download.pl "$(DL_DIR)" "$(PKG_FWP54_NAME)" "$(PKG_FWP54_MD5SUM)" $(PKG_FWP54_URL)

PKG_EXTRA_KCONFIG:= \
	CONFIG_P54_COMMON=m \
	
  ifneq ($(CONFIG_PACKAGE_kmod-p54-pci),)
    PKG_EXTRA_KCONFIG+= CONFIG_P54_PCI=m
  endif
  ifneq ($(CONFIG_PACKAGE_kmod-p54-usb),)
    PKG_EXTRA_KCONFIG+= CONFIG_P54_USB=m
  endif

EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(PKG_EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(PKG_EXTRA_KCONFIG)))) \

MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	LINUXINCLUDE="-I$(STAGING_DIR)/usr/include/mac80211 -I$(LINUX_DIR)/include -include linux/autoconf.h" \
	$(PKG_EXTRA_KCONFIG)

ifneq ($(findstring 2.6.23,$(LINUX_VERSION)),)
  define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	$(CP) $(DL_DIR)/$(PKG_FWP54_NAME) $(PKG_BUILD_DIR) 
  endef

  define Build/Configure
  endef

  define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
  endef
else
  override CONFIG_PACKAGE_kmod-p54-common=
  override CONFIG_PACKAGE_kmod-p54-pci=
  override CONFIG_PACKAGE_kmod-p54-usb=
endif

define KernelPackage/p54-pci/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/2.7.0.0.arm $(1)/lib/firmware/isl3886
endef

define KernelPackage/p54-usb/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/2.7.0.0.arm $(1)/lib/firmware/isl3886
endef


$(eval $(call KernelPackage,p54-common))
$(eval $(call KernelPackage,p54-pci))
$(eval $(call KernelPackage,p54-usb))
