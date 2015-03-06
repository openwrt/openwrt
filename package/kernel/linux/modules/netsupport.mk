#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

NETWORK_SUPPORT_MENU:=Network Support

define KernelPackage/atm
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=ATM support
  KCONFIG:= \
	CONFIG_ATM \
	CONFIG_ATM_BR2684
  FILES:= \
	$(LINUX_DIR)/net/atm/atm.ko \
	$(LINUX_DIR)/net/atm/br2684.ko
  AUTOLOAD:=$(call AutoLoad,30,atm br2684)
endef

define KernelPackage/atm/description
 Kernel modules for ATM support
endef

$(eval $(call KernelPackage,atm))


define KernelPackage/atmtcp
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=ATM over TCP
  DEPENDS:=kmod-atm
  KCONFIG:=CONFIG_ATM_TCP CONFIG_ATM_DRIVERS=y
  FILES:=$(LINUX_DIR)/drivers/atm/atmtcp.ko
  AUTOLOAD:=$(call AutoLoad,40,atmtcp)
endef

define KernelPackage/atmtcp/description
 Kernel module for ATM over TCP support
endef

$(eval $(call KernelPackage,atmtcp))


define KernelPackage/appletalk
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=Appletalk protocol support
  DEPENDS:=+PACKAGE_kmod-llc:kmod-llc
  KCONFIG:= \
	CONFIG_ATALK \
	CONFIG_DEV_APPLETALK \
	CONFIG_IPDDP \
	CONFIG_IPDDP_ENCAP=y \
	CONFIG_IPDDP_DECAP=y
  FILES:= \
	$(LINUX_DIR)/net/appletalk/appletalk.ko \
	$(LINUX_DIR)/drivers/net/appletalk/ipddp.ko
  AUTOLOAD:=$(call AutoLoad,40,appletalk ipddp)
endef

define KernelPackage/appletalk/description
 Kernel module for AppleTalk protocol.
endef

$(eval $(call KernelPackage,appletalk))


define KernelPackage/bonding
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=Ethernet bonding driver
  KCONFIG:=CONFIG_BONDING
  FILES:=$(LINUX_DIR)/drivers/net/bonding/bonding.ko
  AUTOLOAD:=$(call AutoLoad,40,bonding)
endef

define KernelPackage/bonding/description
 Kernel module for NIC bonding.
endef

$(eval $(call KernelPackage,bonding))


define KernelPackage/bridge
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=Ethernet bridging support
  DEPENDS:=+kmod-stp
  KCONFIG:= \
	CONFIG_BRIDGE \
	CONFIG_BRIDGE_IGMP_SNOOPING=y
  FILES:=$(LINUX_DIR)/net/bridge/bridge.ko
  AUTOLOAD:=$(call AutoLoad,11,bridge)
endef

define KernelPackage/bridge/description
 Kernel module for Ethernet bridging.
endef

$(eval $(call KernelPackage,bridge))

define KernelPackage/llc
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=ANSI/IEEE 802.2 LLC support
  KCONFIG:=CONFIG_LLC
  FILES:= \
	$(LINUX_DIR)/net/llc/llc.ko \
	$(LINUX_DIR)/net/802/p8022.ko \
	$(LINUX_DIR)/net/802/psnap.ko
  AUTOLOAD:=$(call AutoLoad,09,llc p8022 psnap)
endef

define KernelPackage/llc/description
 Kernel module for ANSI/IEEE 802.2 LLC support.
endef

$(eval $(call KernelPackage,llc))

define KernelPackage/stp
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=Ethernet Spanning Tree Protocol support
  DEPENDS:=+kmod-llc
  KCONFIG:=CONFIG_STP
  FILES:=$(LINUX_DIR)/net/802/stp.ko
  AUTOLOAD:=$(call AutoLoad,10,stp)
endef

define KernelPackage/stp/description
 Kernel module for Ethernet Spanning Tree Protocol support.
endef

$(eval $(call KernelPackage,stp))

define KernelPackage/8021q
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=802.1Q VLAN support
  KCONFIG:=CONFIG_VLAN_8021Q \
		CONFIG_VLAN_8021Q_GVRP=n
  FILES:=$(LINUX_DIR)/net/8021q/8021q.ko
  AUTOLOAD:=$(call AutoLoad,12,8021q)
endef

define KernelPackage/8021q/description
 Kernel module for 802.1Q VLAN support
endef

$(eval $(call KernelPackage,8021q))


define KernelPackage/udptunnel4
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=IPv4 UDP tunneling support
  DEPENDS:=@(!LINUX_3_8&&!LINUX_3_10&&!LINUX_3_13&&!LINUX_3_14)
  KCONFIG:=CONFIG_NET_UDP_TUNNEL
  FILES:=$(LINUX_DIR)/net/ipv4/udp_tunnel.ko
  AUTOLOAD:=$(call AutoLoad,32,udp_tunnel)
