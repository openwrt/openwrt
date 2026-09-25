include $(TOPDIR)/rules.mk

PKG_NAME:=dap1620-leds
PKG_RELEASE:=1
PKG_MAINTAINER:=Boyler Fourcade <Boilerplate4U@gmail.com>
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=

include $(INCLUDE_DIR)/package.mk

define Package/dap1620-leds
  SECTION:=utils
  PKGARCH:=all
  CATEGORY:=Utilities
  TITLE:=D-Link DAP-1620 A2 uplink and signal LEDs
  DEPENDS:=@TARGET_ramips_mt7620 +iw +ubus +libubox +jshn
endef

define Package/dap1620-leds/conffiles
/etc/config/dap1620-leds
endef

define Build/Compile
endef

define Package/dap1620-leds/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/usr/libexec
	$(INSTALL_CONF) ./files/config $(1)/etc/config/dap1620-leds
	$(INSTALL_BIN) ./files/init $(1)/etc/init.d/dap1620-leds
	$(INSTALL_BIN) ./files/daemon $(1)/usr/libexec/dap1620-leds
endef

$(eval $(call BuildPackage,dap1620-leds))
