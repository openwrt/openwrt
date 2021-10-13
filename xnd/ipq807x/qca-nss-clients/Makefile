include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=qca-nss-clients
#PKG_SOURCE_PROTO:=git
PKG_BRANCH:=master
PKG_RELEASE:=2
#PKG_SOURCE_URL:=https://source.codeaurora.org/quic/qsdk/oss/lklm/nss-clients/
PKG_VERSION:=153998d70fdba508a59a28c13a606032cbf32686

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_VERSION)

#PKG_BUILD_DEPENDS := PACKAGE_kmod-qca-nss-crypto:kmod-qca-nss-crypto
MAKE_OPTS:=

include $(INCLUDE_DIR)/package.mk

# Keep default as ipq806x for branches that does not have subtarget framework
subtarget:=$(SUBTARGET)

# DTLS Manager v2.0 for Hawkeye/Cypress
  DTLSMGR_DIR:=v2.0
# IPsec Manager v2.0 for Hawkeye/Cypress
  IPSECMGR_DIR:=v2.0

define KernelPackage/qca-nss-drv-tun6rd
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for NSS (connection manager) - tun6rd
  DEPENDS:=+kmod-qca-nss-drv +kmod-sit @!LINUX_3_18
  FILES:=$(PKG_BUILD_DIR)/qca-nss-tun6rd.ko
  AUTOLOAD:=$(call AutoLoad,60,qca-nss-tun6rd)
endef

define KernelPackage/qca-nss-drv-tun6rd/Description
Kernel modules for NSS connection manager - Support for 6rd tunnel
endef

define KernelPackage/qca-nss-drv-dtlsmgr
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for NSS (connection manager) - dtlsmgr
  DEPENDS:=+kmod-qca-nss-drv +kmod-qca-nss-cfi-cryptoapi @!LINUX_3_18
  FILES:=$(PKG_BUILD_DIR)/dtls/$(DTLSMGR_DIR)/qca-nss-dtlsmgr.ko
endef

define KernelPackage/qca-nss-drv-dtls/Description
Kernel modules for NSS connection manager - Support for DTLS sessions
endef

define KernelPackage/qca-nss-drv-tlsmgr
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for NSS (connection manager) - tlsmgr
  DEPENDS:=@TARGET_ipq_ipq807x||TARGET_ipq_ipq807x_ipq807x||TARGET_ipq807x||TARGET_ipq807x_ipq807x||TARGET_ipq_ipq60xx||TARGET_ipq807x_ipq60xx +kmod-qca-nss-drv +kmod-qca-nss-cfi @!LINUX_3_18
  FILES:=$(PKG_BUILD_DIR)/tls/qca-nss-tlsmgr.ko
endef

define KernelPackage/qca-nss-drv-tls/Description
Kernel modules for NSS connection manager - Support for TLS sessions
endef

define KernelPackage/qca-nss-drv-l2tpv2
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for NSS (connection manager) - l2tp
  DEPENDS:=+kmod-qca-nss-drv +kmod-ppp +kmod-l2tp @!LINUX_3_18
  FILES:=$(PKG_BUILD_DIR)/l2tp/l2tpv2/qca-nss-l2tpv2.ko
  AUTOLOAD:=$(call AutoLoad,51,qca-nss-l2tpv2)
endef

define KernelPackage/qca-nss-drv-l2tp/Description
Kernel modules for NSS connection manager - Support for l2tp tunnel
endef

define KernelPackage/qca-nss-drv-pptp
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for NSS (connection manager) - PPTP
  DEPENDS:=+kmod-qca-nss-drv +kmod-pptp @!LINUX_3_18
  FILES:=$(PKG_BUILD_DIR)/pptp/qca-nss-pptp.ko
  AUTOLOAD:=$(call AutoLoad,51,qca-nss-pptp)
endef

define KernelPackage/qca-nss-drv-pptp/Description
Kernel modules for NSS connection manager - Support for PPTP tunnel
endef

