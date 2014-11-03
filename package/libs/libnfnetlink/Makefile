#
# Copyright (C) 2007-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libnfnetlink
PKG_VERSION:=1.0.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:= \
	http://www.netfilter.org/projects/libnfnetlink/files/ \
	ftp://ftp.netfilter.org/pub/libnfnetlink/ \
	http://mirrors.evolva.ro/netfilter.org/libnfnetlink/
PKG_MD5SUM:=98927583d2016a9fb1936fed992e2c5e
PKG_MAINTAINER:=Jo-Philipp Wich <jow@openwrt.org>
PKG_LICENSE:=GPL-2.0+

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=COPYING

PKG_FIXUP:=autoreconf

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libnfnetlink
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=A low-level library for netfilter related kernel/userspace communication
  URL:=http://netfilter.org/projects/libnfnetlink/
endef

define Package/libnfnetlink/description
 libnfnetlink is is the low-level library for netfilter related kernel/userspace communication.
 It provides a generic messaging infrastructure for in-kernel netfilter subsystems
 (such as nfnetlink_log, nfnetlink_queue, nfnetlink_conntrack) and their respective users
 and/or management tools in userspace.
endef

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--enable-static \
	--enable-shared

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/libnfnetlink
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/libnfnetlink/{libnfnetlink,linux_nfnetlink,linux_nfnetlink_compat}.h \
		$(1)/usr/include/libnfnetlink/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libnfnetlink.{a,so*} \
		$(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libnfnetlink.pc \
		$(1)/usr/lib/pkgconfig/
endef

define Package/libnfnetlink/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libnfnetlink.so.* \
		$(1)/usr/lib/
endef

$(eval $(call BuildPackage,libnfnetlink))
