# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$


define KernelPackage/crypto
TITLE:=CryptoAPI modules
DESCRIPTION:=CryptoAPI modules
KCONFIG:= \
  $(CONFIG_CRYPTO_HMAC) \
  $(CONFIG_CRYPTO_NULL) \
  $(CONFIG_CRYPTO_MD4) \
  $(CONFIG_CRYPTO_MD5) \
  $(CONFIG_CRYPTO_SHA1) \
  $(CONFIG_CRYPTO_SHA256) \
  $(CONFIG_CRYPTO_SHA512) \
  $(CONFIG_CRYPTO_WP512) \
  $(CONFIG_CRYPTO_TGR192) \
  $(CONFIG_CRYPTO_DES) \
  $(CONFIG_CRYPTO_BLOWFISH) \
  $(CONFIG_CRYPTO_TWOFISH) \
  $(CONFIG_CRYPTO_SERPENT) \
  $(CONFIG_CRYPTO_AES) \
  $(CONFIG_CRYPTO_CAST5) \
  $(CONFIG_CRYPTO_CAST6) \
  $(CONFIG_CRYPTO_TEA) \
  $(CONFIG_CRYPTO_ARC4) \
  $(CONFIG_CRYPTO_KHAZAD) \
  $(CONFIG_CRYPTO_ANUBIS) \
  $(CONFIG_CRYPTO_DEFLATE) \
  $(CONFIG_CRYPTO_MICHAEL_MIC) \
  $(CONFIG_CRYPTO_CRC32C)
