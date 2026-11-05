# SPDX-License-Identifier: GPL-2.0

include $(TOPDIR)/rules.mk

PKG_NAME:=gpond
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=COPYING
PKG_MAINTAINER:=Lukasz M <lukasz1992m+openwrt@gmail.com>
PKG_BUILD_DEPENDS:=an7552-gpon

include $(INCLUDE_DIR)/package.mk

define Package/gpond
  SECTION:=net
  CATEGORY:=Network
  TITLE:=AN7552/AN7563 GPON daemon (configuration, OMCI)
  DEPENDS:=@TARGET_airoha_an7563 +kmod-an7552-gpon +libubox +libubus +libuci +libnl-tiny \
	+tc-tiny +kmod-sched-core +kmod-sched-act-police +kmod-sched-flower \
	+kmod-sched-act-vlan +ip-bridge +omcproxy \
	+kmod-nft-bridge +nftables-json
endef

define Package/gpond/description
 Userspace part of the AN7552/AN7563 GPON stack (gpond).  Keeps the persistent
 configuration in /etc/config/gpon and applies it to the an7552-gpon
 driver over generic netlink, runs the GPON OMCI MIB (with its Ethernet
 side written to /etc/config/network for netifd: bridges, VLAN devices,
 UNI settings, IP host and VEIP WAN interfaces) and exposes the "gpon" ubus
 object.  Runs under procd; devices and interfaces are created by
 netifd, policers by the gpon-qos service, tc VLAN filters by the
 gpon-vlan service, the per port multicast group limits by the gpon-mcast
 service (bridge link mcast_max_groups, plus its own nft table), the IGMP
 proxy by omcproxy (/etc/config/omcproxy).  The firewall of the system is
 never touched: the ping / traceroute response the OLT asks for (ME 134) is
 only reported, so whether a host answers is the firewall configuration.
endef

define Package/gpond/conffiles
/etc/config/gpon
/usr/lib/gpon/gpon.json
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/. $(PKG_BUILD_DIR)/
endef

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		STAGING_INC="$(STAGING_DIR)/usr/include"
endef

define Package/gpond/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/usr/lib/gpon $(1)/etc/init.d \
		$(1)/etc/config $(1)/etc/uci-defaults $(1)/etc/hotplug.d/iface \
		$(1)/etc/hotplug.d/net
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/gpond $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/gpond.init $(1)/etc/init.d/gpond
	$(INSTALL_BIN) ./files/gpon-qos.init $(1)/etc/init.d/gpon-qos
	$(INSTALL_BIN) ./files/gpon-vlan.init $(1)/etc/init.d/gpon-vlan
	$(INSTALL_BIN) ./files/gpon-mcast.init $(1)/etc/init.d/gpon-mcast
	$(INSTALL_CONF) ./files/gpon.config $(1)/etc/config/gpon
	$(INSTALL_BIN) ./files/gpon.uci-defaults $(1)/etc/uci-defaults/90-gpon-igmp
	$(INSTALL_BIN) ./files/pon-activate.hotplug \
		$(1)/etc/hotplug.d/iface/05-pon-activate
	$(INSTALL_BIN) ./files/pon-lower.hotplug \
		$(1)/etc/hotplug.d/net/05-pon-lower
endef

$(eval $(call BuildPackage,gpond))
