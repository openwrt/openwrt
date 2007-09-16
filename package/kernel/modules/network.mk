# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

NSMENU:=Network Support

define KernelPackage/atm
  SUBMENU:=$(NSMENU)
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
  SUBMENU:=$(NSMENU)
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
  SUBMENU:=$(NSMENU)
  TITLE:=Ethernet bonding driver
  KCONFIG:=CONFIG_BONDING
  FILES:=$(LINUX_DIR)/drivers/net/bonding/bonding.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,bonding)
endef

define KernelPackage/bonding/description
 Kernel module for NIC bonding.
endef

$(eval $(call KernelPackage,bonding))


define KernelPackage/ipip
  SUBMENU:=$(NSMENU)
  TITLE:=IP in IP encapsulation support
  KCONFIG:= \
	CONFIG_NET_IPIP \
	CONFIG_INET_TUNNEL
  FILES:=$(LINUX_DIR)/net/ipv4/ipip.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,ipip)
endef

define KernelPackage/ipip/2.6
#  KCONFIG+=CONFIG_INET_TUNNEL
  FILES+=$(LINUX_DIR)/net/ipv4/tunnel4.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD+=$(call AutoLoad,31,tunnel4)
endef

define KernelPackage/ipip/description
 Kernel modules for IP in IP encapsulation
endef

$(eval $(call KernelPackage,ipip))


define KernelPackage/ipsec
  SUBMENU:=$(NSMENU)
  TITLE:=IPsec related modules (IPv4 and IPv6)
  DEPENDS:=@LINUX_2_6 +kmod-crypto
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
  SUBMENU:=$(NSMENU)
  TITLE:=IPsec related modules (IPv4)
  DEPENDS:=kmod-ipsec
  KCONFIG:= \
	CONFIG_INET_AH \
	CONFIG_INET_ESP \
	CONFIG_INET_IPCOMP \
	CONFIG_INET_XFRM_TUNNEL
  FILES:= $(foreach mod,ah4 esp4 ipcomp xfrm4_tunnel, \
	$(LINUX_DIR)/net/ipv4/$(mod).$(LINUX_KMOD_SUFFIX) \
  )
endef

define KernelPackage/ipsec4/description
 Kernel modules for IPsec support in IPv4.
 Includes:
 - ah4
 - esp4
 - ipcomp
 - xfrm4_tunnel
endef

$(eval $(call KernelPackage,ipsec4))


define KernelPackage/ipsec6
  SUBMENU:=$(NSMENU)
  TITLE:=IPsec related modules (IPv6)
  DEPENDS:=kmod-ipsec
  KCONFIG:= \
	CONFIG_INET6_AH \
	CONFIG_INET6_ESP \
	CONFIG_INET6_IPCOMP \
	CONFIG_INET6_XFRM_TUNNEL \
	CONFIG_INET6_TUNNEL
  FILES:= $(foreach mod,ah6 esp6 ipcomp6 xfrm6_tunnel tunnel6, \
	$(LINUX_DIR)/net/ipv6/$(mod).$(LINUX_KMOD_SUFFIX) \
  )
endef

define KernelPackage/ipsec6/description
 Kernel modules for IPsec support in IPv6.
 Includes:
 - ah6
 - esp6
 - ipcomp6
 - xfrm6_tunnel
 - tunnel6
endef

$(eval $(call KernelPackage,ipsec6))


define KernelPackage/ipv6
  SUBMENU:=$(NSMENU)
  TITLE:=IPv6 support
  KCONFIG:= \
	CONFIG_IPV6 \
  	CONFIG_IPV6_SIT
  FILES:=$(LINUX_DIR)/net/ipv6/ipv6.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,ipv6)
endef

define KernelPackage/ipv6/2.6
#  KCONFIG+=CONFIG_IPV6_SIT
  FILES+=$(LINUX_DIR)/net/ipv6/sit.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD+=$(call AutoLoad,21,sit)
endef

define KernelPackage/ipv6/description
 Kernel modules for IPv6 support
