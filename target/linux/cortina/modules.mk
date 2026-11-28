# SPDX-License-Identifier: GPL-2.0-only

define KernelPackage/cs7542-flowtable
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Cortina CS7542 hardware flowtable offload (RFC)
  DEPENDS:=@TARGET_cortina +kmod-nft-offload
  KCONFIG:= \
	CONFIG_CS7542_FE_BACKEND \
	CONFIG_CS7542_FLOWTABLE
  FILES:= \
	$(LINUX_DIR)/drivers/net/ethernet/cortina/cs7542_fe_backend.ko \
	$(LINUX_DIR)/drivers/net/ethernet/cortina/flowtable/cs7542_flowtable.ko
  AUTOLOAD:=$(call AutoProbe,cs7542_fe_backend cs7542_flowtable)
endef

define KernelPackage/cs7542-flowtable/description
 Experimental wired IPv4 NAT and plain routed IPv6 TCP/UDP offload
 for CS7542 with QCA8337 and native AR9580/QCA9880 FE paths. Enable
 hardware flow offloading in the firewall settings to request it.
 Native MAC, encryption and TX status remain in the radio driver.
 NAT66 and unsupported flows remain in software. The implementation
 has sixteen simultaneous direction selectors and eight result graphs.
 Withdrawn graphs may be reused after owner-checked quiescence;
 uncertain state stays reserved.
endef

$(eval $(call KernelPackage,cs7542-flowtable))