endef


$(eval $(call KernelPackage,udptunnel4))

define KernelPackage/udptunnel6
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=IPv6 UDP tunneling support
  DEPENDS:=@(!LINUX_3_8&&!LINUX_3_10&&!LINUX_3_13&&!LINUX_3_14) +kmod-ipv6
  KCONFIG:=CONFIG_NET_UDP_TUNNEL
  FILES:=$(LINUX_DIR)/net/ipv6/ip6_udp_tunnel.ko
  AUTOLOAD:=$(call AutoLoad,32,ip6_udp_tunnel)
endef

$(eval $(call KernelPackage,udptunnel6))


define KernelPackage/vxlan
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=Native VXLAN Kernel support
  DEPENDS:= \
	+kmod-iptunnel \
	+(!LINUX_3_8&&!LINUX_3_10&&!LINUX_3_13&&!LINUX_3_14):kmod-udptunnel4 \
	+(!LINUX_3_8&&!LINUX_3_10&&!LINUX_3_13&&!LINUX_3_14&&IPV6):kmod-udptunnel6
  KCONFIG:=CONFIG_VXLAN
  FILES:=$(LINUX_DIR)/drivers/net/vxlan.ko
  AUTOLOAD:=$(call AutoLoad,13,vxlan)
endef

define KernelPackage/vxlan/description
 Kernel module for supporting VXLAN in the Kernel.
 Requires Kernel 3.12 or newer.
endef

$(eval $(call KernelPackage,vxlan))

define KernelPackage/capi
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=CAPI (ISDN) Support
  KCONFIG:= \
	CONFIG_ISDN_CAPI \
	CONFIG_ISDN_CAPI_CAPI20 \
	CONFIG_ISDN_CAPIFS \
	CONFIG_ISDN_CAPI_CAPIFS
  FILES:= \
	$(LINUX_DIR)/drivers/isdn/capi/kernelcapi.ko \
	$(LINUX_DIR)/drivers/isdn/capi/capi.ko
  AUTOLOAD:=$(call AutoLoad,30,kernelcapi capi)
endef

define KernelPackage/capi/description
 Kernel module for basic CAPI (ISDN) support
endef

$(eval $(call KernelPackage,capi))

define KernelPackage/misdn
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=mISDN (ISDN) Support
  KCONFIG:= \
	CONFIG_ISDN=y \
  	CONFIG_MISDN \
	CONFIG_MISDN_DSP \
	CONFIG_MISDN_L1OIP
  FILES:= \
  	$(LINUX_DIR)/drivers/isdn/mISDN/mISDN_core.ko \
	$(LINUX_DIR)/drivers/isdn/mISDN/mISDN_dsp.ko \
	$(LINUX_DIR)/drivers/isdn/mISDN/l1oip.ko
  AUTOLOAD:=$(call AutoLoad,30,mISDN_core mISDN_dsp l1oip)
endef

define KernelPackage/misdn/description
  Modular ISDN driver support
endef

$(eval $(call KernelPackage,misdn))


define KernelPackage/isdn4linux
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=Old ISDN4Linux (deprecated)
  DEPENDS:=+kmod-ppp
  KCONFIG:= \
	CONFIG_ISDN=y \
    CONFIG_ISDN_I4L \
    CONFIG_ISDN_PPP=y \
    CONFIG_ISDN_PPP_VJ=y \
    CONFIG_ISDN_MPP=y \
    CONFIG_IPPP_FILTER=y \
    CONFIG_ISDN_PPP_BSDCOMP \
    CONFIG_ISDN_CAPI_MIDDLEWARE=y \
    CONFIG_ISDN_CAPI_CAPIFS_BOOL=y \
    CONFIG_ISDN_AUDIO=y \
    CONFIG_ISDN_TTY_FAX=y \
    CONFIG_ISDN_X25=y \
    CONFIG_ISDN_DIVERSION
  FILES:= \
    $(LINUX_DIR)/drivers/isdn/divert/dss1_divert.ko \
	$(LINUX_DIR)/drivers/isdn/i4l/isdn.ko \
	$(LINUX_DIR)/drivers/isdn/i4l/isdn_bsdcomp.ko
  AUTOLOAD:=$(call AutoLoad,40,isdn isdn_bsdcomp dss1_divert)
endef

define KernelPackage/isdn4linux/description
  This driver allows you to use an ISDN adapter for networking
endef

$(eval $(call KernelPackage,isdn4linux))


