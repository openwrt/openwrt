# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=pwc
PKG_VERSION:=10.0.12-rc1
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.saillard.org/linux/pwc/files/
PKG_MD5SUM:=8763f3d6fd0f9738ef9854de205a126d
PKG_CAT:=bzcat

include $(INCLUDE_DIR)/package.mk

define Package/kmod-pwc
  SECTION:=kernel
  CATEGORY:=Kernel drivers
  DEPENDS:=@LINUX_2_6
  TITLE:=USB Philips camera driver
  URL:=http://www.saillard.org/linux/pwc/
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef

define Build/Compile
	$(MAKE) -C $(LINUX_DIR) \
		PATH="$(TARGET_PATH)" \
                KVER="$(LINUX_VERSION)" \
                KSRC="$(LINUX_DIR)" \
                SUBDIRS="$(PKG_BUILD_DIR)" \
                KERNEL_DIR="$(KERNEL_DIR)" \
                ARCH="$(LINUX_KARCH)" \
                CROSS_COMPILE="$(TARGET_CROSS)" \
		modules
endef

define Package/kmod-pwc/install
	mkdir -p $(1)/etc/modules.d
	$(INSTALL_DATA) ./files/pwc.modules $(1)/etc/modules.d/70-pwc
	$(INSTALL_DIR) $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_BUILD_DIR)/pwc.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
endef

$(eval $(call BuildPackage,kmod-pwc))
