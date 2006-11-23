# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

NSMENU:=Network Support

define KernelPackage/atm
  TITLE:=ATM support
  DEPENDS:=@ATM_SUPPORT
  DESCRIPTION:= \
    Kernel modules for ATM support
  FILES:= \
    $(MODULES_DIR)/kernel/net/atm/atm.$(LINUX_KMOD_SUFFIX) \
    $(MODULES_DIR)/kernel/net/atm/br2684.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=$(CONFIG_ATM)
  SUBMENU:=$(NSMENU)
  AUTOLOAD:=$(call AutoLoad,30,atm br2684)
endef
$(eval $(call KernelPackage,atm))

define KernelPackage/atmtcp
  TITLE:=ATM over TCP
  DESCRIPTION:= \
    Kernel module for ATM over TCP support
  DEPENDS:=@LINUX_2_6 kmod-atm
  FILES:=$(MODULES_DIR)/kernel/drivers/atm/atmtcp.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=$(CONFIG_ATM_TCP)
  SUBMENU:=$(NSMENU)
  AUTOLOAD:=$(call AutoLoad,40,atmtcp)
endef
$(eval $(call KernelPackage,atmtcp))

define KernelPackage/bonding
  TITLE:=Ethernet bonding driver
  DESCRIPTION:= \
    Kernel module for NIC bonding.
  DEPENDS:=@LINUX_2_6_X86
  FILES:=$(MODULES_DIR)/kernel/drivers/net/bonding/bonding.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=$(CONFIG_BONDING)
  SUBMENU:=$(NSMENU)
  AUTOLOAD:=$(call AutoLoad,40,bonding)
endef
$(eval $(call KernelPackage,bonding))

define KernelPackage/ipip
  TITLE:=IP in IP encapsulation support
  DESCRIPTION:=\
  	Kernel modules for IP in IP encapsulation
  KCONFIG:=$(CONFIG_NET_IPIP)
  SUBMENU:=$(NSMENU)
endef
$(eval $(call KernelPackage,ipip))

define KernelPackage/ipip/2.4
  FILES:=$(MODULES_DIR)/kernel/net/ipv4/ipip.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,ipip)
endef

define KernelPackage/ipip/2.6
  FILES:= \
	$(MODULES_DIR)/kernel/net/ipv4/ipip.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/net/ipv4/tunnel4.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,ipip tunnel4)
endef


define KernelPackage/ipsec
  TITLE:=IPsec related modules (IPv4 and IPv6)
  DESCRIPTION:=\
	Kernel modules for IPsec support in both IPv4 and IPv6.\\\
	Includes:\\\
	- af_key\\\
	- xfrm_user
  SUBMENU:=$(NSMENU)
  DEPENDS:=@LINUX_2_6
  KCONFIG:=$(CONFIG_NET_KEY)
  FILES:= \
	$(MODULES_DIR)/kernel/net/key/af_key.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/net/xfrm/xfrm_user.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,ipsec))

define KernelPackage/ipsec4
  TITLE:=IPsec related modules (IPv4)
  DESCRIPTION:=\
	Kernel modules for IPsec support in IPv4.\\\
	Includes:\\\
	- ah4\\\
	- esp4\\\
	- ipcomp\\\
	- xfrm4_tunnel
  SUBMENU:=$(NSMENU)
  KCONFIG:=$(CONFIG_INET_AH)
  DEPENDS:=kmod-ipsec
  FILES:= $(foreach mod,ah4 esp4 ipcomp xfrm4_tunnel, \
	$(MODULES_DIR)/kernel/net/ipv4/$(mod).$(LINUX_KMOD_SUFFIX) \
  )
endef
$(eval $(call KernelPackage,ipsec4))

