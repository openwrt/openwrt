include $(TOPDIR)/rules.mk

PKG_NAME:=gnulib
PKG_CPE_ID:=cpe:/a:gnu:$(PKG_NAME)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2024-02-13
PKG_SOURCE_URL:=https://git.savannah.gnu.org/git/gnulib
PKG_SOURCE_VERSION:=c99c8d491850dc3a6e0b8604a2729d8bc5c0eff1
PKG_MIRROR_HASH:=9a5db113df68ccc6c6752d3c9557af00d090683ae0e1fbb399d5c933a532c5b2

include $(INCLUDE_DIR)/host-build.mk

define Host/Configure
endef

define Host/Install
	$(INSTALL_DIR) $(1)/share/aclocal
	$(INSTALL_DATA) $(HOST_BUILD_DIR)/m4/*.m4 $(1)/share/aclocal/
	$(INSTALL_DIR) $(1)/share/gnulib
	$(CP) $(HOST_BUILD_DIR)/* $(1)/share/gnulib/
	ln -sf ../share/gnulib/gnulib-tool $(STAGING_DIR_HOST)/bin/gnulib-tool
endef

define Host/Clean
	rm -rf $(STAGING_DIR_HOST)/bin/gnulib-tool $(STAGING_DIR_HOST)/share/gnulib
endef

$(eval $(call HostBuild))
