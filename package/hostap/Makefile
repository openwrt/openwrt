# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=hostap-driver
PKG_VERSION:=0.4.9
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://hostap.epitest.fi/releases/
PKG_MD5SUM:=c7534dc040ab90218257a78488ecd378
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk
ifeq ($(DUMP),)
  -include $(LINUX_DIR)/.config
endif

define KernelPackage/hostap/Default/2.4
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef
define KernelPackage/hostap/Default/2.6
  VERSION:=$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef
define KernelPackage/hostap/Default
  SUBMENU:=Wireless Drivers
  DESCRIPTION:=\
	Host AP is a driver for 802.11b wireless cards based on Intersil \\\
	Prism2/2.5/3 chipset. It supports so called Host AP mode that allows the \\\
	card to act as an IEEE 802.11 access point.
  URL:=http://hostap.epitest.fi/
  $(call KernelPackage/hostap/Default/$(KERNEL))
endef


define KernelPackage/hostap
  $(call KernelPackage/hostap/Default)
  DEPENDS:=@PCI_SUPPORT||PCMCIA_SUPPORT +kmod-ieee80211 +kmod-crypto
  TITLE:=Host AP support for Prism2/2.5/3
  DESCRIPTION+=\\\
	\\\
	This package contains the base Host AP driver code that is shared by \\\
	different hardware models. You will also need to enable support for \\\
	PLX/PCI/CS version of the driver to actually use the driver.
endef
define KernelPackage/hostap/2.4
  DEPENDS:=+kmod-crypto
  FILES:= \
	$(PKG_BUILD_DIR)/driver/modules/hostap.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/driver/modules/hostap_crypt_ccmp.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/driver/modules/hostap_crypt_tkip.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/driver/modules/hostap_crypt_wep.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,hostap hostap_crypt_wep hostap_crypt_tkip hostap_crypt_ccmp)
endef
define KernelPackage/hostap/2.6
  KCONFIG:=$(CONFIG_HOSTAP)
  FILES:= \
	$(MODULES_DIR)/kernel/drivers/net/wireless/hostap/hostap.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,hostap)
endef


define KernelPackage/hostap-pci
  $(call KernelPackage/hostap/Default)
  DEPENDS:=kmod-hostap
  TITLE:=Host AP driver for PCI adaptors
  DESCRIPTION+=\\\
	\\\
	This package contains the Host AP driver for Prism2.5 PCI adaptors.
  AUTOLOAD:=$(call AutoLoad,60,hostap_pci)
endef
define KernelPackage/hostap-pci/2.4
  FILES:= \
	$(PKG_BUILD_DIR)/driver/modules/hostap_pci.$(LINUX_KMOD_SUFFIX)
endef
define KernelPackage/hostap-pci/2.6
  KCONFIG:=$(CONFIG_HOSTAP_PCI)
  FILES:= \
	$(MODULES_DIR)/kernel/drivers/net/wireless/hostap/hostap_pci.$(LINUX_KMOD_SUFFIX)
endef


define KernelPackage/hostap-plx
  $(call KernelPackage/hostap/Default)
  DEPENDS:=kmod-hostap
  TITLE:=Host AP driver for PLX9052 based PCI adaptors
  DESCRIPTION+=\\\
	\\\
	This package contains the Host AP driver for Prism2/2.5/3 in PLX9052 \\\
	based PCI adaptors.
  AUTOLOAD:=$(call AutoLoad,60,hostap_plx)
endef
define KernelPackage/hostap-plx/2.4
  FILES:= \
	$(PKG_BUILD_DIR)/driver/modules/hostap_plx.$(LINUX_KMOD_SUFFIX)
endef
define KernelPackage/hostap-plx/2.6
  KCONFIG:=$(CONFIG_HOSTAP_PLX)
  FILES:= \
	$(MODULES_DIR)/kernel/drivers/net/wireless/hostap/hostap_plx.$(LINUX_KMOD_SUFFIX)
endef


ifeq ($(KERNEL),2.4)
  define Build/Compile
	$(MAKE) -C $(LINUX_DIR)/ \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		SUBDIRS="$(PKG_BUILD_DIR)/driver/modules" \
		modules
  endef

  define Package/kmod-hostap-pci/install
	$(INSTALL_DIR) $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) \
		$(1)/lib/modules/$(LINUX_VERSION)/
  endef

  define Package/kmod-hostap-plx/install
	$(INSTALL_DIR) $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) \
		$(PKG_BUILD_DIR)/driver/modules/hostap_plx.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
  endef
else
  define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
  endef
  
  define Build/Configure
  endef
  
  define Build/Compile
  endef
endif


$(eval $(call KernelPackage,hostap))
$(eval $(call KernelPackage,hostap-pci))
$(eval $(call KernelPackage,hostap-plx))
