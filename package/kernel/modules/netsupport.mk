#
# Copyright (C) 2006-2008 OpenWrt.org
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
	$(LINUX_DIR)/drivers/isdn/capi/capifs.ko \
	$(LINUX_DIR)/drivers/isdn/capi/capi.ko
  AUTOLOAD:=$(call AutoLoad,30,kernelcapi capifs capi)
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
  DEPENDS:=+kmod-iptunnel4
  KCONFIG:=CONFIG_NET_IPIP
  FILES:=$(LINUX_DIR)/net/ipv4/ipip.ko
  AUTOLOAD:=$(call AutoLoad,32,ipip)
endef

define KernelPackage/ipip/description
 Kernel modules for IP-in-IP encapsulation
endef

$(eval $(call KernelPackage,ipip))


IPSEC-m:= \
	key/af_key \
	xfrm/xfrm_ipcomp \
	xfrm/xfrm_user \

define KernelPackage/ipsec
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=IPsec related modules (IPv4 and IPv6)
  DEPENDS:=+kmod-crypto-core +kmod-crypto-des +kmod-crypto-hmac +kmod-crypto-md5 +kmod-crypto-sha1 +kmod-crypto-deflate +kmod-crypto-cbc
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
 - ipcomp
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


# NOTE: tunnel4 is not selectable by itself, so enable ipip for that
define KernelPackage/iptunnel4
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=IPv4 tunneling
  KCONFIG:= \
	CONFIG_NET_IPIP \
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
  DEPENDS:=+kmod-ipv6 +kmod-iptunnel4
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
  KCONFIG:=CONFIG_NET_IPGRE CONFIG_NET_IPGRE_DEMUX
 ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.37)),1)
  FILES:=$(LINUX_DIR)/net/ipv4/ip_gre.ko $(LINUX_DIR)/net/ipv4/gre.ko
  AUTOLOAD:=$(call AutoLoad,39,gre ip_gre)
 else
  FILES:=$(LINUX_DIR)/net/ipv4/ip_gre.ko
  AUTOLOAD:=$(call AutoLoad,39,ip_gre)
 endif
endef

define KernelPackage/gre/description
 Generic Routing Encapsulation support
endef

$(eval $(call KernelPackage,gre))


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


define KernelPackage/ppp
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=PPP modules
  DEPENDS:=+kmod-crc-ccitt
  KCONFIG:= \
	CONFIG_PPP \
	CONFIG_PPP_ASYNC \
	CONFIG_SLHC
  FILES:= \
	$(LINUX_DIR)/drivers/net/ppp_async.ko \
	$(LINUX_DIR)/drivers/net/ppp_generic.ko \
	$(LINUX_DIR)/drivers/net/slhc.ko
  AUTOLOAD:=$(call AutoLoad,30,slhc ppp_generic ppp_async)
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
  FILES:=$(LINUX_DIR)/drivers/net/ppp_synctty.ko
  AUTOLOAD:=$(call AutoLoad,40,ppp_synctty)
endef

define KernelPackage/ppp-synctty/description
 Kernel modules for PPP sync tty support
endef

$(eval $(call KernelPackage,ppp-synctty))


define KernelPackage/pppoe
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=PPPoE support
  DEPENDS:=kmod-ppp
  KCONFIG:=CONFIG_PPPOE
  FILES:= \
	$(LINUX_DIR)/drivers/net/pppoe.ko \
	$(LINUX_DIR)/drivers/net/pppox.ko
  AUTOLOAD:=$(call AutoLoad,40,pppox pppoe)
endef

define KernelPackage/pppoe/description
 Kernel modules for PPPoE (PPP over Ethernet) support
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
  DEPENDS:=kmod-ppp +kmod-gre @LINUX_2_6_37||LINUX_2_6_38
  KCONFIG:=CONFIG_PPTP
  FILES:=$(LINUX_DIR)/drivers/net/pptp.ko
  AUTOLOAD:=$(call AutoLoad,41,pptp)
endef

$(eval $(call KernelPackage,pptp))
	