define KernelPackage/ipip
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=IP-in-IP encapsulation
  DEPENDS:=+kmod-iptunnel +kmod-iptunnel4
  KCONFIG:=CONFIG_NET_IPIP
  FILES:=$(LINUX_DIR)/net/ipv4/ipip.ko
  AUTOLOAD:=$(call AutoLoad,32,ipip)
endef

define KernelPackage/ipip/description
 Kernel modules for IP-in-IP encapsulation
endef

$(eval $(call KernelPackage,ipip))


IPSEC-m:= \
	xfrm/xfrm_algo \
	xfrm/xfrm_ipcomp \
	xfrm/xfrm_user \
	key/af_key \

define KernelPackage/ipsec
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=IPsec related modules (IPv4 and IPv6)
  DEPENDS:=+kmod-crypto-authenc +kmod-crypto-iv +kmod-crypto-des +kmod-crypto-hmac +kmod-crypto-md5 +kmod-crypto-sha1 +kmod-crypto-deflate +kmod-crypto-cbc
  KCONFIG:= \
	CONFIG_NET_KEY \
	CONFIG_XFRM_USER \
	CONFIG_INET_IPCOMP \
	CONFIG_XFRM_IPCOMP
  FILES:=$(foreach mod,$(IPSEC-m),$(LINUX_DIR)/net/$(mod).ko)
  AUTOLOAD:=$(call AutoLoad,30,$(notdir $(IPSEC-m)))
endef

define KernelPackage/ipsec/description
 Kernel modules for IPsec support in both IPv4 and IPv6.
 Includes:
 - af_key
 - xfrm_ipcomp
 - xfrm_user
endef

$(eval $(call KernelPackage,ipsec))


IPSEC4-m:= \
	ipv4/ah4 \
	ipv4/esp4 \
	ipv4/xfrm4_mode_beet \
	ipv4/xfrm4_mode_transport \
	ipv4/xfrm4_mode_tunnel \
	ipv4/xfrm4_tunnel \
	ipv4/ipcomp \

define KernelPackage/ipsec4
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=IPsec related modules (IPv4)
  DEPENDS:=kmod-ipsec +kmod-iptunnel4
  KCONFIG:= \
	CONFIG_INET_AH \
	CONFIG_INET_ESP \
	CONFIG_INET_IPCOMP \
	CONFIG_INET_XFRM_MODE_BEET \
	CONFIG_INET_XFRM_MODE_TRANSPORT \
	CONFIG_INET_XFRM_MODE_TUNNEL \
	CONFIG_INET_XFRM_TUNNEL
  FILES:=$(foreach mod,$(IPSEC4-m),$(LINUX_DIR)/net/$(mod).ko)
  AUTOLOAD:=$(call AutoLoad,32,$(notdir $(IPSEC4-m)))
endef

define KernelPackage/ipsec4/description
 Kernel modules for IPsec support in IPv4.
 Includes:
 - ah4
 - esp4
 - ipcomp4
 - xfrm4_mode_beet
 - xfrm4_mode_transport
 - xfrm4_mode_tunnel
 - xfrm4_tunnel
endef

$(eval $(call KernelPackage,ipsec4))


IPSEC6-m:= \
	ipv6/ah6 \
	ipv6/esp6 \
	ipv6/xfrm6_mode_beet \
	ipv6/xfrm6_mode_transport \
	ipv6/xfrm6_mode_tunnel \
	ipv6/xfrm6_tunnel \
	ipv6/ipcomp6 \

define KernelPackage/ipsec6
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=IPsec related modules (IPv6)
  DEPENDS:=kmod-ipsec +kmod-iptunnel6
  KCONFIG:= \
	CONFIG_INET6_AH \
	CONFIG_INET6_ESP \
	CONFIG_INET6_IPCOMP \
	CONFIG_INET6_XFRM_MODE_BEET \
	CONFIG_INET6_XFRM_MODE_TRANSPORT \
	CONFIG_INET6_XFRM_MODE_TUNNEL \
	CONFIG_INET6_XFRM_TUNNEL
  FILES:=$(foreach mod,$(IPSEC6-m),$(LINUX_DIR)/net/$(mod).ko)
  AUTOLOAD:=$(call AutoLoad,32,$(notdir $(IPSEC6-m)))
endef

define KernelPackage/ipsec6/description
 Kernel modules for IPsec support in IPv6.
 Includes:
 - ah6
 - esp6
 - ipcomp6
 - xfrm6_mode_beet
 - xfrm6_mode_transport
 - xfrm6_mode_tunnel
 - xfrm6_tunnel
endef

$(eval $(call KernelPackage,ipsec6))