define KernelPackage/qca-nss-drv-pppoe
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for NSS (connection manager) - PPPoE
  DEPENDS:=+kmod-qca-nss-drv +kmod-pppoe @!LINUX_3_18 \
		+!(TARGET_ipq_ipq807x_QSDK_256||TARGET_ipq_ipq60xx_QSDK_256):kmod-bonding
  FILES:=$(PKG_BUILD_DIR)/pppoe/qca-nss-pppoe.ko
  AUTOLOAD:=$(call AutoLoad,51,qca-nss-pppoe)
endef

define KernelPackage/qca-nss-drv-pppoe/Description
Kernel modules for NSS connection manager - Support for PPPoE
endef

define KernelPackage/qca-nss-drv-map-t
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for NSS (connection manager) - MAP-T
  DEPENDS:=+kmod-qca-nss-drv +kmod-nat46 @!LINUX_3_18
  FILES:=$(PKG_BUILD_DIR)/map/map-t/qca-nss-map-t.ko
  AUTOLOAD:=$(call AutoLoad,51,qca-nss-map-t)
endef

define KernelPackage/qca-nss-drv-map-t/Description
Kernel modules for NSS connection manager - Support for MAP-T
endef

define KernelPackage/qca-nss-drv-gre
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for NSS (connection manager) - GRE
  DEPENDS:=@TARGET_ipq_ipq806x||TARGET_ipq806x||TARGET_ipq_ipq807x||TARGET_ipq_ipq807x_ipq807x||TARGET_ipq807x||TARGET_ipq807x_ipq807x||TARGET_ipq_ipq60xx||TARGET_ipq807x_ipq60xx||TARGET_ipq_ipq50xx||TARGET_ipq_ipq50xx_64 \
	   +kmod-qca-nss-drv @!LINUX_3_18 +kmod-gre6
  FILES:=$(PKG_BUILD_DIR)/gre/qca-nss-gre.ko $(PKG_BUILD_DIR)/gre/test/qca-nss-gre-test.ko
  AUTOLOAD:=$(call AutoLoad,51,qca-nss-gre)
endef

define KernelPackage/qca-nss-drv-gre/Description
Kernel modules for NSS connection manager - Support for GRE
endef

define KernelPackage/qca-nss-drv-tunipip6
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for NSS (connection manager) - DS-lite and ipip6 Tunnel
  DEPENDS:=+kmod-qca-nss-drv +kmod-iptunnel6 +kmod-ip6-tunnel @!LINUX_3_18
  FILES:=$(PKG_BUILD_DIR)/tunipip6/qca-nss-tunipip6.ko
  AUTOLOAD:=$(call AutoLoad,60,qca-nss-tunipip6)
endef

define KernelPackage/qca-nss-drv-tunipip6/Description
Kernel modules for NSS connection manager
Add support for DS-lite and ipip6 tunnel
endef

define KernelPackage/qca-nss-drv-profile
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=+kmod-qca-nss-drv @!LINUX_3_18
  TITLE:=Profiler for QCA NSS driver (IPQ806x)
  FILES:=$(PKG_BUILD_DIR)/profiler/qca-nss-profile-drv.ko
endef

define KernelPackage/qca-nss-drv-profile/Description
This package contains a NSS driver profiler for QCA chipset
endef

define KernelPackage/qca-nss-drv-ipsecmgr
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for NSS (ipsec manager) - ipsecmgr
  DEPENDS:=@TARGET_ipq806x||TARGET_ipq_ipq806x||TARGET_ipq_ipq807x||TARGET_ipq_ipq807x_ipq807x||TARGET_ipq807x||TARGET_ipq807x_ipq807x||TARGET_ipq_ipq60xx||TARGET_ipq807x_ipq60xx \
		+kmod-qca-nss-drv +kmod-qca-nss-cfi-cryptoapi +kmod-qca-nss-cfi-ocf @!LINUX_3_18
ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-l2tpv2),)
  DEPENDS:=+kmod-qca-nss-drv-l2tpv2
