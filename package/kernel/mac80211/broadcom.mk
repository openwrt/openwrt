PKG_DRIVERS += \
	b43 brcmfmac brcmutil

PKG_CONFIG_DEPENDS += \
	CONFIG_PACKAGE_B43_DEBUG \
	CONFIG_PACKAGE_B43_PIO \
	CONFIG_PACKAGE_B43_PHY_G \
	CONFIG_PACKAGE_B43_PHY_N \
	CONFIG_PACKAGE_B43_PHY_LP \
	CONFIG_PACKAGE_B43_PHY_HT \
	CONFIG_PACKAGE_B43_BUSES_BCMA_AND_SSB \
	CONFIG_PACKAGE_B43_BUSES_BCMA \
	CONFIG_PACKAGE_B43_BUSES_SSB \
	CONFIG_PACKAGE_BRCM80211_DEBUG

config-$(call config_package,b43) += B43
config-$(CONFIG_PACKAGE_B43_BUSES_BCMA_AND_SSB) += B43_BUSES_BCMA_AND_SSB
config-$(CONFIG_PACKAGE_B43_BUSES_BCMA) += B43_BUSES_BCMA
config-$(CONFIG_PACKAGE_B43_BUSES_SSB) += B43_BUSES_SSB
config-$(CONFIG_PACKAGE_B43_PHY_G) += B43_PHY_G
config-$(CONFIG_PACKAGE_B43_PHY_N) += B43_PHY_N
config-$(CONFIG_PACKAGE_B43_PHY_LP) += B43_PHY_LP
config-$(CONFIG_PACKAGE_B43_PHY_HT) += B43_PHY_HT
config-$(CONFIG_PACKAGE_B43_PIO) += B43_PIO
config-$(CONFIG_PACKAGE_B43_DEBUG) += B43_DEBUG

config-$(call config_package,brcmutil) += BRCMUTIL
config-$(call config_package,brcmfmac) += BRCMFMAC
config-$(CONFIG_BRCMFMAC_SDIO) += BRCMFMAC_SDIO
config-$(CONFIG_BRCMFMAC_USB) += BRCMFMAC_USB
config-$(CONFIG_BRCMFMAC_PCIE) += BRCMFMAC_PCIE
config-$(CONFIG_PACKAGE_BRCM80211_DEBUG) += BRCMDBG

config-$(CONFIG_LEDS_TRIGGERS) += B43_LEDS B43LEGACY_LEDS

#Broadcom firmware
PKG_B43_FWV4_NAME:=broadcom-wl
PKG_B43_FWV4_VERSION:=5.100.138
PKG_B43_FWV4_OBJECT:=$(PKG_B43_FWV4_NAME)-$(PKG_B43_FWV4_VERSION)/linux/wl_apsta.o
PKG_B43_FWV4_SOURCE:=$(PKG_B43_FWV4_NAME)-$(PKG_B43_FWV4_VERSION).tar.bz2

ifneq ($(CONFIG_PACKAGE_b43-firmware),)
define Download/b43
  FILE:=$(PKG_B43_FWV4_SOURCE)
  URL:=@OPENWRT
  HASH:=f1e7067aac5b62b67b8b6e4c517990277804339ac16065eb13c731ff909ae46f
endef
$(eval $(call Download,b43))
endif

PKG_B43_OPENFW_VERSION:=5.2
PKG_B43_OPENFW_OBJECT:=openfwwf-$(PKG_B43_OPENFW_VERSION)
PKG_B43_OPENFW_SOURCE:=openfwwf-$(PKG_B43_OPENFW_VERSION).tar.gz

ifneq ($(CONFIG_PACKAGE_b43-firmware-open),)
define Download/b43-open
  FILE:=$(PKG_B43_OPENFW_SOURCE)
  URL:=http://netweb.ing.unibs.it/openfwwf/firmware \
	https://src.fedoraproject.org/lookaside/pkgs/b43-openfwwf/openfwwf-5.2.tar.gz/e045a135453274e439ae183f8498b0fa
  HASH:=9de03320083201080b2e94b81637ac07a159cf4e6f3481383e1a217e627bc0dc
