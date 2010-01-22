
#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

NF_MENU:=Netfilter Extensions
NF_KMOD:=1
include $(INCLUDE_DIR)/netfilter.mk

define KernelPackage/ipt-core
  SUBMENU:=$(NF_MENU)
  TITLE:=Netfilter core
  KCONFIG:=$(KCONFIG_IPT_CORE)
  FILES:=$(foreach mod,$(IPT_CORE-m),$(LINUX_DIR)/net/$(mod).$(LINUX_KMOD_SUFFIX))
  AUTOLOAD:=$(call AutoLoad,40,$(notdir $(IPT_CORE-m)))
endef

define KernelPackage/ipt-core/description
 Netfilter core kernel modules
 Includes:
 - limit
 - LOG
 - mac
 - multiport
 - TCPMSS
 - REJECT
endef

$(eval $(call KernelPackage,ipt-core))


define KernelPackage/ipt/Depends
  SUBMENU:=$(NF_MENU)
  DEPENDS:= kmod-ipt-core $(1)
endef


define KernelPackage/ipt-conntrack
$(call KernelPackage/ipt/Depends,)
  TITLE:=Basic connection tracking modules
  KCONFIG:=$(KCONFIG_IPT_CONNTRACK)
  FILES:=$(foreach mod,$(IPT_CONNTRACK-m),$(LINUX_DIR)/net/$(mod).$(LINUX_KMOD_SUFFIX))
  AUTOLOAD:=$(call AutoLoad,41,$(notdir $(IPT_CONNTRACK-m)))
endef

define KernelPackage/ipt-conntrack/description
 Netfilter (IPv4) kernel modules for connection tracking
 Includes:
 - state
endef

$(eval $(call KernelPackage,ipt-conntrack))


define KernelPackage/ipt-conntrack-extra
$(call KernelPackage/ipt/Depends,+kmod-ipt-conntrack)
  TITLE:=Extra connection tracking modules
  KCONFIG:=$(KCONFIG_IPT_CONNTRACK_EXTRA)
  FILES:=$(foreach mod,$(IPT_CONNTRACK_EXTRA-m),$(LINUX_DIR)/net/$(mod).$(LINUX_KMOD_SUFFIX))
  AUTOLOAD:=$(call AutoLoad,42,$(notdir $(IPT_CONNTRACK_EXTRA-m)))
endef

define KernelPackage/ipt-conntrack-extra/description
 Netfilter (IPv4) extra kernel modules for connection tracking
 Includes:
 - connbytes
 - connmark/CONNMARK
 - conntrack
 - helper
 - recent
 - NOTRACK
endef

$(eval $(call KernelPackage,ipt-conntrack-extra))


define KernelPackage/ipt-filter
$(call KernelPackage/ipt/Depends,+LINUX_2_6:kmod-textsearch)
  TITLE:=Modules for packet content inspection
  KCONFIG:=$(KCONFIG_IPT_FILTER)
  FILES:=$(foreach mod,$(IPT_FILTER-m),$(LINUX_DIR)/net/$(mod).$(LINUX_KMOD_SUFFIX))
  AUTOLOAD:=$(call AutoLoad,45,$(notdir $(IPT_FILTER-m)))
endef

define KernelPackage/ipt-filter/description
 Netfilter (IPv4) kernel modules for packet content inspection
 Includes:
 - ipt_layer7
 - ipt_string
endef

$(eval $(call KernelPackage,ipt-filter))


define KernelPackage/ipt-ipopt
$(call KernelPackage/ipt/Depends,)
  TITLE:=Modules for matching/changing IP packet options
  KCONFIG:=$(KCONFIG_IPT_IPOPT)
  FILES:=$(foreach mod,$(IPT_IPOPT-m),$(LINUX_DIR)/net/$(mod).$(LINUX_KMOD_SUFFIX))
  AUTOLOAD:=$(call AutoLoad,45,$(notdir $(IPT_IPOPT-m)))
endef

