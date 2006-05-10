# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=wificonf
PKG_RELEASE:=5

PKG_BUILD_DIR:=$(BUILD_DIR)/wificonf

include $(TOPDIR)/package/rules.mk

define Package/wificonf
SECTION:=base
CATEGORY:=Network
DEFAULT:=y
TITLE:=replacement utility for wlconf
DESCRIPTION:=Replacement utility for wlconf
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -Iinclude/ -I$(STAGING_DIR)/usr/include -o $(PKG_BUILD_DIR)/wifi wificonf.c -L$(STAGING_DIR)/usr/lib -lnvram -lshared $(STAGING_DIR)/usr/lib/libiw.so
endef

define Package/wificonf/install
	install -d -m0755 $(IDIR_WIFICONF)/sbin
	install -m0755 $(PKG_BUILD_DIR)/wifi $(IDIR_WIFICONF)/sbin/
endef