endif
  FILES:=$(PKG_BUILD_DIR)/ipsecmgr/$(IPSECMGR_DIR)/qca-nss-ipsecmgr.ko
  AUTOLOAD:=$(call AutoLoad,60,qca-nss-ipsecmgr)
endef

define KernelPackage/qca-nss-drv-ipsecmgr/Description
Kernel module for NSS IPsec offload manager
endef

define KernelPackage/qca-nss-drv-ipsecmgr-klips
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for NSS (ipsec klips)
  DEPENDS:=@TARGET_ipq_ipq807x||TARGET_ipq_ipq807x_ipq807x||TARGET_ipq807x||TARGET_ipq_ipq60xx||TARGET_ipq807x_ipq60xx \
		+kmod-qca-nss-drv-ipsecmgr kmod-qca-nss-ecm
  FILES:=$(PKG_BUILD_DIR)/ipsecmgr/$(IPSECMGR_DIR)/plugins/klips/qca-nss-ipsec-klips.ko
endef

define KernelPackage/qca-nss-drv-ipsecmgr-klips/Description
NSS Kernel module for IPsec klips offload
endef


define KernelPackage/qca-nss-drv-capwapmgr
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=+kmod-qca-nss-drv +kmod-qca-nss-drv-dtlsmgr @!LINUX_3_18
  TITLE:=NSS CAPWAP Manager for QCA NSS driver (IPQ806x)
  FILES:=$(PKG_BUILD_DIR)/capwapmgr/qca-nss-capwapmgr.ko
endef

define KernelPackage/qca-nss-drv-capwapmgr/Description
This package contains a NSS CAPWAP Manager
endef

define KernelPackage/qca-nss-drv-bridge-mgr
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for NSS bridge manager
  DEPENDS:=@TARGET_ipq_ipq807x||TARGET_ipq_ipq807x_ipq807x||TARGET_ipq807x||TARGET_ipq807x_ipq807x||TARGET_ipq_ipq60xx||TARGET_ipq807x_ipq60xx \
		+TARGET_ipq_ipq807x:kmod-qca-nss-drv-vlan-mgr \
		+TARGET_ipq_ipq807x_ipq807x:kmod-qca-nss-drv-vlan-mgr \
		+TARGET_ipq807x:kmod-qca-nss-drv-vlan-mgr \
		+TARGET_ipq807x_ipq807x:kmod-qca-nss-drv-vlan-mgr \
		+TARGET_ipq_ipq60xx:kmod-qca-nss-drv-vlan-mgr \
		+TARGET_ipq807x_ipq60xx:kmod-qca-nss-drv-vlan-mgr @!LINUX_3_18 \
		+!(TARGET_ipq_ipq807x_QSDK_256||TARGET_ipq_ipq60xx_QSDK_256):kmod-bonding
  FILES:=$(PKG_BUILD_DIR)/bridge/qca-nss-bridge-mgr.ko
  AUTOLOAD:=$(call AutoLoad,51,qca-nss-bridge-mgr)
endef

define KernelPackage/qca-nss-drv-bridge-mgr/Description
Kernel modules for NSS bridge manager
endef

define KernelPackage/qca-nss-drv-vlan-mgr
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for NSS vlan manager
  DEPENDS:=@TARGET_ipq_ipq807x||TARGET_ipq_ipq807x_ipq807x||TARGET_ipq807x||TARGET_ipq807x_ipq807x||TARGET_ipq_ipq60xx||TARGET_ipq807x_ipq60xx +kmod-qca-nss-drv @!LINUX_3_18 \
		+!(TARGET_ipq_ipq807x_QSDK_256||TARGET_ipq_ipq60xx_QSDK_256):kmod-bonding
  FILES:=$(PKG_BUILD_DIR)/vlan/qca-nss-vlan.ko
  AUTOLOAD:=$(call AutoLoad,51,qca-nss-vlan)
