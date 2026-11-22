# SPDX-License-Identifier: GPL-2.0-only
#
# Siflower SF21 DPNS (NPU) dataplane drivers with nf_flow_table hardware offload
# (NAT / routing), ported from the Siflower OpenWrt 22.03 BSP.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=sf21-dpns
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=
PKG_MAINTAINER:=Mateusz Trzesiński <matjss3346@gmail.com>

include $(INCLUDE_DIR)/package.mk

define KernelPackage/sf21-dpns
  SUBMENU:=Network Devices
  TITLE:=Siflower SF21 DPNS (NPU) dataplane with flow offload
  DEPENDS:=@TARGET_siflower_sf21 +kmod-nf-flow
  FILES:= \
	$(PKG_BUILD_DIR)/dpns_genl/dpnsGenl.ko \
	$(PKG_BUILD_DIR)/dpns_common/dpnsCommon.ko \
	$(PKG_BUILD_DIR)/dpns_l2/dpnsL2.ko \
	$(PKG_BUILD_DIR)/dpns_vlan/dpnsVlan.ko \
	$(PKG_BUILD_DIR)/dpns_router/dpnsRouter.ko \
	$(PKG_BUILD_DIR)/dpns_nat/dpnsNat.ko \
	$(PKG_BUILD_DIR)/dpns_tmu/dpnsTmu.ko \
	$(PKG_BUILD_DIR)/dpns_acl/dpnsAcl.ko \
	$(PKG_BUILD_DIR)/dpns_mcast/dpnsMcast.ko \
	$(PKG_BUILD_DIR)/dpns/dpnsMain.ko
  AUTOLOAD:=$(call AutoLoad,48,dpnsGenl dpnsCommon dpnsL2 dpnsVlan dpnsRouter dpnsNat dpnsTmu dpnsAcl dpnsMcast dpnsMain,1)
endef

define KernelPackage/sf21-dpns/description
 Drivers for the Dataplane Network Subsystem (NPU) of the Siflower SF21H8898.
 Provides L2/VLAN/router/NAT hardware tables and nf_flow_table hardware
 offload (nftables "flags offload", fw4 flow_offloading_hw).
endef

DPNS_FEATURES:=COMMON GENL L2 VLAN ROUTER NAT TMU ACL MCAST

EXTRA_KCONFIG:= \
	CONFIG_SIFLOWER_DPNS=m \
	CONFIG_SIFLOWER_DPNS_TMU_GENL=y \
	CONFIG_SIFLOWER_DPNS_TMU_DEBUGFS=y \
	CONFIG_SIFLOWER_DPNS_MCAST_GENL=y \
	$(foreach f,$(DPNS_FEATURES),CONFIG_SIFLOWER_DPNS_$(f)=m)

DPNS_CFLAGS:= \
	-Wno-error -Wno-missing-prototypes \
	-DCONFIG_SIFLOWER_DPNS_TMU_GENL -DCONFIG_SIFLOWER_DPNS_TMU_DEBUGFS -DCONFIG_SIFLOWER_DPNS_MCAST_GENL \
	$(foreach f,$(DPNS_FEATURES),-DCONFIG_SIFLOWER_DPNS_$(f)) \
	-I$(PKG_BUILD_DIR)/include \
	-I$(LINUX_DIR)/drivers/net/ethernet/siflower

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	+$(KERNEL_MAKE) $(PKG_JOBS) \
		M="$(PKG_BUILD_DIR)" \
		DPNS_CFLAGS="$(DPNS_CFLAGS)" \
		$(EXTRA_KCONFIG) \
		modules
endef

define KernelPackage/sf21-dpns/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/hotplug.d/iface
	$(INSTALL_BIN) ./files/dpns-wan.sh $(1)/usr/sbin/dpns-wan
	$(INSTALL_DATA) ./files/dpns-wan.hotplug $(1)/etc/hotplug.d/iface/50-dpns-wan
	$(INSTALL_BIN) ./files/dpns-wan.init $(1)/etc/init.d/dpns-wan
endef

$(eval $(call KernelPackage,sf21-dpns))
