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
  DEPENDS:=@LINUX_2_6
  KCONFIG:= \
	CONFIG_ATM \
	CONFIG_ATM_BR2684
  FILES:= \
	$(LINUX_DIR)/net/atm/atm.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/atm/br2684.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,atm br2684)
endef

define KernelPackage/atm/description
 Kernel modules for ATM support
endef

$(eval $(call KernelPackage,atm))


define KernelPackage/atmtcp
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=ATM over TCP
  DEPENDS:=@LINUX_2_6 kmod-atm
  KCONFIG:=CONFIG_ATM_TCP CONFIG_ATM_DRIVERS=y
  FILES:=$(LINUX_DIR)/drivers/atm/atmtcp.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,atmtcp)
endef

define KernelPackage/atmtcp/description
 Kernel module for ATM over TCP support
endef

$(eval $(call KernelPackage,atmtcp))


define KernelPackage/bonding
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=Ethernet bonding driver
  KCONFIG:=CONFIG_BONDING
  FILES:=$(LINUX_DIR)/drivers/net/bonding/bonding.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,bonding)
endef

define KernelPackage/bonding/description
 Kernel module for NIC bonding.
endef

$(eval $(call KernelPackage,bonding))


define KernelPackage/capi
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=CAPI (ISDN) Support
  DEPENDS:=@LINUX_2_6
  KCONFIG:= \
	CONFIG_ISDN_CAPI \
	CONFIG_ISDN_CAPI_CAPI20 \
	CONFIG_ISDN_CAPIFS
  FILES:= \
	$(LINUX_DIR)/drivers/isdn/capi/kernelcapi.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/isdn/capi/capifs.$(LINUX_KMOD_SUFFIX)
	$(LINUX_DIR)/drivers/isdn/capi/capi.$(LINUX_KMOD_SUFFIX) \
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
  	CONFIG_MISDN \
	CONFIG_MISDN_DSP \
	CONFIG_MISDN_L1OIP \
	CONFIG_ISDN_AUDIO=n \
	CONFIG_ISDN_WITH_ABC=n \
	CONFIG_ISDN_DRV_LOOP=n \
	CONFIG_ISDN_DIVERSION=n \
	CONFIG_ISDN_DRV_HISAX=n \
	CONFIG_ISDN_DRV_ICN=n \
	CONFIG_ISDN_DRV_PCBIT=n \
	CONFIG_ISDN_DRV_SC=n \
	CONFIG_ISDN_DRV_ACT2000=n \
	CONFIG_ISDN_DRV_EICON=n \
	CONFIG_ISDN_DRV_TPAM=n \
	CONFIG_HYSDN=n
  FILES:= \
  	$(LINUX_DIR)/drivers/isdn/mISDN/mISDN_core.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/isdn/mISDN/mISDN_dsp.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/isdn/mISDN/l1oip.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,mISDN_core mISDN_dsp l1oip)
endef

define KernelPackage/misdn/2.4
  KCONFIG+= \
	CONFIG_ISDN
endef

define KernelPackage/misdn/2.6
   KCONFIG+= \
	CONFIG_ISDN=y
endef

define KernelPackage/misdn/description
  Modular ISDN driver support
endef

$(eval $(call KernelPackage,misdn))


define KernelPackage/isdn4linux
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=Old ISDN4Linux (deprecated)
  KCONFIG:= \
    CONFIG_ISDN_I4L \
    CONFIG_ISDN_PPP=y \
    CONFIG_ISDN_PPP_VJ=y \
    CONFIG_ISDN_MPP=y \
    CONFIG_IPPP_FILTER=y \
    CONFIG_ISDN_PPP_BSDCOMP \
    CONFIG_ISDN_AUDIO=y \
    CONFIG_ISDN_TTY_FAX=y \
    CONFIG_ISDN_X25=y \
    CONFIG_ISDN_DIVERSION \
    CONFIG_ISDN_CAPI_CAPIDRV=n \
    CONFIG_ISDN_DRV_ACT2000=n \
    CONFIG_ISDN_DRV_GIGASET=n \
    CONFIG_ISDN_DRV_HISAX=n \
    CONFIG_ISDN_DRV_ICN=n \
    CONFIG_ISDN_DRV_LOOP=n \
    CONFIG_ISDN_DRV_PCBIT=n \
    CONFIG_ISDN_DRV_SC=n \
    CONFIG_HYSDN=n
  FILES:= \
    $(LINUX_DIR)/drivers/isdn/divert/dss1_divert.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,isdn isdn_bsdcomp dss1_divert)
endef

define KernelPackage/isdn4linux/2.4
  KCONFIG+= \
	CONFIG_ISDN
  FILES+= \
	$(LINUX_DIR)/drivers/isdn/isdn_bsdcomp.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/isdn4linux/2.6
  KCONFIG+= \
	CONFIG_ISDN=y
  FILES+= \
	$(LINUX_DIR)/drivers/isdn/i4l/isdn.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/isdn/i4l/isdn_bsdcomp.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/isdn4linux/description
  This driver allows you to use an ISDN adapter for networking
