include $(TOPDIR)/rules.mk

PKG_NAME:=rtl83xx-poe
PKG_RELEASE:=2

PKG_LICENSE:=GPL-2.0+

include $(INCLUDE_DIR)/package.mk

define Package/rtl83xx-poe
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libubox-lua +libubus-lua +libuci-lua +lua-rs232
  TITLE:=PoE daemon for realtek switches
endef

define Package/rtl83xx-poe/description
 This package contains an utility to allow triggering the PoE state of realtek switch ports.
endef

define Build/Compile

endef

define Package/rtl83xx-poe/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,rtl83xx-poe))
