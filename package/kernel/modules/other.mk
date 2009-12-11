#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

OTHER_MENU:=Other modules

define KernelPackage/crc-itu-t
  SUBMENU:=$(OTHER_MENU)
  TITLE:=CRC ITU-T V.41 support
  KCONFIG:=CONFIG_CRC_ITU_T
  FILES:=$(LINUX_DIR)/lib/crc-itu-t.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,crc-itu-t)
endef

define KernelPackage/crc-itu-t/description
 Kernel module for CRC ITU-T V.41 support
endef

$(eval $(call KernelPackage,crc-itu-t))

define KernelPackage/crc-ccitt
  SUBMENU:=$(OTHER_MENU)
  TITLE:=CRC-CCITT support
  KCONFIG:=CONFIG_CRC_CCITT
  FILES:=$(LINUX_DIR)/lib/crc-ccitt.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,crc-ccitt)
endef

define KernelPackage/crc-ccitt/description
 Kernel module for CRC-CCITT support
endef

$(eval $(call KernelPackage,crc-ccitt))


define KernelPackage/crc7
  SUBMENU:=$(OTHER_MENU)
  TITLE:=CRC7 support
  KCONFIG:=CONFIG_CRC7
  FILES:=$(LINUX_DIR)/lib/crc7.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,crc7)
endef

define KernelPackage/crc7/description
 Kernel module for CRC7 support
endef

$(eval $(call KernelPackage,crc7))


define KernelPackage/crc16
  SUBMENU:=$(OTHER_MENU)
  TITLE:=CRC16 support
  KCONFIG:=CONFIG_CRC16
  FILES:=$(LINUX_DIR)/lib/crc16.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,crc16)
endef

define KernelPackage/crc16/description
 Kernel module for CRC16 support
endef

$(eval $(call KernelPackage,crc16))


define KernelPackage/eeprom-93cx6
  SUBMENU:=$(OTHER_MENU)
  TITLE:=EEPROM 93CX6 support
  DEPENDS:=@LINUX_2_6
  KCONFIG:=CONFIG_EEPROM_93CX6
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.29)),1)
  FILES:=$(LINUX_DIR)/drivers/misc/eeprom/eeprom_93cx6.$(LINUX_KMOD_SUFFIX)
else
  FILES:=$(LINUX_DIR)/drivers/misc/eeprom_93cx6.$(LINUX_KMOD_SUFFIX)
endif
  AUTOLOAD:=$(call AutoLoad,20,eeprom_93cx6)
endef

define KernelPackage/eeprom-93cx6/description
 Kernel module for EEPROM 93CX6 support
endef

$(eval $(call KernelPackage,eeprom-93cx6))


define KernelPackage/lp
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Parallel port and line printer support
  DEPENDS:=@LINUX_2_4
  KCONFIG:= \
	CONFIG_PARPORT \
	CONFIG_PRINTER \
	CONFIG_PPDEV
  FILES:= \
	$(LINUX_DIR)/drivers/parport/parport.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/char/lp.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/char/ppdev.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,parport lp)
endef

$(eval $(call KernelPackage,lp))

define KernelPackage/pcspkr
  SUBMENU:=$(OTHER_MENU)
  TITLE:=PC speaker support
  DEPENDS:=@LINUX_2_6
  KCONFIG:=CONFIG_INPUT_PCSPKR
  FILES:= \
	$(LINUX_DIR)/drivers/input/misc/pcspkr.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50, \
  	pcspkr \
  )
endef

define KernelPackage/pcspkr/description
 This enables sounds (tones) through the pc speaker
endef

$(eval $(call KernelPackage,pcspkr))

define KernelPackage/pcmcia-core
  SUBMENU:=$(OTHER_MENU)
  TITLE:=PCMCIA/CardBus support
  DEPENDS:=@PCMCIA_SUPPORT
  KCONFIG:= \
	CONFIG_PCMCIA \
	CONFIG_CARDBUS \
	CONFIG_PCCARD \
	CONFIG_PCCARD_NONSTATIC \
	PCMCIA_DEBUG=n
