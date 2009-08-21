# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libpcap
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.tcpdump.org/release/
PKG_MD5SUM:=9ad1358c5dec48456405eac197a46d3d

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/libpcap
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Low-level packet capture library
  URL:=http://www.tcpdump.org/
endef

define Package/libpcap/description
 This package contains a system-independent library for user-level network
 packet capture.
endef

define Package/libpcap/config
	config PCAP_HAS_USB
		bool "Include USB support"
		depends PACKAGE_libpcap
		depends PACKAGE_kmod-usb-core
		default n

	config PCAP_HAS_BT
		bool "Include bluetooth support"
		depends PACKAGE_libpcap
		depends PACKAGE_kmod-bluetooth
		depends BROKEN
		default n

endef

TARGET_CFLAGS += \
	-ffunction-sections -fdata-sections

CONFIGURE_VARS += \
	ac_cv_linux_vers=$(LINUX_VERSION)

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--disable-yydebug \
	--enable-ipv6 \
	--with-build-cc="$(HOSTCC)" \
	--with-pcap=linux \
	--without-septel \
	--without-dag

define Build/Compile
	rm -rf $(PKG_INSTALL_DIR)
	$(if $(CONFIG_PCAP_HAS_USB),,$(SED) '/^#define PCAP_SUPPORT_USB/D' $(PKG_BUILD_DIR)/config.h)
	$(if $(CONFIG_PCAP_HAS_USB),,$(SED) 's/pcap-usb-linux.c *//' $(PKG_BUILD_DIR)/Makefile)
	$(if $(CONFIG_PCAP_HAS_BT),,$(SED) '/^#define PCAP_SUPPORT_BT/D' $(PKG_BUILD_DIR)/config.h)
	$(if $(CONFIG_PCAP_HAS_BT),,$(SED) 's/pcap-bt-linux.c *//' $(PKG_BUILD_DIR)/Makefile)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CCOPT="$(TARGET_CFLAGS) -I$(BUILD_DIR)/linux/include" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/pcap* \
		$(1)/usr/include/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libpcap.{a,so*} \
		$(1)/usr/lib/
endef

define Package/libpcap/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libpcap.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libpcap))
