PKG_VERSION_ORGINAL:=$(PKG_VERSION)

ifeq ($(CONFIG_$(PKG_NAME)_USE_OTHER_VERSION),y)
PKG_VERSION:= $(strip $(subst ",, $(CONFIG_$(PKG_NAME)_OTHER_VERSION)))
PKG_MD5SUM:=
PKG_SOURCE:=$(subst $(PKG_VERSION_ORGINAL),$(PKG_VERSION),$(PKG_SOURCE))
endif

define Package/$(PKG_NAME)/config
	menu "overwrite package version"
		depends on PACKAGE_$(PKG_NAME)
	config $(PKG_NAME)_USE_OTHER_VERSION
		depends on PACKAGE_$(PKG_NAME)
		bool "Use other source version"
		default n
	config $(PKG_NAME)_OTHER_VERSION
		depends on $(PKG_NAME)_USE_OTHER_VERSION
		string "$(PKG_BASE_NAME) version as string (default version: $(PKG_VERSION_ORGINAL))"
		default "$(PKG_VERSION_ORGINAL)"
	endmenu
endef
