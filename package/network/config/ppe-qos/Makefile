include $(TOPDIR)/rules.mk

PKG_NAME:=ppe-qos
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=
PKG_MAINTAINER:=Julius Bairaktaris <julius@bairaktaris.de>

include $(INCLUDE_DIR)/package.mk

define Package/ppe-qos
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Qualcomm PPE hardware QoS classifiers
  DEPENDS:=@TARGET_qualcommax +dcb +tc +kmod-sched-flower +kmod-sched-act-pedit \
	+kmod-sched-prio +kmod-sched-ets +kmod-sched-mqprio +kmod-sched-act-police
  PKGARCH:=all
endef

define Package/ppe-qos/description
Configures the classifiers that pick a hardware egress queue on the
Qualcomm PPE: per-port DSCP-to-priority maps (via dcb app) and the
switch's small-packet priority, so latency-sensitive traffic is served
past the bulk queue of a shaped port. Pairs with sqm-scripts' hw_ppe.qos,
which owns the rates and the bulk queue depth.
endef

define Build/Compile
endef

define Package/ppe-qos/conffiles
/etc/config/ppe-qos
endef

define Package/ppe-qos/install
	$(INSTALL_DIR) $(1)/etc/config $(1)/etc/init.d
	$(INSTALL_CONF) ./files/ppe-qos.config $(1)/etc/config/ppe-qos
	$(INSTALL_BIN) ./files/ppe-qos.init $(1)/etc/init.d/ppe-qos
	$(INSTALL_DIR) $(1)/etc/hotplug.d/net
	$(INSTALL_DATA) ./files/ppe-qos.hotplug $(1)/etc/hotplug.d/net/20-ppe-qos
endef

$(eval $(call BuildPackage,ppe-qos))