endef

define KernelPackage/qca-nss-drv-vlan-mgr/Description
Kernel modules for NSS vlan manager
endef

define KernelPackage/qca-nss-drv-qdisc
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Support
  TITLE:=Qdisc for configuring shapers in NSS
  DEPENDS:=+kmod-qca-nss-drv @!LINUX_3_18
  FILES:=$(PKG_BUILD_DIR)/nss_qdisc/qca-nss-qdisc.ko
  AUTOLOAD:=$(call AutoLoad,58,qca-nss-qdisc)
endef

define KernelPackage/qca-nss-drv-qdisc/Description
Linux qdisc that aids in configuring shapers in the NSS
endef

define KernelPackage/qca-nss-drv-igs
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Support
  TITLE:=Action for offloading traffic to an IFB interface to perform ingress shaping.
  DEPENDS:=@TARGET_ipq_ipq807x||TARGET_ipq_ipq807x_ipq807x||TARGET_ipq_ipq60xx||TARGET_ipq807x_ipq60xx||TARGET_ipq_ipq50xx||TARGET_ipq_ipq50xx_64 \
	+kmod-qca-nss-drv +kmod-sched-core +kmod-ifb +kmod-qca-nss-drv-qdisc @!LINUX_3_18
  FILES:=$(PKG_BUILD_DIR)/nss_qdisc/igs/act_nssmirred.ko
endef

define KernelPackage/qca-nss-drv-igs/Description
Linux action that helps in offloading traffic to an IFB interface to perform ingress shaping.
endef

define KernelPackage/qca-nss-drv-lag-mgr
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for NSS LAG manager
  DEPENDS:=+kmod-qca-nss-drv  @!LINUX_3_18 \
	   +TARGET_ipq_ipq807x:kmod-qca-nss-drv-vlan-mgr \
	   +TARGET_ipq_ipq807x_ipq807x:kmod-qca-nss-drv-vlan-mgr @!LINUX_3_18 \
	   +TARGET_ipq807x:kmod-qca-nss-drv-vlan-mgr \
	   +TARGET_ipq807x_ipq807x:kmod-qca-nss-drv-vlan-mgr @!LINUX_3_18 \
	   +TARGET_ipq_ipq60xx:kmod-qca-nss-drv-vlan-mgr @!LINUX_3_18 \
	   +TARGET_ipq807x_ipq60xx:kmod-qca-nss-drv-vlan-mgr @!LINUX_3_18 \
	   +kmod-bonding
  FILES:=$(PKG_BUILD_DIR)/lag/qca-nss-lag-mgr.ko
  AUTOLOAD:=$(call AutoLoad,51,qca-nss-lag-mgr)
endef

define KernelPackage/qca-nss-drv-lag-mgr/Description
Kernel modules for NSS LAG manager
endef

define KernelPackage/qca-nss-drv-netlink
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=@TARGET_ipq807x||TARGET_ipq_ipq807x||TARGET_ipq807x_ipq807x||TARGET_ipq_ipq807x_ipq807x||TARGET_ipq_ipq60xx||TARGET_ipq807x_ipq60xx||TARGET_ipq_ipq50xx||TARGET_ipq_ipq50xx_64 \
		+kmod-qca-nss-drv @!LINUX_3_18 \
		+PACKAGE_kmod-qca-nss-drv-ipsecmgr:kmod-qca-nss-drv-ipsecmgr \
		+PACKAGE_kmod-qca-nss-drv-dtlsmgr:kmod-qca-nss-drv-dtlsmgr \
		+PACKAGE_kmod-qca-nss-drv-capwapmgr:kmod-qca-nss-drv-capwapmgr @!LINUX_3_18
  TITLE:=NSS NETLINK Manager for QCA NSS driver
  FILES:=$(PKG_BUILD_DIR)/netlink/qca-nss-netlink.ko
endef

define KernelPackage/qca-nss-drv-netlink/Description
Kernel module for NSS netlink manager
endef

