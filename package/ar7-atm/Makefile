# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id:$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=sangam-atm
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://downloads.openwrt.org/sources
PKG_MD5SUM:=90c10702c9c3129e2ad1c01ce3975bf5
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/kmod-sangam-atm-annex-a
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=@LINUX_2_4_AR7 +kmod-atm
  DEFAULT:=y
  TITLE:=AR7 ADSL driver (Annex A)
  DESCRIPTION:=The AR7 ADSL driver for Annex A
  VERSION:=$(PKG_VERSION)+$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef

define Package/kmod-sangam-atm-annex-b
  $(call Package/kmod-sangam-atm-annex-a)
  TITLE:=AR7 ADSL driver (Annex B)
  DESCRIPTION:=The AR7 ADSL driver for Annex B
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		modules
endef

define install
	install -m0755 -d $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_BUILD_DIR)/tiatm.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)
	install -m0644 $(PKG_BUILD_DIR)/ar0700xx_a.bin $(1)/lib/modules/ar0700xx.bin
	install -m0755 -d $(1)/etc/modules.d/
	printf 'tiatm\n' > $(1)/etc/modules.d/20-switch
endef

define Package/kmod-sangam-atm-annex-a/install
	$(call install,$(1),a)
endef

define Package/kmod-sangam-atm-annex-b/install
	$(call install,$(1),b)
endef

$(eval $(call BuildPackage,kmod-sangam-atm-annex-a))
$(eval $(call BuildPackage,kmod-sangam-atm-annex-b))