endef

define KernelPackage/pcmcia-core/2.4
#  KCONFIG:= \
#	CONFIG_PCMCIA \
#	CONFIG_CARDBUS
  FILES:= \
	$(LINUX_DIR)/drivers/pcmcia/pcmcia_core.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/pcmcia/ds.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,pcmcia_core ds)
endef

define KernelPackage/pcmcia-core/2.6
#  KCONFIG:= \
#	CONFIG_PCCARD \
#	CONFIG_PCMCIA \
#	CONFIG_PCCARD_NONSTATIC \
#	PCMCIA_DEBUG=n
  FILES:= \
	$(LINUX_DIR)/drivers/pcmcia/pcmcia_core.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/pcmcia/pcmcia.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,pcmcia_core pcmcia)
endef

define KernelPackage/pcmcia-core/description
 Kernel support for PCMCIA/CardBus controllers
endef

$(eval $(call KernelPackage,pcmcia-core))


define KernelPackage/pcmcia-yenta
  SUBMENU:=$(OTHER_MENU)
  TITLE:=yenta socket driver
  DEPENDS:=kmod-pcmcia-core
  KCONFIG:=CONFIG_YENTA
  FILES:= \
	$(LINUX_DIR)/drivers/pcmcia/rsrc_nonstatic.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/pcmcia/yenta_socket.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,rsrc_nonstatic yenta_socket)
endef

$(eval $(call KernelPackage,pcmcia-yenta))

define KernelPackage/pcmcia-au1000
  SUBMENU:=$(OTHER_MENU)
  TITLE:=RMI/AMD Au1000 PCMCIA support
  DEPENDS:=kmod-pcmcia-core @TARGET_au1000
  FILES:=$(LINUX_DIR)/drivers/pcmcia/au1x00_ss.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,au1x00_ss)
endef

$(eval $(call KernelPackage,pcmcia-au1000))

define KernelPackage/pcmcia-bcm63xx
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Broadcom BCM63xx PCMCIA support
  DEPENDS:=kmod-pcmcia-core @TARGET_brcm63xx
  KCONFIG:=CONFIG_PCMCIA_BCM63XX
  FILES:=$(LINUX_DIR)/drivers/pcmcia/bcm63xx_pcmcia.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,bcm63xx_pcmcia)
endef

define KernelPackage/pcmcia-bcm63xx/description
  Kernel support for PCMCIA/CardBus controller on the BCM63xx SoC
endef

$(eval $(call KernelPackage,pcmcia-bcm63xx))

define KernelPackage/pcmcia-serial
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Serial devices support
  DEPENDS:=kmod-pcmcia-core
  KCONFIG:= \
	CONFIG_PCMCIA_SERIAL_CS \
	CONFIG_SERIAL_8250_CS
  AUTOLOAD:=$(call AutoLoad,45,serial_cs)
endef

define KernelPackage/pcmcia-serial/2.4
#  KCONFIG:=CONFIG_PCMCIA_SERIAL_CS
  FILES:=$(LINUX_DIR)/drivers/char/pcmcia/serial_cs.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/pcmcia-serial/2.6
#  KCONFIG:=CONFIG_SERIAL_8250_CS
  FILES:=$(LINUX_DIR)/drivers/serial/serial_cs.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/pcmcia-serial/description
 Kernel support for PCMCIA/CardBus serial devices
endef

$(eval $(call KernelPackage,pcmcia-serial))

define KernelPackage/ssb
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Silicon Sonics Backplane glue code
  DEPENDS:=@LINUX_2_6 @PCI_SUPPORT @!TARGET_brcm47xx @!TARGET_brcm63xx
  KCONFIG:=\
	CONFIG_SSB \
	CONFIG_SSB_B43_PCI_BRIDGE=y \
	CONFIG_SSB_DRIVER_MIPS=n \
	CONFIG_SSB_DRIVER_PCICORE=y \
	CONFIG_SSB_DRIVER_PCICORE_POSSIBLE=y \
	CONFIG_SSB_PCIHOST=y \
	CONFIG_SSB_PCIHOST_POSSIBLE=y \
	CONFIG_SSB_POSSIBLE=y \
	CONFIG_SSB_SPROM=y \
	CONFIG_SSB_SILENT=y
  FILES:=$(LINUX_DIR)/drivers/ssb/ssb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,29,ssb)
