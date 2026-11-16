include $(TOPDIR)/rules.mk

PKG_NAME:=qca-ppe-qos
PKG_RELEASE:=2
PKG_LICENSE:=GPL-2.0-only
PKG_MAINTAINER:=Julius Bairaktaris <julius@bairaktaris.de>

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/qca-ppe-qos
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Qualcomm PPE hardware QoS classifiers
  DEPENDS:=@(TARGET_qualcommax_ipq807x||TARGET_qualcommax_ipq60xx) \
	+KERNEL_DCB:dcb +tc +kmod-sched-flower +kmod-sched-act-pedit \
	+kmod-sched-prio +kmod-sched-ets +kmod-sched-mqprio +kmod-sched-act-police
endef

define Package/qca-ppe-qos/description
Configures the classifiers that pick a hardware egress queue on the
Qualcomm PPE: per-port DSCP-to-priority maps (via dcb app) and the
switch's small-packet priority, so latency-sensitive traffic is served
past the bulk queue of a shaped port. Pairs with sqm-scripts' hw_ppe.qos,
which owns the rates and the bulk queue depth, or ppe_cake.qos.
endef

define Build/Compile
endef

define Package/qca-ppe-qos/conffiles
/etc/config/qca-ppe-qos
endef

define Package/qca-ppe-qos/install
	$(INSTALL_DIR) $(1)/etc/config $(1)/etc/init.d
	$(INSTALL_CONF) ./files/qca-ppe-qos.config $(1)/etc/config/qca-ppe-qos
	$(INSTALL_BIN) ./files/qca-ppe-qos.init $(1)/etc/init.d/qca-ppe-qos
	$(INSTALL_DIR) $(1)/etc/hotplug.d/net
	$(INSTALL_DATA) ./files/qca-ppe-qos.hotplug $(1)/etc/hotplug.d/net/20-qca-ppe-qos
endef

$(eval $(call BuildPackage,qca-ppe-qos))
