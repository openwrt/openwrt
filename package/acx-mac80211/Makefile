#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=acx-mac80211
PKG_REV:=e7a55711
PKG_VERSION:=20100302
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=git://gitorious.org/~oli1417/acx-mac80211/oli1417-clone.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/acx-mac80211
  SUBMENU:=Wireless Drivers
  TITLE:=ACX111 Mac80211 driver
  DEPENDS:=@LINUX_2_6 @PCI_SUPPORT||TARGET_ar7 +kmod-mac80211 @BROKEN
  FILES:=$(PKG_BUILD_DIR)/acx-mac80211.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,acx-mac80211)
endef

define KernelPackage/acx-mac80211/description
	Driver for acx111 cards (Mac80211 version)
endef


C16_FW:=tiacx111c16
C19_FW:=tiacx111c19

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


PKG_EXTRA_KCONFIG:= \
	CONFIG_ACX_MAC80211=m \
	CONFIG_ACX_MAC80211_PCI=m \

PKG_EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(PKG_EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(PKG_EXTRA_KCONFIG)))) \

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		$(PKG_EXTRA_KCONFIG) \
		EXTRA_CFLAGS="$(PKG_EXTRA_CFLAGS)" \
		LINUXINCLUDE="-I$(STAGING_DIR)/usr/include/mac80211 -I$(LINUX_DIR)/include \
			-Iarch/$(LINUX_KARCH)/include \
			-include linux/autoconf.h" \
		V="$(V)" \
		modules
endef

define Build/Configure
endef

define KernelPackage/acx-mac80211/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(DL_DIR)/$(C16_FW) $(1)/lib/firmware/
	$(INSTALL_DATA) $(DL_DIR)/$(C19_FW) $(1)/lib/firmware/
endef

$(eval $(call KernelPackage,acx-mac80211))