define KernelPackage/ipsec6
  TITLE:=IPsec related modules (IPv6)
  DESCRIPTION:=\
	Kernel modules for IPsec support in IPv6.\\\
	Includes:\\\
	- ah6\\\
	- esp6\\\
	- ipcomp6\\\
	- xfrm6_tunnel
  SUBMENU:=$(NSMENU)
  KCONFIG:=$(CONFIG_INET6_AH)
  DEPENDS:=kmod-ipsec
  FILES:= $(foreach mod,ah6 esp6 ipcomp6 xfrm6_tunnel, \
	$(MODULES_DIR)/kernel/net/ipv6/{ah6,esp6,ipcomp6,xfrm6_tunnel}.$(LINUX_KMOD_SUFFIX) \
  )
endef
$(eval $(call KernelPackage,ipsec6))


define KernelPackage/ipv6
  TITLE:=IPv6 support
  DESCRIPTION:=\
	Kernel modules for IPv6 support
  KCONFIG:=$(CONFIG_IPV6)
  FILES:=$(MODULES_DIR)/kernel/net/ipv6/ipv6.$(LINUX_KMOD_SUFFIX)
  SUBMENU:=$(NSMENU)
endef
$(eval $(call KernelPackage,ipv6))


define KernelPackage/gre
  TITLE:=GRE support
  DESCRIPTION:=\
	Generic Routing Encapsulation support
  KCONFIG=$(CONFIG_NET_IPGRE)
  FILES=$(MODULES_DIR)/kernel/net/ipv4/ip_gre.$(LINUX_KMOD_SUFFIX)
  SUBMENU:=$(NSMENU)
endef
$(eval $(call KernelPackage,gre))


define KernelPackage/tun
  TITLE:=Universal TUN/TAP driver
  DESCRIPTION:=\
	Kernel support for the TUN/TAP tunneling device
  KCONFIG:=$(CONFIG_TUN)
  FILES:=$(MODULES_DIR)/kernel/drivers/net/tun.$(LINUX_KMOD_SUFFIX)
  SUBMENU:=$(NSMENU)
  AUTOLOAD:=$(call AutoLoad,30,tun)
endef
$(eval $(call KernelPackage,tun))


define KernelPackage/ppp
  TITLE:=PPP modules
  DESCRIPTION:=\
	Kernel modules for PPP support
  KCONFIG:=$(CONFIG_PPP)
  SUBMENU:=$(NSMENU)
endef

define KernelPackage/ppp/2.6
  FILES:= \
  	$(MODULES_DIR)/kernel/drivers/net/ppp_async.ko \
	$(MODULES_DIR)/kernel/drivers/net/ppp_generic.ko \
	$(MODULES_DIR)/kernel/drivers/net/slhc.ko \
	$(MODULES_DIR)/kernel/lib/crc-ccitt.ko
  AUTOLOAD:=$(call AutoLoad,30,crc-ccitt slhc ppp_generic ppp_async)
endef

define KernelPackage/ppp/2.4
  FILES:= \
  	$(MODULES_DIR)/kernel/drivers/net/ppp_async.o \
	$(MODULES_DIR)/kernel/drivers/net/ppp_generic.o \
	$(MODULES_DIR)/kernel/drivers/net/slhc.o
  AUTOLOAD:=$(call AutoLoad,30,slhc ppp_generic ppp_async)
endef
$(eval $(call KernelPackage,ppp))


define KernelPackage/pppoe
  TITLE:=PPPoE support
  DESCRIPTION:=\
	Kernel modules for PPPoE (PPP over Ethernet) support
  DEPENDS:=kmod-ppp
  KCONFIG:=$(CONFIG_PPPOE)
  FILES:= \
  	$(MODULES_DIR)/kernel/drivers/net/pppoe.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/net/pppox.$(LINUX_KMOD_SUFFIX)
  SUBMENU:=$(NSMENU)
endef
$(eval $(call KernelPackage,pppoe))


define KernelPackage/pppoa
  TITLE:=PPPoA support
  DESCRIPTION:=\
	Kernel modules for PPPoA (PPP over ATM) support
  DEPENDS:=kmod-ppp
  KCONFIG:=$(CONFIG_PPPOATM)
  FILES:=$(MODULES_DIR)/kernel/net/atm/pppoatm.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,pppoatm)
  SUBMENU:=$(NSMENU)
