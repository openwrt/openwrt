include $(TOPDIR)/rules.mk

PKG_NAME:=econet-xpon-config
PKG_VERSION:=1
PKG_RELEASE:=1
PKG_MAINTAINER:=AK Sharma <maskachoska@yahoo.com>
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=COPYING

include $(INCLUDE_DIR)/package.mk

define Package/econet-xpon-config
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=WWAN
  TITLE:=EcoNet EN7528 GPON ONU identity and module loader
  DEPENDS:=@TARGET_econet +kmod-econet-xpon
  PKGARCH:=all
endef

define Package/econet-xpon-config/description
  UCI configuration (config econet-xpon: ONU serial / PLOAM password / WAN MAC)
  and the init.d loader for the econet-xpon GPON driver. The loader loads the
  kernel module with the identity from UCI; if no serial is provisioned the
  driver is not loaded (so it never presents the MediaTek default serial to the
  OLT). No credentials are hardcoded.
endef

define Build/Compile
endef

define Package/econet-xpon-config/conffiles
/etc/config/econet-xpon
endef

define Package/econet-xpon-config/install
	$(INSTALL_DIR) $(1)/etc/config $(1)/etc/init.d
	$(INSTALL_CONF) ./files/etc/config/econet-xpon $(1)/etc/config/econet-xpon
	$(INSTALL_BIN) ./files/etc/init.d/econet-xpon $(1)/etc/init.d/econet-xpon
endef

$(eval $(call BuildPackage,econet-xpon-config))