define KernelPackage/iptunnel
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=IP tunnel support
  HIDDEN:=1
  KCONFIG:= \
	CONFIG_NET_IP_TUNNEL
  FILES:=$(LINUX_DIR)/net/ipv4/ip_tunnel.ko
  AUTOLOAD:=$(call AutoLoad,31,ip_tunnel)
endef

define KernelPackage/iptunnel/description
 Kernel module for generic IP tunnel support
endef

$(eval $(call KernelPackage,iptunnel))


define KernelPackage/ipvti
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=IP VTI (Virtual Tunnel Interface)
  DEPENDS:=+kmod-iptunnel +kmod-iptunnel4 +kmod-ipsec4
  KCONFIG:=CONFIG_NET_IPVTI
  FILES:=$(LINUX_DIR)/net/ipv4/ip_vti.ko
  AUTOLOAD:=$(call AutoLoad,33,ip_vti)
endef

define KernelPackage/ipvti/description
 Kernel modules for IP VTI (Virtual Tunnel Interface)
endef

$(eval $(call KernelPackage,ipvti))


define KernelPackage/iptunnel4
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=IPv4 tunneling
  HIDDEN:=1
  KCONFIG:= \
	CONFIG_INET_TUNNEL
  FILES:=$(LINUX_DIR)/net/ipv4/tunnel4.ko
  AUTOLOAD:=$(call AutoLoad,31,tunnel4)
endef

define KernelPackage/iptunnel4/description
 Kernel modules for IPv4 tunneling
endef

$(eval $(call KernelPackage,iptunnel4))


define KernelPackage/iptunnel6
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=IPv6 tunneling
  DEPENDS:= +kmod-ipv6
  KCONFIG:= \
	CONFIG_INET6_TUNNEL
  FILES:=$(LINUX_DIR)/net/ipv6/tunnel6.ko
  AUTOLOAD:=$(call AutoLoad,31,tunnel6)
endef

define KernelPackage/iptunnel6/description
 Kernel modules for IPv6 tunneling
endef

$(eval $(call KernelPackage,iptunnel6))


define KernelPackage/ipv6
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=IPv6 support
  KCONFIG:= \
	CONFIG_IPV6 \
	CONFIG_IPV6_PRIVACY=y \
	CONFIG_IPV6_MULTIPLE_TABLES=y \
	CONFIG_IPV6_MROUTE=y \
	CONFIG_IPV6_PIMSM_V2=n \
	CONFIG_IPV6_SUBTREES=y
  FILES:=$(LINUX_DIR)/net/ipv6/ipv6.ko
  AUTOLOAD:=$(call AutoLoad,20,ipv6)
endef

define KernelPackage/ipv6/description
 Kernel modules for IPv6 support
endef

$(eval $(call KernelPackage,ipv6))


define KernelPackage/sit
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  DEPENDS:=+kmod-ipv6 +kmod-iptunnel +kmod-iptunnel4
  TITLE:=IPv6-in-IPv4 tunnel
  KCONFIG:=CONFIG_IPV6_SIT \
	CONFIG_IPV6_SIT_6RD=y
  FILES:=$(LINUX_DIR)/net/ipv6/sit.ko
  AUTOLOAD:=$(call AutoLoad,32,sit)
endef

define KernelPackage/sit/description
 Kernel modules for IPv6-in-IPv4 tunnelling
endef

$(eval $(call KernelPackage,sit))


define KernelPackage/ip6-tunnel
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=IP-in-IPv6 tunnelling
  DEPENDS:= +kmod-ipv6 +kmod-iptunnel6
  KCONFIG:= CONFIG_IPV6_TUNNEL
  FILES:=$(LINUX_DIR)/net/ipv6/ip6_tunnel.ko
  AUTOLOAD:=$(call AutoLoad,32,ip6_tunnel)
endef

define KernelPackage/ip6-tunnel/description
 Kernel modules for IPv6-in-IPv6 and IPv4-in-IPv6 tunnelling
endef

$(eval $(call KernelPackage,ip6-tunnel))


define KernelPackage/gre
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=GRE support
  DEPENDS:=+PACKAGE_kmod-ipv6:kmod-ipv6 +kmod-iptunnel
  KCONFIG:=CONFIG_NET_IPGRE CONFIG_NET_IPGRE_DEMUX
  FILES:=$(LINUX_DIR)/net/ipv4/ip_gre.ko $(LINUX_DIR)/net/ipv4/gre.ko
  AUTOLOAD:=$(call AutoLoad,39,gre ip_gre)
endef

define KernelPackage/gre/description
 Generic Routing Encapsulation support
endef

$(eval $(call KernelPackage,gre))


