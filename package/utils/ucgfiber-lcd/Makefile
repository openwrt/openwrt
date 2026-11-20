# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=ucgfiber-lcd
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=
PKG_MAINTAINER:=Alex Rau <alex@retroduction.org>
PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/ucgfiber-lcd
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=WAN rate display for the UCG-Fiber front panel
  DEPENDS:=@TARGET_qualcommbe +kmod-drm-panel-mipi-dbi +mipi-dbi-ubnt-ucgfiber
endef

define Package/ucgfiber-lcd/description
  Small daemon that draws the WAN download and upload rate on the front
  panel display of the Ubiquiti UniFi Cloud Gateway Fiber: a graph of the
  recent rates, a ring for each current rate and a mark per port. Draws
  into a DRM dumb buffer with plain ioctls; needs no DRM library.
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CPPFLAGS) $(TARGET_CFLAGS) -Wall -Wextra -o $(PKG_BUILD_DIR)/ucgfiber-lcd \
		$(PKG_BUILD_DIR)/ucgfiber-lcd.c $(PKG_BUILD_DIR)/draw.c $(TARGET_LDFLAGS) -lm
endef

define Package/ucgfiber-lcd/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ucgfiber-lcd $(1)/usr/sbin/ucgfiber-lcd
	$(INSTALL_BIN) ./files/ucgfiber-lcd.init $(1)/etc/init.d/ucgfiber-lcd
endef

$(eval $(call BuildPackage,ucgfiber-lcd))
