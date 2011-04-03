
#
# Copyright (C) 2006-2010 OpenWrt.org
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
  KCONFIG:= \
  	CONFIG_NETFILTER=y \
	CONFIG_NETFILTER_ADVANCED=y \
	$(KCONFIG_IPT_CORE)
  FILES:=$(foreach mod,$(IPT_CORE-m),$(LINUX_DIR)/net/$(mod).ko)
  AUTOLOAD:=$(call AutoLoad,40,$(notdir $(IPT_CORE-m)))
endef

define KernelPackage/ipt-core/description
 Netfilter core kernel modules
 Includes:
 - comment (2.6)
 - limit
 - LOG
 - mac
 - multiport
 - REJECT
 - TCPMSS
endef

$(eval $(call KernelPackage,ipt-core))


define AddDepends/ipt
  SUBMENU:=$(NF_MENU)
  DEPENDS+= kmod-ipt-core $(1)
endef


define KernelPackage/ipt-conntrack
  TITLE:=Basic connection tracking modules
  KCONFIG:=$(KCONFIG_IPT_CONNTRACK)
  FILES:=$(foreach mod,$(IPT_CONNTRACK-m),$(LINUX_DIR)/net/$(mod).ko)
  AUTOLOAD:=$(call AutoLoad,41,$(notdir $(IPT_CONNTRACK-m)))
  $(call AddDepends/ipt)
endef

define KernelPackage/ipt-conntrack/description
 Netfilter (IPv4) kernel modules for connection tracking
 Includes:
 - conntrack
 - defrag (2.6)
 - iptables_raw
 - NOTRACK
 - state
endef

$(eval $(call KernelPackage,ipt-conntrack))


define KernelPackage/ipt-conntrack-extra
  TITLE:=Extra connection tracking modules
  KCONFIG:=$(KCONFIG_IPT_CONNTRACK_EXTRA)
  FILES:=$(foreach mod,$(IPT_CONNTRACK_EXTRA-m),$(LINUX_DIR)/net/$(mod).ko)
  AUTOLOAD:=$(call AutoLoad,42,$(notdir $(IPT_CONNTRACK_EXTRA-m)))
  $(call AddDepends/ipt,+kmod-ipt-conntrack)
endef

define KernelPackage/ipt-conntrack-extra/description
 Netfilter (IPv4) extra kernel modules for connection tracking
 Includes:
 - connbytes
 - connmark/CONNMARK
 - conntrack
 - helper
 - recent
endef

$(eval $(call KernelPackage,ipt-conntrack-extra))


define KernelPackage/ipt-filter
  TITLE:=Modules for packet content inspection
  KCONFIG:=$(KCONFIG_IPT_FILTER)
  FILES:=$(foreach mod,$(IPT_FILTER-m),$(LINUX_DIR)/net/$(mod).ko)
  AUTOLOAD:=$(call AutoLoad,45,$(notdir $(IPT_FILTER-m)))
  $(call AddDepends/ipt,+kmod-textsearch)
endef

define KernelPackage/ipt-filter/description
 Netfilter (IPv4) kernel modules for packet content inspection
 Includes:
 - layer7
 - string
endef

$(eval $(call KernelPackage,ipt-filter))


define KernelPackage/ipt-ipopt
  TITLE:=Modules for matching/changing IP packet options
  KCONFIG:=$(KCONFIG_IPT_IPOPT)
  FILES:=$(foreach mod,$(IPT_IPOPT-m),$(LINUX_DIR)/net/$(mod).ko)
  AUTOLOAD:=$(call AutoLoad,45,$(notdir $(IPT_IPOPT-m)))
  $(call AddDepends/ipt)
endef

define KernelPackage/ipt-ipopt/description
 Netfilter (IPv4) modules for matching/changing IP packet options
 Includes:
 - CLASSIFY
 - dscp/DSCP
 - ecn/ECN
 - hl/HL (2.6.30 and later)
 - length
 - mark/MARK
 - statistic (2.6)
 - tcpmss
 - time
 - tos/TOS (prior to 2.6.25)
 - ttl/TTL (prior to 2.6.30)
 - unclean
endef

$(eval $(call KernelPackage,ipt-ipopt))


define KernelPackage/ipt-ipsec
  TITLE:=Modules for matching IPSec packets
  KCONFIG:=$(KCONFIG_IPT_IPSEC)
  FILES:=$(foreach mod,$(IPT_IPSEC-m),$(LINUX_DIR)/net/$(mod).ko)
  AUTOLOAD:=$(call AutoLoad,45,$(notdir $(IPT_IPSEC-m)))
  $(call AddDepends/ipt)
endef

define KernelPackage/ipt-ipsec/description
 Netfilter (IPv4) modules for matching IPSec packets
 Includes:
 - ah
 - esp
 - policy (2.6)
