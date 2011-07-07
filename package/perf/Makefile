#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=perf
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/perf
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:= +libdw +libelf +libpthread +librt
  TITLE:=Linux performance monitoring tool
  VERSION:=$(LINUX_VERSION)-$(PKG_RELEASE)
  URL:=http://www.kernel.org
endef

define Package/perf/description
  perf is the Linux performance monitoring tool
endef

define Build/Prepare
endef

define Build/Compile
	$(MAKE) -C $(LINUX_DIR)/tools/perf \
		ARCH="$(LINUX_KARCH)" \
		NO_LIBPERL=1 \
		NO_LIBPYTHON=1 \
		NO_NEWT=1 \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		CC="$(TARGET_CC)" \
		LD="$(TARGET_CROSS)ld" \
		CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		V=1 \
		prefix=/usr
endef

define Package/perf/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(LINUX_DIR)/tools/perf/perf $(1)/usr/bin/
endef

$(eval $(call BuildPackage,perf))
