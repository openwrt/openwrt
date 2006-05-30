# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=robocfg
PKG_VERSION:=0.01
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/robocfg

include $(TOPDIR)/package/rules.mk

define Package/robocfg
SECTION:=base
TITLE:=BCM5325E/536x switch configuration utility
DESCRIPTION:=Broadcom BCM5325E/536x switch configuration utility
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/robocfg robocfg.c
endef

define Package/robocfg/install
	install -d -m0755 $(1)/sbin
	install -m0755 $(PKG_BUILD_DIR)/robocfg $(1)/sbin/
	$(RSTRIP) $(1)
	$(IPKG_BUILD) $(1) $(PACKAGE_DIR)
endef

$(eval $(call BuildPackage,robocfg))
