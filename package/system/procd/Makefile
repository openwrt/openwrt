#
# Copyright (C) 2014-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=procd
PKG_VERSION:=2015-03-06

PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/luci2/procd.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=47d5be7160e33966f5ded484232d10dcc220f172
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
CMAKE_INSTALL:=1

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=John Crispin <blogic@openwrt.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

TARGET_LDFLAGS += $(if $(CONFIG_USE_EGLIBC),-lrt)

define Package/procd
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=+ubusd +ubus +libjson-script +ubox +USE_EGLIBC:librt +libubox +libubus +NAND_SUPPORT:procd-nand
  TITLE:=OpenWrt system process manager
endef

define Package/procd-nand
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@NAND_SUPPORT +ubi-utils
  TITLE:=OpenWrt sysupgrade nand helper
endef

define Package/procd-nand-firstboot
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=procd-nand
  TITLE:=OpenWrt firstboot nand helper
endef

define Package/procd/config
menu "Configuration"
	depends on PACKAGE_procd

config PROCD_SHOW_BOOT
	bool
	default n
	prompt "Print the shutdown to the console as well as logging it to syslog"

config PROCD_ZRAM_TMPFS
	bool
	default n
	prompt "Mount /tmp using zram."
endmenu
endef

PKG_CONFIG_DEPENDS:= CONFIG_NAND_SUPPORT CONFIG_PROCD_SHOW_BOOT CONFIG_PROCD_ZRAM_TMPFS

ifeq ($(CONFIG_NAND_SUPPORT),y)
  CMAKE_OPTIONS += -DBUILD_UPGRADED=1
endif

ifeq ($(CONFIG_PROCD_SHOW_BOOT),y)
  CMAKE_OPTIONS += -DSHOW_BOOT_ON_CONSOLE=1
endif

ifeq ($(CONFIG_PROCD_ZRAM_TMPFS),y)
  CMAKE_OPTIONS += -DZRAM_TMPFS=1
endif

define Package/procd/install
	$(INSTALL_DIR) $(1)/sbin $(1)/etc $(1)/lib/functions

	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/{init,procd,askfirst,udevtrigger} $(1)/sbin/
	$(INSTALL_BIN) ./files/reload_config $(1)/sbin/
	$(INSTALL_DATA) ./files/hotplug*.json $(1)/etc/
	$(INSTALL_DATA) ./files/procd.sh $(1)/lib/functions/
endef

define Package/procd-nand/install
	$(INSTALL_DIR) $(1)/sbin $(1)/lib/upgrade

	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/upgraded $(1)/sbin/
	$(INSTALL_DATA) ./files/nand.sh $(1)/lib/upgrade/
endef

define Package/procd-nand-firstboot/install
	$(INSTALL_DIR) $(1)/lib/preinit

	$(INSTALL_DATA) ./files/nand-preinit.sh $(1)/lib/preinit/60-nand-firstboot.sh
endef

$(eval $(call BuildPackage,procd))
$(eval $(call BuildPackage,procd-nand))
$(eval $(call BuildPackage,procd-nand-firstboot))