define KernelPackage/ipt-ipopt/description
 Netfilter (IPv4) modules for matching/changing IP packet options
 Includes:
 - ipt_CLASSIFY
 - ipt_dscp/DSCP
 - ipt_ecn/ECN
 - ipt_length
 - ipt_tos/TOS
 - ipt_tcpmms
 - ipt_ttl/TTL
 - ipt_unclean
endef

$(eval $(call KernelPackage,ipt-ipopt))


define KernelPackage/ipt-ipsec
$(call KernelPackage/ipt/Depends,)
  TITLE:=Modules for matching IPSec packets
  KCONFIG:=$(KCONFIG_IPT_IPSEC)
  FILES:=$(foreach mod,$(IPT_IPSEC-m),$(LINUX_DIR)/net/$(mod).$(LINUX_KMOD_SUFFIX))
  AUTOLOAD:=$(call AutoLoad,45,$(notdir $(IPT_IPSEC-m)))
endef

define KernelPackage/ipt-ipsec/description
 Netfilter (IPv4) modules for matching IPSec packets
 Includes:
 - ipt_ah
 - ipt_esp
endef

$(eval $(call KernelPackage,ipt-ipsec))


define KernelPackage/ipt-nat
$(call KernelPackage/ipt/Depends,+kmod-ipt-conntrack)
  TITLE:=Basic NAT targets
  KCONFIG:=$(KCONFIG_IPT_NAT)
  FILES:=$(foreach mod,$(IPT_NAT-m),$(LINUX_DIR)/net/$(mod).$(LINUX_KMOD_SUFFIX))
  AUTOLOAD:=$(call AutoLoad,42,$(notdir $(IPT_NAT-m)))
endef

define KernelPackage/ipt-nat/description
 Netfilter (IPv4) kernel modules for basic NAT targets
 Includes:
 - MASQUERADE
endef

$(eval $(call KernelPackage,ipt-nat))


define KernelPackage/ipt-nat-extra
$(call KernelPackage/ipt/Depends,+kmod-ipt-nat)
  TITLE:=Extra NAT targets
  KCONFIG:=$(KCONFIG_IPT_NAT_EXTRA)
  FILES:=$(foreach mod,$(IPT_NAT_EXTRA-m),$(LINUX_DIR)/net/$(mod).$(LINUX_KMOD_SUFFIX))
  AUTOLOAD:=$(call AutoLoad,43,$(notdir $(IPT_NAT_EXTRA-m)))
endef

define KernelPackage/ipt-nat-extra/description
 Netfilter (IPv4) kernel modules for extra NAT targets
 Includes:
 - MIRROR
 - NETMAP
 - REDIRECT
endef

$(eval $(call KernelPackage,ipt-nat-extra))


define KernelPackage/ipt-nathelper
$(call KernelPackage/ipt/Depends,+kmod-ipt-nat)
  TITLE:=Basic Conntrack and NAT helpers
  KCONFIG:=$(KCONFIG_IPT_NATHELPER)
  FILES:=$(foreach mod,$(IPT_NATHELPER-m),$(LINUX_DIR)/net/$(mod).$(LINUX_KMOD_SUFFIX))
  AUTOLOAD:=$(call AutoLoad,45,$(notdir $(IPT_NATHELPER-m)))
endef

define KernelPackage/ipt-nathelper/description
 Default Netfilter (IPv4) Conntrack and NAT helpers
 Includes:
 - conntrack_ftp
 - nat_ftp
 - conntrack_irc
 - nat_irc
 - conntrack_tftp
 - nat_tftp
endef

$(eval $(call KernelPackage,ipt-nathelper))


define KernelPackage/ipt-nathelper-extra
$(call KernelPackage/ipt/Depends,+kmod-ipt-nat +LINUX_2_6:kmod-textsearch)
  TITLE:=Extra Conntrack and NAT helpers
  KCONFIG:=$(KCONFIG_IPT_NATHELPER_EXTRA)
  FILES:=$(foreach mod,$(IPT_NATHELPER_EXTRA-m),$(LINUX_DIR)/net/$(mod).$(LINUX_KMOD_SUFFIX))
  AUTOLOAD:=$(call AutoLoad,45,$(notdir $(IPT_NATHELPER_EXTRA-m)))
