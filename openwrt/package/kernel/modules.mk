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

define KernelPackage/ieee80211softmac
TITLE:=802.11 Networking stack
DESCRIPTION:=802.11 Networking stack\\\
Includes: \\\
	* ieee80211_crypt \\\
	* ieee80211 \\\
	* ieee80211_crypt_wep \\\
	* ieee80211_crypt_tkip \\\
	* ieee80211_crytp_ccmp \\\
	* ieee80211softmac
KCONFIG:=$(CONFIG_IEEE80211_SOFTMAC)
FILES:=$(MODULES_DIR)/kernel/net/ieee80211/*.$(LINUX_KMOD_SUFFIX) $(MODULES_DIR)/kernel/net/ieee80211/softmac/*.$(LINUX_KMOD_SUFFIX)
AUTOLOAD:=$(call AutoLoad,10,ieee80211_crypt \
	ieee80211 \
	ieee80211_crypt_wep \
	ieee80211_crypt_tkip \
	ieee80211_crypt_ccmp \
	ieee80211softmac)
endef
$(eval $(call KernelPackage,ieee80211softmac))

WIMENU:=Wireless drivers

define KernelPackage/bcm43xx
TITLE:=Broadcom BCM43xx driver
DESCRIPTION:=Open source BCM43xx driver\\\
Includes: \\\
	* bcm43xx
KCONFIG:=$(CONFIG_BCM43XX)
SUBMENU:=$(WIMENU)
FILES:=$(MODULES_DIR)/kernel/drivers/net/wireless/bcm43xx/bcm43xx.$(LINUX_KMOD_SUFFIX)
AUTOLOAD:=$(call Autoload,50,bcm43xx)
endef
$(eval $(call KernelPackage,bcm43xx))

define KernelPackage/ipw2100
TITLE:=Intel IPw2100 driver
DESCRIPTION:=Intel IPW2100 driver\\\
Includes: \\\
        * ipw2100
KCONFIG:=$(CONFIG_IPW2100)
SUBMENU:=$(WIMENU)
FILES:=$(MODULES_DIR)/kernel/drivers/net/wireless/ipw2100.$(LINUX_KMOD_SUFFIX)
AUTOLOAD:=$(call Autoload,50,ipw2100)
endef
$(eval $(call KernelPackage,ipw2100))

define KernelPackage/ipw2200
TITLE:=Intel IPw2200 driver
DESCRIPTION:=Intel IPW2200 driver\\\
Includes: \\\
        * ipw2200
KCONFIG:=$(CONFIG_IPW2200)
SUBMENU:=$(WIMENU)
FILES:=$(MODULES_DIR)/kernel/drivers/net/wireless/ipw2200.$(LINUX_KMOD_SUFFIX)
AUTOLOAD:=$(call Autoload,50,ipw2200)
endef
$(eval $(call KernelPackage,ipw2200))

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
KCONFIG:=$(CONFIG_IP_NF_TARGET_IMQ)
SUBMENU:=$(NFMENU)
FILES:=$(MODULES_DIR)/kernel/net/ipv4/netfilter/*IMQ*.$(LINUX_KMOD_SUFFIX) $(MODULES_DIR)/kernel/drivers/net/imq.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,ipt-imq))


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
KCONFIG:=$(CONFIG_IP6_NF_IPTABLES)
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

define KernelPackage/ipv6
TITLE:=IPv6 support
DESCRIPTION:=Kernel modules for IPv6 support
KCONFIG:=$(CONFIG_IPV6)
FILES:=$(MODULES_DIR)/kernel/net/ipv6/ipv6.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,ipv6))

define KernelPackage/ppp
TITLE:=PPP modules
DESCRIPTION:=Kernel modules for PPP support
KCONFIG:=$(CONFIG_PPP)
endef

define KernelPackage/ppp/2.6
FILES:=$(MODULES_DIR)/kernel/drivers/net/ppp_async.ko $(MODULES_DIR)/kernel/drivers/net/ppp_generic.ko $(MODULES_DIR)/kernel/drivers/net/slhc.ko $(MODULES_DIR)/kernel/lib/crc-ccitt.ko
AUTOLOAD:=$(call AutoLoad,30,crc-ccitt)
endef

define KernelPackage/ppp/2.4
FILES:=$(MODULES_DIR)/kernel/drivers/net/ppp_async.o $(MODULES_DIR)/kernel/drivers/net/ppp_generic.o $(MODULES_DIR)/kernel/drivers/net/slhc.o
endef
$(eval $(call KernelPackage,ppp))

define KernelPackage/pppoe
TITLE:=PPPoE modules
DESCRIPTION:=Kernel modules for PPP over Ethernet support
DEPENDS:=kmod-ppp
KCONFIG:=$(CONFIG_PPPOE)
FILES:=$(MODULES_DIR)/kernel/drivers/net/pppoe.$(LINUX_KMOD_SUFFIX) $(MODULES_DIR)/kernel/drivers/net/pppox.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,pppoe))

define KernelPackage/pppoa
TITLE:=PPPoA modules
DESCRIPTION:=Kernel modules for PPP over ATM support
DEPENDS:=kmod-ppp
KCONFIG:=$(CONFIG_PPPOATM)
FILES:=$(MODULES_DIR)/kernel/net/atm/pppoatm.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,pppoa))


define KernelPackage/mppe
TITLE:=Microsoft PPP compression/encryption
DESCRIPTION:=Kernel modules for Microsoft PPP compression/encryption
DEPENDS:=kmod-ppp
KCONFIG:=$(CONFIG_PPP_MPPE)
endef

define KernelPackage/mppe/2.4
FILES:=$(MODULES_DIR)/kernel/drivers/net/ppp_mppe_mppc.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/mppe/2.6
FILES:=$(MODULES_DIR)/kernel/drivers/net/ppp_mppe.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,mppe))



NDMENU:=Network Devices


define KernelPackage/natsemi
TITLE:=National Semiconductor DP8381x series 
DESCRIPTION:=National Semiconductor DP8381x series PCI Ethernet kernel support
SUBMENU:=$(NDMENU)
FILES:=$(MODULES_DIR)/kernel/drivers/net/natsemi.$(LINUX_KMOD_SUFFIX)
KCONFIG:=$(CONFIG_NATSEMI)
DEPENDS:=@LINUX_2_6_X86||LINUX_2_4_X86
endef
$(eval $(call KernelPackage,natsemi))

define KernelPackage/sis900
TITLE:=SiS 900 Ethernet support
DESCRIPTION:=Sis 900 kernel ethernet support
SUBMENU:=$(NDMENU)
FILES:=$(MODULES_DIR)/kernel/drivers/net/sis900.$(LINUX_KMOD_SUFFIX)
KCONFIG:=$(CONFIG_SIS900)
DEPENDS:=@LINUX_2_6_X86||LINUX_2_4_X86
endef
$(eval $(call KernelPackage,sis900))


define KernelPackage/via-rhine
TITLE:=Via Rhine ethernet support
DESCRIPTION:=Kernel modules for Via Rhine ethernet chipsets
SUBMENU:=$(NDMENU)
FILES:=$(MODULES_DIR)/kernel/drivers/net/via-rhine.$(LINUX_KMOD_SUFFIX)
KCONFIG:=$(CONFIG_VIA_RHINE)
DEPENDS:=@LINUX_2_6_X86||LINUX_2_4_X86
endef
$(eval $(call KernelPackage,via-rhine))

define KernelPackage/8139too
TITLE:=RealTek RTL-8139 PCI Fast Ethernet Adapter kernel support
DESCRIPTION:=Kernel modules for RealTek RTL-8139 PCI Fast Ethernet Adapter kernel support
SUBMENU:=$(NDMENU)
FILES:=$(MODULES_DIR)/kernel/drivers/net/8139too.$(LINUX_KMOD_SUFFIX)
KCONFIG:=$(CONFIG_8139TOO)
DEPENDS:=@LINUX_2_6_X86||LINUX_2_4_X86
endef
$(eval $(call KernelPackage,8139too))

define KernelPackage/e100
TITLE:=Intel(R) PRO/100+ cards kernel support
DESCRIPTION:=Kernel modules for Intel(R) PRO/100+ cards kernel support
SUBMENU:=$(NDMENU)
FILES:=$(MODULES_DIR)/kernel/drivers/net/e100.$(LINUX_KMOD_SUFFIX)
KCONFIG:=$(CONFIG_E100)
DEPENDS:=@LINUX_2_6_X86||LINUX_2_4_X86
endef
$(eval $(call KernelPackage,e100))

define KernelPackage/3c59x
TITLE:=3Com 3c590/3c900 series (592/595/597) Vortex/Boomerang
DESCRIPTION:= \
	  This option enables driver support for a large number of 10mbps and \
	  10/100mbps EISA, PCI and PCMCIA 3Com network cards: \
	  \
	   * "Vortex"    (Fast EtherLink 3c590/3c592/3c595/3c597) EISA and PCI \
	   * "Boomerang" (EtherLink XL 3c900 or 3c905)            PCI	\
	   * "Cyclone"   (3c540/3c900/3c905/3c980/3c575/3c656)    PCI and Cardbus \
	   * "Tornado"   (3c905)                                  PCI \
	   * "Hurricane" (3c555/3cSOHO)                           PCI 
SUBMENU:=$(NDMENU)
FILES:=$(MODULES_DIR)/kernel/drivers/net/3c59x.$(LINUX_KMOD_SUFFIX)
KCONFIG:=$(CONFIG_3C59X)
DEPENDS:=@LINUX_2_6_X86||LINUX_2_4_X86
endef
$(eval $(call KernelPackage,3c59x))



