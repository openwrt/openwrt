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

define Package/kmod-hostap/Default
  SECTION:=kernel
  CATEGORY:=Kernel drivers
  DESCRIPTION:=\
	Host AP is a driver for 802.11b wireless cards based on Intersil \\\
	Prism2/2.5/3 chipset. It supports so called Host AP mode that allows the \\\
	card to act as an IEEE 802.11 access point.
  URL:=http://hostap.epitest.fi/
  ifeq ($(KERNEL),2.6)
    VERSION:=$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE)
  else
    VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
  endif
endef

define Package/kmod-hostap
  $(call Package/kmod-hostap/Default)
  DEPENDS:=@PCI_SUPPORT||PCMCIA_SUPPORT
  TITLE:=Host AP support for Prism2/2.5/3
  DESCRIPTION+=\\\
	\\\
	This package contains the base Host AP driver code that is shared by \\\
	different hardware models. You will also need to enable support for \\\
	PLX/PCI/CS version of the driver to actually use the driver.
endef

define Package/kmod-hostap-pci
  $(call Package/kmod-hostap/Default)
  DEPENDS:=kmod-hostap
  TITLE:=Host AP driver for PCI adaptors
  DESCRIPTION+=\\\
	\\\
	This package contains the Host AP driver for Prism2.5 PCI adaptors.
endef

define Package/kmod-hostap-plx
  $(call Package/kmod-hostap/Default)
  DEPENDS:=kmod-hostap
  TITLE:=Host AP driver for PLX9052 based PCI adaptors
  DESCRIPTION+=\\\
	\\\
	This package contains the Host AP driver for Prism2/2.5/3 in PLX9052 \\\
	based PCI adaptors.
endef

ifeq ($(KERNEL),2.4)
  define Build/Compile
	$(MAKE) -C $(LINUX_DIR)/ \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		SUBDIRS="$(PKG_BUILD_DIR)/driver/modules" \
		modules
  endef

  define Package/kmod-hostap/install
	install -m0755 -d $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) \
		$(PKG_BUILD_DIR)/driver/modules/hostap.$(LINUX_KMOD_SUFFIX) \
		$(PKG_BUILD_DIR)/driver/modules/hostap_crypt_ccmp.$(LINUX_KMOD_SUFFIX) \
		$(PKG_BUILD_DIR)/driver/modules/hostap_crypt_tkip.$(LINUX_KMOD_SUFFIX) \
		$(PKG_BUILD_DIR)/driver/modules/hostap_crypt_wep.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
  endef

  define Package/kmod-hostap-pci/install
	install -m0755 -d $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) \
		$(PKG_BUILD_DIR)/driver/modules/hostap_pci.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
  endef

  define Package/kmod-hostap-plx/install
	install -m0755 -d $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) \
		$(PKG_BUILD_DIR)/driver/modules/hostap_plx.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
  endef
endif

ifeq ($(KERNEL),2.6)
  define Build/Compile
  endef
  
ifeq ($(CONFIG_HOSTAP),m)
  define Package/kmod-hostap/install
	install -m0755 -d $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) \
		$(MODULES_DIR)/kernel/drivers/net/wireless/hostap/hostap.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
  endef
endif

ifeq ($(CONFIG_HOSTAP_PCI),m)
  define Package/kmod-hostap-pci/install
	install -m0755 -d $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) \
		$(MODULES_DIR)/kernel/drivers/net/wireless/hostap/hostap_pci.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
  endef
endif

ifeq ($(CONFIG_HOSTAP_PLX),m)
  define Package/kmod-hostap-plx/install
	install -m0755 -d $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) \
		$(MODULES_DIR)/kernel/drivers/net/wireless/hostap/hostap_plx.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
  endef
endif
endif

$(eval $(call BuildPackage,kmod-hostap))
$(eval $(call BuildPackage,kmod-hostap-pci))
$(eval $(call BuildPackage,kmod-hostap-plx))