endef

$(eval $(call KernelPackage,ipt-ipsec))


define KernelPackage/ipt-nat
  TITLE:=Basic NAT targets
  KCONFIG:=$(KCONFIG_IPT_NAT)
  FILES:=$(foreach mod,$(IPT_NAT-m),$(LINUX_DIR)/net/$(mod).ko)
  AUTOLOAD:=$(call AutoLoad,42,$(notdir $(IPT_NAT-m)))
  $(call AddDepends/ipt,+kmod-ipt-conntrack)
endef

define KernelPackage/ipt-nat/description
 Netfilter (IPv4) kernel modules for basic NAT targets
 Includes:
 - MASQUERADE
endef

$(eval $(call KernelPackage,ipt-nat))


define KernelPackage/ipt-nat-extra
  TITLE:=Extra NAT targets
  KCONFIG:=$(KCONFIG_IPT_NAT_EXTRA)
  FILES:=$(foreach mod,$(IPT_NAT_EXTRA-m),$(LINUX_DIR)/net/$(mod).ko)
  AUTOLOAD:=$(call AutoLoad,43,$(notdir $(IPT_NAT_EXTRA-m)))
  $(call AddDepends/ipt,+kmod-ipt-nat)
endef

define KernelPackage/ipt-nat-extra/description
 Netfilter (IPv4) kernel modules for extra NAT targets
 Includes:
 - MIRROR (2.4)
 - NETMAP
 - REDIRECT
endef

$(eval $(call KernelPackage,ipt-nat-extra))


define KernelPackage/ipt-nathelper
  TITLE:=Basic Conntrack and NAT helpers
  KCONFIG:=$(KCONFIG_IPT_NATHELPER)
  FILES:=$(foreach mod,$(IPT_NATHELPER-m),$(LINUX_DIR)/net/$(mod).ko)
  AUTOLOAD:=$(call AutoLoad,45,$(notdir $(IPT_NATHELPER-m)))
  $(call AddDepends/ipt,+kmod-ipt-nat)
endef

define KernelPackage/ipt-nathelper/description
 Default Netfilter (IPv4) Conntrack and NAT helpers
 Includes:
 - ftp
 - irc
 - tftp
endef

$(eval $(call KernelPackage,ipt-nathelper))


define KernelPackage/ipt-nathelper-extra
  TITLE:=Extra Conntrack and NAT helpers
  KCONFIG:=$(KCONFIG_IPT_NATHELPER_EXTRA)
  FILES:=$(foreach mod,$(IPT_NATHELPER_EXTRA-m),$(LINUX_DIR)/net/$(mod).ko)
  AUTOLOAD:=$(call AutoLoad,45,$(notdir $(IPT_NATHELPER_EXTRA-m)))
  $(call AddDepends/ipt,+kmod-ipt-nat +kmod-textsearch)
endef

define KernelPackage/ipt-nathelper-extra/description
 Extra Netfilter (IPv4) Conntrack and NAT helpers
 Includes:
 - amanda
 - h323
 - mms
 - pptp (2.6)
 - proto_gre (2.6)
 - rtsp
 - sip (2.6)
 - snmp_basic
endef

$(eval $(call KernelPackage,ipt-nathelper-extra))


define KernelPackage/ipt-queue
  TITLE:=Module for user-space packet queueing
  KCONFIG:=$(KCONFIG_IPT_QUEUE)
  FILES:=$(foreach mod,$(IPT_QUEUE-m),$(LINUX_DIR)/net/$(mod).ko)
  AUTOLOAD:=$(call AutoLoad,45,$(notdir $(IPT_QUEUE-m)))
  $(call AddDepends/ipt)
endef

define KernelPackage/ipt-queue/description
 Netfilter (IPv4) module for user-space packet queueing
 Includes:
 - QUEUE
endef

$(eval $(call KernelPackage,ipt-queue))


define KernelPackage/ipt-ulog
  TITLE:=Module for user-space packet logging
  KCONFIG:=$(KCONFIG_IPT_ULOG)
  FILES:=$(foreach mod,$(IPT_ULOG-m),$(LINUX_DIR)/net/$(mod).ko)
  AUTOLOAD:=$(call AutoLoad,45,$(notdir $(IPT_ULOG-m)))
  $(call AddDepends/ipt)
endef

define KernelPackage/ipt-ulog/description
 Netfilter (IPv4) module for user-space packet logging
 Includes:
 - ULOG
endef

$(eval $(call KernelPackage,ipt-ulog))


define KernelPackage/ipt-led
  TITLE:=Module to trigger a LED with a Netfilter rule
  KCONFIG:=$(KCONFIG_IPT_LED)
  FILES:=$(foreach mod,$(IPT_LED-m),$(LINUX_DIR)/net/$(mod).$(LINUX_KMOD_SUFFIX))
  AUTOLOAD:=$(call AutoLoad,61,$(notdir $(IPT_LED-m)))
  $(call AddDepends/ipt)
