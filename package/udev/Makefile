# 
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=udev
PKG_VERSION:=106
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@KERNEL/linux/utils/kernel/hotplug/
PKG_MD5SUM:=320ccd2d0f4540d10e021bafa14f8985

include $(INCLUDE_DIR)/package.mk

define Package/udev
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@LINUX_2_6
  TITLE:=Dynamic device management subsystem
  URL:=http://www.kernel.org/pub/linux/utils/kernel/hotplug/udev.html
endef

define Package/udev/description
 udev provides a dynamic device directory containing only the files for 
 actually present devices. It creates or removes device node files in 
 the /dev directory, or it renames network interfaces.
endef

define Package/udevextras
$(call Package/udev)
  TITLE:=Utilities for udev scripts, for general device discovery
  MENU:=1
endef

define Package/udevextras/config
  source "$(SOURCE)/udevextras-config.in"
endef

UDEV_ALL_EXTRAS=ata_id cdrom_id dasd_id edd_id scsi_id path_id usb_id volume_id
UDEV_EXTRAS=$(strip $(foreach e, $(UDEV_ALL_EXTRAS), \
		$(if $(CONFIG_UDEVEXTRAS_$(e)),extras/$(e))))

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		STRIP="/bin/true" \
		OPTFLAGS="$(TARGET_CFLAGS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		INSTALL="install -c" \
		EXTRAS="$(UDEV_EXTRAS)" \
		VOLUME_ID_STATIC=true \
		all install
endef

define Package/udev/install
	install -d -m0775 $(1)/etc/udev
	$(CP) $(PKG_INSTALL_DIR)/etc/udev/* $(1)/etc/udev/
	install -d -m0775 $(1)/lib/udev
	install -d -m0775 $(1)/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/sbin/udevcontrol $(1)/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/sbin/udevd $(1)/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/sbin/udevsettle $(1)/sbin/
	install -d -m0775 $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/udevinfo $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/udevtest $(1)/usr/bin/
	install -d -m0775 $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/udevmonitor $(1)/usr/sbin/
	install -d -m0775 $(1)/etc/udev/rules.d
	$(CP) files/20-input-grouping.rules $(1)/etc/udev/rules.d/
endef

define Package/udevextras/install
	$(INSTALL_DIR) $(1)/etc/udev
	$(CP) $(PKG_BUILD_DIR)/etc/udev/rules.d $(1)/etc/udev/
	$(INSTALL_DIR) $(1)/lib/udev
	$$(foreach f, $$(shell $(FIND) $(PKG_INSTALL_DIR)/lib/udev/ -type f), \
		$(CP) $$(f) $(1)/lib/udev/ ; \
	)
endef

$(eval $(call BuildPackage,udev))
$(eval $(call BuildPackage,udevextras))
