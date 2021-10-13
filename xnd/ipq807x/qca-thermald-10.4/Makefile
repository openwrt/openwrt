include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=qca-thermald-10.4
PKG_RELEASE:=1

QCA_THERMALD_MAKE_OPTS:= \
        CROSS=$(TARGET_CROSS) \
        THERMALDIR=$(PKG_BUILD_DIR) \

include $(INCLUDE_DIR)/package.mk

define Package/qca-thermald-10.4
  SECTION:=utils
  CATEGORY:=Utilities
  URL:=http://www.qca.qualcomm.com
  MAINTAINER:=Qualcomm Atheros
  DEPENDS:=+libpthread @TARGET_ipq807x
  TITLE:=Thermal Mitigation daemon
endef

define Package/qca-thermald-10.4/description
  This package is IPQ Thermal Mitigation daemon.
endef

define Build/Compile
	LDFLAGS="$(TARGET_LDFLAGS)" \
	$(MAKE) -C $(PKG_BUILD_DIR) $(strip $(QCA_THERMALD_MAKE_OPTS))
endef

define Package/qca-thermald-10.4/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/install/usr/sbin/thermald $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/etc/thermal
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/install/etc/thermal/ipq-thermald-8064.conf $(1)/etc/thermal
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/install/etc/thermal/ipq-thermald-8066.conf $(1)/etc/thermal
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/install/etc/thermal/ipq-thermald-8069.conf $(1)/etc/thermal
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/thermal.init $(1)/etc/init.d/thermal
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_BIN) ./files/thermal.config $(1)/etc/config/thermal
endef

$(eval $(call BuildPackage,qca-thermald-10.4))