endef

define KernelPackage/ssb/description
  Silicon Sonics Backplane glue code.
endef

$(eval $(call KernelPackage,ssb))


define KernelPackage/bluetooth
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Bluetooth support
  DEPENDS:=@USB_SUPPORT +kmod-usb-core +kmod-hid
  KCONFIG:= \
	CONFIG_BLUEZ \
	CONFIG_BLUEZ_L2CAP \
	CONFIG_BLUEZ_SCO \
	CONFIG_BLUEZ_RFCOMM \
	CONFIG_BLUEZ_BNEP \
	CONFIG_BLUEZ_HCIUART \
	CONFIG_BLUEZ_HCIUSB \
	CONFIG_BLUEZ_HIDP \
	CONFIG_BT \
	CONFIG_BT_L2CAP \
	CONFIG_BT_SCO \
	CONFIG_BT_RFCOMM \
	CONFIG_BT_BNEP \
	CONFIG_BT_HCIBTUSB \
	CONFIG_BT_HCIUSB \
	CONFIG_BT_HCIUART \
	CONFIG_BT_HIDP
endef

define KernelPackage/bluetooth/2.4
#  KCONFIG:= \
#	CONFIG_BLUEZ \
#	CONFIG_BLUEZ_L2CAP \
#	CONFIG_BLUEZ_SCO \
#	CONFIG_BLUEZ_RFCOMM \
#	CONFIG_BLUEZ_BNEP \
#	CONFIG_BLUEZ_HCIUART \
#	CONFIG_BLUEZ_HCIUSB
  FILES:= \
	$(LINUX_DIR)/net/bluetooth/bluez.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/l2cap.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/sco.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/rfcomm/rfcomm.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/bnep/bnep.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/bluetooth/hci_uart.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/bluetooth/hci_usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,bluez l2cap sco rfcomm bnep hci_uart hci_usb)
endef

define KernelPackage/bluetooth/2.6
#  KCONFIG:= \
#	CONFIG_BT \
#	CONFIG_BT_L2CAP \
#	CONFIG_BT_SCO \
#	CONFIG_BT_RFCOMM \
#	CONFIG_BT_BNEP \
#	CONFIG_BT_HCIUSB \
#	CONFIG_BT_HCIUART
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.27)),1)
  FILES:= \
	$(LINUX_DIR)/net/bluetooth/bluetooth.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/l2cap.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/sco.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/rfcomm/rfcomm.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/bnep/bnep.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/hidp/hidp.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/bluetooth/hci_uart.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/bluetooth/btusb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,bluetooth l2cap sco rfcomm bnep hidp hci_uart btusb)
else
  FILES:= \
	$(LINUX_DIR)/net/bluetooth/bluetooth.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/l2cap.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/sco.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/rfcomm/rfcomm.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/bnep/bnep.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/hidp/hidp.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/bluetooth/hci_uart.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/bluetooth/hci_usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,bluetooth l2cap sco rfcomm bnep hidp hci_uart hci_usb)
endif
endef

define KernelPackage/bluetooth/description
 Kernel support for Bluetooth devices
endef

$(eval $(call KernelPackage,bluetooth))


define KernelPackage/mmc
  SUBMENU:=$(OTHER_MENU)
  TITLE:=MMC/SD Card Support
  DEPENDS:=@LINUX_2_6
  KCONFIG:= \
	CONFIG_MMC \
	CONFIG_MMC_BLOCK \
	CONFIG_MMC_DEBUG=n \
	CONFIG_MMC_UNSAFE_RESUME=n \
	CONFIG_MMC_BLOCK_BOUNCE=y \
	CONFIG_MMC_SDHCI=n \
	CONFIG_MMC_TIFM_SD=n \
	CONFIG_MMC_WBSD=n \
	CONFIG_SDIO_UART=n
  FILES:= \
	$(LINUX_DIR)/drivers/mmc/core/mmc_core.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/mmc/card/mmc_block.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,mmc_core mmc_block)
