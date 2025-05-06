include $(TOPDIR)/rules.mk

PKG_NAME:=gnulib
PKG_CPE_ID:=cpe:/a:gnu:$(PKG_NAME)
PKG_VERSION:=320db6ee7a3cd44ee77f09d30c8a9002159beb4b# # stable-202407 in 202505

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://git.savannah.gnu.org/cgit/$(PKG_NAME).git/snapshot
PKG_HASH:=0be734e8c5f88e259bd593d63da2be275b93aedb551ccdcd78db9825bddeb94e

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