endef

define KernelPackage/ipt-nathelper-extra/description
 Extra Netfilter (IPv4) Conntrack and NAT helpers
 Includes:
 - ip_conntrack_amanda
 - ip_conntrack_proto_gre
 - ip_nat_proto_gre
 - ip_conntrack_pptp
 - ip_nat_pptp
 - ip_conntrack_sip
 - ip_nat_sip
 - ip_nat_snmp_basic
endef

$(eval $(call KernelPackage,ipt-nathelper-extra))


define KernelPackage/ipt-imq
$(call KernelPackage/ipt/Depends,)
  TITLE:=Intermediate Queueing support
  KCONFIG:= \
	CONFIG_IMQ \
	CONFIG_IMQ_BEHAVIOR_BA=y \
	CONFIG_IMQ_NUM_DEVS=2 \
	CONFIG_NETFILTER_XT_TARGET_IMQ
  FILES:= \
	$(LINUX_DIR)/drivers/net/imq.$(LINUX_KMOD_SUFFIX) \
	$(foreach mod,$(IPT_IMQ-m),$(LINUX_DIR)/net/$(mod).$(LINUX_KMOD_SUFFIX))
  AUTOLOAD:=$(call AutoLoad,45,$(notdir \
	imq \
	$(IPT_IMQ-m) \
  ))
endef

define KernelPackage/ipt-imq/description
 Kernel support for Intermediate Queueing devices
endef

$(eval $(call KernelPackage,ipt-imq))


define KernelPackage/ipt-queue
$(call KernelPackage/ipt/Depends,)
  TITLE:=Module for user-space packet queueing
  KCONFIG:=$(KCONFIG_IPT_QUEUE)
  FILES:=$(foreach mod,$(IPT_QUEUE-m),$(LINUX_DIR)/net/$(mod).$(LINUX_KMOD_SUFFIX))
  AUTOLOAD:=$(call AutoLoad,45,$(notdir $(IPT_QUEUE-m)))
endef

define KernelPackage/ipt-queue/description
 Netfilter (IPv4) module for user-space packet queueing
 Includes:
 - QUEUE
endef

$(eval $(call KernelPackage,ipt-queue))


define KernelPackage/ipt-ulog
$(call KernelPackage/ipt/Depends,)
  TITLE:=Module for user-space packet logging
  KCONFIG:=$(KCONFIG_IPT_ULOG)
  FILES:=$(foreach mod,$(IPT_ULOG-m),$(LINUX_DIR)/net/$(mod).$(LINUX_KMOD_SUFFIX))
  AUTOLOAD:=$(call AutoLoad,45,$(notdir $(IPT_ULOG-m)))
endef

define KernelPackage/ipt-ulog/description
 Netfilter (IPv4) module for user-space packet logging
 Includes:
 - ipt_ULOG
endef

$(eval $(call KernelPackage,ipt-ulog))


define KernelPackage/ipt-iprange
$(call KernelPackage/ipt/Depends,)
  TITLE:=Module for matching ip ranges
  KCONFIG:=$(KCONFIG_IPT_IPRANGE)
  FILES:=$(foreach mod,$(IPT_IPRANGE-m),$(LINUX_DIR)/net/$(mod).$(LINUX_KMOD_SUFFIX))
  AUTOLOAD:=$(call AutoLoad,45,$(notdir $(IPT_IPRANGE-m)))
endef

define KernelPackage/ipt-iprange/description
 Netfilter (IPv4) module for matching ip ranges
 Includes:
 - ipt_IPRANGE
endef

$(eval $(call KernelPackage,ipt-iprange))