define KernelPackage/gre6
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=GRE support over IPV6
  DEPENDS:=+kmod-ipv6 +kmod-iptunnel +kmod-ip6-tunnel
  KCONFIG:=CONFIG_IPV6_GRE
  FILES:=$(LINUX_DIR)/net/ipv6/ip6_gre.ko
  AUTOLOAD:=$(call AutoLoad,39,ip6_gre)
endef

define KernelPackage/gre6/description
 Generic Routing Encapsulation support over IPv6
endef

$(eval $(call KernelPackage,gre6))


define KernelPackage/tun
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=Universal TUN/TAP driver
  KCONFIG:=CONFIG_TUN
  FILES:=$(LINUX_DIR)/drivers/net/tun.ko
  AUTOLOAD:=$(call AutoLoad,30,tun)
endef

define KernelPackage/tun/description
 Kernel support for the TUN/TAP tunneling device
endef

$(eval $(call KernelPackage,tun))


define KernelPackage/veth
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=Virtual ethernet pair device
  KCONFIG:=CONFIG_VETH
  FILES:=$(LINUX_DIR)/drivers/net/veth.ko
  AUTOLOAD:=$(call AutoLoad,30,veth)
endef

define KernelPackage/veth/description
 This device is a local ethernet tunnel. Devices are created in pairs.
 When one end receives the packet it appears on its pair and vice
 versa.
endef

$(eval $(call KernelPackage,veth))


define KernelPackage/slhc
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  HIDDEN:=1
  TITLE:=Serial Line Header Compression
  DEPENDS:=+kmod-lib-crc-ccitt
  KCONFIG:=CONFIG_SLHC
  FILES:=$(LINUX_DIR)/drivers/net/slip/slhc.ko
endef

$(eval $(call KernelPackage,slhc))


define KernelPackage/ppp
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=PPP modules
  DEPENDS:=+kmod-lib-crc-ccitt +kmod-slhc
  KCONFIG:= \
	CONFIG_PPP \
	CONFIG_PPP_ASYNC
  FILES:= \
	$(LINUX_DIR)/drivers/net/ppp/ppp_async.ko \
	$(LINUX_DIR)/drivers/net/ppp/ppp_generic.ko
  AUTOLOAD:=$(call AutoProbe,ppp_async)
endef

define KernelPackage/ppp/description
 Kernel modules for PPP support
endef

$(eval $(call KernelPackage,ppp))


define KernelPackage/ppp-synctty
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=PPP sync tty support
  DEPENDS:=kmod-ppp
  KCONFIG:=CONFIG_PPP_SYNC_TTY
  FILES:=$(LINUX_DIR)/drivers/net/ppp/ppp_synctty.ko
  AUTOLOAD:=$(call AutoProbe,ppp_synctty)
endef

define KernelPackage/ppp-synctty/description
 Kernel modules for PPP sync tty support
endef

$(eval $(call KernelPackage,ppp-synctty))


define KernelPackage/pppox
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=PPPoX helper
  DEPENDS:=kmod-ppp
  KCONFIG:=CONFIG_PPPOE
  FILES:=$(LINUX_DIR)/drivers/net/ppp/pppox.ko
endef

define KernelPackage/pppox/description
 Kernel helper module for PPPoE and PPTP support
endef

$(eval $(call KernelPackage,pppox))


define KernelPackage/pppoe
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=PPPoE support
  DEPENDS:=kmod-ppp +kmod-pppox
  KCONFIG:=CONFIG_PPPOE
  FILES:=$(LINUX_DIR)/drivers/net/ppp/pppoe.ko
  AUTOLOAD:=$(call AutoProbe,pppoe)
endef

define KernelPackage/pppoe/description
 Kernel module for PPPoE (PPP over Ethernet) support
endef

$(eval $(call KernelPackage,pppoe))


define KernelPackage/pppoa
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=PPPoA support
  DEPENDS:=kmod-ppp +kmod-atm
  KCONFIG:=CONFIG_PPPOATM CONFIG_ATM_DRIVERS=y
  FILES:=$(LINUX_DIR)/net/atm/pppoatm.ko
  AUTOLOAD:=$(call AutoLoad,40,pppoatm)
endef

define KernelPackage/pppoa/description
 Kernel modules for PPPoA (PPP over ATM) support
endef

$(eval $(call KernelPackage,pppoa))


define KernelPackage/pptp
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=PPtP support
  DEPENDS:=kmod-ppp +kmod-gre +kmod-pppox
  KCONFIG:=CONFIG_PPTP
  FILES:=$(LINUX_DIR)/drivers/net/ppp/pptp.ko
  AUTOLOAD:=$(call AutoProbe,pptp)
endef

$(eval $(call KernelPackage,pptp))


