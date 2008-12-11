# 
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

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
	$(LINUX_DIR)/drivers/isdn/capi/capi.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/isdn/capi/capifs.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,kernelcapi capi)
endef

define KernelPackage/capi/description
 Kernel module for basic CAPI (ISDN) support
endef

$(eval $(call KernelPackage,capi))


define KernelPackage/ipip
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  TITLE:=IP-in-IP encapsulation
  DEPENDS:=+kmod-iptunnel4
  KCONFIG:=CONFIG_NET_IPIP
  FILES:=$(LINUX_DIR)/net/ipv4/ipip.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,ipip)
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
  DEPENDS:= @LINUX_2_6
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
	CONFIG_IPV6_SUBTREES=y
  FILES:=$(LINUX_DIR)/net/ipv6/ipv6.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,ipv6)
endef

define KernelPackage/ipv6/description
 Kernel modules for IPv6 support
endef

$(eval $(call KernelPackage,ipv6))


define KernelPackage/sit
  SUBMENU:=$(NETWORK_SUPPORT_MENU)
  DEPENDS:=+kmod-iptunnel4
  TITLE:=IPv6-in-IPv4 tunnelling
endef

# sit is compiled in the 2.4 ipv6 stack
define KernelPackage/sit/2.4
  DEPENDS:= @LINUX_2_4 +kmod-ipv6
  KCONFIG:=CONFIG_IPV6
endef

