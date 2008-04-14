#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=aodv-uu
PKG_VERSION:=0.9.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://core.it.uu.se/core/files/
PKG_MD5SUM:=05460543054449cb4b170252a7168c65

include $(INCLUDE_DIR)/package.mk

define Package/aodv-uu/Default
  TITLE:=Ad-hoc On-demand Distance Vector Routing
  DEPENDS:=@BROKEN
  URL:=http://core.it.uu.se/core/index.php/AODV-UU
endef

define Package/aodv-uu/Default/description
 AODV is the Ad-hoc On-demand Distance Vector routing protocol
 implementation created at Uppsala University.
endef

define Package/aodv-uu
$(call Package/aodv-uu/Default)
  SECTION:=net
  CATEGORY:=Network
  TITLE+= (daemon)
endef

define Package/aodv-uu/description
$(call Package/aodv-uu/Default/description)
 This package contains the AODV userland daemon.
endef

define KernelPackage/aodv-uu
$(call Package/aodv-uu/Default)
  TITLE+= (kernel module)
  FILES:=$(PKG_BUILD_DIR)/lnx/kaodv.$(LINUX_KMOD_SUFFIX)
  SUBMENU:=Network Support
  AUTOLOAD:=$(call AutoLoad,80,$(shell cat ./files/aodv-uu.modules))
endef

define KernelPackage/aodv-uu/description
$(call Package/aodv-uu/Default/description)
 This package contains the AODV kernel module.
endef

ifeq ($(CONFIG_LINUX_2_6),y)
  define Build/Compile/linux26
	$(MAKE) -C $(LINUX_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		PATCHLEVEL="$(LINUX_VERSION)" \
		KERNDIR="$(LINUX_DIR)" \
		SUBDIRS="$(PKG_BUILD_DIR)/lnx" \
                modules
  endef
else
  # We assume 2.4 builds are only for brcm-2.4 yet
  define Build/Compile/linux24-brcm
	$(call Build/Compile/Default,\
		KERNEL_DIR="$(LINUX_DIR)" \
		KCC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		kaodv-mips \
	)
	cp $(PKG_BUILD_DIR)/lnx/kaodv-mips.$(LINUX_KMOD_SUFFIX) $(PKG_BUILD_DIR)/lnx/kaodv.$(LINUX_KMOD_SUFFIX)
  endef
endif

define Build/Compile
	$(call Build/Compile/linux26)
	$(call Build/Compile/linux24-brcm)
	$(call Build/Compile/Default,\
		KERNEL_DIR="$(LINUX_DIR)" \
		KCC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		aodvd \
	)
endef

define Package/aodv-uu/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/aodvd $(1)/usr/sbin
endef

define Package/kmod-aodv-uu/install
	$(INSTALL_DIR) $(1)/etc/modules.d
	$(INSTALL_DATA) ./files/aodv-uu.modules $(1)/etc/modules.d/80-aodv-uu
	$(INSTALL_DIR) $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_BUILD_DIR)/lnx/kaodv.$(LINUX_KMOD_SUFFIX) $(1)/lib/modules/$(LINUX_VERSION)/
endef

$(eval $(call BuildPackage,aodv-uu))
$(eval $(call KernelPackage,aodv-uu))