define KernelPackage/pppol2tp
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=PPPoL2TP support
  DEPENDS:=kmod-ppp +kmod-pppox +kmod-l2tp
  KCONFIG:=CONFIG_PPPOL2TP
  FILES:=$(LINUX_DIR)/net/l2tp/l2tp_ppp.ko
  AUTOLOAD:=$(call AutoProbe,l2tp_ppp)
endef

define KernelPackage/pppol2tp/description
  Kernel modules for PPPoL2TP (PPP over L2TP) support
endef

$(eval $(call KernelPackage,pppol2tp))


define KernelPackage/ipoa
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=IPoA support
  DEPENDS:=kmod-atm
  KCONFIG:=CONFIG_ATM_CLIP
  FILES:=$(LINUX_DIR)/net/atm/clip.ko
  AUTOLOAD:=$(call AutoProbe,clip)
endef

define KernelPackage/ipoa/description
  Kernel modules for IPoA (IP over ATM) support
endef

$(eval $(call KernelPackage,ipoa))


define KernelPackage/mppe
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=Microsoft PPP compression/encryption
  DEPENDS:=kmod-ppp +kmod-crypto-core +kmod-crypto-arc4 +kmod-crypto-sha1 +kmod-crypto-ecb
  KCONFIG:= \
	CONFIG_PPP_MPPE_MPPC \
	CONFIG_PPP_MPPE
  FILES:=$(LINUX_DIR)/drivers/net/ppp/ppp_mppe.ko
  AUTOLOAD:=$(call AutoProbe,ppp_mppe)
endef

define KernelPackage/mppe/description
 Kernel modules for Microsoft PPP compression/encryption
endef

$(eval $(call KernelPackage,mppe))


