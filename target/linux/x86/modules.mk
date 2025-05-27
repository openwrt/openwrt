# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2017 Cezary Jackiewicz <cezary@eko.one.pll>

define KernelPackage/amd-xgbe
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=AMD Ethernet on SoC support
  DEPENDS:=@PCI_SUPPORT @TARGET_x86_64 +kmod-lib-crc32c +kmod-ptp +kmod-libphy +kmod-mdio-devres
  KCONFIG:=CONFIG_AMD_XGBE
  FILES:=$(LINUX_DIR)/drivers/net/ethernet/amd/xgbe/amd-xgbe.ko
  AUTOLOAD:=$(call AutoLoad,35,amd-xgbe)
endef

define KernelPackage/amd-xgbe/description
 Kernel modules for AMD 10GbE Ethernet device on an AMD SoC.
endef

$(eval $(call KernelPackage,amd-xgbe))


define KernelPackage/dwmac-intel
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Intel GMAC support
  DEPENDS:=@TARGET_x86_64 +kmod-stmmac-core
  KCONFIG:=CONFIG_DWMAC_INTEL
  FILES=$(LINUX_DIR)/drivers/net/ethernet/stmicro/stmmac/dwmac-intel.ko
  AUTOLOAD=$(call AutoLoad,45,dwmac-intel)
endef

$(eval $(call KernelPackage,dwmac-intel))


define KernelPackage/f71808e-wdt
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Fintek F718xx/F818xx Watchdog Timer
  DEPENDS:=@TARGET_x86
  KCONFIG:=\
	CONFIG_F71808E_WDT \
	CONFIG_WATCHDOG_CORE=y
  FILES:=$(LINUX_DIR)/drivers/watchdog/f71808e_wdt.ko
  AUTOLOAD:=$(call AutoProbe,f71808e-wdt,1)
endef

define KernelPackage/f71808e-wdt/description
  Kernel module for the watchdog timer found on many Fintek Super-IO chips.
endef

$(eval $(call KernelPackage,f71808e-wdt))


define KernelPackage/sound-cs5535audio
  TITLE:=CS5535/CS5536 Audio Controller
  DEPENDS:=@TARGET_x86_geode +kmod-ac97
  KCONFIG:=CONFIG_SND_CS5535AUDIO
  FILES:=$(LINUX_DIR)/sound/pci/cs5535audio/snd-cs5535audio.ko
  AUTOLOAD:=$(call AutoLoad,36,snd-cs5535audio)
  $(call AddDepends/sound)
endef

define KernelPackage/sound-cs5535audio/description
 Support for the integrated AC97 sound device on motherboards
 with AMD CS5535/CS5536 chipsets.
endef

$(eval $(call KernelPackage,sound-cs5535audio))

define KernelPackage/sp5100-tco
  SUBMENU:=$(OTHER_MENU)
  TITLE:=SP5100 Watchdog Support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_SP5100_TCO
  FILES:=$(LINUX_DIR)/drivers/watchdog/sp5100_tco.ko
  AUTOLOAD:=$(call AutoLoad,50,sp5100_tco,1)
endef

define KernelPackage/sp5100-tco/description
 Kernel module for the SP5100_TCO hardware watchdog.
endef

$(eval $(call KernelPackage,sp5100-tco))


define KernelPackage/ib700-wdt
  SUBMENU:=$(OTHER_MENU)
  TITLE:=IB700 SBC Watchdog Timer
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_IB700_WDT
  FILES:=$(LINUX_DIR)/drivers/watchdog/ib700wdt.ko
  AUTOLOAD:=$(call AutoLoad,50,ib700wdt,1)
endef

define KernelPackage/ib700-wdt/description
  Kernel module for the hardware watchdog on the IB700 Single
  Board Computer produced by TMC Technology (www.tmc-uk.com).
  Also used by QEMU/libvirt.
endef

$(eval $(call KernelPackage,ib700-wdt))

define KernelPackage/it87-wdt
  SUBMENU:=$(OTHER_MENU)
  TITLE:=ITE IT87 Watchdog Timer
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_IT87_WDT
  FILES:=$(LINUX_DIR)/drivers/watchdog/it87_wdt.ko
  AUTOLOAD:=$(call AutoLoad,50,it87-wdt,1)
  MODPARAMS.it87-wdt:= \
	nogameport=1 \
	nocir=1
endef

define KernelPackage/it87-wdt/description
  Kernel module for ITE IT87 Watchdog Timer
endef

$(eval $(call KernelPackage,it87-wdt))


define KernelPackage/itco-wdt
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Intel iTCO Watchdog Timer
  DEPENDS:=@TARGET_x86
  KCONFIG:=\
	CONFIG_ITCO_WDT \
	CONFIG_ITCO_VENDOR_SUPPORT=y \
	CONFIG_WATCHDOG_CORE=y
  FILES:=$(LINUX_DIR)/drivers/watchdog/iTCO_wdt.ko \
         $(LINUX_DIR)/drivers/watchdog/iTCO_vendor_support.ko
  AUTOLOAD:=$(call AutoLoad,50,iTCO_vendor_support iTCO_wdt,1)
endef

define KernelPackage/itco-wdt/description
  Kernel module for Intel iTCO Watchdog Timer
endef

$(eval $(call KernelPackage,itco-wdt))


define KernelPackage/pcengines-apuv2
  SUBMENU:=$(OTHER_MENU)
  TITLE:=PC Engines APUv2/3 front button and LEDs driver
  DEPENDS:=@TARGET_x86 +kmod-gpio-amd-fch +kmod-leds-gpio
  KCONFIG:=CONFIG_PCENGINES_APU2
  FILES:=$(LINUX_DIR)/drivers/platform/x86/pcengines-apuv2.ko
  AUTOLOAD:=$(call AutoLoad,60,pcengines-apuv2)
endef

define KernelPackage/pcengines-apuv2/description
  This driver provides support for the front button and LEDs on
  PC Engines APUv2/APUv3 board.
endef

$(eval $(call KernelPackage,pcengines-apuv2))


define KernelPackage/meraki-mx100
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Cisco Meraki MX100 Platform Driver
  DEPENDS:=@TARGET_x86 +kmod-tg3 +kmod-gpio-button-hotplug +kmod-leds-gpio \
    +kmod-usb-ledtrig-usbport +PACKAGE_kmod-meraki-mx100:nu801 +kmod-itco-wdt \
    +kmod-leds-uleds
  KCONFIG:=CONFIG_MERAKI_MX100
  FILES:=$(LINUX_DIR)/drivers/platform/x86/meraki-mx100.ko
  AUTOLOAD:=$(call AutoLoad,60,meraki-mx100,1)
endef

define KernelPackage/meraki-mx100/description
  This driver provides support for the front button and LEDs on
  the Cisco Meraki MX100 (Tinkerbell) 1U appliance. Note this also
  selects the gpio-cdev nu801 userspace driver to support the Status
  LED, as well as other required platform drivers.
endef

$(eval $(call KernelPackage,meraki-mx100))

define KernelPackage/w83627hf-wdt
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Winbond 83627HF Watchdog Timer
  DEPENDS:=@TARGET_x86
  KCONFIG:=\
	CONFIG_W83627HF_WDT \
	CONFIG_WATCHDOG_CORE=y
  FILES:=$(LINUX_DIR)/drivers/watchdog/w83627hf_wdt.ko
  AUTOLOAD:=$(call AutoLoad,50,w83627hf-wdt,1)
endef

define KernelPackage/w83627hf-wdt/description
  Kernel module for Winbond 83627HF Watchdog Timer
endef

$(eval $(call KernelPackage,w83627hf-wdt))