endef
$(eval $(call KernelPackage,pppoa))


define KernelPackage/mppe
  TITLE:=Microsoft PPP compression/encryption
  DESCRIPTION:=Kernel modules for Microsoft PPP compression/encryption
  DEPENDS:=kmod-ppp
  KCONFIG:=$(CONFIG_PPP_MPPE)
  SUBMENU:=$(NSMENU)
endef

define KernelPackage/mppe/2.4
  FILES:=$(MODULES_DIR)/kernel/drivers/net/ppp_mppe_mppc.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/mppe/2.6
  FILES:=$(MODULES_DIR)/kernel/drivers/net/ppp_mppe.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,mppe))


define KernelPackage/sched
  TITLE:=Traffic schedulers
  DESCRIPTION:=\
	Kernel schedulers for IP traffic
  FILES:=$(MODULES_DIR)/kernel/net/sched/*.$(LINUX_KMOD_SUFFIX)
  SUBMENU:=$(NSMENU)
endef
$(eval $(call KernelPackage,sched))



define KernelPackage/ax25
  TITLE:=AX25 support
  DESCRIPTION:=Kernel modules for AX25 support
  SUBMENU:=$(NSMENU)
  KCONFIG:=$(CONFIG_AX25)
  FILES:= \
	$(MODULES_DIR)/kernel/net/ax25/ax25.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/net/hamradio/mkiss.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,80,ax25 mkiss)
endef
$(eval $(call KernelPackage,ax25))




NDMENU:=Network Devices

define KernelPackage/natsemi
  TITLE:=National Semiconductor DP8381x series 
  DESCRIPTION:=\
	Kernel modules for National Semiconductor DP8381x series PCI Ethernet \\\
	adapters.
  FILES:=$(MODULES_DIR)/kernel/drivers/net/natsemi.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=$(CONFIG_NATSEMI)
  DEPENDS:=@LINUX_2_6_X86
  DEFAULT:=y
  SUBMENU:=$(NDMENU)
  AUTOLOAD:=$(call AutoLoad,20,natsemi)
endef
$(eval $(call KernelPackage,natsemi))

define KernelPackage/sis900
  TITLE:=SiS 900 Ethernet support
  DESCRIPTION:=\
	Kernel modules for Sis 900 Ethernet adapters.
  FILES:=$(MODULES_DIR)/kernel/drivers/net/sis900.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=$(CONFIG_SIS900)
  DEPENDS:=@LINUX_2_6_X86
  SUBMENU:=$(NDMENU)
  AUTOLOAD:=$(call AutoLoad,50,sis900)
endef
$(eval $(call KernelPackage,sis900))

define KernelPackage/via-rhine
  TITLE:=Via Rhine ethernet support
  DESCRIPTION:=\
	Kernel modules for Via Rhine Ethernet chipsets.
  FILES:=$(MODULES_DIR)/kernel/drivers/net/via-rhine.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=$(CONFIG_VIA_RHINE)
  DEPENDS:=@LINUX_2_6_X86
  SUBMENU:=$(NDMENU)
  AUTOLOAD:=$(call AutoLoad,50,via-rhine)
endef
$(eval $(call KernelPackage,via-rhine))

define KernelPackage/8139too
  TITLE:=RealTek RTL-8139 PCI Fast Ethernet Adapter kernel support
  DESCRIPTION:=\
	Kernel modules for RealTek RTL-8139 PCI Fast Ethernet adapters.
  FILES:=$(MODULES_DIR)/kernel/drivers/net/8139too.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=$(CONFIG_8139TOO)
  DEPENDS:=@LINUX_2_6_X86
  SUBMENU:=$(NDMENU)
  AUTOLOAD:=$(call AutoLoad,50,8139too)
endef
$(eval $(call KernelPackage,8139too))

define KernelPackage/r8169
  TITLE:=RealTek RTL-8169 PCI Gigabit Ethernet Adapter kernel support
  DESCRIPTION:=\
	Kernel modules for RealTek RTL-8169 PCI Gigabit Ethernet adapters.
  FILES:=$(MODULES_DIR)/kernel/drivers/net/r8169.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=$(CONFIG_R8169)
  DEPENDS:=@LINUX_2_6_X86 
  SUBMENU:=$(NDMENU)
  AUTOLOAD:=$(call AutoLoad,50,r8169)
endef
$(eval $(call KernelPackage,r8169))

define KernelPackage/ne2k-pci
  TITLE:=ne2k-pci Ethernet Adapter kernel support
  DESCRIPTION:=\
	ne2k-pci Ethernet Adapter kernel support.
  FILES:=$(MODULES_DIR)/kernel/drivers/net/ne2k-pci.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/net/8390.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=$(CONFIG_NE2K-PCI)
  DEPENDS:=@LINUX_2_6_X86
  SUBMENU:=$(NDMENU)
  AUTOLOAD:=$(call AutoLoad,50,8390 ne2k-pci)
endef
$(eval $(call KernelPackage,ne2k-pci))

define KernelPackage/ixp4xx-npe
  TITLE:=Intel(R) IXP4xx ethernet support
  DESCRIPTION:=\
	Kernel modules for Intel(R) IXP4xx onboard ethernet.
  FILES:=$(MODULES_DIR)/kernel/drivers/net/ixp4xx/ixp4xx_npe.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/net/ixp4xx/ixp4xx_qmgr.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/net/ixp4xx/ixp4xx_mac.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=$(CONFIG_IXP4XX_MAC)
  DEPENDS:=@LINUX_2_6_IXP4XX
  SUBMENU:=$(NDMENU)
  AUTOLOAD:=$(call AutoLoad,20,ixp4xx_npe ixp4xx_qmgr ixp4xx_mac)
endef
$(eval $(call KernelPackage,ixp4xx-npe))

define KernelPackage/e100
  TITLE:=Intel(R) PRO/100+ cards kernel support
  DESCRIPTION:=\
	Kernel modules for Intel(R) PRO/100+ Ethernet adapters.
  FILES:=$(MODULES_DIR)/kernel/drivers/net/e100.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=$(CONFIG_E100)
  DEPENDS:=@LINUX_2_6_X86
  SUBMENU:=$(NDMENU)
  AUTOLOAD:=$(call AutoLoad,50,e100)
endef
$(eval $(call KernelPackage,e100))

define KernelPackage/e1000
  TITLE:=Intel(R) PRO/1000 cards kernel support
  DESCRIPTION:=\
	Kernel modules for Intel(R) PRO/1000 Ethernet adapters.
  FILES:=$(MODULES_DIR)/kernel/drivers/net/e1000/e1000.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=$(CONFIG_E1000)
  DEPENDS:=@LINUX_2_6_X86
  SUBMENU:=$(NDMENU)
endef
$(eval $(call KernelPackage,e1000))

define KernelPackage/3c59x
  TITLE:=3Com 3c590/3c900 series (592/595/597) Vortex/Boomerang
  DESCRIPTION:=\
	This option enables driver support for a large number of 10mbps and \\\
	10/100mbps EISA, PCI and PCMCIA 3Com Ethernet adapters: \\\
	- "Vortex"    (Fast EtherLink 3c590/3c592/3c595/3c597) EISA and PCI \\\
	- "Boomerang" (EtherLink XL 3c900 or 3c905)            PCI \\\
	- "Cyclone"   (3c540/3c900/3c905/3c980/3c575/3c656)    PCI and Cardbus \\\
	- "Tornado"   (3c905)                                  PCI  \\\
	- "Hurricane" (3c555/3cSOHO)                           PCI 
  FILES:=$(MODULES_DIR)/kernel/drivers/net/3c59x.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=$(CONFIG_3C59X)
  DEPENDS:=@LINUX_2_6_X86
  SUBMENU:=$(NDMENU)
  AUTOLOAD:=$(call AutoLoad,50,3c59x)
endef
$(eval $(call KernelPackage,3c59x))