define KernelPackage/qca-nss-drv-ovpn-mgr
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for NSS OpenVPN manager
  DEPENDS:=+kmod-qca-nss-drv +kmod-qca-nss-cfi +kmod-tun +kmod-ipt-conntrack @!LINUX_3_18 \
	  @TARGET_ipq_ipq807x||TARGET_ipq_ipq807x_ipq807x||TARGET_ipq_ipq60xx||TARGET_ipq807x_ipq60xx
  FILES:=$(PKG_BUILD_DIR)/openvpn/src/qca-nss-ovpn-mgr.ko
endef

define KernelPackage/qca-nss-drv-ovpn-mgr/Description
Kernel module for NSS OpenVPN manager
endef

define KernelPackage/qca-nss-drv-ovpn-link
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for interfacing NSS OpenVPN manager with ECM
  DEPENDS:=+kmod-qca-nss-drv-ovpn-mgr +kmod-qca-nss-ecm-premium @!LINUX_3_18 \
	  @TARGET_ipq_ipq807x||TARGET_ipq_ipq807x_ipq807x||TARGET_ipq_ipq60xx||TARGET_ipq807x_ipq60xx
  FILES:=$(PKG_BUILD_DIR)/openvpn/plugins/qca-nss-ovpn-link.ko
endef

define KernelPackage/qca-nss-drv-ovpn-link/Description
This module registers with ECM and communicates with NSS OpenVPN manager for supporting OpenVPN offload.
endef

define KernelPackage/qca-nss-drv-pvxlanmgr
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=+kmod-qca-nss-drv @!LINUX_3_18
  TITLE:=NSS PVXLAN Manager for QCA NSS driver
  FILES:=$(PKG_BUILD_DIR)/pvxlanmgr/qca-nss-pvxlanmgr.ko
endef

define KernelPackage/qca-nss-drv-pvxlanmgr/Description
Kernel module for managing NSS PVxLAN
endef

define KernelPackage/qca-nss-drv-eogremgr
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=+kmod-qca-nss-drv +kmod-qca-nss-drv-gre @!LINUX_3_18
  TITLE:=NSS EOGRE Manager for QCA NSS driver
  FILES:=$(PKG_BUILD_DIR)/eogremgr/qca-nss-eogremgr.ko
endef

define KernelPackage/qca-nss-drv-eogremgr/Description
Kernel module for managing NSS EoGRE
endef

define KernelPackage/qca-nss-drv-clmapmgr
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=+kmod-qca-nss-drv +kmod-qca-nss-drv-eogremgr @!LINUX_3_18
  TITLE:=NSS clmap Manager for QCA NSS driver
  FILES:=$(PKG_BUILD_DIR)/clmapmgr/qca-nss-clmapmgr.ko
endef

define KernelPackage/qca-nss-drv-clmapmgr/Description
Kernel module for managing NSS clmap
endef

define KernelPackage/qca-nss-drv-vxlanmgr
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=+kmod-qca-nss-drv +kmod-vxlan @!LINUX_3_18
  TITLE:=NSS VxLAN Manager for QCA NSS driver
  FILES:=$(PKG_BUILD_DIR)/vxlanmgr/qca-nss-vxlanmgr.ko
  AUTOLOAD:=$(call AutoLoad,51,qca-nss-vxlanmgr)
endef

define KernelPackage/qca-nss-drv-vxlanmgr/Description
Kernel module for managing NSS VxLAN
endef

define KernelPackage/qca-nss-drv-match
 SECTION:=kernel
 CATEGORY:=Kernel modules
 SUBMENU:=Network Devices
 DEPENDS:=+kmod-qca-nss-drv @!LINUX_3_18
 TITLE:=NSS Match for QCA NSS driver
 FILES:=$(PKG_BUILD_DIR)/match/qca-nss-match.ko
endef

define KernelPackage/qca-nss-drv-match/Description
Kernel module for managing NSS Match
endef

