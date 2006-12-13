# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

NFMENU:=Netfilter Extensions

define KernelPackage/ipt-conntrack
  TITLE:=Modules for connection tracking
  DESCRIPTION:=\
	Netfilter (IPv4) kernel modules for connection tracking\\\
	\\\
	Includes: \\\
	- ipt_conntrack \\\
	- ipt_helper \\\
	- ipt_connmark/CONNMARK
  FILES:=$(foreach mod,$(IPT_CONNTRACK-m),$(MODULES_DIR)/kernel/net/$(mod).$(LINUX_KMOD_SUFFIX))
  SUBMENU:=$(NFMENU)
  AUTOLOAD:=$(call AutoLoad,40,$(notdir $(IPT_CONNTRACK-m)))
endef
$(eval $(call KernelPackage,ipt-conntrack))

define KernelPackage/ipt-filter
  TITLE:=Modules for packet content inspection
  DESCRIPTION:=\
	Netfilter (IPv4) kernel modules for packet content inspection \\\
	\\\
	Includes: \\\
	- ipt_ipp2p \\\
	- ipt_layer7
  FILES:=$(foreach mod,$(IPT_FILTER-m),$(MODULES_DIR)/kernel/net/$(mod).$(LINUX_KMOD_SUFFIX))
  SUBMENU:=$(NFMENU)
  AUTOLOAD:=$(call AutoLoad,40,$(notdir $(IPT_FILTER-m)))
endef
$(eval $(call KernelPackage,ipt-filter))

define KernelPackage/ipt-ipopt
  TITLE:=Modules for matching/changing IP packet options
  DESCRIPTION:=\
	Netfilter (IPv4) modules for matching/changing IP packet options \\\
	\\\
	Includes: \\\
	- ipt_dscp/DSCP \\\
	- ipt_ecn/ECN \\\
	- ipt_length \\\
	- ipt_mac \\\
	- ipt_tos/TOS \\\
	- ipt_tcpmms \\\
	- ipt_ttl/TTL \\\
	- ipt_unclean
  FILES:=$(foreach mod,$(IPT_IPOPT-m),$(MODULES_DIR)/kernel/net/$(mod).$(LINUX_KMOD_SUFFIX))
  SUBMENU:=$(NFMENU)
  AUTOLOAD:=$(call AutoLoad,40,$(notdir $(IPT_IPOPT-m)))
endef
$(eval $(call KernelPackage,ipt-ipopt))

define KernelPackage/ipt-ipsec
  TITLE:=Modules for matching IPSec packets
  DESCRIPTION:=\
	Netfilter (IPv4) modules for matching IPSec packets \\\
	\\\
	Includes: \\\
	- ipt_ah \\\
	- ipt_esp
  FILES:=$(foreach mod,$(IPT_IPSEC-m),$(MODULES_DIR)/kernel/net/$(mod).$(LINUX_KMOD_SUFFIX))
  SUBMENU:=$(NFMENU)
  AUTOLOAD:=$(call AutoLoad,40,$(notdir $(IPT_IPSEC-m)))
endef
$(eval $(call KernelPackage,ipt-ipsec))

define KernelPackage/ipt-nat
  TITLE:=Modules for extra NAT targets
  DESCRIPTION:=\
	Netfilter (IPv4) modules for extra NAT targets \\\
	\\\
	Includes: \\\
	- ipt_REDIRECT \\\
	- ipt_NETMAP
  FILES:=$(foreach mod,$(IPT_NAT-m),$(MODULES_DIR)/kernel/net/$(mod).$(LINUX_KMOD_SUFFIX))
  SUBMENU:=$(NFMENU)
  AUTOLOAD:=$(call AutoLoad,40,$(notdir $(IPT_NAT-m)))
endef
$(eval $(call KernelPackage,ipt-nat))

define KernelPackage/ipt-nathelper
  TITLE:=Default Conntrack and NAT helpers
  DEFAULT:=y
  DESCRIPTION:=\
	Default Netfilter (IPv4) Conntrack and NAT helpers \\\
	\\\
	Includes: \\\
	- ip_conntrack_ftp \\\
	- ip_nat_ftp \\\
	- ip_conntrack_irc \\\
	- ip_nat_irc \\\
	- ip_conntrack_tftp
  FILES:=$(foreach mod,$(IPT_NAT_DEFAULT-m),$(MODULES_DIR)/kernel/net/$(mod).$(LINUX_KMOD_SUFFIX))
  SUBMENU:=$(NFMENU)
  AUTOLOAD:=$(call AutoLoad,40,$(notdir $(IPT_NAT_DEFAULT-m)))
endef
$(eval $(call KernelPackage,ipt-nathelper))

define KernelPackage/ipt-nathelper-extra
  TITLE:=Extra Conntrack and NAT helpers
  DESCRIPTION:=\
	Extra Netfilter (IPv4) Conntrack and NAT helpers \\\
	\\\
	Includes: \\\
	- ip_conntrack_amanda \\\
	- ip_conntrack_proto_gre \\\
	- ip_nat_proto_gre \\\
	- ip_conntrack_pptp \\\
	- ip_nat_pptp \\\
	- ip_conntrack_sip \\\
	- ip_nat_sip \\\
	- ip_nat_snmp_basic
  FILES:=$(foreach mod,$(IPT_NAT_EXTRA-m),$(MODULES_DIR)/kernel/net/$(mod).$(LINUX_KMOD_SUFFIX))
  SUBMENU:=$(NFMENU)
  AUTOLOAD:=$(call AutoLoad,40,$(notdir $(IPT_NAT_EXTRA-m)))
