#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libtapi
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/libtapi
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=libtapi
  DEPENDS:=@TARGET_lantiq_xway
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	CFLAGS="$(TARGT_CPPFLAGS) $(TARGET_CFLAGS)" \
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/{include,lib}
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/*.h $(1)/usr/include/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/*.so $(1)/usr/lib/
endef

define Package/libtapi/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libtapi.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libtapi))
