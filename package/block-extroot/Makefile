#
# Copyright (C) 2009-2010 OpenWrt.org
# Copyright (C) 2010 Vertical Communications
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=block-extroot
PKG_VERSION:=0.0.1
PKG_RELEASE:=3

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/block-extroot/Default
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=root filesystem on secondary storage
endef

define Package/block-extroot
  $(call Package/block-extroot/Default)
  MENU:=1
  DEPENDS:=+block-mount @PACKAGE_kmod-ide-core||PACKAGE_kmod-ata-core||PACKAGE_kmod-usb-storage||PACKAGE_kmod-mmc||PACKAGE_kmod-broadcom-sdhc
endef

define Package/block-extroot/config
	source "$(SOURCE)/Config.in"
endef

define Package/block-extroot/description
  Based on the moduluarized preinit and firstboot, adds the option to have
  the root filesystem on storage other than the jffs or the boot root device.
  For a squashfs image this package must be installed into the image, not as
  a package to add later.
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/block-extroot/install
	$(INSTALL_DIR) $(1)/lib/functions
	$(INSTALL_DATA) ./files/extmount.sh $(1)/lib/functions/
	$(INSTALL_DIR) $(1)/lib/preinit
	$(INSTALL_DATA) ./files/50_determine_usb_root $(1)/lib/preinit/
	$(INSTALL_DATA) ./files/55_determine_extroot_sysupgrade $(1)/lib/preinit/
	$(INSTALL_DATA) ./files/60_pivot_usb_root $(1)/lib/preinit/
	$(INSTALL_DIR) $(1)/lib/preinit
	echo "extroot_settle_time=\"$(CONFIG_EXTROOT_SETTLETIME)\"" >$(1)/lib/preinit/00_extroot.conf
	$(INSTALL_DIR) $(1)/overlay
endef

$(eval $(call BuildPackage,block-extroot))

