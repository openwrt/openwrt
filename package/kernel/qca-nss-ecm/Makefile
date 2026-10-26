include $(TOPDIR)/rules.mk

PKG_NAME:=qca-nss-ecm
# Adapted from kuncy7/nss-packages 5344fc35d79fc947b501e9feee5ced2b07c664fa.
PKG_RELEASE:=7
PKG_LICENSE:=ISC

# Source: https://git.codelinaro.org/clo/qsdk/oss/lklm/qca-nss-ecm.git @ 7894b769eeb64f1c44fde547a107a8b866997590
# (head of win.nss.1.0.r39, which carries every change of NHSS.QSDK.14.0.r9-00040-O;
# pre-generated 'git archive' of the commit).
# Fetched from our release mirror so the download is deterministic and hash-verified;
# git-proto regenerates the zstd archive locally and its hash varies by toolchain.
PKG_SOURCE_DATE:=2026-09-06
PKG_SOURCE_VERSION:=7894b769eeb64f1c44fde547a107a8b866997590
QSDK_VERSION:=14.0
PKG_VERSION:=$(QSDK_VERSION).$(subst -,.,$(PKG_SOURCE_DATE))~$(PKG_SOURCE_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(subst ~,.,$(PKG_VERSION)).tar.zst
PKG_SOURCE_URL:=https://github.com/kuncy7/nss-packages/releases/download/nss-source-tarballs
PKG_HASH:=a4d5f4c4a6c4bfac3ded4af993c88249f7afc4444ed78368f367eda4ef231886

PKG_BUILD_PARALLEL:=1
PKG_FLAGS:=nonshared
PKG_BUILD_FLAGS:=gc-sections lto

PKG_CONFIG_DEPENDS:= \
	CONFIG_KERNEL_IP_MROUTE \
	CONFIG_KERNEL_IPV6_MROUTE \
	CONFIG_NSS_DRV_PPPOE_ENABLE \
	CONFIG_NSS_DRV_RMNET_ENABLE \
	CONFIG_NSS_FIRMWARE_VERSION_11_4 \
	CONFIG_NSS_FIRMWARE_VERSION_12_5 \
	CONFIG_PACKAGE_kmod-ipsec \
	CONFIG_PACKAGE_kmod-nat46 \
	CONFIG_PACKAGE_kmod-pppoe \
	CONFIG_PACKAGE_kmod-qca-nss-drv \
	CONFIG_PACKAGE_kmod-qca-nss-drv-igs \
	CONFIG_TARGET_qualcommax_ipq50xx

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/qca-nss-ecm
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Support
  DEPENDS:=@TARGET_qualcommax_ipq50xx +kmod-nf-conntrack \
		   +kmod-qca-nss-drv +@NSS_DRV_IPV6_ENABLE +@NSS_DRV_VIRT_IF_ENABLE \
			   +PACKAGE_kmod-pppoe:kmod-pppoe
  TITLE:=QCA NSS Enhanced Connection Manager (ECM)
  FILES:=$(PKG_BUILD_DIR)/ecm.ko
  KCONFIG:= \
	CONFIG_NF_CONNTRACK_DSCPREMARK_EXT=y \
	CONFIG_NF_CONNTRACK_EVENTS=y
endef

define KernelPackage/qca-nss-ecm/Description
This package contains the QCA NSS Enhanced Connection Manager
endef

define KernelPackage/qca-nss-ecm/install
		$(INSTALL_DIR) $(1)/etc/init.d $(1)/usr/bin
		$(INSTALL_BIN) ./files/qca-nss-ecm.init $(1)/etc/init.d/qca-nss-ecm
		$(INSTALL_BIN) ./files/ecm_dump.sh $(1)/usr/bin/
endef

EXTRA_CFLAGS+= \
		 -I$(STAGING_DIR)/usr/include/qca-nss-drv \
		 -I$(LINUX_DIR)/net/bridge \
		 -I$(STAGING_DIR)/usr/include/nat46

ifneq (, $(findstring $(CONFIG_TARGET_BOARD), "qualcommax" "ipq60xx"))
ECM_MAKE_OPTS+= ECM_NON_PORTED_SUPPORT_ENABLE=y \
			   ECM_INTERFACE_VLAN_ENABLE=y \
			   ECM_CLASSIFIER_MARK_ENABLE=y \
			   ECM_CLASSIFIER_DSCP_ENABLE=y \
			   ECM_CLASSIFIER_PCC_ENABLE=n
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv),)
ECM_MAKE_OPTS+=ECM_FRONT_END_NSS_ENABLE=y
endif

# Disable ECM IPv6 support when global IPv6 support is disabled.
ifneq ($(CONFIG_IPV6),)
ECM_MAKE_OPTS+=ECM_IPV6_ENABLE=y
endif

# Disable ECM Bridge VLAN Filtering support when global kernel config is disabled
ifneq ($(CONFIG_BRIDGE_VLAN_FILTERING),)
ECM_MAKE_OPTS+=ECM_BRIDGE_VLAN_FILTERING_ENABLE=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-igs),)
ECM_MAKE_OPTS+=ECM_CLASSIFIER_DSCP_IGS=y
endif

