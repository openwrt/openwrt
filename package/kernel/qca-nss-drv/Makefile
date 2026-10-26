include $(TOPDIR)/rules.mk

PKG_NAME:=qca-nss-drv
# Adapted from kuncy7/nss-packages 5344fc35d79fc947b501e9feee5ced2b07c664fa.
PKG_RELEASE:=20
PKG_LICENSE:=ISC

# Source: https://git.codelinaro.org/clo/qsdk/oss/lklm/nss-drv.git @ 705629dab29e45e1fbfe57c4fcba73f3d58dee24
# (head of win.nss.1.0.r39, three commits past NHSS.QSDK.14.0.r9-00040-O;
# pre-generated 'git archive' of the commit).
# Fetched from our release mirror so the download is deterministic and hash-verified;
# git-proto regenerates the zstd archive locally and its hash varies by toolchain.
PKG_SOURCE_DATE:=2026-07-29
PKG_SOURCE_VERSION:=705629dab29e45e1fbfe57c4fcba73f3d58dee24
QSDK_VERSION:=14.0
PKG_VERSION:=$(QSDK_VERSION).$(subst -,.,$(PKG_SOURCE_DATE))~$(PKG_SOURCE_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(subst ~,.,$(PKG_VERSION)).tar.zst
PKG_SOURCE_URL:=https://github.com/kuncy7/nss-packages/releases/download/nss-source-tarballs
PKG_HASH:=da91403856431c46c197d2c1137bd6b55ba8a39b39ada2435893b24d4a601c67

PKG_BUILD_PARALLEL:=1
PKG_FLAGS:=nonshared

PKG_CONFIG_DEPENDS:= \
	CONFIG_NSS_FIRMWARE_VERSION_11_4 \
	CONFIG_NSS_FIRMWARE_VERSION_12_2 \
	CONFIG_NSS_FIRMWARE_VERSION_12_5 \
	CONFIG_NSS_MEM_PROFILE_HIGH \
	CONFIG_NSS_MEM_PROFILE_LOW \
	CONFIG_NSS_MEM_PROFILE_MEDIUM \
	CONFIG_PACKAGE_kmod-gre \
	CONFIG_PACKAGE_kmod-ipsec \
	CONFIG_PACKAGE_kmod-l2tp \
	CONFIG_PACKAGE_kmod-nat46 \
	CONFIG_PACKAGE_kmod-pppoe \
	CONFIG_PACKAGE_kmod-pptp \
	CONFIG_PACKAGE_kmod-sit \
	CONFIG_NSS_DRV_BRIDGE_ENABLE \
	CONFIG_NSS_DRV_CAPWAP_ENABLE \
	CONFIG_NSS_DRV_C2C_ENABLE \
	CONFIG_NSS_DRV_CLMAP_ENABLE \
	CONFIG_NSS_DRV_CRYPTO_ENABLE \
	CONFIG_NSS_DRV_DTLS_ENABLE \
	CONFIG_NSS_DRV_GRE_ENABLE \
	CONFIG_NSS_DRV_GRE_REDIR_ENABLE \
	CONFIG_NSS_DRV_GRE_TUNNEL_ENABLE \
	CONFIG_NSS_DRV_IGS_ENABLE \
	CONFIG_NSS_DRV_IPSEC_ENABLE \
	CONFIG_NSS_DRV_IPV4_REASM_ENABLE \
	CONFIG_NSS_DRV_IPV6_ENABLE \
	CONFIG_NSS_DRV_IPV6_REASM_ENABLE \
	CONFIG_NSS_DRV_LSO_RX_ENABLE \
	CONFIG_NSS_DRV_L2TP_ENABLE \
	CONFIG_NSS_DRV_LAG_ENABLE \
	CONFIG_NSS_DRV_MAPT_ENABLE \
	CONFIG_NSS_DRV_MATCH_ENABLE \
	CONFIG_NSS_DRV_MIRROR_ENABLE \
	CONFIG_NSS_DRV_OAM_ENABLE \
	CONFIG_NSS_DRV_PORTID_ENABLE \
	CONFIG_NSS_DRV_PPPOE_ENABLE \
	CONFIG_NSS_DRV_PPTP_ENABLE \
	CONFIG_NSS_DRV_PVXLAN_ENABLE \
	CONFIG_NSS_DRV_QRFS_ENABLE \
	CONFIG_NSS_DRV_QVPN_ENABLE \
	CONFIG_NSS_DRV_RMNET_ENABLE \
	CONFIG_NSS_DRV_SHAPER_ENABLE \
	CONFIG_NSS_DRV_SJACK_ENABLE \
	CONFIG_NSS_DRV_UDP_ST_ENABLE \
	CONFIG_NSS_DRV_TLS_ENABLE \
	CONFIG_NSS_DRV_TRUSTSEC_ENABLE \
	CONFIG_NSS_DRV_TRUSTSEC_RX_ENABLE \
	CONFIG_NSS_DRV_TSTAMP_ENABLE \
	CONFIG_NSS_DRV_TUN6RD_ENABLE \
	CONFIG_NSS_DRV_TUNIPIP6_ENABLE \
	CONFIG_NSS_DRV_VIRT_IF_ENABLE \
	CONFIG_NSS_DRV_VLAN_ENABLE \
	CONFIG_NSS_DRV_VXLAN_ENABLE \
	CONFIG_NSS_DRV_WIFIOFFLOAD_ENABLE \
	CONFIG_NSS_DRV_WIFI_EXT_VDEV_ENABLE \
	CONFIG_NSS_DRV_WIFI_MESH_ENABLE \
	CONFIG_NSS_DRV_WIFI_LEGACY_ENABLE

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk


define KernelPackage/qca-nss-drv
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=@TARGET_qualcommax_ipq50xx +kmod-qca-nss-dp +nss-firmware
  TITLE:=Kernel driver for NSS (core driver)
  FILES:=$(PKG_BUILD_DIR)/qca-nss-drv.ko
  # No AUTOLOAD: WF810DF startup controls NSS/ath11k/ECM ordering
endef

define KernelPackage/qca-nss-drv/config
	source "$(SOURCE)/Config.in"
endef

define KernelPackage/qca-nss-drv/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/hotplug.d/firmware
	$(INSTALL_DIR) $(1)/usr/bin

	# WF810DF startup owns module ordering; no generic IRQ/RPS service.
	$(INSTALL_DATA) ./files/qca-nss-drv.uci $(1)/etc/config/nss
	$(INSTALL_DATA) ./files/qca-nss-drv.hotplug $(1)/etc/hotplug.d/firmware/10-qca-nss-fw
	$(INSTALL_BIN) ./files/qca-nss-drv.debug $(1)/usr/bin/nss_stats
endef

define KernelPackage/qca-nss-drv/conffiles
/etc/config/nss
endef

define KernelPackage/qca-nss-drv/Description
This package contains a NSS driver for QCA chipset
endef

ifeq ($(CONFIG_TARGET_SUBTARGET), "ipq807x")
  SOC="ipq807x_64"
  subtarget:=$(CONFIG_TARGET_SUBTARGET)
else ifeq ($(CONFIG_TARGET_SUBTARGET), "ipq60xx")
  SOC="ipq60xx_64"
  subtarget:=$(CONFIG_TARGET_SUBTARGET)
else ifeq ($(CONFIG_TARGET_SUBTARGET), "ipq50xx")
  SOC="ipq50xx_64"
  subtarget:=$(CONFIG_TARGET_SUBTARGET)
endif

# The MP firmware refuses nssfq_codel ("zero flow queue size") unless the
# host gives it a QoS memory pool at core init (0146); 1 MB carries two
# full trees (shaper + IFB). The HK firmware does without. The
# qos_mem_size module parameter still overrides this.
ifeq ($(CONFIG_TARGET_SUBTARGET), "ipq50xx")
  EXTRA_CFLAGS += -DNSS_QOS_MEM_SIZE_DEFAULT=1
endif

define Build/InstallDev
	mkdir -p $(1)/usr/include/qca-nss-drv
	$(CP) $(PKG_BUILD_DIR)/exports/* $(1)/usr/include/qca-nss-drv/
ifneq (, $(findstring $(subtarget), "ipq807x" "ipq807x_64" "ipq60xx" "ipq60xx_64" "ipq50xx" "ipq50xx_64"))
	$(RM) $(1)/usr/include/qca-nss-drv/nss_ipsecmgr.h
endif
endef

EXTRA_CFLAGS+= -I$(STAGING_DIR)/usr/include/qca-nss-dp \
			   -Wno-missing-declarations -Wno-missing-prototypes \
			   -Wno-empty-body -Wno-unused-variable

ifdef CONFIG_NSS_MEM_PROFILE_MEDIUM
  EXTRA_CFLAGS += -DNSS_MEM_PROFILE_MEDIUM
else ifdef CONFIG_NSS_MEM_PROFILE_LOW
  EXTRA_CFLAGS += -DNSS_MEM_PROFILE_LOW
endif

ifdef CONFIG_NSS_FIRMWARE_VERSION_11_4
  EXTRA_CFLAGS += -DNSS_FIRMWARE_VERSION_11_4
endif

# 12.2 lays the wifili peer stats out without four 12.5 fields (0144);
# ath11k has to see the same define through the mac80211 package.
ifdef CONFIG_NSS_FIRMWARE_VERSION_12_2
  EXTRA_CFLAGS += -DNSS_FIRMWARE_VERSION_12_2
endif

DRV_MAKE_OPTS:=

# Native IPQ5018 GMAC handoff does not use PPE virtual ports.
DRV_MAKE_OPTS += NSS_DRV_PPE_VP_ENABLE=n

ifndef CONFIG_NSS_DRV_BRIDGE_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_BRIDGE_ENABLE=n
endif

ifneq ($(CONFIG_TARGET_BOARD), "ipq806x")
ifndef CONFIG_NSS_DRV_C2C_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_C2C_ENABLE=n
endif
endif

ifndef CONFIG_NSS_DRV_CLMAP_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_CLMAP_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_CRYPTO_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_CRYPTO_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_DTLS_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_DTLS_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_GRE_ENABLE
ifndef CONFIG_PACKAGE_kmod-gre
   DRV_MAKE_OPTS += NSS_DRV_GRE_ENABLE=n
endif
endif

ifndef CONFIG_NSS_DRV_GRE_REDIR_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_GRE_REDIR_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_GRE_TUNNEL_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_GRE_TUNNEL_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_IGS_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_IGS_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_IPSEC_ENABLE
ifndef CONFIG_PACKAGE_kmod-ipsec
   DRV_MAKE_OPTS += NSS_DRV_IPSEC_ENABLE=n
endif
endif

ifndef CONFIG_NSS_DRV_IPV4_REASM_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_IPV4_REASM_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_IPV6_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_IPV6_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_IPV6_REASM_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_IPV6_REASM_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_L2TP_ENABLE
ifndef CONFIG_PACKAGE_kmod-l2tp
   DRV_MAKE_OPTS += NSS_DRV_L2TP_ENABLE=n
endif
endif

ifndef CONFIG_NSS_DRV_LAG_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_LAG_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_MAPT_ENABLE
ifndef CONFIG_PACKAGE_kmod-nat46
   DRV_MAKE_OPTS += NSS_DRV_MAPT_ENABLE=n
endif
endif

ifndef CONFIG_NSS_DRV_MATCH_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_MATCH_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_MIRROR_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_MIRROR_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_OAM_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_OAM_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_PORTID_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_PORTID_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_PPPOE_ENABLE
ifndef CONFIG_PACKAGE_kmod-pppoe
   DRV_MAKE_OPTS += NSS_DRV_PPPOE_ENABLE=n
endif
endif

ifndef CONFIG_NSS_DRV_PPTP_ENABLE
ifndef CONFIG_PACKAGE_kmod-pptp
   DRV_MAKE_OPTS += NSS_DRV_PPTP_ENABLE=n
endif
endif

ifndef CONFIG_NSS_DRV_PVXLAN_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_PVXLAN_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_QRFS_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_QRFS_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_QVPN_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_QVPN_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_RMNET_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_RMNET_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_SHAPER_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_SHAPER_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_SJACK_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_SJACK_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_TLS_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_TLS_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_TRUSTSEC_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_TRUSTSEC_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_TRUSTSEC_RX_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_TRUSTSEC_RX_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_TSTAMP_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_TSTAMP_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_TUN6RD_ENABLE
ifndef CONFIG_PACKAGE_kmod-sit
   DRV_MAKE_OPTS += NSS_DRV_TUN6RD_ENABLE=n
endif
endif

ifndef CONFIG_NSS_DRV_TUNIPIP6_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_TUNIPIP6_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_VIRT_IF_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_VIRT_IF_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_VLAN_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_VLAN_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_VXLAN_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_VXLAN_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_WIFIOFFLOAD_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_WIFIOFFLOAD_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_WIFI_EXT_VDEV_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_WIFI_EXT_VDEV_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_WIFI_MESH_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_WIFI_MESH_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_UDP_ST_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_UDP_ST_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_CAPWAP_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_CAPWAP_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_LSO_RX_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_LSO_RX_ENABLE=n
endif

ifndef CONFIG_NSS_DRV_WIFI_LEGACY_ENABLE
   DRV_MAKE_OPTS += NSS_DRV_WIFI_LEGACY_ENABLE=n
endif

define Build/Configure
	$(LN) arch/nss_$(SOC).h $(PKG_BUILD_DIR)/exports/nss_arch.h
endef

define Build/Compile
	+$(MAKE) -C "$(LINUX_DIR)" $(strip $(DRV_MAKE_OPTS)) \
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

$(eval $(call KernelPackage,qca-nss-drv))