endef

define KernelPackage/mmc/description
 Kernel support for MMC/SD cards
endef

$(eval $(call KernelPackage,mmc))


define KernelPackage/mmc-at91
  SUBMENU:=$(OTHER_MENU)
  TITLE:=MMC/SD Card Support on AT91
  DEPENDS:=@TARGET_at91 +kmod-mmc
  KCONFIG:=CONFIG_MMC_AT91
  FILES:=$(LINUX_DIR)/drivers/mmc/host/at91_mci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,at91_mci)
endef

define KernelPackage/mmc-at91/description
 Kernel support for MMC/SD cards on the AT91 target
endef

$(eval $(call KernelPackage,mmc-at91))


# XXX: added a workaround for watchdog path changes
ifeq ($(KERNEL),2.4)
  WATCHDOG_DIR=char
endif
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.24)),1)
  WATCHDOG_DIR=watchdog
endif
WATCHDOG_DIR?=char/watchdog

define KernelPackage/atmel-wdt
  SUBMENU:=$(OTHER_MENU)
  TITLE:=AT32AP700x watchdog
  DEPENDS:=@TARGET_avr32
  KCONFIG:=CONFIG_AT32AP700X_WDT
  FILES:=$(LINUX_DIR)/drivers/$(WATCHDOG_DIR)/at32ap700x_wdt.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,at32ap700x_wdt)
endef

define KernelPackage/atmel-wdt/description
 AT32AP700x watchdog
endef

$(eval $(call KernelPackage,atmel-wdt))


define KernelPackage/softdog
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Software watchdog driver
  KCONFIG:=CONFIG_SOFT_WATCHDOG
  FILES:=$(LINUX_DIR)/drivers/$(WATCHDOG_DIR)/softdog.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,softdog)
endef

define KernelPackage/softdog/description
 Software watchdog driver
endef

$(eval $(call KernelPackage,softdog))

define KernelPackage/rdc321x-wdt
  SUBMENU:=$(OTHER_MENU)
  TITLE:=RDC321x watchdog
  DEPENDS:=@TARGET_rdc
  KCONFIG:=CONFIG_RDC321X_WDT
  FILES:=$(LINUX_DIR)/drivers/$(WATCHDOG_DIR)/rdc321x_wdt.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,rdc321_wdt)
endef

define KernelPackage/rdc321x-wdt/description
  RDC-321x watchdog driver
endef

$(eval $(call KernelPackage,rdc321x-wdt))


define KernelPackage/leds-gpio
  SUBMENU:=$(OTHER_MENU)
  TITLE:=GPIO LED support
  DEPENDS:= @GPIO_SUPPORT
  KCONFIG:=CONFIG_LEDS_GPIO
  FILES:=$(LINUX_DIR)/drivers/leds/leds-gpio.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,leds-gpio)
endef

define KernelPackage/leds-gpio/description
 Kernel module for LEDs on GPIO lines
endef

$(eval $(call KernelPackage,leds-gpio))


define KernelPackage/ledtrig-adm5120-switch
  SUBMENU:=$(OTHER_MENU)
  TITLE:=LED ADM5120 Switch Port Status Trigger
  DEPENDS:=@TARGET_adm5120
  KCONFIG:=CONFIG_LEDS_TRIGGER_ADM5120_SWITCH
  FILES:=$(LINUX_DIR)/drivers/leds/ledtrig-adm5120-switch.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,ledtrig-adm5120-switch)
endef

define KernelPackage/ledtrig-adm5120-switch/description
 Kernel module to allow LEDs to be controlled by the port states
 of the ADM5120 built-in ethernet switch.
endef

$(eval $(call KernelPackage,ledtrig-adm5120-switch))


