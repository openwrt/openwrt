#
# Copyright (C) 2011-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libevent2
PKG_VERSION:=2.1.9-beta
PKG_RELEASE:=1

PKG_SOURCE:=libevent-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/libevent/libevent/releases/download/release-$(PKG_VERSION)
PKG_HASH:=eeb4c6eb2c4021e22d6278cdcd02815470243ed81077be0cbd0f233fa6fc07e8
PKG_BUILD_DIR:=$(BUILD_DIR)/libevent-$(PKG_VERSION)

PKG_MAINTAINER:=Jo-Philipp Wich <jo@mein.io>
PKG_LICENSE:=BSD-3-Clause
PKG_CPE_ID:=cpe:/a:libevent_project:libevent

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libevent2/Default
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Event notification
  URL:=http://libevent.org
  ABI_VERSION:=7
endef

define Package/libevent2/Default/description
	The libevent API provides a mechanism to execute a callback function
	when a specific event occurs on a file descriptor or after a timeout
	has been reached. Furthermore, libevent also support callbacks due
	to signals or regular timeouts.

	libevent is meant to replace the event loop found in event driven
	network servers. An application just needs to call event_dispatch()
	and then add or remove events dynamically without having to change
	the event loop.
endef

define Package/libevent2
  $(call Package/libevent2/Default)
  TITLE+= library (version 2.1)
endef

define Package/libevent2/description
	$(call Package/libevent2/Default/description)

	This package contains the libevent shared library historically
	containing both the core & extra libraries.
endef

define Package/libevent2-core
  $(call Package/libevent2/Default)
  TITLE+= core library (version 2.1)
endef

define Package/libevent2-core/description
	$(call Package/libevent2/Default/description)

	This package contains the libevent core shared library for the event,
	buffer & utility functions.
endef

define Package/libevent2-extra
  $(call Package/libevent2/Default)
  TITLE+= extra library (version 2.1)
  DEPENDS+=+libevent2-core
endef

define Package/libevent2-extra/description
	$(call Package/libevent2/Default/description)

	This package contains the libevent extra shared library for specific
	protocols including HTTP, DNS & RPC.
endef

define Package/libevent2-openssl
  $(call Package/libevent2/Default)
  TITLE+= OpenSSL library (version 2.1)
  DEPENDS+=+libopenssl +libevent2-core
endef

define Package/libevent2-openssl/description
	$(call Package/libevent2/Default/description)

	This package contains the libevent OpenSSL shared library for encrypted
	bufferevents.
endef

define Package/libevent2-pthreads
  $(call Package/libevent2/Default)
  TITLE+= Pthreads library (version 2.1)
  DEPENDS+=+libpthread +libevent2-core
endef

define Package/libevent2-pthreads/description
	$(call Package/libevent2/Default/description)

	This package contains the libevent Pthreads shared library for
	threading & locking.
endef

TARGET_CFLAGS += $(FPIC) -ffunction-sections -fdata-sections -flto
TARGET_LDFLAGS += -Wl,--gc-sections,--as-needed -flto

CMAKE_OPTIONS += \
	-DEVENT__DISABLE_BENCHMARK:BOOL=ON \
	-DEVENT__DISABLE_DEBUG_MODE:BOOL=ON \
	-DEVENT__DISABLE_REGRESS:BOOL=ON \
	-DEVENT__DISABLE_SAMPLES:BOOL=ON \
	$(if $(CONFIG_PACKAGE_libevent2-openssl),-DEVENT__DISABLE_OPENSSL:BOOL=OFF,-DEVENT__DISABLE_OPENSSL:BOOL=ON) \
	$(if $(CONFIG_PACKAGE_libevent2-pthreads),-DEVENT__DISABLE_THREAD_SUPPORT:BOOL=OFF,-DEVENT__DISABLE_THREAD_SUPPORT:BOOL=ON) \
	-DEVENT__DISABLE_TESTS:BOOL=ON

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libevent*.{a,so}* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libevent*.pc $(1)/usr/lib/pkgconfig/
endef

define Package/libevent2/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libevent.so.2.1.* $(1)/usr/lib/
endef

define Package/libevent2-core/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libevent_core.so.2.1.* $(1)/usr/lib/
endef

define Package/libevent2-extra/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libevent_extra.so.2.1.* $(1)/usr/lib/
endef

define Package/libevent2-openssl/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libevent_openssl.so.2.1.* $(1)/usr/lib/
endef

define Package/libevent2-pthreads/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libevent_pthreads.so.2.1.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libevent2))
$(eval $(call BuildPackage,libevent2-core))
$(eval $(call BuildPackage,libevent2-extra))
$(eval $(call BuildPackage,libevent2-openssl))
$(eval $(call BuildPackage,libevent2-pthreads))
