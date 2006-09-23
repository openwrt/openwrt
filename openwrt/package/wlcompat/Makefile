# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kmod-wlcompat
PKG_RELEASE:=4

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/kmod-wlcompat/Default
  SECTION:=kernel
  CATEGORY:=Kernel drivers
  DEPENDS:=kmod-brcm-wl
  TITLE:=Broadcom wl wrapper module
  DESCRIPTION:=\
	This package contains a wrapper module, that provides Wireless Extension \\\
	support for the proprietary Broadcom wl module.
  VERSION:=$(LINUX_VERSION)+$(BOARD)-$(PKG_RELEASE)
endef

define Package/kmod-wlcompat
  $(call Package/kmod-wlcompat/Default)
  DEFAULT:=y
endef

define Package/kmod-wlcompat-debug
  $(call Package/kmod-wlcompat/Default)
  TITLE+= (debug)
  DESCRIPTION+=\\\
	\\\
	This is a debugging version.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="-DDEBUG -I$(LINUX_DIR)/arch/mips/bcm947xx/include -I$(PKG_BUILD_DIR)/include" \
		modules
	mv $(PKG_BUILD_DIR)/wlcompat.o $(PKG_BUILD_DIR)/wlcompat-debug.o
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="-I$(LINUX_DIR)/arch/mips/bcm947xx/include -I$(PKG_BUILD_DIR)/include" \
		modules
endef

define Package/kmod-wlcompat/install
	install -d -m0755 $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_BUILD_DIR)/wlcompat.o $(1)/lib/modules/$(LINUX_VERSION)/
	install -d -m0755 $(1)/etc/modules.d
	echo "wlcompat" > $(1)/etc/modules.d/30-wlcompat
endef

define Package/kmod-wlcompat-debug/install
	install -d -m0755 $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_BUILD_DIR)/wlcompat-debug.o $(1)/lib/modules/$(LINUX_VERSION)/
endef

$(eval $(call BuildPackage,kmod-wlcompat))
$(eval $(call BuildPackage,kmod-wlcompat-debug))
