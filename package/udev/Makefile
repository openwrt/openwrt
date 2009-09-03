#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=udev
PKG_VERSION:=142
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@KERNEL/linux/utils/kernel/hotplug/
PKG_MD5SUM:=3edc4cf383dccb06d866c5156d59ddd5

include $(INCLUDE_DIR)/package.mk

PKG_INSTALL=1

define Package/udev
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=@LINUX_2_6
  TITLE:=Dynamic device management subsystem
  URL:=http://www.kernel.org/pub/linux/utils/kernel/hotplug/udev.html
  MAINTAINER:=Geoff Levand <geoffrey.levand@am.sony.com>
endef

define Package/udev/description
	udev allows Linux users to have a dynamic /dev directory and it
	provides the ability to have persistent device names.
endef

define Package/udev/config
	source "$(SOURCE)/udev-config.in"
endef

udev-args-$(CONFIG_UDEV_DISABLE_LOGGING) += --disable-logging
udev-args-$(CONFIG_UDEV_ENABLE_DEBUG) += --enable-debug

CONFIGURE_ARGS += --prefix=/usr --exec-prefix= --sysconfdir=/etc \
	--sbindir=/sbin $(udev-args-y)

udev-extra-lib-bin-$(CONFIG_UDEV_EXTRA_ata_id) += ata_id

udev-extra-lib-bin-$(CONFIG_UDEV_EXTRA_cdrom_id) += cdrom_id
udev-extra-rules-$(CONFIG_UDEV_EXTRA_cdrom_id) += 60-cdrom_id.rules

udev-extra-lib-bin-$(CONFIG_UDEV_EXTRA_collect) += collect

udev-extra-lib-bin-$(CONFIG_UDEV_EXTRA_edd_id) += edd_id
udev-extra-rules-$(CONFIG_UDEV_EXTRA_edd_id) += 61-persistent-storage-edd.rules

udev-extra-lib-bin-$(CONFIG_UDEV_EXTRA_firmware) += firmware.sh
udev-extra-rules-$(CONFIG_UDEV_EXTRA_firmware) += 50-firmware.rules

udev-extra-lib-bin-$(CONFIG_UDEV_EXTRA_floppy) += create_floppy_devices

udev-extra-lib-bin-$(CONFIG_UDEV_EXTRA_fstab_import) += fstab_import
udev-extra-rules-$(CONFIG_UDEV_EXTRA_fstab_import) += 79-fstab_import.rules

udev-extra-lib-bin-$(CONFIG_UDEV_EXTRA_path_id) += path_id

udev-extra-lib-bin-$(CONFIG_UDEV_EXTRA_rule_generator) += \
	write_cd_rules write_net_rules
udev-extra-lib-data-$(CONFIG_UDEV_EXTRA_rule_generator) += \
	rule_generator.functions
udev-extra-rules-$(CONFIG_UDEV_EXTRA_rule_generator) += \
	75-cd-aliases-generator.rules 75-persistent-net-generator.rules

udev-extra-lib-bin-$(CONFIG_UDEV_EXTRA_scsi_id) += scsi_id

udev-extra-lib-bin-$(CONFIG_UDEV_EXTRA_usb_id) += usb_id

define Package/udev/install
	$(INSTALL_DIR) $(1)/etc/udev/rules.d
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/udev/udev.conf $(1)/etc/udev

	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/sbin/udevadm \
		$(PKG_INSTALL_DIR)/sbin/udevd \
		$(1)/sbin

	$(INSTALL_DIR) $(1)/lib/udev/rules.d
	$(INSTALL_DATA) \
		$(addprefix $(PKG_INSTALL_DIR)/lib/udev/rules.d/, \
			$(udev-extra-rules-y)) \
		$(addprefix $(PKG_INSTALL_DIR)/lib/udev/rules.d/, \
			50-udev-default.rules \
			60-persistent-input.rules \
			60-persistent-serial.rules \
			60-persistent-storage.rules \
			80-drivers.rules \
			95-udev-late.rules) \
		$(1)/lib/udev/rules.d

	$(INSTALL_DIR) $(1)/lib
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/lib/libudev.so.* \
		$(1)/lib

	$(INSTALL_DIR) $(1)/lib/udev
ifneq ($(udev-extra-lib-bin-y),)
		$(INSTALL_BIN) \
			$(addprefix $(PKG_INSTALL_DIR)/lib/udev/, \
				$(udev-extra-lib-bin-y)) \
			$(1)/lib/udev/
endif
ifneq ($(udev-extra-lib-data-y),)
		$(INSTALL_DATA) \
			$(addprefix $(PKG_INSTALL_DIR)/lib/udev/, \
				$(udev-extra-lib-data-y)) \
			$(1)/lib/udev/
endif
endef

$(eval $(call BuildPackage,udev))
