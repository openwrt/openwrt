include $(TOPDIR)/rules.mk

PKG_NAME:=wf810df-nss
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only

include $(INCLUDE_DIR)/package.mk

define Package/wf810df-nss
  SECTION:=net
  CATEGORY:=Network
  TITLE:=WF810DF native WAN and AP NSS startup
  DEPENDS:=@TARGET_qualcommax_ipq50xx +kmod-qca-nss-drv +kmod-qca-nss-ecm +kmod-ath11k-ahb \
	+@NSS_DRV_WIFIOFFLOAD_ENABLE +@NSS_DRV_WIFI_EXT_VDEV_ENABLE +@ATH11K_NSS_SUPPORT
endef

define Build/Compile
endef

define Package/wf810df-nss/install
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/lib/functions
	$(INSTALL_BIN) ./files/wf810df-nss.init $(1)/etc/init.d/wf810df-nss
	$(INSTALL_DATA) ./files/wf810df-nss.sh $(1)/lib/functions/wf810df-nss.sh
endef

$(eval $(call BuildPackage,wf810df-nss))