define KernelPackage/ipt-extra
$(call KernelPackage/ipt/Depends,)
  TITLE:=Extra modules
  KCONFIG:=$(KCONFIG_IPT_EXTRA)
  FILES:=$(foreach mod,$(IPT_EXTRA-m),$(LINUX_DIR)/net/$(mod).$(LINUX_KMOD_SUFFIX))
  AUTOLOAD:=$(call AutoLoad,45,$(notdir $(IPT_EXTRA-m)))
endef

define KernelPackage/ipt-extra/description
 Other Netfilter (IPv4) kernel modules
 Includes:
 - ipt_owner
 - ipt_physdev
 - ipt_pkttype
 - ipt_recent
 - iptable_raw
 - xt_NOTRACK
endef

$(eval $(call KernelPackage,ipt-extra))


define KernelPackage/ip6tables
  SUBMENU:=$(NF_MENU)
  TITLE:=IPv6 modules
  DEPENDS:=+kmod-ipv6
  KCONFIG:=$(KCONFIG_IPT_IPV6)
  FILES:=$(foreach mod,$(IPT_IPV6-m),$(LINUX_DIR)/net/$(mod).$(LINUX_KMOD_SUFFIX))
  AUTOLOAD:=$(call AutoLoad,49,$(notdir $(IPT_IPV6-m)))
endef

define KernelPackage/ip6tables/description
 Netfilter IPv6 firewalling support
endef

$(eval $(call KernelPackage,ip6tables))


define KernelPackage/arptables
  SUBMENU:=$(NF_MENU)
  TITLE:=ARP firewalling modules
  FILES:=$(LINUX_DIR)/net/ipv4/netfilter/arp*.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=CONFIG_IP_NF_ARPTABLES \
    CONFIG_IP_NF_ARPFILTER \
    CONFIG_IP_NF_ARP_MANGLE
  AUTOLOAD:=$(call AutoLoad,49,$(notdir $(patsubst %.$(LINUX_KMOD_SUFFIX),%,$(wildcard $(LINUX_DIR)/net/ipv4/netfilter/arp*.$(LINUX_KMOD_SUFFIX)))))
endef

define KernelPackage/arptables/description
 Kernel modules for ARP firewalling
endef

$(eval $(call KernelPackage,arptables))


define KernelPackage/ebtables
  SUBMENU:=$(NF_MENU)
  TITLE:=Bridge firewalling modules
  DEPENDS:=@LINUX_2_6
  FILES:=$(foreach mod,$(EBTABLES-m),$(LINUX_DIR)/net/$(mod).$(LINUX_KMOD_SUFFIX))
  KCONFIG:=CONFIG_BRIDGE_NETFILTER=y \
	$(KCONFIG_EBTABLES)
  AUTOLOAD:=$(call AutoLoad,49,$(notdir $(EBTABLES-m)))
endef

define KernelPackage/ebtables/description
  ebtables is a general, extensible frame/packet identification
  framework. It provides you to do Ethernet
  filtering/NAT/brouting on the Ethernet bridge.
endef

$(eval $(call KernelPackage,ebtables))


define KernelPackage/ebtables/Depends
  SUBMENU:=$(NF_MENU)
  DEPENDS:=kmod-ebtables $(1)
endef


define KernelPackage/ebtables-ipv4
$(call KernelPackage/ebtables/Depends,)
  TITLE:=ebtables: IPv4 support
  FILES:=$(foreach mod,$(EBTABLES_IP4-m),$(LINUX_DIR)/net/$(mod).$(LINUX_KMOD_SUFFIX))
  KCONFIG:=$(KCONFIG_EBTABLES_IP4)
  AUTOLOAD:=$(call AutoLoad,49,$(notdir $(EBTABLES_IP4-m)))
endef

define KernelPackage/ebtables-ipv4/description
 This option adds the IPv4 support to ebtables, which allows basic
 IPv4 header field filtering, ARP filtering as well as SNAT, DNAT targets.
endef

$(eval $(call KernelPackage,ebtables-ipv4))


