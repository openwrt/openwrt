include $(TOPDIR)/rules.mk

# Name, version and release number
# The name and version of your package are used to define the variable to point to the build directory of your package: $(PKG_BUILD_DIR)
PKG_NAME:=wifiHandleController
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:= git@github.com:Inspectron/wifiHandleController.git
PKG_SOURCE_VERSION:=86759fc968359e86154ad98067d106569b58438f


include $(INCLUDE_DIR)/package.mk

# Package definition; instructs on how and where our package will appear in the overall configuration menu ('make menuconfig')
define Package/wifiHandleController
	SECTION:=inspectron
	CATEGORY:=inspectron
	TITLE:=wifiHandleController
endef

# Package description; a more verbose description on what our package does
define Package/wifiHandleController/description
	wifiHandleController application.
endef


define Package/wifiHandleController/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wifiHandleController $(1)/usr/bin
endef

$(eval $(call BuildPackage,wifiHandleController))