endef
$(eval $(call KernelPackage,ipt-nathelper-extra))

define KernelPackage/ipt-imq
  TITLE:=Intermediate Queueing support
  DESCRIPTION:=\
	Kernel support for Intermediate Queueing devices
  KCONFIG:=$(CONFIG_IP_NF_TARGET_IMQ)
  FILES:=$(MODULES_DIR)/kernel/net/ipv4/netfilter/*IMQ*.$(LINUX_KMOD_SUFFIX) $(MODULES_DIR)/kernel/drivers/net/imq.$(LINUX_KMOD_SUFFIX)
  SUBMENU:=$(NFMENU)
  AUTOLOAD:=$(call AutoLoad,40,$(notdir $(patsubst %.ko,%,$(wildcard $(MODULES_DIR)/kernel/net/ipv4/netfilter/*IMQ*.$(LINUX_KMOD_SUFFIX) $(MODULES_DIR)/kernel/drivers/net/imq.$(LINUX_KMOD_SUFFIX)))))
endef
$(eval $(call KernelPackage,ipt-imq))


define KernelPackage/ipt-queue
  TITLE:=Module for user-space packet queueing
  DESCRIPTION:=\
	Netfilter (IPv4) module for user-space packet queueing \\\
	\\\
	Includes: \\\
	- ipt_QUEUE
  FILES:=$(foreach mod,$(IPT_QUEUE-m),$(MODULES_DIR)/kernel/net/$(mod).$(LINUX_KMOD_SUFFIX))
  SUBMENU:=$(NFMENU)
  AUTOLOAD:=$(call AutoLoad,40,$(notdir $(IPT_QUEUE-m)))
endef
$(eval $(call KernelPackage,ipt-queue))

define KernelPackage/ipt-ulog
  TITLE:=Module for user-space packet logging
  DESCRIPTION:=\
	Netfilter (IPv4) module for user-space packet logging \\\
	\\\
	Includes: \\\
	- ipt_ULOG
  FILES:=$(foreach mod,$(IPT_ULOG-m),$(MODULES_DIR)/kernel/net/$(mod).$(LINUX_KMOD_SUFFIX))
  SUBMENU:=$(NFMENU)
  AUTOLOAD:=$(call AutoLoad,40,$(notdir $(IPT_ULOG-m)))
endef
$(eval $(call KernelPackage,ipt-ulog))

define KernelPackage/ipt-extra
  TITLE:=Extra modules
  DESCRIPTION:=\
	Other Netfilter (IPv4) kernel modules\\\
	Includes: \\\
	- ipt_limit \\\
	- ipt_owner \\\
	- ipt_physdev \\\
	- ipt_pkttype \\\
	- ipt_recent
  FILES:=$(foreach mod,$(IPT_EXTRA-m),$(MODULES_DIR)/kernel/net/$(mod).$(LINUX_KMOD_SUFFIX))
  SUBMENU:=$(NFMENU)
  AUTOLOAD:=$(call AutoLoad,40,$(notdir $(IPT_EXTRA-m)))
endef
$(eval $(call KernelPackage,ipt-extra))

define KernelPackage/ip6tables
  TITLE:=IPv6 modules
  DESCRIPTION:=\
	Netfilter IPv6 firewalling support
  KCONFIG:=$(CONFIG_IP6_NF_IPTABLES)
  FILES:=$(MODULES_DIR)/kernel/net/ipv6/netfilter/ip*.$(LINUX_KMOD_SUFFIX)
  SUBMENU:=$(NFMENU)
  AUTOLOAD:=$(call AutoLoad,40,$(notdir $(patsubst %.ko,%,$(wildcard $(MODULES_DIR)/kernel/net/ipv6/netfilter/ip6_*.$(LINUX_KMOD_SUFFIX)) $(wildcard $(MODULES_DIR)/kernel/net/ipv6/netfilter/ip6table_*.$(LINUX_KMOD_SUFFIX)) $(wildcard $(MODULES_DIR)/kernel/net/ipv6/netfilter/ip6t_*.$(LINUX_KMOD_SUFFIX)))))
endef
$(eval $(call KernelPackage,ip6tables))


define KernelPackage/arptables
  TITLE:=ARP firewalling modules
  DESCRIPTION:=\
	Kernel modules for ARP firewalling
  FILES:=$(MODULES_DIR)/kernel/net/ipv4/netfilter/arp*.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=$(CONFIG_IP_NF_ARPTABLES)
  SUBMENU:=$(NFMENU)
  AUTOLOAD:=$(call AutoLoad,40,$(notdir $(patsubst %.ko,%,$(wildcard $(MODULES_DIR)/kernel/net/ipv4/netfilter/arp*.$(LINUX_KMOD_SUFFIX)))))
endef
$(eval $(call KernelPackage,arptables))


define KernelPackage/ebtables
  TITLE:=Bridge firewalling modules
  DESCRIPTION:=\
	Kernel modules for Ethernet Bridge firewalling
  FILES:=$(MODULES_DIR)/kernel/net/bridge/netfilter/*.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=$(CONFIG_BRIDGE_NF_EBTABLES)
  SUBMENU:=$(NFMENU)
  AUTOLOAD:=$(call AutoLoad,40,$(notdir $(patsubst %.ko,%,ebtables.ko $(wildcard $(MODULES_DIR)/kernel/net/bridge/netfilter/ebtable_*.$(LINUX_KMOD_SUFFIX)) $(wildcard $(MODULES_DIR)/kernel/net/bridge/netfilter/ebt_*.$(LINUX_KMOD_SUFFIX)))))
endef
$(eval $(call KernelPackage,ebtables))
