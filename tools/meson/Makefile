include $(TOPDIR)/rules.mk

PKG_NAME:=meson
PKG_VERSION:=0.59.1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/mesonbuild/meson/releases/download/$(PKG_VERSION)
PKG_HASH:=db586a451650d46bbe10984a87b79d9bcdc1caebf38d8e189f8848f8d502356d

PKG_MAINTAINER:=Andre Heider <a.heider@gmail.com>
PKG_LICENSE:=Apache-2.0
PKG_LICENSE_FILES:=COPYING

include $(INCLUDE_DIR)/host-build.mk

define Host/Configure
endef

define Host/Compile
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/lib/meson
	$(CP) $(HOST_BUILD_DIR)/* $(STAGING_DIR_HOST)/lib/meson/
endef

define Host/Clean
	$(call Host/Clean/Default)
	rm -rf $(STAGING_DIR_HOST)/lib/meson
endef

$(eval $(call HostBuild))
