# 
# Copyright (C) 2011-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=polarssl
PKG_VERSION:=1.3.4
PKG_RELEASE:=1
PKG_USE_MIPS16:=0

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-gpl.tgz
PKG_SOURCE_URL:=https://polarssl.org/code/releases
PKG_MD5SUM:=30a75c5f171be49f805f3bf64a0af054

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/polarssl/Default
  SUBMENU:=SSL
  TITLE:=Embedded SSL
  URL:=http://polarssl.org/
endef

define Package/polarssl/Default/description
The aim of the PolarSSL project is to provide a quality, open-source
cryptographic library written in C and targeted at embedded systems.
endef

define Package/libpolarssl
$(call Package/polarssl/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE+= (library)
  ABI_VERSION:=$(PKG_VERSION)
endef

define Package/libpolarssl/description
$(call Package/polarssl/Default/description)
This package contains the PolarSSL library.
endef

define Package/polarssl-progs
$(call Package/polarssl/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libpolarssl +libpthread
  TITLE+= (programs)
endef

define Package/polarssl-progs/description
$(call Package/polarssl/Default/description)
This package contains the PolarSSL programs.
endef

PKG_INSTALL:=1

CMAKE_OPTIONS += \
	-DCMAKE_BUILD_TYPE:String="Release" \
	-DUSE_SHARED_POLARSSL_LIBRARY:Bool=ON \

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/polarssl $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libpolarssl.so* $(1)/usr/lib/
endef

define Package/libpolarssl/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libpolarssl.so* $(1)/usr/lib/
endef

define Package/polarssl-progs/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/* $(1)/usr/bin/
endef

$(eval $(call BuildPackage,libpolarssl))
$(eval $(call BuildPackage,polarssl-progs))
