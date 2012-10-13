#
# Copyright (C) 2007-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=acx-mac80211
PKG_REV:=v2012-09-23-WIP
PKG_VERSION:=20121004
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=git://acx100.git.sourceforge.net/gitroot/acx100/acx-mac80211
PKG_SOURCE_PROTO:=git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_BUILD_DEPENDS:=mac80211

include $(INCLUDE_DIR)/package.mk

define KernelPackage/acx-mac80211
  SUBMENU:=Wireless Drivers
  TITLE:=ACX1xx mac80211 driver
  DEPENDS:=@(PCI_SUPPORT||TARGET_ar7) @mipsel +kmod-mac80211
  FILES:=$(PKG_BUILD_DIR)/acx-mac80211.ko
  AUTOLOAD:=$(call AutoLoad,50,acx-mac80211)
  MAINTAINER:=Florian Fainelli <florian@openwrt.org>
  MENU:=1
endef

define KernelPackage/acx-mac80211/config
	menu "Configuration"
		depends on PACKAGE_kmod-acx-mac80211

	config ACX_ID_0D
		bool "ACX1xx Radio ID 0D firmware"
		help
		  Download and install firmware for:
		    ACX1xx cards with Radio ID 0D into /lib/firmware.

	config ACX_ID_11
		bool "ACX1xx Radio ID 11 firmware"
		help
		  Download and install firmware for:
		    ACX1xx cards with Radio ID 11 into /lib/firmware.

	config ACX_ID_15
		bool "ACX1xx Radio ID 15 firmware"
		help
		  Download and install firmware for:
		    ACX1xx cards with Radio ID 15 into /lib/firmware.

	config ACX_ID_16
		bool "ACX1xx Radio ID 16 firmware"
		default y
		help
		  Download and install firmware for:
		    ACX1xx cards with Radio ID 16 into /lib/firmware.

	choice
		prompt "ACX111 firmware version"
		depends on ACX_ID_16
		default ACX_DEFAULT if !TARGET_adm5120
		default ACX_1_2_1_34 if TARGET_adm5120
		help
		  This option allows you to select the version of the acx firmware.

	config ACX_DEFAULT
		bool "Default"
		help
		  Default firmware for ACX111 devices.

		  If unsure, select this.

	config ACX_1_2_1_34
		bool "1.2.1_34"
		help
		  1.2.1_34 firmware for ACX111 devices. Works with Zyxel P-334WT.

		  If unsure, select the "default" firmware.

	endchoice

	config ACX_ID_17
		bool "ACX1xx Radio ID 17 firmware"
		help
		  Download and install firmware for:
		    ACX1xx cards with Radio ID 17 into /lib/firmware.

	config ACX_ID_19
		bool "ACX1xx Radio ID 19 firmware"
		default y
		help
		  Download and install firmware for:
		    ACX1xx cards with Radio ID 19 into /lib/firmware.

	config ACX_ID_1B
		bool "ACX1xx Radio ID 1B firmware"
		help
		  Download and install firmware for:
		    ACX1xx cards with Radio ID 1b into /lib/firmware.

	endmenu
endef

define KernelPackage/acx-mac80211/description
	Driver for acx111 cards (mac80211 version)
endef

define Download/tiacx100
	FILE:=tiacx100
	URL:=http://acx100.erley.org/fw/acx100_1.9.8.b/
	MD5SUM:=24a54fd30f7658fcbffc825b0dd7aa5b
endef

define Download/tiacx100r0d
	FILE:=tiacx100r0D
	URL:=http://acx100.erley.org/fw/acx100_1.9.8.b/
	MD5SUM:=1c7413e7b0be4ef7d1e424a132e17fab
endef

define Download/tiacx100r11
	FILE:=tiacx100r11
	URL:=http://acx100.erley.org/fw/acx100_1.9.8.b/
	MD5SUM:=a150750ad33c512edc4afee5270b37cb
endef

define Download/tiacx100r15
	FILE:=tiacx100r15
	URL:=http://acx100.erley.org/fw/acx100_1.9.8.b/
	MD5SUM:=c99d01d4fcf0d6cc00441aff60690be4
