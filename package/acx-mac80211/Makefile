#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=acx-mac80211
PKG_REV:=11145e4b4895d0a8ead16cbe052baf3dde4627bd
PKG_VERSION:=20110123
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=git://acx100.git.sourceforge.net/gitroot/acx100/acx-mac80211
PKG_SOURCE_PROTO:=git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/acx-mac80211
  SUBMENU:=Wireless Drivers
  TITLE:=ACX111 Mac80211 driver
  DEPENDS:=@PCI_SUPPORT||TARGET_ar7 +kmod-mac80211
  FILES:=$(PKG_BUILD_DIR)/acx-mac80211.ko
  AUTOLOAD:=$(call AutoLoad,50,acx-mac80211)
  MENU:=1
endef

define KernelPackage/acx-mac80211/config
	menu "Configuration"
		depends on PACKAGE_kmod-acx-mac80211

	choice
		prompt "acx firmware version"
		default ACX_DEFAULT if !TARGET_adm5120
		default ACX_1_2_1_34 if TARGET_adm5120
		help
		  This option allows you to select the version of the acx firmware.

	config ACX_DEFAULT
		bool "Default"
		help
		  Default firmware for ACX devices.

		  If unsure, select this.

	config ACX_1_2_1_34
		bool "1.2.1_34"
		help
		  1.2.1_34 firmware for ACX devices. Works with Zyxel P-334WT.

		  If unsure, select the "default" firmware.

	endchoice

	endmenu
endef

define KernelPackage/acx-mac80211/description
	Driver for acx111 cards (Mac80211 version)
endef


C16_FW:=tiacx111c16
C19_FW:=tiacx111c19

ifeq ($(CONFIG_ACX_DEFAULT),y)
  define Download/tiacx111c16
	FILE:=$(C16_FW)
	URL:=http://teknoraver.net/software/acx-mac80211/
	MD5SUM:=7026826460376f6b174f9225bd7781b9
  endef
  $(eval $(call Download,tiacx111c16))

  define Download/tiacx111c19
	FILE:=$(C19_FW)
	URL:=http://ipkg.k1k2.de/files/
	MD5SUM:=a1fa9681e297b4e36e257090fc12265a
  endef
  $(eval $(call Download,tiacx111c19))
endif

ifeq ($(CONFIG_ACX_1_2_1_34),y)
  define Download/tiacx111c16
	FILE:=$(C16_FW)_1.2.1_34
#	URL:=http://acx100.erley.org/fw/acx111_1.2.1.34/
	URL:=http://sites.google.com/site/atorkhov/files/
	MD5SUM:=fcd07de4b25e1d2aaf3b78b27c5b7ee9
  endef
  $(eval $(call Download,tiacx111c16))
endif

PKG_EXTRA_KCONFIG:= \
	CONFIG_ACX_MAC80211=m \
	CONFIG_ACX_MAC80211_PCI=m \

PKG_EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(PKG_EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(PKG_EXTRA_KCONFIG)))) \

ifneq ($(CONFIG_LINUX_2_6_30)$(CONFIG_LINUX_2_6_31)$(CONFIG_LINUX_2_6_32),)
  LINUX_AUTOCONF_FILE:= linux/autoconf.h
else
  LINUX_AUTOCONF_FILE:= generated/autoconf.h
endif

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		$(PKG_EXTRA_KCONFIG) \
		EXTRA_CFLAGS="$(PKG_EXTRA_CFLAGS) -DCONFIG_ACX_MAC80211_VERSION=\"KERNEL_VERSION(2,6,37)\"" \
		LINUXINCLUDE="-I$(STAGING_DIR)/usr/include/mac80211 -I$(LINUX_DIR)/include \
			-Iarch/$(LINUX_KARCH)/include \
			-include $(LINUX_AUTOCONF_FILE) \
			-include linux/compat-2.6.h" \
		V="$(V)" \
		modules
endef

define Build/Configure
endef

define KernelPackage/acx-mac80211/install
	$(INSTALL_DIR) $(1)/lib/firmware
ifeq ($(CONFIG_ACX_DEFAULT),y)
	$(INSTALL_DATA) $(DL_DIR)/$(C16_FW) $(1)/lib/firmware/
	$(INSTALL_DATA) $(DL_DIR)/$(C19_FW) $(1)/lib/firmware/
endif
ifeq ($(CONFIG_ACX_1_2_1_34),y)
	$(INSTALL_DATA) $(DL_DIR)/$(C16_FW)_1.2.1_34 $(1)/lib/firmware/$(C16_FW)
endif
endef

$(eval $(call KernelPackage,acx-mac80211))
