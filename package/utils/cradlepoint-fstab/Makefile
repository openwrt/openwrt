include $(TOPDIR)/rules.mk

PKG_NAME:=cradlepoint-fstab
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_LICENSE:=GPL-2.0-or-later

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/cradlepoint-fstab/Default
	SECTION:=firmware
	CATEGORY:=Firmware
	TITLE:=fstab Options for Cradlepoint
endef

define Build/Compile/Default
	echo "No build necessary"
endef

define Package/cradlepoint-fstab-ibr-c
	$(call Package/cradlepoint-fstab/Default)
	TITLE+= (ibr-c)
endef

define Package/cradlepoint-fstab-ibr-c/description
	Allows auto-mounting of differently named ubifs partition for IBR900.
endef

define Package/cradlepoint-fstab-ibr-c/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) files/ibr-c-nand-fstab $(1)/etc/config/fstab
endef

$(eval $(call BuildPackage,cradlepoint-fstab-ibr-c))
