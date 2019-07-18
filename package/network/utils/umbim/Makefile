include $(TOPDIR)/rules.mk

PKG_NAME:=umbim
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(PROJECT_GIT)/project/umbim.git
PKG_SOURCE_DATE:=2019-03-11
PKG_SOURCE_VERSION:=24f9dc71a624cecd67c181cec77b4101038a39d9
PKG_MIRROR_HASH:=452bd6f1019ce53fd02119b12a8d6b13e946b73de02642e20e9fd85c482c452d
PKG_MAINTAINER:=John Crispin <john@phrozen.org>

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=

PKG_BUILD_PARALLEL:=1

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/umbim
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libubox +kmod-usb-net +kmod-usb-net-cdc-mbim +wwan
  TITLE:=Control utility for mobile broadband modems
endef

define Package/umbim/description
  umbim is a command line tool for controlling mobile broadband modems using
  the MBIM-protocol.
endef

TARGET_CFLAGS += \
	-I$(STAGING_DIR)/usr/include -ffunction-sections -fdata-sections

TARGET_LDFLAGS += -Wl,--gc-sections

define Package/umbim/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/umbim $(1)/sbin/
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,umbim))
