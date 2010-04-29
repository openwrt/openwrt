# remember the provided package version
PKG_VERSION_ORGINAL:=$(PKG_VERSION)

# in case that another version is provided, overwrite the original
ifeq ($(CONFIG_$(PKG_NAME)_USE_CUSTOM_VERSION),y)
PKG_VERSION:=$(call qstrip,$(CONFIG_$(PKG_NAME)_CUSTOM_VERSION))
PKG_SOURCE:=$(subst $(PKG_VERSION_ORGINAL),$(PKG_VERSION),$(PKG_SOURCE))
PKG_MD5SUM:=
endif

# package specific configuration
# if includeded the package version can be overwritten within the .config file (instead of changing the package specific Makefile)
define Package/$(PKG_NAME)/override_version
	menu "overwrite package version"
		depends on PACKAGE_$(PKG_NAME)
	config $(PKG_NAME)_USE_CUSTOM_VERSION
		depends on PACKAGE_$(PKG_NAME)
		bool "Use custom package version"
		default n
	config $(PKG_NAME)_CUSTOM_VERSION
		depends on $(PKG_NAME)_USE_CUSTOM_VERSION
		string "$(PKG_BASE_NAME) version as string (default version: $(PKG_VERSION_ORGINAL))"
		default "$(PKG_VERSION_ORGINAL)"
	endmenu
endef

# in case that an customer source path is provided, set the acc. default variable
ifeq ($(CONFIG_$(PKG_NAME)_USE_CUSTOM_SOURCE_DIR),y)
PKG_DEFAULT_CUSTOM_SOURCE_DIR:= $(call qstrip,$(CONFIG_$(PKG_NAME)_CUSTOM_SOURCE_DIR))
endif

# package specific configuration
# if includeded the package source path can be overwritten within the .config file (instead of changing the package specific Makefile)
# instead of using a source ball (eg tar.gz) the specified path will point to the location of the sources
define Package/$(PKG_NAME)/override_source_path
	menu "custom source directory"
		depends on PACKAGE_$(PKG_NAME)
	config $(PKG_NAME)_USE_CUSTOM_SOURCE_DIR
		depends on PACKAGE_$(PKG_NAME)
		bool "Use custom source directory"
		default n
	config $(PKG_NAME)_CUSTOM_SOURCE_DIR
		depends on $(PKG_NAME)_USE_CUSTOM_SOURCE_DIR
		string "Custom source directory"
		default "$(PKG_DEFAULT_CUSTOM_SOURCE_DIR)"
	endmenu
endef

# default:
# include both configurations as long this file is included before package.mk
# in case that you're defining your own onfiguration within the package Makefile just include the stuff by yourself
define Package/$(PKG_NAME)/config
   $(call Package/$(PKG_NAME)/override_version)
   $(call Package/$(PKG_NAME)/override_source_path)
endef

# hook for custom source path
# in case that the specified path is valid a link to the PKG_SOURCE_DIR is created
# otherwise the make is stopped
define prepare_custom_source_directory
	if [ -d $(CONFIG_$(PKG_NAME)_CUSTOM_SOURCE_DIR) ]; then \
		rm -Rf $(PKG_BUILD_DIR); \
		echo "Preparing Custom Source Directory link: $(CONFIG_$(PKG_NAME)_CUSTOM_SOURCE_DIR)"; \
		ln -snf $(CONFIG_$(PKG_NAME)_CUSTOM_SOURCE_DIR) $(PKG_BUILD_DIR); \
		( cd $(PKG_BUILD_DIR); autoreconf -i; ) \
	else \
		echo "Custom Source Directory $(CONFIG_$(PKG_NAME)_CUSTOM_SOURCE_DIR) is invalid"; \
		false; \
	fi
endef