SCHED_MODULES = $(patsubst $(LINUX_DIR)/net/sched/%.ko,%,$(wildcard $(LINUX_DIR)/net/sched/*.ko))
SCHED_MODULES_CORE = sch_ingress sch_fq_codel sch_hfsc cls_fw cls_route cls_flow cls_tcindex cls_u32 em_u32 act_mirred act_skbedit
SCHED_MODULES_FILTER = $(SCHED_MODULES_CORE) act_connmark sch_esfq
SCHED_MODULES_EXTRA = $(filter-out $(SCHED_MODULES_FILTER),$(SCHED_MODULES))
SCHED_FILES = $(patsubst %,$(LINUX_DIR)/net/sched/%.ko,$(filter $(SCHED_MODULES_CORE),$(SCHED_MODULES)))
SCHED_FILES_EXTRA = $(patsubst %,$(LINUX_DIR)/net/sched/%.ko,$(SCHED_MODULES_EXTRA))

define KernelPackage/sched-core
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=Traffic schedulers
  KCONFIG:= \
	CONFIG_NET_SCHED=y \
	CONFIG_NET_SCH_HFSC \
	CONFIG_NET_SCH_INGRESS \
	CONFIG_NET_SCH_FQ_CODEL \
	CONFIG_NET_CLS=y \
	CONFIG_NET_CLS_ACT=y \
	CONFIG_NET_CLS_FLOW \
	CONFIG_NET_CLS_FW \
	CONFIG_NET_CLS_ROUTE4 \
	CONFIG_NET_CLS_TCINDEX \
	CONFIG_NET_CLS_U32 \
	CONFIG_NET_ACT_MIRRED \
	CONFIG_NET_ACT_SKBEDIT \
	CONFIG_NET_EMATCH=y \
	CONFIG_NET_EMATCH_U32
  FILES:=$(SCHED_FILES)
  AUTOLOAD:=$(call AutoLoad,70, $(SCHED_MODULES_CORE))
endef

define KernelPackage/sched-core/description
 Core kernel scheduler support for IP traffic
endef

$(eval $(call KernelPackage,sched-core))


define KernelPackage/sched-connmark
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=Traffic shaper conntrack mark support
  DEPENDS:=+kmod-sched-core +kmod-ipt-core +kmod-ipt-conntrack-extra
  KCONFIG:=CONFIG_NET_ACT_CONNMARK
  FILES:=$(LINUX_DIR)/net/sched/act_connmark.ko
  AUTOLOAD:=$(call AutoLoad,71, act_connmark)
endef
$(eval $(call KernelPackage,sched-connmark))

define KernelPackage/sched-esfq
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=Traffic shaper ESFQ support
  DEPENDS:=+kmod-sched-core +kmod-ipt-core +kmod-ipt-conntrack
  KCONFIG:= \
	CONFIG_NET_SCH_ESFQ \
	CONFIG_NET_SCH_ESFQ_NFCT=y
  FILES:=$(LINUX_DIR)/net/sched/sch_esfq.ko
  AUTOLOAD:=$(call AutoLoad,72, sch_esfq)
endef
$(eval $(call KernelPackage,sched-esfq))

define KernelPackage/sched
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=Extra traffic schedulers
  DEPENDS:=+kmod-sched-core +kmod-ipt-core
  KCONFIG:= \
	CONFIG_NET_SCH_CODEL \
	CONFIG_NET_SCH_DSMARK \
	CONFIG_NET_SCH_HTB \
	CONFIG_NET_SCH_FIFO \
	CONFIG_NET_SCH_GRED \
	CONFIG_NET_SCH_PRIO \
	CONFIG_NET_SCH_RED \
	CONFIG_NET_SCH_TBF \
	CONFIG_NET_SCH_SFQ \
	CONFIG_NET_SCH_TEQL \
	CONFIG_NET_CLS_BASIC \
	CONFIG_NET_ACT_POLICE \
	CONFIG_NET_ACT_IPT \
	CONFIG_NET_EMATCH_CMP \
	CONFIG_NET_EMATCH_NBYTE \
	CONFIG_NET_EMATCH_META \
	CONFIG_NET_EMATCH_TEXT
  FILES:=$(SCHED_FILES_EXTRA)
  AUTOLOAD:=$(call AutoLoad,73, $(SCHED_MODULES_EXTRA))
endef

define KernelPackage/sched/description
 Extra kernel schedulers modules for IP traffic
endef

$(eval $(call KernelPackage,sched))


define KernelPackage/ax25
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=AX25 support
  KCONFIG:= \
	CONFIG_HAMRADIO=y \
	CONFIG_AX25 \
	CONFIG_MKISS
  FILES:= \
	$(LINUX_DIR)/net/ax25/ax25.ko \
	$(LINUX_DIR)/drivers/net/hamradio/mkiss.ko
  AUTOLOAD:=$(call AutoLoad,80,ax25 mkiss)
  $(call AddDepends/crc16)
endef

define KernelPackage/ax25/description
 Kernel modules for AX25 support
endef

$(eval $(call KernelPackage,ax25))


define KernelPackage/mp-alg
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=ECMP caching algorithms
  KCONFIG:= \
	CONFIG_IP_ROUTE_MULTIPATH_RR \
	CONFIG_IP_ROUTE_MULTIPATH_RANDOM \
	CONFIG_IP_ROUTE_MULTIPATH_WRANDOM \
	CONFIG_IP_ROUTE_MULTIPATH_DRR
  FILES:= \
	$(LINUX_DIR)/net/ipv4/multipath_rr.ko \
	$(LINUX_DIR)/net/ipv4/multipath_random.ko \
	$(LINUX_DIR)/net/ipv4/multipath_wrandom.ko \
	$(LINUX_DIR)/net/ipv4/multipath_drr.ko
  AUTOLOAD:=$(call AutoLoad,35,multipath_rr multipath_random multipath_wrandom multipath_drr)
endef

define KernelPackage/mp-alg/description
 Kernel modules that provide several different algorithms for multipath
 route selection from the route cache. The iproute "mpath" argument allows
 specifying which algorithm to use for routes.
 quagga (at least <=0.99.6) requires a multipath patch to support this
 cached mp route feature.
endef

$(eval $(call KernelPackage,mp-alg))


define KernelPackage/pktgen
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  DEPENDS:=@!TARGET_uml
  TITLE:=Network packet generator
  KCONFIG:=CONFIG_NET_PKTGEN
  FILES:=$(LINUX_DIR)/net/core/pktgen.ko
  AUTOLOAD:=$(call AutoLoad,99,pktgen)
endef

define KernelPackage/pktgen/description
  Kernel modules for the Network Packet Generator
endef

$(eval $(call KernelPackage,pktgen))

define KernelPackage/l2tp
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=Layer Two Tunneling Protocol (L2TP)
  DEPENDS:= \
	+IPV6:kmod-ipv6 \
	+(!LINUX_3_8&&!LINUX_3_10&&!LINUX_3_13&&!LINUX_3_14):kmod-udptunnel4 \
	+(!LINUX_3_8&&!LINUX_3_10&&!LINUX_3_13&&!LINUX_3_14&&IPV6):kmod-udptunnel6
  KCONFIG:=CONFIG_L2TP \
	CONFIG_L2TP_V3=y \
	CONFIG_L2TP_DEBUGFS=n
  FILES:=$(LINUX_DIR)/net/l2tp/l2tp_core.ko \
	$(LINUX_DIR)/net/l2tp/l2tp_netlink.ko
  AUTOLOAD:=$(call AutoLoad,32,l2tp_core l2tp_netlink)
endef

define KernelPackage/l2tp/description
 Kernel modules for L2TP V3 Support
endef

$(eval $(call KernelPackage,l2tp))


define KernelPackage/l2tp-eth
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=L2TP ethernet pseudowire support for L2TPv3
  DEPENDS:=+kmod-l2tp
  KCONFIG:=CONFIG_L2TP_ETH
  FILES:=$(LINUX_DIR)/net/l2tp/l2tp_eth.ko
  AUTOLOAD:=$(call AutoLoad,33,l2tp_eth)
endef

define KernelPackage/l2tp-eth/description
 Kernel modules for L2TP ethernet pseudowire support for L2TPv3
endef

$(eval $(call KernelPackage,l2tp-eth))

define KernelPackage/l2tp-ip
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=L2TP IP encapsulation for L2TPv3
  DEPENDS:=+kmod-l2tp
  KCONFIG:=CONFIG_L2TP_IP
  FILES:=$(LINUX_DIR)/net/l2tp/l2tp_ip.ko
  AUTOLOAD:=$(call AutoLoad,33,l2tp_ip)
endef

define KernelPackage/l2tp-ip/description
 Kernel modules for L2TP IP encapsulation for L2TPv3
endef

$(eval $(call KernelPackage,l2tp-ip))


define KernelPackage/sctp
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=SCTP protocol kernel support
  KCONFIG:=\
     CONFIG_IP_SCTP \
     CONFIG_SCTP_DBG_MSG=n \
     CONFIG_SCTP_DBG_OBJCNT=n \
     CONFIG_SCTP_HMAC_NONE=n \
     CONFIG_SCTP_HMAC_SHA1=n \
     CONFIG_SCTP_HMAC_MD5=y \
     CONFIG_SCTP_COOKIE_HMAC_SHA1=n \
     CONFIG_SCTP_COOKIE_HMAC_MD5=y \
     CONFIG_SCTP_DEFAULT_COOKIE_HMAC_NONE=n \
     CONFIG_SCTP_DEFAULT_COOKIE_HMAC_SHA1=n \
     CONFIG_SCTP_DEFAULT_COOKIE_HMAC_MD5=y
  FILES:= $(LINUX_DIR)/net/sctp/sctp.ko
  AUTOLOAD:= $(call AutoLoad,32,sctp)
  DEPENDS:=+kmod-lib-crc32c +kmod-crypto-md5 +kmod-crypto-hmac +IPV6:kmod-ipv6
endef

define KernelPackage/sctp/description
 Kernel modules for SCTP protocol support
endef

$(eval $(call KernelPackage,sctp))


define KernelPackage/netem
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=Network emulation functionality
  DEPENDS:=+kmod-sched
  KCONFIG:=CONFIG_NET_SCH_NETEM
  FILES:=$(LINUX_DIR)/net/sched/sch_netem.ko
  AUTOLOAD:=$(call AutoLoad,99,netem)
endef

define KernelPackage/netem/description
  Kernel modules for emulating the properties of wide area networks
endef

$(eval $(call KernelPackage,netem))

define KernelPackage/slip
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  DEPENDS:=+kmod-slhc
  TITLE:=SLIP modules
  KCONFIG:= \
       CONFIG_SLIP \
       CONFIG_SLIP_COMPRESSED=y \
       CONFIG_SLIP_SMART=y \
       CONFIG_SLIP_MODE_SLIP6=y

  FILES:= \
       $(LINUX_DIR)/drivers/net/slip/slip.ko
  AUTOLOAD:=$(call AutoLoad,30,slip)
endef

define KernelPackage/slip/description
 Kernel modules for SLIP support
endef

$(eval $(call KernelPackage,slip))

define KernelPackage/dnsresolver
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=In-kernel DNS Resolver
  KCONFIG:= CONFIG_DNS_RESOLVER
  FILES:=$(LINUX_DIR)/net/dns_resolver/dns_resolver.ko
  AUTOLOAD:=$(call AutoLoad,30,dns_resolver)
endef

$(eval $(call KernelPackage,dnsresolver))

define KernelPackage/rxrpc
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=AF_RXRPC support
  KCONFIG:= \
	CONFIG_AF_RXRPC \
	CONFIG_RXKAD=m \
	CONFIG_AF_RXRPC_DEBUG=n
  FILES:= \
	$(LINUX_DIR)/net/rxrpc/af-rxrpc.ko \
	$(LINUX_DIR)/net/rxrpc/rxkad.ko
  AUTOLOAD:=$(call AutoLoad,30,rxkad af-rxrpc)
  DEPENDS:=+kmod-crypto-core +kmod-crypto-manager +kmod-crypto-pcbc +kmod-crypto-fcrypt
endef

define KernelPackage/rxrpc/description
  Kernel support for AF_RXRPC; required for AFS client
endef

$(eval $(call KernelPackage,rxrpc))
