#
# Copyright (C) 2007-2017 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pciutils
PKG_VERSION:=3.12.0
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@KERNEL/software/utils/pciutils
PKG_HASH:=f185d116d5ff99b797497efce8f19f1ee8ccc5a668b97a159e3d13472f674154

PKG_MAINTAINER:=Lucian Cristian <lucian.cristian@gmail.com>
PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=COPYING
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/pciutils
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Linux PCI Utilities
  URL:=http://mj.ucw.cz/pciutils.shtml
  DEPENDS:=+libkmod +libpci +pciids
endef

define Package/pciutils/description
 contains collection of programs for inspecting and manipulating configuration
 of PCI devices
endef

define Package/libpci
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Linux PCI Libraries
  URL:=http://mj.ucw.cz/pciutils.shtml
endef

TARGET_CFLAGS += $(FPIC)

MAKE_FLAGS += \
	CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
	PREFIX="/usr" \
	HOST="Linux" \
	HWDB="no" \
	ZLIB="no" \
	SHARED="yes"

ifneq ($(CONFIG_USE_GLIBC),)
TARGET_LDFLAGS += -lresolv
endif

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libpci.so.3 \
		$(PKG_INSTALL_DIR)/usr/lib/libpci.so
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so* $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_BUILD_DIR)/lib/libpci.pc $(1)/usr/lib/pkgconfig
	$(SED) 's,/usr/include,$$$${prefix}/include,g' $(1)/usr/lib/pkgconfig/libpci.pc
	$(SED) 's,/usr/lib,$$$${prefix}/lib,g' $(1)/usr/lib/pkgconfig/libpci.pc
	$(INSTALL_DIR) $(1)/usr/include/pci
	$(CP) $(foreach i,pci.h config.h header.h types.h, \
		$(PKG_BUILD_DIR)/lib/$(i)) $(1)/usr/include/pci
endef

define Package/pciutils/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/lspci $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/setpci $(1)/usr/sbin/
endef

define Package/libpci/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so* $(1)/usr/lib/
endef


$(eval $(call BuildPackage,libpci))
$(eval $(call BuildPackage,pciutils))