endef

$(eval $(call KernelPackage,isdn4linux))


define KernelPackage/ipip
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=IP-in-IP encapsulation
  DEPENDS:=+LINUX_2_6:kmod-iptunnel4
  KCONFIG:=CONFIG_NET_IPIP
  FILES:=$(LINUX_DIR)/net/ipv4/ipip.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,32,ipip)
endef

define KernelPackage/ipip/description
 Kernel modules for IP-in-IP encapsulation
endef

$(eval $(call KernelPackage,ipip))


define KernelPackage/ipsec
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=IPsec related modules (IPv4 and IPv6)
  DEPENDS:=@LINUX_2_6 +kmod-crypto-core +kmod-crypto-des +kmod-crypto-hmac +kmod-crypto-md5 +kmod-crypto-sha1
  KCONFIG:= \
	CONFIG_NET_KEY \
	CONFIG_XFRM_USER
  FILES:= \
	$(LINUX_DIR)/net/key/af_key.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/xfrm/xfrm_user.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/ipsec/description
 Kernel modules for IPsec support in both IPv4 and IPv6.
 Includes:
 - af_key
 - xfrm_user
endef

$(eval $(call KernelPackage,ipsec))


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
  FILES:= $(foreach mod,ah4 esp4 ipcomp xfrm4_mode_beet xfrm4_mode_transport xfrm4_mode_tunnel xfrm4_tunnel , \
	$(LINUX_DIR)/net/ipv4/$(mod).$(LINUX_KMOD_SUFFIX) \
  )
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
  FILES:= $(foreach mod,ah6 esp6 ipcomp6 xfrm6_mode_beet xfrm6_mode_transport xfrm6_mode_tunnel xfrm6_tunnel, \
	$(LINUX_DIR)/net/ipv6/$(mod).$(LINUX_KMOD_SUFFIX) \
  )
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
  DEPENDS:=@LINUX_2_6
  KCONFIG:= \
	CONFIG_NET_IPIP \
	CONFIG_INET_TUNNEL
  FILES:= $(foreach mod,tunnel4, \
	$(LINUX_DIR)/net/ipv4/$(mod).$(LINUX_KMOD_SUFFIX) \
  )
  AUTOLOAD:=$(call AutoLoad,31,tunnel4)
endef

define KernelPackage/iptunnel4/description
 Kernel modules for IPv4 tunneling
endef

$(eval $(call KernelPackage,iptunnel4))


define KernelPackage/iptunnel6
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=IPv6 tunneling
  DEPENDS:= @LINUX_2_6 +kmod-ipv6
  KCONFIG:= \
	CONFIG_INET6_TUNNEL
  FILES:= $(foreach mod,tunnel6, \
	$(LINUX_DIR)/net/ipv6/$(mod).$(LINUX_KMOD_SUFFIX) \
  )
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
  FILES:=$(LINUX_DIR)/net/ipv6/ipv6.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,ipv6)
endef

define KernelPackage/ipv6/description
 Kernel modules for IPv6 support
endef

$(eval $(call KernelPackage,ipv6))


# sit is not selectable on 2.4, but built when ipv6 is enabled
define KernelPackage/sit
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  DEPENDS:=+kmod-ipv6 +LINUX_2_6:kmod-iptunnel4
  TITLE:=IPv6-in-IPv4 tunnelling
  KCONFIG:=CONFIG_IPV6 CONFIG_IPV6_SIT
  FILES:=$(LINUX_DIR)/net/ipv6/sit.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,32,sit)
endef

define KernelPackage/sit/description
 Kernel modules for IPv6-in-IPv4 tunnelling
endef

$(eval $(call KernelPackage,sit))


define KernelPackage/ip6-tunnel
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=IP-in-IPv6 tunnelling
  DEPENDS:= @LINUX_2_6 +kmod-ipv6 +kmod-iptunnel6
  KCONFIG:= CONFIG_IPV6_TUNNEL
  FILES:= $(foreach mod,ip6_tunnel, \
	$(LINUX_DIR)/net/ipv6/$(mod).$(LINUX_KMOD_SUFFIX) \
  )
  AUTOLOAD:=$(call AutoLoad,32,ip6_tunnel)
endef

define KernelPackage/ip6-tunnel/description
 Kernel modules for IPv6-in-IPv6 and IPv4-in-IPv6 tunnelling
endef

$(eval $(call KernelPackage,ip6-tunnel))


define KernelPackage/gre
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=GRE support
  KCONFIG:=CONFIG_NET_IPGRE
  FILES=$(LINUX_DIR)/net/ipv4/ip_gre.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/gre/description
 Generic Routing Encapsulation support