endef
$(eval $(call Download,b43-open))
endif

PKG_B43_FW784_NAME:=broadcom-wl
PKG_B43_FW784_VERSION:=6.30.163.46
PKG_B43_FW784_OBJECT:=$(PKG_B43_FW784_NAME)-$(PKG_B43_FW784_VERSION).wl_apsta.o
PKG_B43_FW784_SOURCE:=$(PKG_B43_FW784_NAME)-$(PKG_B43_FW784_VERSION).tar.bz2

# 784.2 split by PHY type: each package has the files b43-fwsquash.py keeps
# for its types; ucode13.fw (revision 13 G-PHY and LP-PHY cores) is in the
# LP-PHY package only
B43_FW784_PHYS:=ac ht n lp g lcn
B43_FW784_TYPES_ac:=AC
B43_FW784_TYPES_ht:=HT
B43_FW784_TYPES_n:=N
B43_FW784_TYPES_lp:=LP
B43_FW784_TYPES_g:=G,A,AG
B43_FW784_TYPES_lcn:=LCN,LCN40
B43_FW784_DROP_g:=ucode13.fw
B43_FW784_TITLE_ac:=AC-PHY
B43_FW784_TITLE_ht:=HT-PHY
B43_FW784_TITLE_n:=N-PHY
B43_FW784_TITLE_lp:=LP-PHY
B43_FW784_TITLE_g:=A-PHY and G-PHY
B43_FW784_TITLE_lcn:=LCN-PHY and LCN40-PHY
B43_FW784_COREREVS:=2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42

ifneq ($(foreach phy,$(B43_FW784_PHYS),$(CONFIG_PACKAGE_b43-firmware-784-$(phy))),)
define Download/b43-784
  FILE:=$(PKG_B43_FW784_SOURCE)
  URL:=@SF/b43-firmware
  HASH:=a07c3b6b277833c7dbe61daa511f908cd66c5e2763eb7a0859abc36cd9335c2d
endef
$(eval $(call Download,b43-784))
endif

define KernelPackage/b43
  $(call KernelPackage/mac80211/Default)
  TITLE:=Broadcom 43xx wireless support
  URL:=https://wireless.wiki.kernel.org/en/users/drivers/b43
  KCONFIG:= \
  	CONFIG_HW_RANDOM=y
  # Depend on PCI_SUPPORT to make sure we can select kmod-bcma or kmod-ssb
  DEPENDS += \
	@PCI_SUPPORT +kmod-mac80211 +kmod-lib-cordic +b43-firmware \
	$(if $(CONFIG_PACKAGE_B43_USE_SSB),+kmod-ssb) \
	$(if $(CONFIG_PACKAGE_B43_USE_BCMA),+kmod-bcma)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/broadcom/b43/b43.ko
  AUTOLOAD:=$(call AutoProbe,b43)
  MENU:=1
endef

define Package/b43-firmware
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=Broadcom 43xx firmware 666.2 (b43)
  URL:=https://wireless.wiki.kernel.org/en/users/drivers/b43/firmware
  MENU:=1
endef

define Package/b43-firmware/description
 Firmware 666.2 from Broadcom driver 5.100.138, cut with b43-fwcutter, for
 kmod-b43.
endef

define Package/b43-firmware-open
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=Open FirmWare for WiFi networks (b43)
  URL:=http://netweb.ing.unibs.it/openfwwf/
endef

define Package/b43-firmware-open/description
 OpenFWWF 5.2, open source firmware for the G-PHY cores of revision 5 to 10,
 installed in /lib/firmware/b43-open, where b43 loads it when the
 proprietary firmware is missing. No QoS and no hardware crypto.
endef

