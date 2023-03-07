include $(TOPDIR)/rules.mk

# Name, version and release number
# The name and version of your package are used to define the variable to point to the build directory of your package: $(PKG_BUILD_DIR)
PKG_NAME:=wifiHandleController
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_PRO_FILE:=wifiHandleController.pro

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:= git@github.com:Inspectron/wifiHandleController.git
PKG_SOURCE_VERSION:=8529c321828446dc201008e9974869e1f483ddab

include $(INCLUDE_DIR)/package.mk

# Package definition; instructs on how and where our package will appear in the overall configuration menu ('make menuconfig')
define Package/wifiHandleController
	SECTION:=inspectron
	CATEGORY:=inspectron
	TITLE:=wifiHandleController
	DEPENDS:=+qt5-core +qt5-network +glib2 +gstreamer1 +libqmqtt
endef

# Package description; a more verbose description on what our package does
define Package/wifiHandleController/description
	wifiHandleController application.
endef

define Build/Configure
	cd $(PKG_BUILD_DIR); $(TOOLCHAIN_DIR)/bin/qmake $(PKG_BUILD_DIR)/trunk/$(PKG_PRO_FILE)
endef

define Build/Compile
	cd $(PKG_BUILD_DIR); $(MAKE) CXXFLAGS="-I$(STAGING_DIR)/usr/include/gstreamer-1.0 -I$(STAGING_DIR)/usr/include/glib-2.0"
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/root
	$(CP) $(PKG_BUILD_DIR)/wifiHandleController $(1)/root
endef

define Package/wifiHandleController/install
	$(INSTALL_DIR) $(1)/root
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wifiHandleController $(1)/root
endef

$(eval $(call BuildPackage,wifiHandleController))
