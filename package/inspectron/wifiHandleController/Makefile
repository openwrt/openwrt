include $(TOPDIR)/rules.mk

# Name, version and release number
# The name and version of your package are used to define the variable to point to the build directory of your package: $(PKG_BUILD_DIR)
PKG_NAME:=wifiHandleController
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_PRO_FILE:=wifiHandleController.pro

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:= git@github.com:Inspectron/wifiHandleController.git
PKG_SOURCE_VERSION:=bc16c0d3813a9e62a37bba81b1705ebffe0bac10


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

define Build/Compile
        $(TOOLCHAIN_DIR)/bin/qmake $(PKG_BUILD_DIR)/trunk/$(PKG_PRO_FILE)
endef


define Package/wifiHandleController/install
	$(INSTALL_DIR) $(1)/root
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build*/wifiHandleController $(1)/root
endef

$(eval $(call BuildPackage,wifiHandleController))