define Package/b43-firmware/config
  if PACKAGE_b43-firmware

	config B43_FW_SQUASH
		bool "Remove unnecessary firmware files"
		default y
		help
		  This options allows you to remove unnecessary b43 firmware files
		  from the final rootfs image. This can reduce the rootfs size by
		  up to 200k.

		  If unsure, say Y.

	config B43_FW_SQUASH_COREREVS
		string "Core revisions to include"
		depends on B43_FW_SQUASH
		default "5,6,7,8,9,10,11,13,15" if TARGET_bcm47xx_legacy
		default "16,28,29,30" if TARGET_bcm47xx_mips74k
		default "5,6,7,8,9,10,11,13,15,16,28,29,30"
		help
		  This is a comma separated list of core revision numbers.

		  Example (keep files for rev5 only):
		    5

		  Example (keep files for rev5 and rev11):
		    5,11

	config B43_FW_SQUASH_PHYTYPES
		string "PHY types to include"
		depends on B43_FW_SQUASH
		default "G,N,LP" if TARGET_bcm47xx_legacy
		default "N,HT" if TARGET_bcm47xx_mips74k
		default "G,N,LP,HT"
		help
		  This is a comma separated list of PHY types:
		    A  => A-PHY
		    AG => Dual A-PHY G-PHY
		    G  => G-PHY
		    LP => LP-PHY
		    N  => N-PHY
		    HT  => HT-PHY
		    LCN  => LCN-PHY
		    LCN40  => LCN40-PHY
		    AC  => AC-PHY

		  Example (keep files for G-PHY only):
		    G

		  Example (keep files for G-PHY and N-PHY):
		    G,N

  endif
endef

