include $(TOPDIR)/rules.mk

PKG_NAME:=qca-nss-fw
PKG_VERSION:=383

PKG_MAINTAINER:=John Crispin <john@phrozen.org>

include $(INCLUDE_DIR)/package.mk

define Package/qca-nss-fw-default
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=
endef

define Package/qca-nss-fw-ipq60xx
$(Package/qca-nss-fw-default)
  TITLE:=NSS firmware for IPQ60xx devices
  DEPENDS:=@TARGET_ipq807x_ipq60xx
endef

define Package/qca-nss-fw-ipq807x
$(Package/qca-nss-fw-default)
  TITLE:=NSS firmware for IPQ807x devices
  DEPENDS:=@TARGET_ipq807x_ipq807x
endef

define Package/qca-nss-fw-ipq50xx
$(Package/qca-nss-fw-default)
  TITLE:=NSS firmware for IPQ50xx devices
  DEPENDS:=@TARGET_ipq807x_ipq50xx
endef

define Package/qca-nss-fw-ipq60xx/description
Retail NSS firmware for IPQ60xx from QCA
endef

define Package/qca-nss-fw-ipq807x/description
Retail NSS firmware for IPQ807x from QCA
endef

define Package/qca-nss-fw-ipq50xx/description
Retail NSS firmware for IPQ50xx from QCA
endef

define Build/Compile

endef

define Package/qca-nss-fw-ipq60xx/install
	$(INSTALL_DIR) $(1)/lib/firmware/
	$(INSTALL_DATA) ./files/IPQ6018/* \
		$(1)/lib/firmware/
endef

define Package/qca-nss-fw-ipq807x/install
	$(INSTALL_DIR) $(1)/lib/firmware/
	$(INSTALL_DATA) ./files/IPQ8074/* \
		$(1)/lib/firmware/
endef

define Package/qca-nss-fw-ipq50xx/install
	$(INSTALL_DIR) $(1)/lib/firmware/
	$(INSTALL_DATA) ./files/IPQ5018/* \
		$(1)/lib/firmware/
endef

$(eval $(call BuildPackage,qca-nss-fw-ipq50xx))
$(eval $(call BuildPackage,qca-nss-fw-ipq60xx))
$(eval $(call BuildPackage,qca-nss-fw-ipq807x))
