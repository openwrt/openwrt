ifneq ($(__ninja_mk_inc),1)
__ninja_mk_inc=1

HOST_NINJA_DIR:=$(STAGING_DIR_HOST)
HOST_NINJA_BIN:=$(HOST_NINJA_DIR)/usr/bin/ninja

CMAKE_INSTALL_PREFIX=$(PKG_BUILD_DIR)/ipkg-install/usr
CMAKE_GENERATOR="Ninja"

define Build/Ninja/Compile
	cd $(PKG_BUILD_DIR)/$(1) && $(HOST_NINJA_BIN)
endef

define Build/Ninja/Install
	DESTDIR=$(PKG_BUILD_DIR)/ipkg-install  $(HOST_NINJA_BIN) install -C $(PKG_BUILD_DIR)/$(strip $(1))
endef

# Don't overwrite Compile or Install section in
# case of meson and use one defined in meson.mk
ifneq ($(__meson_mk_inc),1)

define Build/Compile
	$(call Build/Ninja/Compile)
endef

define Build/Install
	$(call Build/Ninja/Install)
endef
endif

endif
