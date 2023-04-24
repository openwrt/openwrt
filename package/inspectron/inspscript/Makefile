include $(TOPDIR)/rules.mk

# Name, version and release number
# The name and version of your package are used to define the variable to point to the build directory of your package: $(PKG_BUILD_DIR)
PKG_NAME:=inspscript
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:= git@github.com:Inspectron/inspscript.git
PKG_SOURCE_VERSION:=535a583d12a140b2bfb14a8f30c274978b59e3e7

include $(INCLUDE_DIR)/package.mk

# Package definition; instructs on how and where our package will appear in the overall configuration menu ('make menuconfig')
define Package/inspscript
	SECTION:=inspectron
	CATEGORY:=inspectron
	TITLE:=inspscript
endef

# Package description; a more verbose description on what our package does
define Package/inspscript/description
	This package adds boot script and wireless config to snap-on board
endef

define Build/Compile
	#$(warning add commands to compile inspscript)
endef

# Install boot and wireless config
define Build/InstallDev
	# Install boot
	$(INSTALL_DIR) $(1)/etc/init.d
	$(CP) $(PKG_BUILD_DIR)/snaponBoot $(1)/etc/init.d

	# Install wireless config
	$(INSTALL_DIR) $(1)/etc/config
	$(CP) $(PKG_BUILD_DIR)/snapon_wireless_config $(1)/etc/config/wireless

	# Install boot check and hard reset scripts
	$(CP) $(PKG_BUILD_DIR)/1_check_boot_ok.sh $(1)/etc/init.d
	$(CP) $(PKG_BUILD_DIR)/99_create_bootok.sh $(1)/etc/init.d
	$(CP) $(PKG_BUILD_DIR)/bootok $(1)/etc
endef

define Package/inspscript/install
	# Install boot
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/snaponBoot $(1)/etc/init.d

	# Install wireless config
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/snapon_wireless_config $(1)/etc/config/wireless

	# Install boot check and hard reset scripts
	$(CP) $(PKG_BUILD_DIR)/1_check_boot_ok.sh $(1)/etc/init.d
	$(CP) $(PKG_BUILD_DIR)/99_create_bootok.sh $(1)/etc/init.d
	$(CP) $(PKG_BUILD_DIR)/bootok $(1)/etc
endef

$(eval $(call BuildPackage,inspscript))
