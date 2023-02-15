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
endef

# Package description; a more verbose description on what our package does
define Package/helloworld/description
	Inspectron Hardware Interface Library.
endef

# Package preparation instructions; create the build directory and copy the source code.Inspectron Hardware Interface Library. 
# The last command is necessary to ensure our preparation instructions remain compatible with the patching system.
define Build/Prepare
	#TODO add commands to configure libInspHardware
	$(warning add commands to configure libInspHardware)
endef

# Package build instructions; invoke the target-specific compiler to first compile the source file, and then to link the file into the final executable
define Build/Compile
	#TODO add commands to make libInspHardware
	$(warning add commands to make libInspHardware)
endef

# Package install instructions; create a directory inside the package to hold our executable, and then copy the executable we built previously into the folder
define Package/libInspHardware/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libInspHardware $(1)/usr/lib
endef

# This command is always the last, it uses the definitions and variables we give above in order to get the job done
$(eval $(call BuildPackage,libInspHardware))