define KernelPackage/leds-net48xx
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Soekris Net48xx LED support
  DEPENDS:=@TARGET_x86 +kmod-scx200-gpio
  KCONFIG:=CONFIG_LEDS_NET48XX
  FILES:=$(LINUX_DIR)/drivers/leds/leds-net48xx.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,leds-net48xx)
endef

define KernelPackage/leds-net48xx/description
 Kernel module for Soekris Net48xx LEDs
endef

$(eval $(call KernelPackage,leds-net48xx))


define KernelPackage/leds-wrap
  SUBMENU:=$(OTHER_MENU)
  TITLE:=PCengines WRAP LED support
  DEPENDS:=@TARGET_x86 +kmod-scx200-gpio
  KCONFIG:=CONFIG_LEDS_WRAP
  FILES:=$(LINUX_DIR)/drivers/leds/leds-wrap.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,leds-wrap)
endef

define KernelPackage/leds-wrap/description
 Kernel module for PCengines WRAP LEDs
endef

$(eval $(call KernelPackage,leds-wrap))


define KernelPackage/leds-alix
  SUBMENU:=$(OTHER_MENU)
  TITLE:=PCengines ALIX LED support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_LEDS_ALIX2
  FILES:=$(LINUX_DIR)/drivers/leds/leds-alix2.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,leds-alix2)
endef

define KernelPackage/leds-alix/description
 Kernel module for PCengines ALIX LEDs
endef

$(eval $(call KernelPackage,leds-alix))


define KernelPackage/leds-wndr3700-usb
  SUBMENU:=$(OTHER_MENU)
  TITLE:=WNDR3700 USB LED support
  DEPENDS:= @TARGET_ar71xx
  KCONFIG:=CONFIG_LEDS_WNDR3700_USB
  FILES:=$(LINUX_DIR)/drivers/leds/leds-wndr3700-usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,leds-wndr3700-usb)
endef

define KernelPackage/leds-wndr3700-usb/description
 Kernel module for the USB LED on the NETGWR WNDR3700 board.
endef

$(eval $(call KernelPackage,leds-wndr3700-usb))


define KernelPackage/ledtrig-netdev
  SUBMENU:=$(OTHER_MENU)
  TITLE:=LED NETDEV Trigger
  KCONFIG:=CONFIG_LEDS_TRIGGER_NETDEV
  FILES:=$(LINUX_DIR)/drivers/leds/ledtrig-netdev.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,ledtrig-netdev)
endef

define KernelPackage/ledtrig-netdev/description
 Kernel module to drive LEDs based on network activity.
endef

$(eval $(call KernelPackage,ledtrig-netdev))


define KernelPackage/ledtrig-morse
  SUBMENU:=$(OTHER_MENU)
  TITLE:=LED Morse Trigger
  KCONFIG:=CONFIG_LEDS_TRIGGER_MORSE
  FILES:=$(LINUX_DIR)/drivers/leds/ledtrig-morse.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,ledtrig-morse)
endef

define KernelPackage/ledtrig-morse/description
 Kernel module to show morse coded messages on LEDs.
endef

$(eval $(call KernelPackage,ledtrig-morse))


define KernelPackage/gpio-dev
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Generic GPIO char device support
  DEPENDS:=@GPIO_SUPPORT
  KCONFIG:=CONFIG_GPIO_DEVICE
  FILES:=$(LINUX_DIR)/drivers/char/gpio_dev.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,gpio_dev)
endef

define KernelPackage/gpio-dev/description
  Kernel module to allows control of GPIO pins using a character device.
endef

$(eval $(call KernelPackage,gpio-dev))


define KernelPackage/nsc-gpio
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Natsemi GPIO support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_NSC_GPIO
  FILES:=$(LINUX_DIR)/drivers/char/nsc_gpio.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,nsc_gpio)
endef

define KernelPackage/nsc-gpio/description
 Kernel module for Natsemi GPIO
endef

$(eval $(call KernelPackage,nsc-gpio))