endef

define KernelPackage/ipt-led/description
 Netfilter target to trigger a LED when a network packet is matched.
endef

$(eval $(call KernelPackage,ipt-led))

define KernelPackage/ipt-tproxy
  TITLE:=Transparent proxying support
  KCONFIG:= \
  	CONFIG_NETFILTER_TPROXY \
  	CONFIG_NETFILTER_XT_MATCH_SOCKET \
  	CONFIG_NETFILTER_XT_TARGET_TPROXY
  FILES:= \
  	$(LINUX_DIR)/net/netfilter/nf_tproxy_core.ko \
  	$(foreach mod,$(IPT_TPROXY-m),$(LINUX_DIR)/net/$(mod).ko)
  AUTOLOAD:=$(call AutoLoad,45,$(notdir nf_tproxy_core $(IPT_TPROXY-m)))
  $(call AddDepends/ipt)
endef

define KernelPackage/ipt-tproxy/description
  Kernel modules for Transparent Proxying
endef

$(eval $(call KernelPackage,ipt-tproxy))


define KernelPackage/ipt-iprange
  TITLE:=Module for matching ip ranges
  KCONFIG:=$(KCONFIG_IPT_IPRANGE)
  FILES:=$(foreach mod,$(IPT_IPRANGE-m),$(LINUX_DIR)/net/$(mod).ko)
  AUTOLOAD:=$(call AutoLoad,45,$(notdir $(IPT_IPRANGE-m)))
  $(call AddDepends/ipt)
endef

define KernelPackage/ipt-iprange/description
 Netfilter (IPv4) module for matching ip ranges
 Includes:
 - iprange
endef

$(eval $(call KernelPackage,ipt-iprange))


define KernelPackage/ipt-extra
  TITLE:=Extra modules
  KCONFIG:=$(KCONFIG_IPT_EXTRA)
  FILES:=$(foreach mod,$(IPT_EXTRA-m),$(LINUX_DIR)/net/$(mod).ko)
  AUTOLOAD:=$(call AutoLoad,45,$(notdir $(IPT_EXTRA-m)))
  $(call AddDepends/ipt)
endef

define KernelPackage/ipt-extra/description
 Other Netfilter (IPv4) kernel modules
 Includes:
 - condition (2.4 only)
 - owner
 - physdev (if bridge support was enabled in kernel)
 - pkttype
 - quota
endef

$(eval $(call KernelPackage,ipt-extra))


define KernelPackage/ip6tables
  SUBMENU:=$(NF_MENU)
  TITLE:=IPv6 modules
  DEPENDS:=+kmod-ipv6
  KCONFIG:=$(KCONFIG_IPT_IPV6)
  FILES:=$(foreach mod,$(IPT_IPV6-m),$(LINUX_DIR)/net/$(mod).ko)
  AUTOLOAD:=$(call AutoLoad,49,$(notdir $(IPT_IPV6-m)))
endef

define KernelPackage/ip6tables/description
 Netfilter IPv6 firewalling support
endef

$(eval $(call KernelPackage,ip6tables))


define KernelPackage/arptables
  SUBMENU:=$(NF_MENU)
  TITLE:=ARP firewalling modules
  FILES:=$(LINUX_DIR)/net/ipv4/netfilter/arp*.ko
  KCONFIG:=CONFIG_IP_NF_ARPTABLES \
    CONFIG_IP_NF_ARPFILTER \
    CONFIG_IP_NF_ARP_MANGLE
  AUTOLOAD:=$(call AutoLoad,49,$(notdir $(patsubst %.ko,%,$(wildcard $(LINUX_DIR)/net/ipv4/netfilter/arp*.ko))))
endef

define KernelPackage/arptables/description
 Kernel modules for ARP firewalling
endef

$(eval $(call KernelPackage,arptables))


define KernelPackage/ebtables
  SUBMENU:=$(NF_MENU)
  TITLE:=Bridge firewalling modules
  FILES:=$(foreach mod,$(EBTABLES-m),$(LINUX_DIR)/net/$(mod).ko)
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


define AddDepends/ebtables
  SUBMENU:=$(NF_MENU)
  DEPENDS+=kmod-ebtables $(1)
endef


define KernelPackage/ebtables-ipv4
  TITLE:=ebtables: IPv4 support
  FILES:=$(foreach mod,$(EBTABLES_IP4-m),$(LINUX_DIR)/net/$(mod).ko)
  KCONFIG:=$(KCONFIG_EBTABLES_IP4)
  AUTOLOAD:=$(call AutoLoad,49,$(notdir $(EBTABLES_IP4-m)))
  $(call AddDepends/ebtables)
endef

