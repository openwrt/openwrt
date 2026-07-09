#
# Copyright (C) 2013-2023 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=accel-ppp
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/RADIUSdesk/accel-ppp.git
PKG_SOURCE_DATE:=2023-11-15
PKG_SOURCE_VERSION:=115358c07ac58440b2e9e4e98a1416e416e0a82b
PKG_MIRROR_HASH:=3abb8e22a594f5a9c7458eaf6b232519946066121f88b17ac125e2783f25d1b1

PKG_MAINTAINER:=Abdulkader Alrezej <alrazj.abdulkader@gmail.com>
PKG_LICENSE:=GPL-2.0
PKG_BUILD_DEPENDS:=pcre2

#PKG_INSTALL:=1
#PKG_CONFIG_DEPENDS:=CONFIG_PACKAGE_accel-ppp_$(BUILD_VARIANT)_ext_cer_id

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/accel-ppp
  SECTION:=net
  CATEGORY:=Network
  TITLE:=accel-ppp IPoE VPN server
  DEPENDS:=+libpcre2 +libopenssl +libpthread +librt +libatomic +libucontext +kmod-sched-core +kmod-pptp +kmod-pppoe +kmod-ppp
endef

CMAKE_OPTIONS += \
	-DBUILD_DRIVER=FALSE \
	-DKDIR=$(LINUX_DIR) \
	-DCMAKE_INSTALL_PREFIX=/usr \
	-DCMAKE_BUILD_TYPE=Release \
	-DLOG_PGSQL=FALSE \
	-DSHAPER=TRUE \
	-DRADIUS=TRUE \
	-DNETSNMP=FALSE \
	-DLOG_FILE=TRUE \
	-DLOG_WTMP=FALSE \
	-DLIB_SUFFIX= \

TARGET_CFLAGS += -I$(STAGING_DIR)/opt/include -D_GNU_SOURCE -g -O2 -Wno-incompatible-pointer-types
TARGET_LDFLAGS += -lpthread -latomic

define Package/accel-ppp/install
	$(INSTALL_DIR) \
		$(1)/usr/sbin \
		$(1)/usr/bin \
		$(1)/usr/share/accel-ppp \
		$(1)/usr/share/accel-ppp/radius \
		$(1)/usr/share/accel-ppp/l2tp \
		$(1)/usr/lib/accel-ppp \
		$(1)/etc/init.d \
		$(1)/etc/accel-ppp

	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/sbin/accel-pppd \
		$(1)/usr/sbin/

	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/bin/accel-cmd \
		$(1)/usr/bin/

	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/lib/accel-ppp/*.so \
		$(1)/usr/lib/accel-ppp/

	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/lib/accel-ppp/libtriton.so \
		$(1)/usr/lib/

	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/share/accel-ppp/radius/dictionary* \
		$(1)/usr/share/accel-ppp/radius/

	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/share/accel-ppp/l2tp/dictionary* \
		$(1)/usr/share/accel-ppp/l2tp/

	$(INSTALL_BIN) \
		files/accel-ppp.init \
		$(1)/etc/init.d/accel-ppp

	$(INSTALL_DATA) \
		files/accel-ppp.conf \
		$(1)/etc/accel-ppp

endef

$(eval $(call BuildPackage,accel-ppp))
