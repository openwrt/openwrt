include $(TOPDIR)/rules.mk

PKG_NAME:=qca-nss-firmware
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/qca-nss-firmware
  PKGARCH:=all
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=QCA NSS Firmware
endef

define Build/Compile
endef

define Package/qca-nss-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(CP) ./files/nss-firmware/qca-nss0-retail.bin $(1)/lib/firmware/qca-nss0.bin
	$(CP) ./files/nss-firmware/qca-nss1-retail.bin $(1)/lib/firmware/qca-nss1.bin
endef

$(eval $(call BuildPackage,qca-nss-firmware))
