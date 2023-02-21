include $(TOPDIR)/rules.mk

# Name, version and release number
# The name and version of your package are used to define the variable to point to the build directory of your package: $(PKG_BUILD_DIR)
PKG_NAME:=libInspHardware
PKG_VERSION:=1.0
PKG_RELEASE:=1

# Source settings (i.e. where to find the source codes)
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:= git@github.com:Inspectron/libInspHardware.git
PKG_SOURCE_VERSION:=2de7d2070264488582c30b5f0a47122898d89f4c
PKG_BUILD_DEPENDS:=qt5

include $(INCLUDE_DIR)/package.mk

# Package definition; instructs on how and where our package will appear in the overall configuration menu ('make menuconfig')
define Package/libInspHardware
	SECTION:=inspectron
	CATEGORY:=inspectron
	TITLE:=libInspHardware
	DEPENDS:=+qt5-core
endef

# Package description; a more verbose description on what our package does
define Package/libInspHardware/description
	Inspectron Hardware Interface Library.
endef

define Build/Configure
        cd $(PKG_BUILD_DIR); $(TOOLCHAIN_DIR)/bin/qmake $(PKG_BUILD_DIR)/trunk/$(PKG_PRO_FILE)
endef

define Build/Compile
        cd $(PKG_BUILD_DIR); $(MAKE)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/trunk/*.hpp $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/libInspHardware.so* $(1)/usr/lib/
endef

define Package/libInspHardware/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libInspHardware.so* $(1)/usr/lib
endef

# This command is always the last, it uses the definitions and variables we give above in order to get the job done
$(eval $(call BuildPackage,libInspHardware))