endef

$(eval $(call KernelPackage,gre))


define KernelPackage/tun
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=Universal TUN/TAP driver
  KCONFIG:=CONFIG_TUN
  FILES:=$(LINUX_DIR)/drivers/net/tun.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,tun)
endef

define KernelPackage/tun/description
 Kernel support for the TUN/TAP tunneling device
endef

$(eval $(call KernelPackage,tun))


define KernelPackage/ppp
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=PPP modules
  DEPENDS:=+LINUX_2_6:kmod-crc-ccitt
  KCONFIG:= \
	CONFIG_PPP \
	CONFIG_PPP_ASYNC \
	CONFIG_SLHC
  FILES:= \
	$(LINUX_DIR)/drivers/net/ppp_async.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/ppp_generic.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/slhc.$(LINUX_KMOD_SUFFIX)
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
  FILES:=$(LINUX_DIR)/drivers/net/ppp_synctty.$(LINUX_KMOD_SUFFIX)
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
	$(LINUX_DIR)/drivers/net/pppoe.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/pppox.$(LINUX_KMOD_SUFFIX)
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
  FILES:=$(LINUX_DIR)/net/atm/pppoatm.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,pppoatm)
endef

define KernelPackage/pppoa/description
 Kernel modules for PPPoA (PPP over ATM) support
endef

$(eval $(call KernelPackage,pppoa))

define KernelPackage/pppol2tp
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=PPPoL2TP support
  DEPENDS:=kmod-ppp +kmod-pppoe @!LINUX_2_6_21||!LINUX_2_6_25
  KCONFIG:=CONFIG_PPPOL2TP
  FILES:=$(LINUX_DIR)/drivers/net/pppol2tp.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,pppol2tp)
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
  FILES:=$(LINUX_DIR)/net/atm/clip.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,clip)
endef

define KernelPackage/ipoa/description
  Kernel modules for IPoA (IP over ATM) support
endef

$(eval $(call KernelPackage,ipoa))


define KernelPackage/mppe
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=Microsoft PPP compression/encryption
  DEPENDS:=kmod-ppp +kmod-crypto-arc4 +kmod-crypto-sha1
  KCONFIG:= \
	CONFIG_PPP_MPPE_MPPC \
	CONFIG_PPP_MPPE
endef

define KernelPackage/mppe/2.4
#  KCONFIG:=CONFIG_PPP_MPPE_MPPC
  FILES:=$(LINUX_DIR)/drivers/net/ppp_mppe_mppc.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,31,ppp_mppe_mppc)
endef

define KernelPackage/mppe/2.6
#  KCONFIG:=CONFIG_PPP_MPPE
  FILES:=$(LINUX_DIR)/drivers/net/ppp_mppe.$(LINUX_KMOD_SUFFIX)
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
	CONFIG_NET_EMATCH=y \
	CONFIG_NET_EMATCH_CMP \
	CONFIG_NET_EMATCH_NBYTE \
	CONFIG_NET_EMATCH_U32 \
	CONFIG_NET_EMATCH_META \
	CONFIG_NET_EMATCH_TEXT
  FILES:=$(LINUX_DIR)/net/sched/*.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/sched/description
 Kernel schedulers for IP traffic
endef

$(eval $(call KernelPackage,sched))


define KernelPackage/ax25
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=AX25 support
  DEPENDS:=kmod-crc16
  KCONFIG:= \
	CONFIG_AX25 \
	CONFIG_MKISS
  FILES:= \
	$(LINUX_DIR)/net/ax25/ax25.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/hamradio/mkiss.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,80,ax25 mkiss)
endef

define KernelPackage/ax25/description
 Kernel modules for AX25 support
endef

$(eval $(call KernelPackage,ax25))


define KernelPackage/mp-alg
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=ECMP caching algorithms
  DEPENDS:=@LINUX_2_6
  KCONFIG:= \
	CONFIG_IP_ROUTE_MULTIPATH_RR \
	CONFIG_IP_ROUTE_MULTIPATH_RANDOM \
	CONFIG_IP_ROUTE_MULTIPATH_WRANDOM \
	CONFIG_IP_ROUTE_MULTIPATH_DRR
  FILES:= \
	$(LINUX_DIR)/net/ipv4/multipath_rr.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/ipv4/multipath_random.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/ipv4/multipath_wrandom.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/ipv4/multipath_drr.$(LINUX_KMOD_SUFFIX)
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
  DEPENDS:=@LINUX_2_6
  KCONFIG:=CONFIG_NET_PKTGEN
  FILES:=$(LINUX_DIR)/net/core/pktgen.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,99,pktgen)
endef

define KernelPackage/pktgen/description
  Kernel modules for the Network Packet Generator
endef

$(eval $(call KernelPackage,pktgen))

