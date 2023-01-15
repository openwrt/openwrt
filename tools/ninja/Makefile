include $(TOPDIR)/rules.mk

PKG_NAME:=ninja
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=v1.11.1.g95dee.kitware.jobserver-1
PKG_SOURCE_URL:=https://github.com/Kitware/ninja
PKG_MIRROR_HASH:=7788335e184bc137bed2f593b02fdeea2f7672ecf0b41b50076be18f6b47e008

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
