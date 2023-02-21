include $(TOPDIR)/rules.mk

# Name, version and release number
# The name and version of your package are used to define the variable to point to the build directory of your package: $(PKG_BUILD_DIR)
PKG_NAME:=inspkernel
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:= git@github.com:Inspectron/inspscript.git
PKG_SOURCE_VERSION:=663fc7124bfc2502c50ec56af81fd864a58d5147


include $(INCLUDE_DIR)/package.mk

# Package definition; instructs on how and where our package will appear in the overall configuration menu ('make menuconfig')
define Package/inspkernel
	SECTION:=inspectron
	CATEGORY:=inspectron
	TITLE:=inspkernel
endef

# Package description; a more verbose description on what our package does
define Package/inspkernel/description
	This package adds boot script and wireless config to snap-on board
endef

define Build/Compile
	#$(warning add commands to compile inspkernel)
endef

# Install boot and wireless config
define Build/InstallDev
	#Install boot
	$(INSTALL_DIR) $(1)/etc/init.d
	$(CP) $(PKG_BUILD_DIR)/snaponBoot $(1)/etc/init.d

	#Install wireless config
	$(INSTALL_DIR) $(1)/etc/config
	$(CP) $(PKG_BUILD_DIR)/snapon_wireless_config $(1)/etc/config/wireless
endef

define Package/inspkernel/install
	#Install boot
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/snaponBoot $(1)/etc/init.d

	#Install wireless config
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/snapon_wireless_config $(1)/etc/config/wireless
endef

$(eval $(call BuildPackage,inspkernel))