# Multicast needs the ipmr/ip6mr MFC offload hooks and reads the
# bridge MDB directly. Not on ipq50xx yet: the multicast frontend has
# not been measured with the qca-dwmac-nss glue and the VLAN nodes of
# the DSA ports, so those boards keep multicast on the host until it is.
ifeq ($(CONFIG_TARGET_qualcommax_ipq50xx),)
ifneq ($(CONFIG_KERNEL_IP_MROUTE),)
ifeq ($(CONFIG_IPV6),$(CONFIG_KERNEL_IPV6_MROUTE))
ECM_MAKE_OPTS+=ECM_MULTICAST_ENABLE=y \
			   ECM_MCAST_LINUX_SNOOPER_SUPPORT=y
endif
endif
endif

ifneq ($(CONFIG_PACKAGE_kmod-nat46),)
ECM_MAKE_OPTS+=ECM_INTERFACE_MAP_T_ENABLE=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-ipsec),)
ECM_MAKE_OPTS+=ECM_INTERFACE_IPSEC_ENABLE=y
endif

# A raw-IP WAN - a cellular modem in rmnet, MHI or QMI raw-IP mode - reaches the
# firmware through the rmnet_rx node pair, so the interface type is only usable
# when the driver carries that transport and the rule ABI carries the raw-IP
# valid flag, which the 11.4 firmware line does not.
ifneq ($(CONFIG_NSS_DRV_RMNET_ENABLE),)
ifeq ($(CONFIG_NSS_FIRMWARE_VERSION_11_4),)
ECM_MAKE_OPTS+=ECM_INTERFACE_RAWIP_ENABLE=y
endif
endif

ifneq ($(CONFIG_PACKAGE_kmod-pppoe),)
ECM_MAKE_OPTS+=ECM_INTERFACE_PPPOE_ENABLE=y \
			   ECM_INTERFACE_PPP_ENABLE=y
else
ECM_MAKE_OPTS+=ECM_INTERFACE_PPPOE_ENABLE=n \
			   ECM_INTERFACE_PPP_ENABLE=n
endif

# The EDMA-rework kernel carries only the PPPoE ppp hooks (0961); the
# PPTP/L2TP kernel hooks (qosmio 0603-2/0603-3) are not ported, so
# these interface types must stay off regardless of selected kmods.
ECM_MAKE_OPTS+=ECM_INTERFACE_PPTP_ENABLE=n \
			   ECM_INTERFACE_L2TPV2_ENABLE=n

# GRE offload is not supported on this tree - never validated here, so it is
# forced off rather than switched on by whichever kmods happen to be selected.
ECM_MAKE_OPTS+=ECM_INTERFACE_GRE_TAP_ENABLE=n \
			   ECM_INTERFACE_GRE_TUN_ENABLE=n

# 6RD/SIT and IPIP6 offload are not supported on this tree: nothing here has
# been validated against them, and an interface type that is enabled but never
# exercised is worse than one that is off - it changes how ECM classifies
# flows without anyone having checked the result. Forced off regardless of
# kmod-sit / kmod-ip6-tunnel, the same way bonding is below.
ECM_MAKE_OPTS+=ECM_INTERFACE_SIT_ENABLE=n \
			   ECM_INTERFACE_TUNIPIP6_ENABLE=n

# The EDMA-rework kernel does not carry the QCA bonding hooks
# (bond_get_tx_dev), so LAG offload must stay off regardless of
# kmod-bonding.
ECM_MAKE_OPTS+=ECM_INTERFACE_BOND_ENABLE=n

ifneq ($(CONFIG_NSS_FIRMWARE_VERSION_11_4),)
EXTRA_CFLAGS+=-DNSS_FIRMWARE_VERSION_11_4
endif

# The firmware finds a connection by its 5-tuple alone, so a flow that crosses
# the box twice (a guest behind an access point reaching a LAN device behind the
# same access point, through the router on the shared uplink) is forwarded the
# wrong way on its second leg unless the rule checks where the packet came in.
# On by default where that was measured (IPQ5018, firmware 12.2);
# net.ecm.src_interface_check=0 turns it off.
ifneq ($(CONFIG_TARGET_qualcommax_ipq50xx),)
EXTRA_CFLAGS+=-DECM_INTERFACE_SRC_CHECK_DEFAULT=1
endif

define Build/InstallDev
	mkdir -p $(1)/usr/include/qca-nss-ecm
	$(CP) $(PKG_BUILD_DIR)/exports/* $(1)/usr/include/qca-nss-ecm
endef

ifeq ($(CONFIG_TARGET_BOARD), "qualcommax")
    SOC:=$(CONFIG_TARGET_SUBTARGET)
endif

define Build/Compile
	+$(MAKE) -C "$(LINUX_DIR)" $(strip $(ECM_MAKE_OPTS)) \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		M="$(PKG_BUILD_DIR)" \
		NOSTDINC_FLAGS="$(KERNEL_NOSTDINC_FLAGS) $(EXTRA_CFLAGS)" \
		CFLAGS_MODULE="-Wno-error" \
		SoC=$(SOC) \
		$(KERNEL_MAKE_FLAGS) \
		$(PKG_JOBS) \
		modules
endef

$(eval $(call KernelPackage,qca-nss-ecm))