define B43Firmware784
  define Package/b43-firmware-784-$(1)
    SECTION:=firmware
    CATEGORY:=Firmware
    TITLE:=Broadcom 43xx firmware 784.2, $(B43_FW784_TITLE_$(1)) (b43-next)
    URL:=https://wireless.wiki.kernel.org/en/users/drivers/b43/firmware
  endef

  define Package/b43-firmware-784-$(1)/description
   Firmware 784.2 from Broadcom driver 6.30.163.46, cut with b43-fwcutter:
   the files for the $(B43_FW784_TITLE_$(1)) cores, installed in
   /lib/firmware/b43-784 for kmod-b43-next.
  endef

  define Package/b43-firmware-784-$(1)/install
	rm -rf $(PKG_BUILD_DIR)/b43-firmware-784-$(1)
	mkdir -p $(PKG_BUILD_DIR)/b43-firmware-784-$(1)
	tar xjf "$(DL_DIR)/$(PKG_B43_FW784_SOURCE)" -C "$(PKG_BUILD_DIR)/b43-firmware-784-$(1)"
	b43-fwcutter -w $(PKG_BUILD_DIR)/b43-firmware-784-$(1) $(PKG_BUILD_DIR)/b43-firmware-784-$(1)/$(PKG_B43_FW784_OBJECT)
	b43-fwsquash.py "$(B43_FW784_TYPES_$(1))" "$(B43_FW784_COREREVS)" "$(PKG_BUILD_DIR)/b43-firmware-784-$(1)/b43"
	rm -f $(addprefix $(PKG_BUILD_DIR)/b43-firmware-784-$(1)/b43/,$(B43_FW784_DROP_$(1)))
	$$(INSTALL_DIR) $$(1)/lib/firmware/b43-784
	$$(INSTALL_DATA) $(PKG_BUILD_DIR)/b43-firmware-784-$(1)/b43/*.fw $$(1)/lib/firmware/b43-784/
  endef
endef
$(foreach phy,$(B43_FW784_PHYS),$(eval $(call B43Firmware784,$(phy))))

define KernelPackage/b43/config

config PACKAGE_B43_USE_SSB
	select PACKAGE_kmod-ssb
	tristate
	depends on !TARGET_bcm47xx
	default PACKAGE_kmod-b43 if PACKAGE_B43_BUSES_BCMA_AND_SSB
	default PACKAGE_kmod-b43 if PACKAGE_B43_BUSES_SSB

config PACKAGE_B43_USE_BCMA
	select PACKAGE_kmod-bcma
	tristate
	depends on !TARGET_bcm47xx && !TARGET_bcm53xx
	default PACKAGE_kmod-b43 if PACKAGE_B43_BUSES_BCMA_AND_SSB
	default PACKAGE_kmod-b43 if PACKAGE_B43_BUSES_BCMA

  if PACKAGE_kmod-b43

	choice
		prompt "Supported buses"
		default PACKAGE_B43_BUSES_BCMA_AND_SSB
		help
		  This allows choosing buses that b43 should support.

	config PACKAGE_B43_BUSES_BCMA_AND_SSB
		depends on !TARGET_bcm47xx_legacy && !TARGET_bcm47xx_mips74k && !TARGET_bcm53xx && !TARGET_bmips
		bool "BCMA and SSB"

	config PACKAGE_B43_BUSES_BCMA
		depends on !TARGET_bcm47xx_legacy && !TARGET_bmips_bcm6358 && !TARGET_bmips_bcm6368
		bool "BCMA only"

	config PACKAGE_B43_BUSES_SSB
		depends on !TARGET_bcm47xx_mips74k && !TARGET_bcm53xx
		bool "SSB only"

	endchoice

	config PACKAGE_B43_DEBUG
		bool "Enable debug output and debugfs for b43"
		default n
		help
		  Enable additional debug output and runtime sanity checks for b43
		  and enables the debugfs interface.

		  If unsure, say N.

	config PACKAGE_B43_PIO
		bool "Enable support for PIO transfer mode"
		default n
		help
		  Enable support for using PIO instead of DMA. Unless you have DMA
		  transfer problems you don't need this.

		  If unsure, say N.

	config PACKAGE_B43_PHY_G
		bool "Enable support for G-PHYs"
		default n if TARGET_bcm47xx_mips74k
		default y
		help
		  Enable support for G-PHY. This includes support for the following devices:
		  PCI: BCM4306, BCM4311, BCM4318
		  SoC: BCM5352E, BCM4712

		  If unsure, say Y.

	config PACKAGE_B43_PHY_N
		bool "Enable support for N-PHYs"
		default y
		help
		  Enable support for N-PHY. This includes support for the following devices:
		  PCI: BCM4321, BCM4322, BCM43222, BCM43224, BCM43225
		  SoC: BCM4716, BCM4717, BCM4718

		  Currently only 11g speed is available.

		  If unsure, say Y.

	config PACKAGE_B43_PHY_LP
		bool "Enable support for LP-PHYs"
		default n if TARGET_bcm47xx_mips74k
		default y
		help
		  Enable support for LP-PHY. This includes support for the following devices:
		  PCI: BCM4312
		  SoC: BCM5354

		  If unsure, say Y.

	config PACKAGE_B43_PHY_HT
		bool "Enable support for HT-PHYs"
		default n if TARGET_bcm47xx_legacy
		default y
		help
		  Enable support for HT-PHY. This includes support for the following devices:
		  PCI: BCM4331

		  Currently only 11g speed is available.

		  If unsure, say Y.

	config PACKAGE_B43_PHY_LCN
		bool "Enable support for LCN-PHYs"
		depends on BROKEN
		default n
		help
		  Currently broken.

		  If unsure, say N.

  endif
endef

define KernelPackage/b43/description
Kernel module for Broadcom 43xx wireless support (mac80211 stack) new
endef

define KernelPackage/brcmutil
  $(call KernelPackage/mac80211/Default)
  TITLE:=Broadcom IEEE802.11n common driver parts
  URL:=https://wireless.wiki.kernel.org/en/users/drivers/brcm80211
  DEPENDS+=@PCI_SUPPORT||USB_SUPPORT
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/broadcom/brcm80211/brcmutil/brcmutil.ko
  AUTOLOAD:=$(call AutoProbe,brcmutil)
  MENU:=1
endef

define KernelPackage/brcmutil/description
 This module contains some common parts needed by the Broadcom brcmfmac wireless driver.
endef

define KernelPackage/brcmutil/config
  if PACKAGE_kmod-brcmutil

	config PACKAGE_BRCM80211_DEBUG
		bool "Broadcom wireless driver debugging"
		help
		  Say Y, if you want to debug the brcmfmac wireless driver.

  endif
endef

define KernelPackage/brcmfmac
  $(call KernelPackage/mac80211/Default)
  TITLE:=Broadcom IEEE802.11n USB FullMAC WLAN driver
  URL:=https://wireless.wiki.kernel.org/en/users/drivers/brcm80211
  DEPENDS+= @USB_SUPPORT +kmod-cfg80211 +@DRIVER_11AC_SUPPORT \
  	+kmod-brcmutil +BRCMFMAC_SDIO:kmod-mmc @!TARGET_uml \
	+BRCMFMAC_USB:kmod-usb-core +BRCMFMAC_USB:brcmfmac-firmware-usb
  FILES:= \
	$(PKG_BUILD_DIR)/drivers/net/wireless/broadcom/brcm80211/brcmfmac/brcmfmac.ko \
	$(foreach type,bca cyw wcc, \
		$(PKG_BUILD_DIR)/drivers/net/wireless/broadcom/brcm80211/brcmfmac/$(type)/brcmfmac-$(type).ko)
  AUTOLOAD:=$(call AutoProbe,brcmfmac)
endef

define KernelPackage/brcmfmac/description
 Kernel module for Broadcom IEEE802.11n USB Wireless cards
endef

define KernelPackage/brcmfmac/config
  if PACKAGE_kmod-brcmfmac

	config BRCMFMAC_SDIO
		bool "Enable SDIO bus interface support"
		default y if TARGET_bcm27xx
		default y if TARGET_imx_cortexa7
		default y if TARGET_starfive
		default y if TARGET_rockchip
		default y if TARGET_sunxi
		default y if TARGET_stm32
		default y if TARGET_x86
		default n
		help
		  Enable support for cards attached to an SDIO bus.
		  Select this option only if you are sure that your
		  board has a Broadcom wireless chip atacched to
		  that bus.

	config BRCMFMAC_USB
		bool "Enable USB bus interface support"
		depends on USB_SUPPORT
		default y
		help
		  Supported USB connected chipsets:
		  BCM43235, BCM43236, BCM43238 (all in revision 3 only)
		  BCM43143, BCM43242, BCM43566, BCM43569

	config BRCMFMAC_PCIE
		bool "Enable PCIE bus interface support"
		depends on PCI_SUPPORT
		default y
		help
		  Supported PCIe connected chipsets:
		  BCM4354, BCM4356, BCM43567, BCM43570, BCM43602

  endif
endef


define Package/b43-firmware/install
	rm -rf $(1)/lib/firmware/
	tar xjf "$(DL_DIR)/$(PKG_B43_FWV4_SOURCE)" -C "$(PKG_BUILD_DIR)"
	$(INSTALL_DIR) $(1)/lib/firmware/
	b43-fwcutter -w $(1)/lib/firmware/ $(PKG_BUILD_DIR)/$(PKG_B43_FWV4_OBJECT)
ifneq ($(CONFIG_B43_FW_SQUASH),)
	b43-fwsquash.py "$(CONFIG_B43_FW_SQUASH_PHYTYPES)" "$(CONFIG_B43_FW_SQUASH_COREREVS)" "$(1)/lib/firmware/b43"
endif
endef

define Package/b43-firmware-open/install
	tar xzf "$(DL_DIR)/$(PKG_B43_OPENFW_SOURCE)" -C "$(PKG_BUILD_DIR)"
	$(MAKE) -C "$(PKG_BUILD_DIR)/$(PKG_B43_OPENFW_OBJECT)/"
	$(INSTALL_DIR) $(1)/lib/firmware/b43-open/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_B43_OPENFW_OBJECT)/ucode5.fw $(1)/lib/firmware/b43-open/ucode5.fw
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_B43_OPENFW_OBJECT)/b0g0bsinitvals5.fw $(1)/lib/firmware/b43-open/b0g0bsinitvals5.fw
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_B43_OPENFW_OBJECT)/b0g0initvals5.fw $(1)/lib/firmware/b43-open/b0g0initvals5.fw
endef