endef

$(eval $(call KernelPackage,ipv6))


define KernelPackage/gre
  SUBMENU:=$(NSMENU)
  TITLE:=GRE support
  KCONFIG:=CONFIG_NET_IPGRE
  FILES=$(LINUX_DIR)/net/ipv4/ip_gre.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/gre/description
 Generic Routing Encapsulation support
endef

$(eval $(call KernelPackage,gre))


define KernelPackage/tun
  SUBMENU:=$(NSMENU)
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
  SUBMENU:=$(NSMENU)
  TITLE:=PPP modules
  KCONFIG:= \
	CONFIG_PPP \
	CONFIG_PPP_ASYNC \
	CONFIG_SLHC \
	CONFIG_CRC_CCITT
  FILES:= \
	$(LINUX_DIR)/drivers/net/ppp_async.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/ppp_generic.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/drivers/net/slhc.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/ppp/2.4
  AUTOLOAD:=$(call AutoLoad,30,slhc ppp_generic ppp_async)
endef

define KernelPackage/ppp/2.6
#  KCONFIG+= \
#	CONFIG_SLHC \
#	CONFIG_CRC_CCITT
  FILES+= \
	$(LINUX_DIR)/lib/crc-ccitt.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,crc-ccitt slhc ppp_generic ppp_async)
endef

define KernelPackage/ppp/description
 Kernel modules for PPP support
endef

$(eval $(call KernelPackage,ppp))


define KernelPackage/ppp-synctty
  SUBMENU:=$(NSMENU)
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
  SUBMENU:=$(NSMENU)
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
  SUBMENU:=$(NSMENU)
  TITLE:=PPPoA support
  DEPENDS:=kmod-ppp +kmod-atm
  KCONFIG:=CONFIG_PPPOATM
  FILES:=$(LINUX_DIR)/net/atm/pppoatm.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,pppoatm)
endef

define KernelPackage/pppoa/description
 Kernel modules for PPPoA (PPP over ATM) support
endef

$(eval $(call KernelPackage,pppoa))


define KernelPackage/ipoa
  SUBMENU:=$(NSMENU)
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
  SUBMENU:=$(NSMENU)
  TITLE:=Microsoft PPP compression/encryption
  DEPENDS:=kmod-ppp +kmod-crypto
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
  SUBMENU:=$(NSMENU)
  TITLE:=Traffic schedulers
  KCONFIG:=CONFIG_NET_SCHED
  FILES:=$(LINUX_DIR)/net/sched/*.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/sched/description
 Kernel schedulers for IP traffic
endef

$(eval $(call KernelPackage,sched))


define KernelPackage/ax25
  SUBMENU:=$(NSMENU)
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
  SUBMENU:=$(NSMENU)
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


NDMENU:=Network Devices

define KernelPackage/natsemi
  SUBMENU:=$(NDMENU)
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
  SUBMENU:=$(NDMENU)
  TITLE:=RDC Fast-Ethernet support
  DEPENDS:=@TARGET_rdc
  KCONFIG:=CONFIG_R6040
  FILES:=$(LINUX_DIR)/drivers/net/r6040.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/r6040/description
 Kernel modules for RDC Fast-Ethernet adapters.
endef

$(eval $(call KernelPackage,r6040))


define KernelPackage/sis900
  SUBMENU:=$(NDMENU)
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


define KernelPackage/via-rhine
  SUBMENU:=$(NDMENU)
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
  SUBMENU:=$(NDMENU)
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
  SUBMENU:=$(NDMENU)
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
  SUBMENU:=$(NDMENU)
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
  SUBMENU:=$(NDMENU)
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
  SUBMENU:=$(NDMENU)
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
  SUBMENU:=$(NDMENU)
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
  SUBMENU:=$(NDMENU)
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


define KernelPackage/3c59x
  SUBMENU:=$(NDMENU)
  TITLE:=3Com 3c590/3c900 series (592/595/597) Vortex/Boomerang
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_3C59X
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