define KernelPackage/pppol2tp
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=PPPoL2TP support
  DEPENDS:=kmod-ppp +kmod-pppoe +LINUX_2_6_35||LINUX_2_6_36||LINUX_2_6_37||LINUX_2_6_38:kmod-l2tp
  KCONFIG:=CONFIG_PPPOL2TP
  ifneq ($(CONFIG_LINUX_2_6_35)$(CONFIG_LINUX_2_6_36)$(CONFIG_LINUX_2_6_37)$(CONFIG_LINUX_2_6_38),)
    FILES:=$(LINUX_DIR)/net/l2tp/l2tp_ppp.ko
    AUTOLOAD:=$(call AutoLoad,40,l2tp_ppp)
  else
    FILES:=$(LINUX_DIR)/drivers/net/pppol2tp.ko
    AUTOLOAD:=$(call AutoLoad,40,pppol2tp)
  endif
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
  AUTOLOAD:=$(call AutoLoad,40,clip)
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
  FILES:=$(LINUX_DIR)/drivers/net/ppp_mppe.ko
  AUTOLOAD:=$(call AutoLoad,31,ppp_mppe)
endef

define KernelPackage/mppe/description
 Kernel modules for Microsoft PPP compression/encryption
endef

$(eval $(call KernelPackage,mppe))


define KernelPackage/sched
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=Traffic schedulers
  KCONFIG:= \
	CONFIG_NET_SCHED=y \
	CONFIG_NET_SCH_DSMARK \
	CONFIG_NET_SCH_ESFQ \
	CONFIG_NET_SCH_ESFQ_NFCT=y \
	CONFIG_NET_SCH_FIFO \
	CONFIG_NET_SCH_GRED \
	CONFIG_NET_SCH_HFSC \
	CONFIG_NET_SCH_HTB \
	CONFIG_NET_SCH_INGRESS \
	CONFIG_NET_SCH_PRIO \
	CONFIG_NET_SCH_RED \
	CONFIG_NET_SCH_TBF \
	CONFIG_NET_SCH_SFQ \
	CONFIG_NET_SCH_TEQL \
	CONFIG_NET_CLS=y \
	CONFIG_NET_CLS_ACT=y \
	CONFIG_NET_CLS_BASIC \
	CONFIG_NET_CLS_FLOW \
	CONFIG_NET_CLS_FW \
	CONFIG_NET_CLS_ROUTE4 \
	CONFIG_NET_CLS_TCINDEX \
	CONFIG_NET_CLS_U32 \
	CONFIG_NET_ACT_MIRRED \
	CONFIG_NET_ACT_IPT \
	CONFIG_NET_ACT_POLICE \
	CONFIG_NET_ACT_CONNMARK \
	CONFIG_NET_ACT_SKBEDIT \
	CONFIG_NET_EMATCH=y \
	CONFIG_NET_EMATCH_CMP \
	CONFIG_NET_EMATCH_NBYTE \
	CONFIG_NET_EMATCH_U32 \
	CONFIG_NET_EMATCH_META \
	CONFIG_NET_EMATCH_TEXT
  FILES:=$(LINUX_DIR)/net/sched/*.ko
endef

define KernelPackage/sched/description
 Kernel schedulers for IP traffic
endef

$(eval $(call KernelPackage,sched))


define KernelPackage/ax25
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=AX25 support
  KCONFIG:= \
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
  DEPENDS:=@LINUX_2_6_35||LINUX_2_6_36||LINUX_2_6_37||LINUX_2_6_38
  TITLE:=Layer Two Tunneling Protocol (L2TP)
  KCONFIG:=CONFIG_L2TP \
	CONFIG_L2TP_V3=y \
	CONFIG_L2TP_DEBUGFS=n
  FILES:=$(LINUX_DIR)/net/l2tp/l2tp_core.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/l2tp/l2tp_netlink.$(LINUX_KMOD_SUFFIX)
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
  FILES:=$(LINUX_DIR)/net/l2tp/l2tp_eth.$(LINUX_KMOD_SUFFIX) 
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
  FILES:=$(LINUX_DIR)/net/l2tp/l2tp_ip.$(LINUX_KMOD_SUFFIX)
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
     CONFIG_SCTP_HMAC_MD5=y
  FILES:= $(LINUX_DIR)/net/sctp/sctp.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:= $(call AutoLoad,32,sctp)
  DEPENDS:=+kmod-libcrc32c +kmod-crypto-md5 +kmod-crypto-hmac
endef

define KernelPackage/sctp/description
 Kernel modules for SCTP protocol support
endef

$(eval $(call KernelPackage,sctp))