define KernelPackage/sit/2.6
  DEPENDS:= @LINUX_2_6 +kmod-ipv6 +kmod-iptunnel4
  KCONFIG+=CONFIG_IPV6_SIT
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
	CONFIG_NET_SCH_FIFO \
	CONFIG_NET_SCH_GRED \
	CONFIG_NET_SCH_HFSC \
	CONFIG_NET_SCH_HTB \
	CONFIG_NET_SCH_INGRESS \
	CONFIG_NET_SCH_PRIO \
	CONFIG_NET_SCH_RED \
	CONFIG_NET_SCH_SFQ \
	CONFIG_NET_SCH_TBF \
	CONFIG_NET_SCH_TEQL
  FILES:=$(LINUX_DIR)/net/sched/*.$(LINUX_KMOD_SUFFIX)
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


NETWORK_DEVICES_MENU:=Network Devices

define KernelPackage/natsemi
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=National Semiconductor DP8381x series 
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_NATSEMI
  FILES:=$(LINUX_DIR)/drivers/net/natsemi.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,natsemi)
endef

define KernelPackage/natsemi/description
 Kernel modules for National Semiconductor DP8381x series PCI Ethernet 
 adapters.
endef

$(eval $(call KernelPackage,natsemi))


define KernelPackage/r6040
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=RDC Fast-Ethernet support
  DEPENDS:=@TARGET_rdc
  KCONFIG:=CONFIG_R6040
  FILES:=$(LINUX_DIR)/drivers/net/r6040.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,99,r6040)
endef

define KernelPackage/r6040/description
 Kernel modules for RDC Fast-Ethernet adapters.
endef

$(eval $(call KernelPackage,r6040))


define KernelPackage/sis900
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=SiS 900 Ethernet support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_SIS900
  FILES:=$(LINUX_DIR)/drivers/net/sis900.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,sis900)
endef

define KernelPackage/sis900/description
 Kernel modules for Sis 900 Ethernet adapters.
endef

$(eval $(call KernelPackage,sis900))

define KernelPackage/sky2
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=SysKonnect Yukon2 support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_SKY2
  FILES:=$(LINUX_DIR)/drivers/net/sky2.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,sky2)
endef

define KernelPackage/sky2/description
  This driver supports Gigabit Ethernet adapters based on the
  Marvell Yukon 2 chipset:
  Marvell 88E8021/88E8022/88E8035/88E8036/88E8038/88E8050/88E8052/
  88E8053/88E8055/88E8061/88E8062, SysKonnect SK-9E21D/SK-9S21

  There is companion driver for the older Marvell Yukon and
  Genesis based adapters: skge.
endef

$(eval $(call KernelPackage,sky2))


define KernelPackage/via-rhine
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Via Rhine ethernet support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_VIA_RHINE
  FILES:=$(LINUX_DIR)/drivers/net/via-rhine.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,via-rhine)
endef

define KernelPackage/via-rhine/description
 Kernel modules for Via Rhine Ethernet chipsets.
endef

$(eval $(call KernelPackage,via-rhine))


define KernelPackage/via-velocity
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=VIA Velocity Gigabit Ethernet Adapter kernel support
  DEPENDS:=@TARGET_ixp4xx
  KCONFIG:=CONFIG_VIA_VELOCITY
  FILES:=$(LINUX_DIR)/drivers/net/via-velocity.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,via-velocity)
endef

define KernelPackage/via-velocity/description
 Kernel modules for VIA Velocity Gigabit Ethernet chipsets.
endef

$(eval $(call KernelPackage,via-velocity))


define KernelPackage/8139too
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=RealTek RTL-8139 PCI Fast Ethernet Adapter kernel support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_8139TOO
  FILES:=$(LINUX_DIR)/drivers/net/8139too.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,8139too)
endef

define KernelPackage/8139too/description
 Kernel modules for RealTek RTL-8139 PCI Fast Ethernet adapters.
endef

$(eval $(call KernelPackage,8139too))


define KernelPackage/r8169
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=RealTek RTL-8169 PCI Gigabit Ethernet Adapter kernel support
  DEPENDS:=@TARGET_x86 
  KCONFIG:=CONFIG_R8169 CONFIG_R8169_NAPI=y CONFIG_R8169_VLAN=n
  FILES:=$(LINUX_DIR)/drivers/net/r8169.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,r8169)
endef

define KernelPackage/r8169/description
 Kernel modules for RealTek RTL-8169 PCI Gigabit Ethernet adapters.
endef

$(eval $(call KernelPackage,r8169))


define KernelPackage/ne2k-pci
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=ne2k-pci Ethernet Adapter kernel support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_NE2K_PCI
  FILES:= \
	$(LINUX_DIR)/drivers/net/ne2k-pci.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/8390.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,8390 ne2k-pci)
endef

define KernelPackage/ne2k-pci/description
 Kernel modules for NE2000 PCI Ethernet Adapter kernel.
endef

$(eval $(call KernelPackage,ne2k-pci))


define KernelPackage/ixp4xx-npe
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Intel(R) IXP4xx ethernet support
  DEPENDS:=@TARGET_ixp4xx
  KCONFIG:=CONFIG_IXP4XX_MAC
  FILES:= \
	$(LINUX_DIR)/drivers/net/ixp4xx/ixp4xx_npe.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/ixp4xx/ixp4xx_qmgr.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/ixp4xx/ixp4xx_mac.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,ixp4xx_npe ixp4xx_qmgr ixp4xx_mac)
endef

define KernelPackage/ixp4xx-npe/description
 Kernel modules for Intel(R) IXP4xx onboard ethernet.
endef

$(eval $(call KernelPackage,ixp4xx-npe))


define KernelPackage/e100
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Intel(R) PRO/100+ cards kernel support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_E100
  FILES:=$(LINUX_DIR)/drivers/net/e100.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,e100)
endef

define KernelPackage/e100/description
 Kernel modules for Intel(R) PRO/100+ Ethernet adapters.
endef

$(eval $(call KernelPackage,e100))


define KernelPackage/e1000
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Intel(R) PRO/1000 cards kernel support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_E1000
  FILES:=$(LINUX_DIR)/drivers/net/e1000/e1000.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,e1000)
endef

define KernelPackage/e1000/description
 Kernel modules for Intel(R) PRO/1000 Ethernet adapters.
endef

$(eval $(call KernelPackage,e1000))

define KernelPackage/b44
  TITLE:=Broadcom 44xx driver
  FILES:=$(LINUX_DIR)/drivers/net/b44.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=CONFIG_B44
  DEPENDS:=@TARGET_x86 @BROKEN
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  AUTOLOAD:=$(call AutoLoad,50,b44)
endef

define KernelPackage/b44/description
 Kernel modules for Broadcom 44xx Ethernet adapters.
endef
$(eval $(call KernelPackage,b44))

define KernelPackage/3c59x
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=3Com 3c590/3c900 series (592/595/597) Vortex/Boomerang
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_VORTEX
  FILES:=$(LINUX_DIR)/drivers/net/3c59x.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,3c59x)
endef

define KernelPackage/3c59x/description
 This option enables driver support for a large number of 10mbps and 
 10/100mbps EISA, PCI and PCMCIA 3Com Ethernet adapters: 
 - "Vortex"    (Fast EtherLink 3c590/3c592/3c595/3c597) EISA and PCI 
 - "Boomerang" (EtherLink XL 3c900 or 3c905)            PCI 
 - "Cyclone"   (3c540/3c900/3c905/3c980/3c575/3c656)    PCI and Cardbus 
 - "Tornado"   (3c905)                                  PCI  
 - "Hurricane" (3c555/3cSOHO)                           PCI 
endef

$(eval $(call KernelPackage,3c59x))

define KernelPackage/pcnet32
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=AMD PCnet32 PCI support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_PCNET32
  FILES:=$(LINUX_DIR)/drivers/net/pcnet32.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,pcnet32)
endef

define KernelPackage/pcnet32/description
 Kernel modules for AMD PCnet32 Ethernet adapters.
endef

$(eval $(call KernelPackage,pcnet32))


define KernelPackage/tg3
  TITLE:=Broadcom Tigon3 Gigabit Ethernet
  FILES:=$(LINUX_DIR)/drivers/net/tg3.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=CONFIG_TIGON3
  DEPENDS:=@LINUX_2_6
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  AUTOLOAD:=$(call AutoLoad,50,tg3)
endef

define KernelPackage/tg3/description
 Kernel modules for Broadcom Tigon3 Gigabit Ethernet adapters.
endef
$(eval $(call KernelPackage,tg3))

define KernelPackage/ssb-gige
  TITLE:=Broadcom SSB Gigabit Ethernet
  KCONFIG:=CONFIG_SSB_DRIVER_GIGE=y
  DEPENDS:=@TARGET_brcm47xx +kmod-tg3
  SUBMENU:=$(NETWORK_DEVICES_MENU)
endef

define KernelPackage/ssb-gige/description
 Kernel modules for Broadcom SSB Gigabit Ethernet adapters.
endef
$(eval $(call KernelPackage,ssb-gige))
