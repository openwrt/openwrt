# $Id: Makefile 2480 2005-11-14 02:07:33Z nbd $

include $(TOPDIR)/rules.mk

PKG_NAME:=wlc
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/wlc

include $(TOPDIR)/package/rules.mk

define Package/wlc
SECTION:=base
CATEGORY:=Base system
DEPENDS:=@PACKAGE_KMOD_BRCM_WL
DEFAULT:=y
TITLE:=Utility for configuring the Broadcom wl driver
DESCRIPTION:=Utility for configuring the Broadcom wl driver
endef


define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)\
		$(TARGET_CONFIGURE_OPTS) \
		CC="$(TARGET_CC)" \
		CFLAGS="-I$(PKG_BUILD_DIR)/include $(TARGET_CFLAGS)" \
		all
endef

define Package/wlc/install
	install -d -m0755 $(1)/sbin
	install -m0755 $(PKG_BUILD_DIR)/wlc $(1)/sbin/
endef

$(eval $(call BuildPackage,wlc))
