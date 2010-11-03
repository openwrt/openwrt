#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tapi_sip
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/tapi_sip
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=tapi_sip
  DEPENDS:=+libuci +libtapi +pjsip +kmod-lqtapi
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)" \
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS)
endef

define Package/tapi_sip/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tapi-sip $(1)/usr/bin/

	$(INSTALL_DIR) $(1)/etc/config $(1)/etc/init.d
	$(INSTALL_DATA) ./files/telephony.conf $(1)/etc/config/telephony
	$(INSTALL_BIN) ./files/telephony.init $(1)/etc/init.d/telephony
endef

$(eval $(call BuildPackage,tapi_sip))
