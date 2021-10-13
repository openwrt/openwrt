include $(TOPDIR)/rules.mk

PKG_NAME:=ath11k-firmware
PKG_VERSION:=19

PKG_MAINTAINER:=John Crispin <john@phrozen.org>

include $(INCLUDE_DIR)/package.mk

define Package/ath11k-firmware-default
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=
endef

define Package/ath11k-firmware-ipq50xx
$(Package/ath11k-firmware-default)
  TITLE:=ath11k firmware for IPQ50xx devices
  DEPENDS:=@TARGET_ipq807x_ipq50xx
endef

define Package/ath11k-firmware-ipq60xx
$(Package/ath11k-firmware-default)
  TITLE:=ath11k firmware for IPQ60xx devices
  DEPENDS:=@TARGET_ipq807x_ipq60xx
endef

define Package/ath11k-firmware-ipq807x
$(Package/ath11k-firmware-default)
  TITLE:=ath11k firmware for IPQ807x devices
  DEPENDS:=@TARGET_ipq807x_ipq807x
endef

define Package/ath11k-firmware-qcn6122
$(Package/ath11k-firmware-default)
  TITLE:=ath11k firmware for QCN6122 devices
  DEPENDS:=@TARGET_ipq807x_ipq50xx
endef

define Package/ath11k-firmware-qcn9000
$(Package/ath11k-firmware-default)
  TITLE:=ath11k firmware for QCN9000 devices
  DEPENDS:=@TARGET_ipq807x
endef

define Package/ath11k-firmware-ipq50xx/description
Standard ath11k firmware for IPQ50xx from QCA
endef

define Package/ath11k-firmware-ipq60xx/description
Standard ath11k firmware for IPQ60xx from QCA
endef

define Package/ath11k-firmware-ipq807x/description
Standard ath11k firmware for IPQ807x from QCA
endef

define Package/ath11k-firmware-qcn6122/description
Standard ath11k firmware for QCN6122 from QCA
endef

define Package/ath11k-firmware-qcn9000/description
Standard ath11k firmware for QCN9000 from QCA
endef

define Build/Compile

endef

define Package/ath11k-firmware-ipq50xx/install
	$(INSTALL_DIR) $(1)/lib/firmware/IPQ5018
	$(INSTALL_DATA) ./files/IPQ5018/* \
		$(1)/lib/firmware/IPQ5018/
endef

define Package/ath11k-firmware-ipq60xx/install
	$(INSTALL_DIR) $(1)/lib/firmware/IPQ6018
	$(INSTALL_DATA) ./files/IPQ6018/* \
		$(1)/lib/firmware/IPQ6018/
endef

define Package/ath11k-firmware-ipq807x/install
	$(INSTALL_DIR) $(1)/lib/firmware/IPQ8074
	$(INSTALL_DATA) ./files/IPQ8074/* \
		$(1)/lib/firmware/IPQ8074
endef

define Package/ath11k-firmware-qcn6122/install
	$(INSTALL_DIR) $(1)/lib/firmware/qcn6122
	$(INSTALL_DATA) ./files/QCN6122/* \
		$(1)/lib/firmware/qcn6122/
endef

define Package/ath11k-firmware-qcn9000/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath11k/QCN9074/hw1.0/
	$(INSTALL_DATA) ./files/QCN9000/* \
		$(1)/lib/firmware/ath11k/QCN9074/hw1.0/
endef

$(eval $(call BuildPackage,ath11k-firmware-ipq50xx))
$(eval $(call BuildPackage,ath11k-firmware-ipq60xx))
$(eval $(call BuildPackage,ath11k-firmware-ipq807x))
$(eval $(call BuildPackage,ath11k-firmware-qcn6122))
$(eval $(call BuildPackage,ath11k-firmware-qcn9000))
