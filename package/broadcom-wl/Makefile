#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=broadcom-wl
PKG_VERSION:=4.150.10.5.3
PKG_RELEASE:=7
WLC_VERSION:=0.2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://downloads.openwrt.org/sources
PKG_MD5SUM:=cc615fd49719eea8bce5b2a8813783f3

WL_WEXT=1

include $(INCLUDE_DIR)/package.mk

define Package/broadcom-wl/Default
  SECTION:=kernel
  CATEGORY:=Kernel modules
  DEPENDS:=@PACKAGE_kmod-brcm-wl||PACKAGE_kmod-brcm-wl-mimo
  SUBMENU:=Proprietary BCM43xx WiFi driver
  SUBMENUDEP:=@TARGET_brcm_2_4
endef

define KernelPackage/brcm-wl/Default
  $(call Package/broadcom-wl/Default)
  SECTION:=kernel
  DEPENDS:=@TARGET_brcm_2_4 +wireless-tools
  TITLE:=Kernel driver for BCM43xx chipsets
  FILES:=$(PKG_BUILD_DIR)/driver/wl$(1).o
  AUTOLOAD:=$(call AutoLoad,30,wl$(1))
endef

define KernelPackage/brcm-wl/Default/description
 This package contains the proprietary wireless driver for the Broadcom 
 BCM43xx chipset.
endef

define KernelPackage/brcm-wl
$(call KernelPackage/brcm-wl/Default,)
  TITLE+= (normal version)
endef

define KernelPackage/brcm-wl/description
$(call KernelPackage/brcm-wl/Default/description)
endef

define KernelPackage/brcm-wl-mimo
$(call KernelPackage/brcm-wl/Default,_mimo)
  TITLE+= (MIMO version)
endef

define KernelPackage/brcm-wl-mimo/description
$(call KernelPackage/brcm-wl/Default/description)
endef

define KernelPackage/wlcompat/Default
  $(call KernelPackage/brcm-wl/Default,)
  TITLE:=Kernel driver for BCM43xx chipsets
  FILES:=$(PKG_BUILD_DIR)/wlcompat/wlcompat$(1).o
endef

define KernelPackage/wlcompat
$(call KernelPackage/wlcompat/Default,)
  AUTOLOAD:=$(call AutoLoad,50,wlcompat)
endef

define KernelPackage/wlcompat/description
 This package contains a wrapper module, that provides Wireless Extension 
 support for the proprietary Broadcom wl module.
endef

define KernelPackage/wlcompat-debug
$(call KernelPackage/wlcompat/Default,-debug)
  TITLE+= (debug)
  AUTOLOAD:=
endef

define KernelPackage/wlcompat-debug/description
$(call KernelPackage/wlcompat/description)
 This is the debugging version.
endef

define Package/wlc
$(call Package/broadcom-wl/Default)
  TITLE:=wl driver setup utility
endef

define Package/wlc/description
 This package contains an utility for initializing the proprietary Broadcom 
 wl driver.
endef

define Package/wl
$(call Package/broadcom-wl/Default)
  TITLE:=Proprietary Broadcom wl driver config utility
endef

define Package/wl/description
 This package contains the proprietary utility (wl) for configuring the 
 proprietary Broadcom wl driver.
endef

define Package/nas
$(call Package/broadcom-wl/Default)
  TITLE:=Proprietary Broadcom WPA/WPA2 authenticator
endef

define Package/nas/description
 This package contains the proprietary WPA/WPA2 authenticator (nas) for the 
 proprietary Broadcom wl driver.
endef

MAKE_KMOD := $(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		PATH="$(TARGET_PATH)" \
		SUBDIRS="$(PKG_BUILD_DIR)/kmod"

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	# Compile the kernel part
	$(MAKE_KMOD) \
		SUBDIRS="$(PKG_BUILD_DIR)/driver" \
		modules
	$(MAKE_KMOD) \
		SUBDIRS="$(PKG_BUILD_DIR)/driver" \
		MOD_NAME="_mimo" \
		modules
	$(MAKE_KMOD) \
		SUBDIRS="$(PKG_BUILD_DIR)/wlcompat" \
		$(if $(WL_WEXT),WL_WEXT=1) \
		modules
	$(MAKE_KMOD) \
		SUBDIRS="$(PKG_BUILD_DIR)/wlcompat" \
		DEBUG=1 \
		$(if $(WL_WEXT),WL_WEXT=1) \
		modules

	# NVRAM stub
	$(TARGET_CC) $(TARGET_CFLAGS) -c -o $(PKG_BUILD_DIR)/nvram/nvram_stub.o $(PKG_BUILD_DIR)/nvram/nvram_stub.c

	# Compile wlc
	$(MAKE) -C $(PKG_BUILD_DIR)/wlc \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		all

	# Compile libshared
	$(MAKE) -C $(PKG_BUILD_DIR)/router/shared \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I. -I$(PKG_BUILD_DIR)/include -Dlinux=1" \
		all
	$(TARGET_CC) -o $(PKG_BUILD_DIR)/nas \
		$(PKG_BUILD_DIR)/nas_exe.o \
		$(PKG_BUILD_DIR)/nvram/nvram_stub.o \
		$(TARGET_LDFLAGS) \
		$(PKG_BUILD_DIR)/router/shared/libshared.a
	$(TARGET_CC) -o $(PKG_BUILD_DIR)/wl $(PKG_BUILD_DIR)/wl_exe.o
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/router/shared/libshared.a $(1)/usr/lib/
endef

define Package/wlc/install
	$(CP) ./files/* $(1)/
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wlc/wlc $(1)/sbin/
endef

define Package/wl/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wl $(1)/usr/sbin/
endef

define Package/nas/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/nas $(1)/usr/sbin/
	ln -sf nas $(1)/usr/sbin/nas4not
	ln -sf nas $(1)/usr/sbin/nas4wds
endef

$(eval $(call KernelPackage,brcm-wl))
$(eval $(call KernelPackage,brcm-wl-mimo))
$(eval $(call KernelPackage,wlcompat))
$(eval $(call KernelPackage,wlcompat-debug))
$(eval $(call BuildPackage,wlc))
$(eval $(call BuildPackage,wl))
$(eval $(call BuildPackage,nas))
