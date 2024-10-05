include $(TOPDIR)/rules.mk

PKG_NAME:=ninja
PKG_VERSION:=1.12.1
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/ninja-build/ninja
PKG_SOURCE_VERSION:=v$(PKG_VERSION)
PKG_MIRROR_HASH:=dd9ce4caf6d7404fa653daeb600867be66c682bc3f07f018b24702b35e3c2c07

include $(INCLUDE_DIR)/host-build.mk

CONFIGURE_ARGS:=
ifneq ($(findstring c,$(OPENWRT_VERBOSE)),)
  CONFIGURE_ARGS+=--verbose
endif

define Host/Configure
endef

define Host/Compile
	cd $(HOST_BUILD_DIR) && \
		CXX="$(HOSTCXX_NOCACHE)" \
		CXXFLAGS="$(HOST_CXXFLAGS) $(HOST_CPPFLAGS)" \
		LDFLAGS="$(HOST_LDFLAGS)" \
		$(STAGING_DIR_HOST)/bin/$(PYTHON) configure.py --bootstrap $(CONFIGURE_ARGS)
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/ninja $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	$(call Host/Clean/Default)
	rm -f $(STAGING_DIR_HOST)/bin/ninja
endef

$(eval $(call HostBuild))
