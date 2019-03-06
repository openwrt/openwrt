include $(TOPDIR)/rules.mk

PKG_NAME:=zsync
PKG_VERSION:=f0ea808bfd957d7589420585b3d25081e0040e6d
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/ahmedammar/zsync/tar.gz/$(PKG_VERSION)?
PKG_HASH:=597c6d3464d6e9abdccfb8066e7c5ef4e812655b21241bffca8cc87166c4ea3e

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/host-build.mk

HOST_UNPACK:=$(TAR) -C $(HOST_BUILD_DIR) --strip-components=2 -xzf $(DL_DIR)/$(PKG_SOURCE) $(PKG_NAME)-$(PKG_VERSION)/c

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin/
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/zsync $(STAGING_DIR_HOST)/bin/zsync
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/zsyncmake $(STAGING_DIR_HOST)/bin/zsyncmake
endef

define Host/Clean
endef

$(eval $(call HostBuild))