define KernelPackage/ebtables-ipv4/description
 This option adds the IPv4 support to ebtables, which allows basic
 IPv4 header field filtering, ARP filtering as well as SNAT, DNAT targets.
endef

$(eval $(call KernelPackage,ebtables-ipv4))


define KernelPackage/ebtables-ipv6
  TITLE:=ebtables: IPv6 support
  FILES:=$(foreach mod,$(EBTABLES_IP6-m),$(LINUX_DIR)/net/$(mod).ko)
  KCONFIG:=$(KCONFIG_EBTABLES_IP6)
  AUTOLOAD:=$(call AutoLoad,49,$(notdir $(EBTABLES_IP6-m)))
  $(call AddDepends/ebtables)
endef

define KernelPackage/ebtables-ipv6/description
 This option adds the IPv6 support to ebtables, which allows basic
 IPv6 header field filtering and target support.
endef

$(eval $(call KernelPackage,ebtables-ipv6))


define KernelPackage/ebtables-watchers
  TITLE:=ebtables: watchers support
  FILES:=$(foreach mod,$(EBTABLES_WATCHERS-m),$(LINUX_DIR)/net/$(mod).ko)
  KCONFIG:=$(KCONFIG_EBTABLES_WATCHERS)
  AUTOLOAD:=$(call AutoLoad,49,$(notdir $(EBTABLES_WATCHERS-m)))
  $(call AddDepends/ebtables)
endef

define KernelPackage/ebtables-watchers/description
 This option adds the log watchers, that you can use in any rule
 in any ebtables table.
endef

$(eval $(call KernelPackage,ebtables-watchers))


define KernelPackage/nfnetlink
  SUBMENU:=$(NF_MENU)
  TITLE:=Netlink-based userspace interface
  DEPENDS:=+kmod-ipt-core
  FILES:=$(LINUX_DIR)/net/netfilter/nfnetlink.ko
  KCONFIG:=CONFIG_NETFILTER_NETLINK
  AUTOLOAD:=$(call AutoLoad,48,nfnetlink)
endef

define KernelPackage/nfnetlink/description
 Kernel modules support for a netlink-based userspace interface
endef

$(eval $(call KernelPackage,nfnetlink))


define AddDepends/nfnetlink
  SUBMENU:=$(NF_MENU)
  DEPENDS+=+kmod-nfnetlink $(1)
endef


define KernelPackage/nfnetlink-log
  TITLE:=Netfilter LOG over NFNETLINK interface
  FILES:=$(LINUX_DIR)/net/netfilter/nfnetlink_log.ko
  KCONFIG:=CONFIG_NETFILTER_NETLINK_LOG
  AUTOLOAD:=$(call AutoLoad,48,nfnetlink_log)
  $(call AddDepends/nfnetlink)
endef

define KernelPackage/nfnetlink-log/description
 Kernel modules support for logging packets via NFNETLINK
endef

$(eval $(call KernelPackage,nfnetlink-log))


define KernelPackage/nfnetlink-queue
  TITLE:=Netfilter QUEUE over NFNETLINK interface
  FILES:=$(LINUX_DIR)/net/netfilter/nfnetlink_queue.ko
  KCONFIG:=CONFIG_NETFILTER_NETLINK_QUEUE
  AUTOLOAD:=$(call AutoLoad,48,nfnetlink_queue)
  $(call AddDepends/nfnetlink)
endef

define KernelPackage/nfnetlink-queue/description
 Kernel modules support for queueing packets via NFNETLINK
endef

$(eval $(call KernelPackage,nfnetlink-queue))


define KernelPackage/nf-conntrack-netlink
  TITLE:=Connection tracking netlink interface
  FILES:=$(LINUX_DIR)/net/netfilter/nf_conntrack_netlink.ko
  KCONFIG:=CONFIG_NF_CT_NETLINK
  AUTOLOAD:=$(call AutoLoad,49,nf_conntrack_netlink)
  $(call AddDepends/nfnetlink,+kmod-ipt-conntrack)
endef

define KernelPackage/nf-conntrack-netlink/description
 Kernel modules support for a netlink-based connection tracking 
 userspace interface
endef

$(eval $(call KernelPackage,nf-conntrack-netlink))

define KernelPackage/ipt-hashlimit
  SUBMENU:=$(NF_MENU)
  TITLE:=Netfilter hashlimit match
  KCONFIG:=$(KCONFIG_IPT_HASHLIMIT)
  FILES:=$(LINUX_DIR)/net/netfilter/xt_hashlimit.ko
  AUTOLOAD:=$(call AutoLoad,50,xt_hashlimit)
  $(call KernelPackage/ipt)
endef

define KernelPackage/ipt-hashlimit/description
 Kernel modules support for the hashlimit bucket match module
endef

$(eval $(call KernelPackage,ipt-hashlimit))
