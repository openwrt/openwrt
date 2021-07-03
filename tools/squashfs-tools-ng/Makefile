include $(TOPDIR)/rules.mk

PKG_NAME:=squashfs-tools-ng
PKG_VERSION:=1.1.1
PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/AgentD/squashfs-tools-ng/tar.gz/v$(PKG_VERSION)?
PKG_HASH:=e4d3dfc9f354267c65bf2dd7e483ad23a148d49b3946e2ab3918e1fbd69015c1

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/host-build.mk

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin/
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/.libs/gensquashfs $(STAGING_DIR_HOST)/bin/gensquashfs
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/.libs/tar2sqfs $(STAGING_DIR_HOST)/bin/tar2sqfs
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/.libs/rdsquashfs $(STAGING_DIR_HOST)/bin/rdsquashfs
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/.libs/libsquashfs* $(STAGING_DIR_HOST)/lib/
endef

define Host/Clean
endef

$(eval $(call HostBuild))
