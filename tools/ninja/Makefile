include $(TOPDIR)/rules.mk

PKG_NAME:=ninja
PKG_VERSION:=1.13.1
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/ninja-build/ninja
PKG_SOURCE_VERSION:=v$(PKG_VERSION)
PKG_MIRROR_HASH:=6e8b53c51243fbd1243736b8ebdc90e942c7ba91bea57bbae650ea2de534a7de

include $(INCLUDE_DIR)/host-build.mk

define Host/Configure
	cd $(HOST_BUILD_DIR) && \
		$(HOST_MAKE_VARS) \
		CXX="$(HOSTCXX_NOCACHE)" \
		$(STAGING_DIR_HOST)/bin/$(PYTHON) configure.py \
			$(if $(shell $(STAGING_DIR_HOST)/bin/ninja --version),,--bootstrap) \
			--no-rebuild \
			--verbose
	-$(Host/Install)
endef

define Host/Compile
	+$(NINJA) -C $(HOST_BUILD_DIR)
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/ninja $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/ninja
endef

$(eval $(call HostBuild))