endef

define Download/tiacx111c16
	FILE:=tiacx111c16
	URL:=http://acx100.erley.org/fw/acx111_2.3.1.31/
	MD5SUM:=7026826460376f6b174f9225bd7781b9
endef

define Download/tiacx111c16_1
	FILE:=tiacx111c16_1.2.1_34
	URL:=http://sites.google.com/site/atorkhov/files/
	MD5SUM:=fcd07de4b25e1d2aaf3b78b27c5b7ee9
endef

define Download/tiacx111c17
	FILE:=tiacx111c17
	URL:=http://acx100.erley.org/fw/acx111_2.3.1.31/
	MD5SUM:=95552544ca6d2b4e8c6aeb80b8ae7fdf
endef

define Download/tiacx111c19
	FILE:=tiacx111c19
	URL:=http://acx100.erley.org/fw/acx111_2.3.1.31/
	MD5SUM:=a1fa9681e297b4e36e257090fc12265a
endef

define Download/tiacx111usbc1b
	FILE:=tiacx111usbc1B
	URL:=http://acx100.erley.org/fw/acx111_2.4.0.70-USB/
	MD5SUM:=c4edecd912b2417779d0b65e3a7dc86d
endef

PKG_EXTRA_KCONFIG:= \
	CONFIG_ACX_MAC80211=m \
	CONFIG_ACX_MAC80211_PCI=m \

PKG_EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(PKG_EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(PKG_EXTRA_KCONFIG)))) \

LINUX_AUTOCONF_FILE:= generated/autoconf.h

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		$(PKG_EXTRA_KCONFIG) \
		EXTRA_CFLAGS="$(PKG_EXTRA_CFLAGS) -DCONFIG_ACX_MAC80211_VERSION=\"KERNEL_VERSION(3,7,0)\"" \
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

ifneq ($(CONFIG_ACX_ID_0D)$(CONFIG_ACX_ID_11)$(CONFIG_ACX_ID_15),)
	$(INSTALL_DATA) $(DL_DIR)/tiacx100 $(1)/lib/firmware/
endif

ifneq ($(CONFIG_ACX_ID_0D),)
	$(INSTALL_DATA) $(DL_DIR)/tiacx100r0D $(1)/lib/firmware/
endif

ifneq ($(CONFIG_ACX_ID_11),)
	$(INSTALL_DATA) $(DL_DIR)/tiacx100r11 $(1)/lib/firmware/
endif

ifneq ($(CONFIG_ACX_ID_15),)
	$(INSTALL_DATA) $(DL_DIR)/tiacx100r15 $(1)/lib/firmware/
endif

ifneq ($(CONFIG_ACX_DEFAULT),)
	$(INSTALL_DATA) $(DL_DIR)/tiacx111c16 $(1)/lib/firmware/
endif

ifneq ($(CONFIG_ACX_1_2_1_34),)
	$(INSTALL_DATA)	$(DL_DIR)/tiacx111c16_1.2.1_34 $(1)/lib/firmware/tiacx111c16
endif

ifneq ($(CONFIG_ACX_ID_17),)
	$(INSTALL_DATA) $(DL_DIR)/tiacx111c17 $(1)/lib/firmware/
endif

ifneq ($(CONFIG_ACX_ID_19),)
	$(INSTALL_DATA) $(DL_DIR)/tiacx111c19 $(1)/lib/firmware/
endif

ifneq ($(CONFIG_ACX_ID_1B),)
	$(INSTALL_DATA) $(DL_DIR)/tiacx111usbc1B $(1)/lib/firmware/
endif

endef

$(eval $(call KernelPackage,acx-mac80211))
$(eval $(call Download,tiacx100))
$(eval $(call Download,tiacx100r0d))
$(eval $(call Download,tiacx100r11))
$(eval $(call Download,tiacx100r15))
$(eval $(call Download,tiacx111c16))
$(eval $(call Download,tiacx111c16_1))
$(eval $(call Download,tiacx111c17))
$(eval $(call Download,tiacx111c19))
$(eval $(call Download,tiacx111usbc1b))
