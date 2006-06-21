# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=robocfg
PKG_VERSION:=0.01
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/robocfg

include $(INCLUDE_DIR)/package.mk

define Package/robocfg
SECTION:=base
TITLE:=BCM5325E/536x switch configuration utility
DESCRIPTION:=Broadcom BCM5325E/536x switch configuration utility
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)" \
		all
endef

define Package/robocfg/install
	install -d -m0755 $(1)/sbin
	install -m0755 $(PKG_BUILD_DIR)/robocfg $(1)/sbin/
endef

$(eval $(call BuildPackage,robocfg))