FILES:=$(MODULES_DIR)/kernel/crypto/*.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,crypto))

define KernelPackage/ide-core
TITLE:=Kernel support for IDE
DESCRIPTION:=Kernel modules for IDE support\\\
	useful for usb mass storage devices (e.g. on WL-HDD)\\\
	\\\
	Includes: \\\
	    - ide-core \\\
	    - ide-detect \\\
	    - ide-disk \\\
	    - pdc202xx_old
KCONFIG:=$(CONFIG_IDE)
FILES:=$(MODULES_DIR)/kernel/drivers/ide/*.$(LINUX_KMOD_SUFFIX)
AUTOLOAD:=$(call AutoLoad,20,ide-core) $(call AutoLoad,90,ide-detect ide-disk)
endef
$(eval $(call KernelPackage,ide-core))

define KernelPackage/ide-pdc202xx
TITLE:=PDC202xx IDE driver
DESCRIPTION:=PDC202xx IDE driver
KCONFIG:=$(CONFIG_BLK_DEV_PDC202XX_OLD)
FILES:=$(MODULES_DIR)/kernel/drivers/ide/pci/pdc202xx_old.$(LINUX_KMOD_SUFFIX)
AUTOLOAD:=$(call AutoLoad,30,pdc202xx_old)
endef
$(eval $(call KernelPackage,ide-pdc202xx))

NFMENU:=Netfilter Extensions

define KernelPackage/ipt-conntrack
TITLE:=Modules for connection tracking
DESCRIPTION:=Netfilter (IPv4) kernel modules for connection tracking\\\
Includes: \\\
	* ipt_conntrack \\\
	* ipt_helper \\\
	* ipt_connmark/CONNMARK
SUBMENU:=$(NFMENU)
FILES:=$(foreach mod,$(IPT_CONNTRACK-m),$(MODULES_DIR)/kernel/net/$(mod).$(LINUX_KMOD_SUFFIX))
endef
$(eval $(call KernelPackage,ipt-conntrack))


define KernelPackage/ipt-filter
TITLE:=Modules for packet content inspection
DESCRIPTION:=Netfilter (IPv4) kernel modules for packet content inspection \\\
Includes: \\\
	* ipt_ipp2p \\\
	* ipt_layer7
SUBMENU:=$(NFMENU)
FILES:=$(foreach mod,$(IPT_FILTER-m),$(MODULES_DIR)/kernel/net/$(mod).$(LINUX_KMOD_SUFFIX))
endef
$(eval $(call KernelPackage,ipt-filter))


define KernelPackage/ipt-ipopt
TITLE:=Modules for matching/changing IP packet options
DESCRIPTION:=Netfilter (IPv4) modules for matching/changing IP packet options \\\
Includes: \\\
	* ipt_dscp/DSCP \\\
	* ipt_ecn/ECN \\\
	* ipt_length \\\
	* ipt_mac \\\
	* ipt_tos/TOS \\\
	* ipt_tcpmms \\\
	* ipt_ttl/TTL \\\
	* ipt_unclean
SUBMENU:=$(NFMENU)
FILES:=$(foreach mod,$(IPT_IPOPT-m),$(MODULES_DIR)/kernel/net/$(mod).$(LINUX_KMOD_SUFFIX))
endef
$(eval $(call KernelPackage,ipt-ipopt))


define KernelPackage/ipt-ipsec
TITLE:=Modules for matching IPSec packets
DESCRIPTION:=Netfilter (IPv4) modules for matching IPSec packets \\\
Includes: \\\
	* ipt_ah \\\
	* ipt_esp
SUBMENU:=$(NFMENU)
FILES:=$(foreach mod,$(IPT_IPSEC-m),$(MODULES_DIR)/kernel/net/$(mod).$(LINUX_KMOD_SUFFIX))
endef
$(eval $(call KernelPackage,ipt-ipsec))


define KernelPackage/ipt-nat
TITLE:=Modules for extra NAT targets
DESCRIPTION:=Netfilter (IPv4) modules for extra NAT targets \\\
Includes: \\\
	* ipt_REDIRECT \\\
	* ipt_NETMAP
SUBMENU:=$(NFMENU)
FILES:=$(foreach mod,$(IPT_NAT-m),$(MODULES_DIR)/kernel/net/$(mod).$(LINUX_KMOD_SUFFIX))
endef
$(eval $(call KernelPackage,ipt-nat))

define KernelPackage/ipt-nathelper
TITLE:=Extra Conntrack and NAT helpers
DESCRIPTION:=Extra Conntrack and NAT helpers (IPv4) \\\
Includes: \\\
	* ip_conntrack_amanda \\\
	* ip_conntrack_proto_gre \\\
	* ip_nat_proto_gre \\\
	* ip_conntrack_pptp \\\
	* ip_nat_pptp \\\
	* ip_conntrack_sip \\\
	* ip_nat_sip \\\
	* ip_nat_snmp_basic \\\
	* ip_conntrack_tftp
SUBMENU:=$(NFMENU)
FILES:=$(foreach mod,$(IPT_NAT_EXTRA-m),$(MODULES_DIR)/kernel/net/$(mod).$(LINUX_KMOD_SUFFIX))
endef
$(eval $(call KernelPackage,ipt-nathelper))

define KernelPackage/ipt-imq
TITLE:=Intermediate Queueing support
DESCRIPTION:=Kernel support for Intermediate Queueing devices
KCONFIG:=$(CONFIG_IMQ)
SUBMENU:=$(NFMENU)
FILES:= \
	$(MODULES_DIR)/kernel/net/ipv4/netfilter/*IMQ*.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/net/imq.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,imq))


define KernelPackage/ipt-queue
TITLE:=Module for user-space packet queueing
DESCRIPTION:=Netfilter (IPv4) module for user-space packet queueing \\\
Includes: \\\
	* ipt_QUEUE
SUBMENU:=$(NFMENU)
FILES:=$(foreach mod,$(IPT_QUEUE-m),$(MODULES_DIR)/kernel/net/$(mod).$(LINUX_KMOD_SUFFIX))
endef
$(eval $(call KernelPackage,ipt-queue))

define KernelPackage/ipt-ulog
TITLE:=Module for user-space packet logging
DESCRIPTION:=Netfilter (IPv4) module for user-space packet logging \\\
Includes: \\\
	* ipt_ULOG
SUBMENU:=$(NFMENU)
FILES:=$(foreach mod,$(IPT_ULOG-m),$(MODULES_DIR)/kernel/net/$(mod).$(LINUX_KMOD_SUFFIX))
endef
$(eval $(call KernelPackage,ipt-ulog))

define KernelPackage/ipt-extra
TITLE:=Extra modules
DESCRIPTION:=Other Netfilter (IPv4) kernel modules\\\
Includes: \\\
	* ipt_limit \\\
	* ipt_owner \\\
	* ipt_physdev \\\
	* ipt_pkttype \\\
	* ipt_recent
SUBMENU:=$(NFMENU)
FILES:=$(foreach mod,$(IPT_EXTRA-m),$(MODULES_DIR)/kernel/net/$(mod).$(LINUX_KMOD_SUFFIX))
endef
$(eval $(call KernelPackage,ipt-extra))

define KernelPackage/ip6tables
TITLE:=IPv6 modules
DESCRIPTION:=Netfilter IPv6 firewalling support
SUBMENU:=$(NFMENU)
FILES:=$(MODULES_DIR)/kernel/net/ipv6/netfilter/ip*.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,ip6tables))


define KernelPackage/arptables
TITLE:=ARP firewalling modules
DESCRIPTION:=Kernel modules for ARP firewalling
SUBMENU:=$(NFMENU)
FILES:=$(MODULES_DIR)/kernel/net/ipv4/netfilter/arp*.$(LINUX_KMOD_SUFFIX)
KCONFIG:=$(CONFIG_IP_NF_ARPTABLES)
endef
$(eval $(call KernelPackage,arptables))


define KernelPackage/ebtables
TITLE:=Bridge firewalling modules
DESCRIPTION:=Kernel modules for Ethernet Bridge firewalling
SUBMENU:=$(NFMENU)
FILES:=$(MODULES_DIR)/kernel/net/bridge/netfilter/*.$(LINUX_KMOD_SUFFIX)
KCONFIG:=$(CONFIG_BRIDGE_NF_EBTABLES)
endef
$(eval $(call KernelPackage,ebtables))