define KernelPackage/scx200-gpio
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Natsemi SCX200 GPIO support
  DEPENDS:=@TARGET_x86 +kmod-nsc-gpio
  KCONFIG:=CONFIG_SCx200_GPIO
  FILES:=$(LINUX_DIR)/drivers/char/scx200_gpio.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,scx200_gpio)
endef

define KernelPackage/scx200-gpio/description
 Kernel module for SCX200 GPIO
endef

$(eval $(call KernelPackage,scx200-gpio))


define KernelPackage/scx200-wdt
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Natsemi SCX200 Watchdog support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_SCx200_WDT
  FILES:=$(LINUX_DIR)/drivers/$(WATCHDOG_DIR)/scx200_wdt.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,scx200_wdt)
endef

define KernelPackage/scx200-wdt/description
 Kernel module for SCX200 Watchdog
endef

$(eval $(call KernelPackage,scx200-wdt))


define KernelPackage/sc520-wdt
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Natsemi SC520 Watchdog support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_SC520_WDT
  FILES:=$(LINUX_DIR)/drivers/$(WATCHDOG_DIR)/sc520_wdt.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,sc520_wdt)
endef

define KernelPackage/sc520-wdt/description
  Kernel module for SC520 Watchdog
endef

$(eval $(call KernelPackage,sc520-wdt))


define KernelPackage/input-core
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Input device core
  DEPENDS:=@LINUX_2_6
  KCONFIG:=CONFIG_INPUT
  FILES:=$(LINUX_DIR)/drivers/input/input-core.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,19,input-core)
endef

define KernelPackage/input-core/description
 Kernel modules for support of input device
endef

$(eval $(call KernelPackage,input-core))


define KernelPackage/input-evdev
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Input event device
  DEPENDS:=+kmod-input-core
  KCONFIG:=CONFIG_INPUT_EVDEV
  FILES:=$(LINUX_DIR)/drivers/input/evdev.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,evdev)
endef

define KernelPackage/input-evdev/description
 Kernel modules for support of input device events
endef

$(eval $(call KernelPackage,input-evdev))


define KernelPackage/hid
  SUBMENU:=$(OTHER_MENU)
  TITLE:=HID Devices
  DEPENDS:=+kmod-input-core +kmod-input-evdev
  KCONFIG:=CONFIG_HID
  FILES:=$(LINUX_DIR)/drivers/hid/hid.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,61,hid)
endef

define KernelPackage/hid/description
 Kernel modules for HID devices
endef

$(eval $(call KernelPackage,hid))


define KernelPackage/input-polldev
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Polled Input device support
  DEPENDS:=+kmod-input-core @LINUX_2_6
  KCONFIG:=CONFIG_INPUT_POLLDEV
  FILES:=$(LINUX_DIR)/drivers/input/input-polldev.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,input-polldev)
endef

define KernelPackage/input-polldev/description
 Kernel module for support of polled input devices
endef

$(eval $(call KernelPackage,input-polldev))


define KernelPackage/input-gpio-keys
  SUBMENU:=$(OTHER_MENU)
  TITLE:=GPIO key support
  DEPENDS:= @GPIO_SUPPORT +kmod-input-core
  KCONFIG:=CONFIG_KEYBOARD_GPIO
  FILES:=$(LINUX_DIR)/drivers/input/keyboard/gpio_keys.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,gpio_keys)
endef

define KernelPackage/input-gpio-keys/description
 This driver implements support for buttons connected
 to GPIO pins of various CPUs (and some other chips).
endef

$(eval $(call KernelPackage,input-gpio-keys))


define KernelPackage/input-gpio-buttons
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Polled GPIO buttons input device
  DEPENDS:=@GPIO_SUPPORT +kmod-input-polldev
  KCONFIG:= \
	CONFIG_INPUT_GPIO_BUTTONS \
	CONFIG_INPUT_MISC=y
  FILES:=$(LINUX_DIR)/drivers/input/misc/gpio_buttons.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,62,gpio_buttons)
endef

define KernelPackage/input-gpio-buttons/description
 Kernel module for support polled GPIO buttons input device
