include $(TOPDIR)/rules.mk

# Name, version and release number
# The name and version of your package are used to define the variable to point to the build directory of your package: $(PKG_BUILD_DIR)
PKG_NAME:=libinspCore
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_PRO_FILE:=libinspCore.pro

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:= git@github.com:Inspectron/libinspCore.git
PKG_SOURCE_VERSION:=33d0afe936fd726ca23a67f018189c180d114c26


include $(INCLUDE_DIR)/package.mk

# Package definition; instructs on how and where our package will appear in the overall configuration menu ('make menuconfig')
define Package/libinspCore
	SECTION:=inspectron
	CATEGORY:=inspectron
	TITLE:=libinspCore
	DEPENDS:=+qt5-core
endef

# Package description; a more verbose description on what our package does
define Package/libinspCore/description
	libinspCore library
endef

define Build/Compile
        cd $(PKG_BUILD_DIR); $(TOOLCHAIN_DIR)/bin/qmake $(PKG_BUILD_DIR)/trunk/$(PKG_PRO_FILE)
endef


define Package/libinspCore/install
	$(INSTALL_DIR) $(STAGING_DIR)/root-ramips/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/.pkgdir/libinspCore $(STAGING_DIR)/root-ramips/usr/lib
endef

$(eval $(call BuildPackage,libinspCore))