define KernelPackage/ebtables-ipv6
$(call KernelPackage/ebtables/Depends,)
  TITLE:=ebtables: IPv6 support
  FILES:=$(foreach mod,$(EBTABLES_IP6-m),$(LINUX_DIR)/net/$(mod).$(LINUX_KMOD_SUFFIX))
  KCONFIG:=$(KCONFIG_EBTABLES_IP6)
  AUTOLOAD:=$(call AutoLoad,49,$(notdir $(EBTABLES_IP6-m)))
endef

define KernelPackage/ebtables-ipv6/description
 This option adds the IPv6 support to ebtables, which allows basic
 IPv6 header field filtering and target support.
endef

$(eval $(call KernelPackage,ebtables-ipv6))


define KernelPackage/ebtables-watchers
$(call KernelPackage/ebtables/Depends,)
  TITLE:=ebtables: watchers support
  FILES:=$(foreach mod,$(EBTABLES_WATCHERS-m),$(LINUX_DIR)/net/$(mod).$(LINUX_KMOD_SUFFIX))
  KCONFIG:=$(KCONFIG_EBTABLES_WATCHERS)
  AUTOLOAD:=$(call AutoLoad,49,$(notdir $(EBTABLES_WATCHERS-m)))
endef

define KernelPackage/ebtables-watchers/description
 This option adds the log watchers, that you can use in any rule
 in any ebtables table.
endef

$(eval $(call KernelPackage,ebtables-watchers))


define KernelPackage/nfnetlink
  SUBMENU:=$(NF_MENU)
  TITLE:=Netlink-based userspace interface
  DEPENDS:=@LINUX_2_6 +kmod-ipt-core
  FILES:=$(LINUX_DIR)/net/netfilter/nfnetlink.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=CONFIG_NETFILTER_NETLINK
  AUTOLOAD:=$(call AutoLoad,48,nfnetlink)
endef

define KernelPackage/nfnetlink/description
 Kernel modules support for a netlink-based userspace interface
endef

$(eval $(call KernelPackage,nfnetlink))


define KernelPackage/nfnetlink/Depends
  SUBMENU:=$(NF_MENU)
  DEPENDS:=@LINUX_2_6 +kmod-nfnetlink $(1)
endef


define KernelPackage/nfnetlink-log
$(call KernelPackage/nfnetlink/Depends,)
  TITLE:=Netfilter LOG over NFNETLINK interface
  FILES:=$(LINUX_DIR)/net/netfilter/nfnetlink_log.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=CONFIG_NETFILTER_NETLINK_LOG
  AUTOLOAD:=$(call AutoLoad,48,nfnetlink_log)
endef

define KernelPackage/nfnetlink-log/description
 Kernel modules support for logging packets via NFNETLINK
endef

$(eval $(call KernelPackage,nfnetlink-log))


define KernelPackage/nfnetlink-queue
$(call KernelPackage/nfnetlink/Depends,)
  TITLE:=Netfilter QUEUE over NFNETLINK interface
  FILES:=$(LINUX_DIR)/net/netfilter/nfnetlink_queue.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=CONFIG_NETFILTER_NETLINK_QUEUE
  AUTOLOAD:=$(call AutoLoad,48,nfnetlink_queue)
endef

define KernelPackage/nfnetlink-queue/description
 Kernel modules support for queueing packets via NFNETLINK
endef

$(eval $(call KernelPackage,nfnetlink-queue))


define KernelPackage/nf-conntrack-netlink
$(call KernelPackage/nfnetlink/Depends,+kmod-ipt-conntrack)
  TITLE:=Connection tracking netlink interface
  FILES:=$(LINUX_DIR)/net/netfilter/nf_conntrack_netlink.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=CONFIG_NF_CT_NETLINK
  AUTOLOAD:=$(call AutoLoad,49,nf_conntrack_netlink)
endef

define KernelPackage/nf-conntrack-netlink/description
 Kernel modules support for a netlink-based connection tracking 
 userspace interface
endef

$(eval $(call KernelPackage,nf-conntrack-netlink))