define KernelPackage/qca-nss-drv-mirror
 SECTION:=kernel
 CATEGORY:=Kernel modules
 SUBMENU:=Network Support
 TITLE:=Module for mirroring packets from NSS to host.
 DEPENDS:=+kmod-qca-nss-drv @!LINUX_3_18
 FILES:=$(PKG_BUILD_DIR)/mirror/qca-nss-mirror.ko
endef

define KernelPackage/qca-nss-drv-mirror/Description
Kernel module for managing NSS Mirror
endef

define Build/InstallDev/qca-nss-clients
	$(INSTALL_DIR) $(1)/usr/include/qca-nss-clients
	$(CP) $(PKG_BUILD_DIR)/netlink/include/* $(1)/usr/include/qca-nss-clients/
	$(CP) $(PKG_BUILD_DIR)/exports/* $(1)/usr/include/qca-nss-clients/
endef

define KernelPackage/qca-nss-drv-wifi-meshmgr
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=+kmod-qca-nss-drv @!LINUX_3_18
  TITLE:=NSS WiFi-Mesh Manager for QCA NSS driver
  FILES:=$(PKG_BUILD_DIR)/wifi_meshmgr/qca-nss-wifi-meshmgr.ko
  AUTOLOAD:=$(call AutoLoad,51,qca-nss-wifi-meshmgr)
endef

define KernelPackage/qca-nss-drv-wifi-meshmgr/Description
Kernel module for WiFi Mesh manager
endef

define Build/InstallDev
	$(call Build/InstallDev/qca-nss-clients,$(1))
endef

define KernelPackage/qca-nss-drv-ovpn-mgr/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/qca-nss-ovpn.init $(1)/etc/init.d/qca-nss-ovpn
endef

define KernelPackage/qca-nss-drv-ipsecmgr-klips/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/qca-nss-ipsec $(1)/etc/init.d/qca-nss-ipsec
endef

define KernelPackage/qca-nss-drv-igs/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/qca-nss-mirred.init $(1)/etc/init.d/qca-nss-mirred
endef

EXTRA_CFLAGS+= \
	-I$(STAGING_DIR)/usr/include/qca-nss-drv \
	-I$(STAGING_DIR)/usr/include/qca-nss-crypto \
	-I$(STAGING_DIR)/usr/include/qca-nss-cfi \
	-I$(STAGING_DIR)/usr/include/qca-nss-gmac \
	-I$(STAGING_DIR)/usr/include/qca-ssdk \
	-I$(STAGING_DIR)/usr/include/qca-ssdk/fal \
	-I$(STAGING_DIR)/usr/include/nat46

# Build individual packages if selected
ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-profile),)
MAKE_OPTS+=profile=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-capwapmgr),)
MAKE_OPTS+=capwapmgr=y
EXTRA_CFLAGS += -DNSS_CAPWAPMGR_ONE_NETDEV
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-tun6rd),)
MAKE_OPTS+=tun6rd=m
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-dtlsmgr),)
MAKE_OPTS+=dtlsmgr=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-tlsmgr),)
MAKE_OPTS+=tlsmgr=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-l2tpv2),)
MAKE_OPTS+=l2tpv2=y
EXTRA_CFLAGS += -DNSS_L2TPV2_ENABLED
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-pptp),)
MAKE_OPTS+=pptp=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-map-t),)
MAKE_OPTS+=map-t=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-tunipip6),)
MAKE_OPTS+=tunipip6=m
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-qdisc),)
MAKE_OPTS+=qdisc=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-igs),)
MAKE_OPTS+=igs=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-ipsecmgr),)
EXTRA_CFLAGS+= -I$(PKG_BUILD_DIR)/exports
MAKE_OPTS+=ipsecmgr=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-ipsecmgr-klips),)
EXTRA_CFLAGS+= -I$(STAGING_DIR)/usr/include/qca-nss-ecm
MAKE_OPTS+=ipsecmgr-klips=m
endif


ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-bridge-mgr),)
MAKE_OPTS+=bridge-mgr=y
#enable OVS bridge if ovsmgr is enabled
ifneq ($(CONFIG_PACKAGE_kmod-qca-ovsmgr),)
MAKE_OPTS+= NSS_BRIDGE_MGR_OVS_ENABLE=y
EXTRA_CFLAGS+= -I$(STAGING_DIR)/usr/include/qca-ovsmgr
endif
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-vlan-mgr),)
MAKE_OPTS+=vlan-mgr=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-lag-mgr),)
MAKE_OPTS+=lag-mgr=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-gre),)
EXTRA_CFLAGS+= -I$(PKG_BUILD_DIR)/exports
MAKE_OPTS+=gre=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-pppoe),)
MAKE_OPTS+=pppoe=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-netlink),)
MAKE_OPTS+=netlink=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-ovpn-mgr),)
MAKE_OPTS+=ovpn-mgr=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-ovpn-link),)
MAKE_OPTS+=ovpn-link=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-pvxlanmgr),)
MAKE_OPTS+=pvxlanmgr=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-eogremgr),)
MAKE_OPTS+=eogremgr=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-clmapmgr),)
MAKE_OPTS+=clmapmgr=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-vxlanmgr),)
MAKE_OPTS+=vxlanmgr=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-match),)
MAKE_OPTS+=match=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-mirror),)
MAKE_OPTS+=mirror=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-wifi-meshmgr),)
MAKE_OPTS+=wifi-meshmgr=y
endif

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" $(strip $(MAKE_OPTS)) \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		M="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		SoC="$(subtarget)" \
		DTLSMGR_DIR="$(DTLSMGR_DIR)" \
		IPSECMGR_DIR="$(IPSECMGR_DIR)" \
		modules
endef

$(eval $(call KernelPackage,qca-nss-drv-profile))
#$(eval $(call KernelPackage,qca-nss-drv-capwapmgr))
$(eval $(call KernelPackage,qca-nss-drv-tun6rd))
#$(eval $(call KernelPackage,qca-nss-drv-dtlsmgr))
$(eval $(call KernelPackage,qca-nss-drv-l2tpv2))
$(eval $(call KernelPackage,qca-nss-drv-pptp))
$(eval $(call KernelPackage,qca-nss-drv-pppoe))
$(eval $(call KernelPackage,qca-nss-drv-map-t))
$(eval $(call KernelPackage,qca-nss-drv-tunipip6))
$(eval $(call KernelPackage,qca-nss-drv-qdisc))
$(eval $(call KernelPackage,qca-nss-drv-igs))
#$(eval $(call KernelPackage,qca-nss-drv-netlink))
#$(eval $(call KernelPackage,qca-nss-drv-ipsecmgr))
#$(eval $(call KernelPackage,qca-nss-drv-ipsecmgr-klips))
$(eval $(call KernelPackage,qca-nss-drv-bridge-mgr))
$(eval $(call KernelPackage,qca-nss-drv-vlan-mgr))
$(eval $(call KernelPackage,qca-nss-drv-lag-mgr))
$(eval $(call KernelPackage,qca-nss-drv-gre))
#$(eval $(call KernelPackage,qca-nss-drv-ovpn-mgr))
#$(eval $(call KernelPackage,qca-nss-drv-ovpn-link))
$(eval $(call KernelPackage,qca-nss-drv-pvxlanmgr))
$(eval $(call KernelPackage,qca-nss-drv-eogremgr))
$(eval $(call KernelPackage,qca-nss-drv-clmapmgr))
$(eval $(call KernelPackage,qca-nss-drv-vxlanmgr))
$(eval $(call KernelPackage,qca-nss-drv-match))
#$(eval $(call KernelPackage,qca-nss-drv-tlsmgr))
$(eval $(call KernelPackage,qca-nss-drv-mirror))
$(eval $(call KernelPackage,qca-nss-drv-wifi-meshmgr))
