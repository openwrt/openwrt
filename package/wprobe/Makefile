# 
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=wprobe
PKG_VERSION:=1

PKG_BUILD_DEPENDS:=PACKAGE_wprobe-export:libipfix

PKG_CONFIG_DEPENDS = \
	CONFIG_PACKAGE_kmod-wprobe \
	CONFIG_PACKAGE_wprobe-export \

include $(INCLUDE_DIR)/package.mk

# XXX: build failure on cris
# wprobe-lib.c:145: error: 'packed' attribute ignored for field of type 'struct <anonymous>'
# wprobe-lib.c:149: error: 'packed' attribute ignored for field of type 'struct <anonymous>'

define Package/wprobe/Default
  DEPENDS:=@LINUX_2_6 @(!(TARGET_ps3||TARGET_pxcab||cris)||BROKEN)
endef

define KernelPackage/wprobe
$(call Package/wprobe/Default)
  SUBMENU:=Network Support
  TITLE:=Wireless driver probe infrastructure
  MAINTAINER:=Felix Fietkau <nbd@openwrt.org>
  FILES:= \
  	$(PKG_BUILD_DIR)/kernel/wprobe.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,01,wprobe)
endef

define KernelPackage/wprobe/description
  A module that exports measurement data from wireless driver to user space
endef

define Package/wprobe-util
$(call Package/wprobe/Default)
  SECTION:=net
  CATEGORY:=Network
  DEPENDS+=+kmod-wprobe +libnl-tiny
  TITLE:=Wireless measurement utility
endef

define Package/wprobe-util/description
  wprobe-util uses the wprobe kernel module to query
  wireless driver measurement data from an interface
endef

define Package/wprobe-export
$(call Package/wprobe/Default)
  SECTION:=net
  CATEGORY:=Network
  DEPENDS+=+wprobe-util
  TITLE:=Wireless measurement data exporter
endef

define Package/wprobe-export/description
  wprobe-export uses the wprobe kernel module to export
  wireless driver measurement data via the IPFIX protocol
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) src/* $(PKG_BUILD_DIR)/
endef

TARGET_CPPFLAGS := \
	-D_GNU_SOURCE \
	-I$(STAGING_DIR)/usr/include/libnl-tiny \
	$(TARGET_CPPFLAGS)

ifdef CONFIG_PACKAGE_kmod-wprobe
  define Build/Compile/kmod
	$(MAKE) -C $(LINUX_DIR) \
		CROSS_COMPILE="$(KERNEL_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)/kernel" \
		KERNELDIR=$(LINUX_DIR) \
		CC="$(TARGET_CC)" \
		EXTRA_CFLAGS="-I$(PKG_BUILD_DIR)/kernel" \
		modules
  endef
endif

define Build/Compile/lib
	$(MAKE) -C $(PKG_BUILD_DIR)/user \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="$(TARGET_CPPFLAGS) -I$(PKG_BUILD_DIR)/kernel" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		HOST_OS=Linux \
		LIBNL="-lnl-tiny"
endef

ifdef CONFIG_PACKAGE_wprobe-export
  define Build/Compile/exporter
	$(MAKE) -C $(PKG_BUILD_DIR)/exporter \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="$(TARGET_CPPFLAGS) -I$(PKG_BUILD_DIR)/kernel -I$(PKG_BUILD_DIR)/user" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		LIBS="$(PKG_BUILD_DIR)/user/libwprobe.a $(STAGING_DIR)/usr/lib/libipfix.a $(STAGING_DIR)/usr/lib/libmisc.a -lnl-tiny -lm"
  endef
endif

define Build/Compile
	$(Build/Compile/kmod)
	$(Build/Compile/lib)
	$(Build/Compile/exporter)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/wprobe
	$(CP) $(PKG_BUILD_DIR)/kernel/linux $(1)/usr/include/wprobe
endef

define Package/wprobe-util/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/user/wprobe-util $(1)/sbin/
endef

define Package/wprobe-export/install
	$(INSTALL_DIR) $(1)/sbin $(1)/etc/init.d $(1)/etc/config
	$(INSTALL_BIN) ./files/wprobe.init $(1)/etc/init.d/wprobe
	$(INSTALL_BIN) ./files/wprobe.config $(1)/etc/config/wprobe
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/exporter/wprobe-export $(1)/sbin/
endef

$(eval $(call KernelPackage,wprobe))
$(eval $(call BuildPackage,wprobe-util))
$(eval $(call BuildPackage,wprobe-export))
