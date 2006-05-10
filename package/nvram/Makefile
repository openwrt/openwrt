# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=nvram
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/package/rules.mk

define Package/nvram
SECTION:=base
# CATEGORY:=FIXME
DEFAULT:=n
TITLE:=Broadcom config utility
DESCRIPTION:=control utility for broadcom's 'nvram' config area
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

$(IPKG_NVRAM):
define Package/nvram/install
	install -d -m0755  $(IDIR_NVRAM)/usr/lib
	install -d -m0755 $(IDIR_NVRAM)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/*.so $(IDIR_NVRAM)/usr/lib
	install -m0755 $(PKG_BUILD_DIR)/nvram $(IDIR_NVRAM)/usr/sbin
endef

compile-targets: install-dev
install-dev: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/*.so $(STAGING_DIR)/usr/lib
	