endef

$(eval $(call KernelPackage,input-gpio-buttons))

define KernelPackage/input-joydev
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Joystick device support
  DEPENDS:=+kmod-input-core
  KCONFIG:=CONFIG_INPUT_JOYDEV
  FILES:=$(LINUX_DIR)/drivers/input/joydev.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,62,joydev)
endef

define KernelPackage/input-joydev/description
  Kernel module for joystick support
endef

$(eval $(call KernelPackage,input-joydev))

define KernelPackage/input-rb532
  SUBMENU:=$(OTHER_MENU)
  TITLE:=RB532 button device support
  DEPENDS:=+kmod-input-core @TARGET_rb532
  KCONFIG:= \
	CONFIG_INPUT_MISC=y \
	CONFIG_INPUT_RB532_BUTTON
  FILES:=$(LINUX_DIR)/drivers/input/misc/rb532_button.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,62,rb532_button)
endef

define KernelPackage/input-rb532/description
  Kernel module for RB532 button
endef

$(eval $(call KernelPackage,input-rb532))


define KernelPackage/mmc-atmelmci
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Amtel MMC Support
  DEPENDS:=@TARGET_avr32 +kmod-mmc
  KCONFIG:=CONFIG_MMC_ATMELMCI
  FILES:=$(LINUX_DIR)/drivers/mmc/host/atmel-mci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,atmel-mci)
endef

define KernelPackage/mmc-atmelmci/description
 Kernel support for  Atmel Multimedia Card Interface.
endef

$(eval $(call KernelPackage,mmc-atmelmci))


define KernelPackage/cs5535-gpio
  SUBMENU:=$(OTHER_MENU)
  TITLE:=AMD CS5535/CS5536 GPIO driver
  DEPENDS:=@TARGET_x86||@TARGET_olpc
  KCONFIG:=CONFIG_CS5535_GPIO
  FILES:=$(LINUX_DIR)/drivers/char/cs5535_gpio.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,cs5535_gpio)
endef

define KernelPackage/cs5535-gpio/description
 This package contains the AMD CS5535/CS5536 GPIO driver
endef

$(eval $(call KernelPackage,cs5535-gpio))


define KernelPackage/textsearch
SUBMENU:=$(OTHER_MENU)
  TITLE:=Textsearch support is selected if needed
  DEPENDS:=@LINUX_2_6 @!LINUX_2_6_21
  KCONFIG:= \
    CONFIG_TEXTSEARCH=y \
    CONFIG_TEXTSEARCH_KMP \
    CONFIG_TEXTSEARCH_BM \
    CONFIG_TEXTSEARCH_FSM
  FILES:= \
    $(LINUX_DIR)/lib/ts_kmp.$(LINUX_KMOD_SUFFIX) \
    $(LINUX_DIR)/lib/ts_bm.$(LINUX_KMOD_SUFFIX) \
    $(LINUX_DIR)/lib/ts_fsm.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,ts_kmp ts_bm ts_fsm)
endef

$(eval $(call KernelPackage,textsearch))


define KernelPackage/rfkill
  SUBMENU:=$(OTHER_MENU)
  TITLE:=RF switch subsystem support
  DEPENDS:=@TARGET_x86||TARGET_olpc||TARGET_s3c24xx||TARGET_brcm47xx
  KCONFIG:= \
    CONFIG_RFKILL \
    CONFIG_RFKILL_INPUT=y \
    CONFIG_RFKILL_LEDS=y
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.31)),1)
  FILES:= \
    $(LINUX_DIR)/net/rfkill/rfkill.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,rfkill)
else
  FILES:= \
    $(LINUX_DIR)/net/rfkill/rfkill.$(LINUX_KMOD_SUFFIX) \
    $(LINUX_DIR)/net/rfkill/rfkill-input.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,rfkill rfkill-input)
endif
endef

define KernelPackage/rfkill/description
  Say Y here if you want to have control over RF switches
  found on many WiFi and Bluetooth cards.
endef

$(eval $(call KernelPackage,rfkill))
