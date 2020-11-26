#
# Copyright (C) 2009-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libnetfilter_queue
PKG_RELEASE:=3

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://git.netfilter.org/libnetfilter_queue
PKG_SOURCE_DATE:=2020-09-30
PKG_SOURCE_VERSION:=55aa47ddccaa525846eb2a70b9255cb1651fb6c8
PKG_MIRROR_HASH:=35262d4dd956a826ccfdb0182e389dadbe6f94652b18986d39e7802eeaec2845

PKG_FIXUP:=autoreconf
PKG_LICENSE:=GPL-2.0+

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libnetfilter-queue
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+libmnl +libnfnetlink
  TITLE:=Userspace API to packets queued by kernel packet filter
  URL:=http://www.netfilter.org/projects/libnetfilter_queue/
  ABI_VERSION:=1
endef

define Package/libnetfilter-queue/description
 libnetfilter_queue is a userspace library providing a programming
 interface (API) to packets that have been queued by the kernel
 packet filter.
endef

TARGET_CFLAGS += $(FPIC) -D_GNU_SOURCE=1

CONFIGURE_ARGS += \
	--enable-static \
	--enable-shared \

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/libnetfilter_queue
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/libnetfilter_queue/*.h \
		$(1)/usr/include/libnetfilter_queue/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libnetfilter_queue.{so*,a,la} \
		$(1)/usr/lib/

	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libnetfilter_queue.pc \
		$(1)/usr/lib/pkgconfig/
endef

define Package/libnetfilter-queue/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libnetfilter_queue.so.* \
		$(1)/usr/lib/
endef

$(eval $(call BuildPackage,libnetfilter-queue))
