include $(TOPDIR)/rules.mk

# Name, version and release number
# The name and version of your package are used to define the variable to point to the build directory of your package: $(PKG_BUILD_DIR)
PKG_NAME:=libqmqtt
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_PRO_FILE:=libqmqtt.pro

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:= git@github.com:Inspectron/libqmqtt.git
PKG_SOURCE_VERSION:=6b8088be8ade49b2879ce63c02cf655ae5f2141c


include $(INCLUDE_DIR)/package.mk

# Package definition; instructs on how and where our package will appear in the overall configuration menu ('make menuconfig')
define Package/libqmqtt
	SECTION:=inspectron
	CATEGORY:=inspectron
	TITLE:=libqmqtt
	DEPENDS:=+qt5-core +qt5-network
endef

# Package description; a more verbose description on what our package does
define Package/libqmqtt/description
	libqmqtt library
endef

define Build/Configure
        cd $(PKG_BUILD_DIR); $(TOOLCHAIN_DIR)/bin/qmake $(PKG_BUILD_DIR)/trunk/$(PKG_PRO_FILE)
endef

define Build/Compile
        cd $(PKG_BUILD_DIR); $(MAKE)
endef


define Package/libqmqtt/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libqmqtt.so* $(1)/usr/lib
endef

$(eval $(call BuildPackage,libqmqtt))
