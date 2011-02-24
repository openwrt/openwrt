#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

OTHER_MENU:=Other modules

WATCHDOG_DIR:=watchdog


define KernelPackage/bluetooth
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Bluetooth support
  DEPENDS:=@USB_SUPPORT +kmod-usb-core
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
	CONFIG_BT_HCIUART_H4 \
	CONFIG_BT_HIDP
  $(call AddDepends/crc16)
  $(call AddDepends/hid)
  $(call AddDepends/rfkill)
  FILES:= \
	$(LINUX_DIR)/net/bluetooth/bluetooth.ko \
	$(LINUX_DIR)/net/bluetooth/l2cap.ko \
	$(LINUX_DIR)/net/bluetooth/sco.ko \
	$(LINUX_DIR)/net/bluetooth/rfcomm/rfcomm.ko \
	$(LINUX_DIR)/net/bluetooth/bnep/bnep.ko \
	$(LINUX_DIR)/net/bluetooth/hidp/hidp.ko \
	$(LINUX_DIR)/drivers/bluetooth/hci_uart.ko \
	$(LINUX_DIR)/drivers/bluetooth/btusb.ko
  AUTOLOAD:=$(call AutoLoad,90,bluetooth l2cap sco rfcomm bnep hidp hci_uart btusb)
endef

define KernelPackage/bluetooth/description
 Kernel support for Bluetooth devices
endef

$(eval $(call KernelPackage,bluetooth))


define KernelPackage/cpu-msr
  SUBMENU:=$(OTHER_MENU)
  TITLE:=x86 CPU MSR support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_X86_MSR
  FILES:=$(LINUX_DIR)/arch/x86/kernel/msr.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,msr)
endef

define KernelPackage/cpu-msr/description
 Kernel module for Model Specific Registers support in x86 CPUs
endef

$(eval $(call KernelPackage,cpu-msr))


define KernelPackage/crc-ccitt
  SUBMENU:=$(OTHER_MENU)
  TITLE:=CRC-CCITT support
  KCONFIG:=CONFIG_CRC_CCITT
  FILES:=$(LINUX_DIR)/lib/crc-ccitt.ko
  AUTOLOAD:=$(call AutoLoad,20,crc-ccitt)
endef

define KernelPackage/crc-ccitt/description
 Kernel module for CRC-CCITT support
endef

$(eval $(call KernelPackage,crc-ccitt))


define KernelPackage/crc-itu-t
  SUBMENU:=$(OTHER_MENU)
  TITLE:=CRC ITU-T V.41 support
  KCONFIG:=CONFIG_CRC_ITU_T
  FILES:=$(LINUX_DIR)/lib/crc-itu-t.ko
  AUTOLOAD:=$(call AutoLoad,20,crc-itu-t)
endef

define KernelPackage/crc-itu-t/description
 Kernel module for CRC ITU-T V.41 support
endef

$(eval $(call KernelPackage,crc-itu-t))


define KernelPackage/crc7
  SUBMENU:=$(OTHER_MENU)
  TITLE:=CRC7 support
  KCONFIG:=CONFIG_CRC7
  FILES:=$(LINUX_DIR)/lib/crc7.ko
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
  FILES:=$(LINUX_DIR)/lib/crc16.ko
  AUTOLOAD:=$(call AutoLoad,20,crc16,1)
  $(call SetDepends/crc16)
endef

define KernelPackage/crc16/description
 Kernel module for CRC16 support
endef

$(eval $(call KernelPackage,crc16))


define KernelPackage/libcrc32c
  SUBMENU:=$(OTHER_MENU)
  TITLE:=CRC32 library support
  KCONFIG:=CONFIG_LIBCRC32C
  DEPENDS:=+kmod-crypto-core +kmod-crypto-misc
  FILES:=$(LINUX_DIR)/lib/libcrc32c.ko
  AUTOLOAD:=$(call AutoLoad,20,crc32c libcrc32c,1)
endef

define KernelPackage/libcrc32c/description
 Kernel module for CRC32 support
endef

$(eval $(call KernelPackage,libcrc32c))


define KernelPackage/eeprom-93cx6
  SUBMENU:=$(OTHER_MENU)
  TITLE:=EEPROM 93CX6 support
  KCONFIG:=CONFIG_EEPROM_93CX6
  FILES:=$(LINUX_DIR)/drivers/misc/eeprom/eeprom_93cx6.ko
  AUTOLOAD:=$(call AutoLoad,20,eeprom_93cx6)
endef

define KernelPackage/eeprom-93cx6/description
 Kernel module for EEPROM 93CX6 support
endef

$(eval $(call KernelPackage,eeprom-93cx6))


define KernelPackage/gpio-cs5535
  SUBMENU:=$(OTHER_MENU)
  TITLE:=AMD CS5535/CS5536 GPIO driver
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_CS5535_GPIO
  FILES:=$(LINUX_DIR)/drivers/char/cs5535_gpio.ko
  AUTOLOAD:=$(call AutoLoad,50,cs5535_gpio)
endef

define KernelPackage/gpio-cs5535/description
 This package contains the AMD CS5535/CS5536 GPIO driver
endef

$(eval $(call KernelPackage,gpio-cs5535))


define KernelPackage/gpio-cs5535-new
  SUBMENU:=$(OTHER_MENU)
  TITLE:=AMD CS5535/CS5536 GPIO driver with improved sysfs support
  DEPENDS:=@TARGET_x86
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.33)),1) 
  KCONFIG:=CONFIG_GPIO_CS5535 CONFIG_PCI CONFIG_GPIOLIB
  FILES:=$(LINUX_DIR)/drivers/gpio/cs5535-gpio.ko
  AUTOLOAD:=$(call AutoLoad,50,cs5535-gpio)
endif
endef

define KernelPackage/gpio-cs5535-new/description
 This package contains the new AMD CS5535/CS5536 GPIO driver
endef

$(eval $(call KernelPackage,gpio-cs5535-new))


define KernelPackage/gpio-dev
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Generic GPIO char device support
  DEPENDS:=@GPIO_SUPPORT
  KCONFIG:=CONFIG_GPIO_DEVICE
  FILES:=$(LINUX_DIR)/drivers/char/gpio_dev.ko
  AUTOLOAD:=$(call AutoLoad,40,gpio_dev)
endef

define KernelPackage/gpio-dev/description
  Kernel module to allows control of GPIO pins using a character device.
endef

$(eval $(call KernelPackage,gpio-dev))


define KernelPackage/gpio-nsc
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Natsemi GPIO support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_NSC_GPIO
  FILES:=$(LINUX_DIR)/drivers/char/nsc_gpio.ko
  AUTOLOAD:=$(call AutoLoad,40,nsc_gpio)
endef

define KernelPackage/gpio-nsc/description
 Kernel module for Natsemi GPIO
endef

$(eval $(call KernelPackage,gpio-nsc))


define KernelPackage/gpio-pc8736x
  SUBMENU:=$(OTHER_MENU)
  TITLE:=PC8736x GPIO support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_PC8736x_GPIO
  FILES:=$(LINUX_DIR)/drivers/char/pc8736x_gpio.ko
  AUTOLOAD:=$(call AutoLoad,40,pc8736x_gpio)
endef

define KernelPackage/gpio-pc8736x/description
 Kernel module for PC8736x GPIO
endef

$(eval $(call KernelPackage,gpio-pc8736x))


define KernelPackage/gpio-scx200
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Natsemi SCX200 GPIO support
  DEPENDS:=@TARGET_x86 +kmod-gpio-nsc
  KCONFIG:=CONFIG_SCx200_GPIO
  FILES:=$(LINUX_DIR)/drivers/char/scx200_gpio.ko
  AUTOLOAD:=$(call AutoLoad,50,scx200_gpio)
endef

define KernelPackage/gpio-scx200/description
 Kernel module for SCX200 GPIO
endef

$(eval $(call KernelPackage,gpio-scx200))

define KernelPackage/gpio-nxp-74hc164
  SUBMENU:=$(OTHER_MENU)
  TITLE:=NXP 74HC164 GPIO expander support
  DEPENDS:=@TARGET_brcm63xx
  KCONFIG:=CONFIG_GPIO_NXP_74HC164
  FILES:=$(LINUX_DIR)/drivers/gpio/nxp_74hc164.ko
  AUTOLOAD:=$(call AutoLoad,99,nxp_74hc164)
endef

define KernelPackage/gpio-nxp-74hc164/description
  Kernel module for NXP 74HC164 GPIO expander
endef

$(eval $(call KernelPackage,gpio-nxp-74hc164))

define KernelPackage/hid
  SUBMENU:=$(OTHER_MENU)
  TITLE:=HID Devices
  DEPENDS:=+kmod-input-evdev
  KCONFIG:=CONFIG_HID
  FILES:=$(LINUX_DIR)/drivers/hid/hid.ko
  AUTOLOAD:=$(call AutoLoad,61,hid)
  $(call SetDepends/hid)
  $(call AddDepends/input)
endef

define KernelPackage/hid/description
 Kernel modules for HID devices
endef

$(eval $(call KernelPackage,hid))


define KernelPackage/input-core
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Input device core
  KCONFIG:=CONFIG_INPUT
  $(call SetDepends/input)
  FILES:=$(LINUX_DIR)/drivers/input/input-core.ko
  AUTOLOAD:=$(call AutoLoad,19,input-core)
endef

define KernelPackage/input-core/description
 Kernel modules for support of input device
endef

$(eval $(call KernelPackage,input-core))


define KernelPackage/input-evdev
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Input event device
  KCONFIG:=CONFIG_INPUT_EVDEV
  FILES:=$(LINUX_DIR)/drivers/input/evdev.ko
  AUTOLOAD:=$(call AutoLoad,60,evdev)
  $(call AddDepends/input)
endef

define KernelPackage/input-evdev/description
 Kernel modules for support of input device events
endef

$(eval $(call KernelPackage,input-evdev))


define KernelPackage/input-gpio-buttons
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Polled GPIO buttons input device
  DEPENDS:=@GPIO_SUPPORT +kmod-input-polldev
  KCONFIG:= \
	CONFIG_INPUT_GPIO_BUTTONS \
	CONFIG_INPUT_MISC=y
  FILES:=$(LINUX_DIR)/drivers/input/misc/gpio_buttons.ko
  AUTOLOAD:=$(call AutoLoad,62,gpio_buttons)
endef

define KernelPackage/input-gpio-buttons/description
 Kernel module for support polled GPIO buttons input device
endef

$(eval $(call KernelPackage,input-gpio-buttons))


define KernelPackage/input-gpio-keys
  SUBMENU:=$(OTHER_MENU)
  TITLE:=GPIO key support
  DEPENDS:= @GPIO_SUPPORT
  KCONFIG:= \
	CONFIG_KEYBOARD_GPIO \
	CONFIG_INPUT_KEYBOARD=y
  FILES:=$(LINUX_DIR)/drivers/input/keyboard/gpio_keys.ko
  AUTOLOAD:=$(call AutoLoad,60,gpio_keys)
  $(call AddDepends/input)
endef

define KernelPackage/input-gpio-keys/description
 This driver implements support for buttons connected
 to GPIO pins of various CPUs (and some other chips).
endef

$(eval $(call KernelPackage,input-gpio-keys))


define KernelPackage/input-gpio-encoder
  SUBMENU:=$(OTHER_MENU)
  TITLE:=GPIO rotay encoder
  KCONFIG:=CONFIG_INPUT_GPIO_ROTARY_ENCODER
  FILES:=$(LINUX_DIR)/drivers/input/misc/rotary_encoder.ko
  AUTOLOAD:=$(call AutoLoad,62,rotary_encoder)
  $(call AddDepends/input,@GPIO_SUPPORT)
endef

define KernelPackage/gpio-encoder/description
 Kernel module to use rotary encoders connected to GPIO pins
endef

$(eval $(call KernelPackage,input-gpio-encoder))


define KernelPackage/input-joydev
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Joystick device support
  KCONFIG:=CONFIG_INPUT_JOYDEV
  FILES:=$(LINUX_DIR)/drivers/input/joydev.ko
  AUTOLOAD:=$(call AutoLoad,62,joydev)
  $(call AddDepends/input)
endef

define KernelPackage/input-joydev/description
  Kernel module for joystick support
endef

$(eval $(call KernelPackage,input-joydev))


define KernelPackage/input-polldev
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Polled Input device support
  KCONFIG:=CONFIG_INPUT_POLLDEV
  FILES:=$(LINUX_DIR)/drivers/input/input-polldev.ko
  AUTOLOAD:=$(call AutoLoad,20,input-polldev)
  $(call AddDepends/input)
endef

define KernelPackage/input-polldev/description
 Kernel module for support of polled input devices
endef

$(eval $(call KernelPackage,input-polldev))


define KernelPackage/leds-alix
  SUBMENU:=$(OTHER_MENU)
  TITLE:=PCengines ALIX LED support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_LEDS_ALIX2
  FILES:=$(LINUX_DIR)/drivers/leds/leds-alix2.ko
  AUTOLOAD:=$(call AutoLoad,50,leds-alix2)
endef

define KernelPackage/leds-alix/description
 Kernel module for PCengines ALIX LEDs
endef

$(eval $(call KernelPackage,leds-alix))


define KernelPackage/leds-gpio
  SUBMENU:=$(OTHER_MENU)
  TITLE:=GPIO LED support
  DEPENDS:= @GPIO_SUPPORT
  KCONFIG:=CONFIG_LEDS_GPIO
  FILES:=$(LINUX_DIR)/drivers/leds/leds-gpio.ko
  AUTOLOAD:=$(call AutoLoad,60,leds-gpio)
endef

define KernelPackage/leds-gpio/description
 Kernel module for LEDs on GPIO lines
endef

$(eval $(call KernelPackage,leds-gpio))


define KernelPackage/leds-net48xx
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Soekris Net48xx LED support
  DEPENDS:=@TARGET_x86 +kmod-gpio-scx200
  KCONFIG:=CONFIG_LEDS_NET48XX
  FILES:=$(LINUX_DIR)/drivers/leds/leds-net48xx.ko
  AUTOLOAD:=$(call AutoLoad,50,leds-net48xx)
endef

define KernelPackage/leds-net48xx/description
 Kernel module for Soekris Net48xx LEDs
endef

$(eval $(call KernelPackage,leds-net48xx))

define KernelPackage/leds-net5501
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Soekris Net5501 LED support
  DEPENDS:=@TARGET_x86 +kmod-gpio-cs5535 +kmod-leds-gpio
  KCONFIG:=CONFIG_LEDS_NET5501
  FILES:=$(LINUX_DIR)/drivers/leds/leds-net5501.ko
  AUTOLOAD:=$(call AutoLoad,50,leds-net5501)
endef

define KernelPackage/leds-net5501/description
 Kernel module for Soekris Net5501 LEDs
endef

$(eval $(call KernelPackage,leds-net5501))


define KernelPackage/leds-rb750
  SUBMENU:=$(OTHER_MENU)
  TITLE:=RouterBOARD 750 LED support
  DEPENDS:=@TARGET_ar71xx
  KCONFIG:=CONFIG_LEDS_RB750
  FILES:=$(LINUX_DIR)/drivers/leds/leds-rb750.ko
  AUTOLOAD:=$(call AutoLoad,60,leds-rb750)
endef

define KernelPackage/leds-rb750/description
 Kernel module for the LEDs on the MikroTik RouterBOARD 750.
endef

$(eval $(call KernelPackage,leds-rb750))


define KernelPackage/leds-wndr3700-usb
  SUBMENU:=$(OTHER_MENU)
  TITLE:=WNDR3700 USB LED support
  DEPENDS:=@TARGET_ar71xx
  KCONFIG:=CONFIG_LEDS_WNDR3700_USB
  FILES:=$(LINUX_DIR)/drivers/leds/leds-wndr3700-usb.ko
  AUTOLOAD:=$(call AutoLoad,60,leds-wndr3700-usb)
endef

define KernelPackage/leds-wndr3700-usb/description
 Kernel module for the USB LED on the NETGWR WNDR3700 board.
endef

$(eval $(call KernelPackage,leds-wndr3700-usb))


define KernelPackage/leds-wrap
  SUBMENU:=$(OTHER_MENU)
  TITLE:=PCengines WRAP LED support
  DEPENDS:=@TARGET_x86 +kmod-gpio-scx200
  KCONFIG:=CONFIG_LEDS_WRAP
  FILES:=$(LINUX_DIR)/drivers/leds/leds-wrap.ko
  AUTOLOAD:=$(call AutoLoad,50,leds-wrap)
endef

define KernelPackage/leds-wrap/description
 Kernel module for PCengines WRAP LEDs
endef

$(eval $(call KernelPackage,leds-wrap))


define KernelPackage/ledtrig-gpio
  SUBMENU:=$(OTHER_MENU)
  TITLE:=LED GPIO Trigger
  KCONFIG:=CONFIG_LEDS_TRIGGER_GPIO
  FILES:=$(LINUX_DIR)/drivers/leds/ledtrig-gpio.ko
  AUTOLOAD:=$(call AutoLoad,50,ledtrig-gpio)
endef

define KernelPackage/ledtrig-gpio/description
 Kernel module that allows LEDs to be controlled by gpio events.
endef

$(eval $(call KernelPackage,ledtrig-gpio))


define KernelPackage/ledtrig-morse
  SUBMENU:=$(OTHER_MENU)
  TITLE:=LED Morse Trigger
  KCONFIG:=CONFIG_LEDS_TRIGGER_MORSE
  FILES:=$(LINUX_DIR)/drivers/leds/ledtrig-morse.ko
  AUTOLOAD:=$(call AutoLoad,50,ledtrig-morse)
endef

define KernelPackage/ledtrig-morse/description
 Kernel module to show morse coded messages on LEDs.
endef

$(eval $(call KernelPackage,ledtrig-morse))


define KernelPackage/ledtrig-netdev
  SUBMENU:=$(OTHER_MENU)
  TITLE:=LED NETDEV Trigger
  KCONFIG:=CONFIG_LEDS_TRIGGER_NETDEV
  FILES:=$(LINUX_DIR)/drivers/leds/ledtrig-netdev.ko
  AUTOLOAD:=$(call AutoLoad,50,ledtrig-netdev)
endef

define KernelPackage/ledtrig-netdev/description
 Kernel module to drive LEDs based on network activity.
endef

$(eval $(call KernelPackage,ledtrig-netdev))


define KernelPackage/ledtrig-netfilter
  SUBMENU:=$(OTHER_MENU)
  TITLE:=LED NetFilter Trigger
  DEPENDS:=kmod-ipt-core
  KCONFIG:=CONFIG_NETFILTER_XT_TARGET_LED
  FILES:=$(LINUX_DIR)/net/netfilter/xt_LED.ko
  AUTOLOAD:=$(call AutoLoad,50,xt_LED)
endef

define KernelPackage/ledtrig-netfilter/description
 Kernel module to flash LED when a particular packets passing through your machine.

 For example to create an LED trigger for incoming SSH traffic:
    iptables -A INPUT -p tcp --dport 22 -j LED --led-trigger-id ssh --led-delay 1000
 Then attach the new trigger to an LED on your system:
    echo netfilter-ssh > /sys/class/leds/<ledname>/trigger
endef

$(eval $(call KernelPackage,ledtrig-netfilter))

define KernelPackage/ledtrig-usbdev
  SUBMENU:=$(OTHER_MENU)
  TITLE:=LED USB device Trigger
  DEPENDS:=@USB_SUPPORT +kmod-usb-core
  KCONFIG:=CONFIG_LEDS_TRIGGER_USBDEV
  FILES:=$(LINUX_DIR)/drivers/leds/ledtrig-usbdev.ko
  AUTOLOAD:=$(call AutoLoad,50,ledtrig-usbdev)
endef

define KernelPackage/ledtrig-usbdev/description
 Kernel module to drive LEDs based on USB device presence/activity.
endef

$(eval $(call KernelPackage,ledtrig-usbdev))


define KernelPackage/lp
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Parallel port and line printer support
  DEPENDS:=@BROKEN
  KCONFIG:= \
	CONFIG_PARPORT \
	CONFIG_PRINTER \
	CONFIG_PPDEV
  FILES:= \
	$(LINUX_DIR)/drivers/parport/parport.ko \
	$(LINUX_DIR)/drivers/char/lp.ko \
	$(LINUX_DIR)/drivers/char/ppdev.ko
  AUTOLOAD:=$(call AutoLoad,50,parport lp)
endef

$(eval $(call KernelPackage,lp))


define KernelPackage/mmc
  SUBMENU:=$(OTHER_MENU)
  TITLE:=MMC/SD Card Support
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
	$(LINUX_DIR)/drivers/mmc/core/mmc_core.ko \
	$(LINUX_DIR)/drivers/mmc/card/mmc_block.ko
  AUTOLOAD:=$(call AutoLoad,90,mmc_core mmc_block,1)
endef

define KernelPackage/mmc/description
 Kernel support for MMC/SD cards
endef

$(eval $(call KernelPackage,mmc))


define KernelPackage/mmc-atmelmci
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Amtel MMC Support
  DEPENDS:=@TARGET_avr32 +kmod-mmc
  KCONFIG:=CONFIG_MMC_ATMELMCI
  FILES:=$(LINUX_DIR)/drivers/mmc/host/atmel-mci.ko
  AUTOLOAD:=$(call AutoLoad,90,atmel-mci)
endef

define KernelPackage/mmc-atmelmci/description
 Kernel support for  Atmel Multimedia Card Interface.
endef

$(eval $(call KernelPackage,mmc-atmelmci,1))


define KernelPackage/oprofile
  SUBMENU:=$(OTHER_MENU)
  TITLE:=OProfile profiling support
  KCONFIG:=CONFIG_OPROFILE
  FILES:=$(LINUX_DIR)/arch/$(LINUX_KARCH)/oprofile/oprofile.$(LINUX_KMOD_SUFFIX)
  DEPENDS:=@KERNEL_PROFILING
endef

define KernelPackage/oprofile/description
  Kernel module for support for oprofile system profiling.
endef

$(eval $(call KernelPackage,oprofile))


define KernelPackage/rfkill
  SUBMENU:=$(OTHER_MENU)
  TITLE:=RF switch subsystem support
  KCONFIG:= \
    CONFIG_RFKILL \
    CONFIG_RFKILL_INPUT=y \
    CONFIG_RFKILL_LEDS=y
ifeq ($(CONFIG_LINUX_2_6_30),)
  FILES:= \
    $(LINUX_DIR)/net/rfkill/rfkill.ko
  AUTOLOAD:=$(call AutoLoad,20,rfkill)
else
  FILES:= \
    $(LINUX_DIR)/net/rfkill/rfkill.ko \
    $(LINUX_DIR)/net/rfkill/rfkill-input.ko
  AUTOLOAD:=$(call AutoLoad,20,rfkill rfkill-input)
endif
  $(call SetDepends/rfkill)
endef

define KernelPackage/rfkill/description
  Say Y here if you want to have control over RF switches
  found on many WiFi and Bluetooth cards.
endef

$(eval $(call KernelPackage,rfkill))


define KernelPackage/softdog
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Software watchdog driver
  KCONFIG:=CONFIG_SOFT_WATCHDOG
  FILES:=$(LINUX_DIR)/drivers/$(WATCHDOG_DIR)/softdog.ko
  AUTOLOAD:=$(call AutoLoad,50,softdog)
endef

define KernelPackage/softdog/description
 Software watchdog driver
endef

$(eval $(call KernelPackage,softdog))


define KernelPackage/ssb
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Silicon Sonics Backplane glue code
  DEPENDS:=@PCI_SUPPORT @!TARGET_brcm47xx @!TARGET_brcm63xx
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
  FILES:=$(LINUX_DIR)/drivers/ssb/ssb.ko
  AUTOLOAD:=$(call AutoLoad,29,ssb)
endef

define KernelPackage/ssb/description
  Silicon Sonics Backplane glue code.
endef

$(eval $(call KernelPackage,ssb))


define KernelPackage/textsearch
SUBMENU:=$(OTHER_MENU)
  TITLE:=Textsearch support is selected if needed
  KCONFIG:= \
    CONFIG_TEXTSEARCH=y \
    CONFIG_TEXTSEARCH_KMP \
    CONFIG_TEXTSEARCH_BM \
    CONFIG_TEXTSEARCH_FSM
  FILES:= \
    $(LINUX_DIR)/lib/ts_kmp.ko \
    $(LINUX_DIR)/lib/ts_bm.ko \
    $(LINUX_DIR)/lib/ts_fsm.ko
  AUTOLOAD:=$(call AutoLoad,20,ts_kmp ts_bm ts_fsm)
endef

$(eval $(call KernelPackage,textsearch))


define KernelPackage/wdt-geode
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Geode/LX Watchdog timer
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_GEODE_WDT
  FILES:=$(LINUX_DIR)/drivers/$(WATCHDOG_DIR)/geodewdt.ko
  AUTOLOAD:=$(call AutoLoad,50,geodewdt)
endef

define KernelPackage/wdt-geode/description
  Kernel module for Geode watchdog timer.
endef

$(eval $(call KernelPackage,wdt-geode))


define KernelPackage/wdt-omap
  SUBMENU:=$(OTHER_MENU)
  TITLE:=OMAP Watchdog timer
  DEPENDS:=@(TARGET_omap24xx||TARGET_omap35xx)
  KCONFIG:=CONFIG_OMAP_WATCHDOG
  FILES:=$(LINUX_DIR)/drivers/$(WATCHDOG_DIR)/omap_wdt.ko
  AUTOLOAD:=$(call AutoLoad,50,omap_wdt.ko)
endef

define KernelPackage/wdt-omap/description
  Kernel module for TI omap watchdog timer.
endef

$(eval $(call KernelPackage,wdt-omap))


define KernelPackage/wdt-orion
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Marvell Orion Watchdog timer
  DEPENDS:=@TARGET_orion
  KCONFIG:=CONFIG_ORION_WATCHDOG
  FILES:=$(LINUX_DIR)/drivers/$(WATCHDOG_DIR)/orion_wdt.ko
  AUTOLOAD:=$(call AutoLoad,50,orion_wdt)
endef

define KernelPackage/wdt-orion/description
  Kernel module for Marvell orion watchdog timer.
endef

$(eval $(call KernelPackage,wdt-orion))


define KernelPackage/wdt-sc520
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Natsemi SC520 Watchdog support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_SC520_WDT
  FILES:=$(LINUX_DIR)/drivers/$(WATCHDOG_DIR)/sc520_wdt.ko
  AUTOLOAD:=$(call AutoLoad,50,sc520_wdt)
endef

define KernelPackage/wdt-sc520/description
  Kernel module for SC520 Watchdog
endef

$(eval $(call KernelPackage,wdt-sc520))


define KernelPackage/wdt-scx200
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Natsemi SCX200 Watchdog support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_SCx200_WDT
  FILES:=$(LINUX_DIR)/drivers/$(WATCHDOG_DIR)/scx200_wdt.ko
  AUTOLOAD:=$(call AutoLoad,50,scx200_wdt)
endef

define KernelPackage/wdt-scx200/description
 Kernel module for SCX200 Watchdog
endef

$(eval $(call KernelPackage,wdt-scx200))

define KernelPackage/pwm
  SUBMENU:=$(OTHER_MENU)
  TITLE:=PWM generic API
  KCONFIG:=CONFIG_GENERIC_PWM
  FILES:=$(LINUX_DIR)/drivers/pwm/pwm.ko
  AUTOLOAD:=$(call AutoLoad,50,pwm)
endef

define KernelPackage/pwm/description
 Kernel module that implement a generic PWM API
endef

$(eval $(call KernelPackage,pwm))


define KernelPackage/pwm-gpio
  SUBMENU:=$(OTHER_MENU)
  TITLE:=PWM over GPIO
  DEPENDS:=+kmod-pwm
  KCONFIG:=CONFIG_GPIO_PWM
  FILES:=$(LINUX_DIR)/drivers/pwm/gpio-pwm.ko
  AUTOLOAD:=$(call AutoLoad,51,gpio-pwm)
endef

define KernelPackage/pwm-gpio/description
 Kernel module to models a single-channel PWM device using a timer and a GPIO pin
endef

$(eval $(call KernelPackage,pwm-gpio))

define KernelPackage/rtc-core
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Real Time Clock class support
  KCONFIG:=CONFIG_RTC_CLASS
  FILES:=$(LINUX_DIR)/drivers/rtc/rtc-core.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,29,rtc-core)
endef

define KernelPackage/rtc-core/description
 Generic RTC class support.
endef

$(eval $(call KernelPackage,rtc-core))

define KernelPackage/rtc-pcf8563
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Philips PCF8563/Epson RTC8564 RTC support
  DEPENDS:=+kmod-rtc-core
  KCONFIG:=CONFIG_RTC_DRV_PCF8563
  FILES:=$(LINUX_DIR)/drivers/rtc/rtc-pcf8563.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,rtc-pcf8563)
endef

define KernelPackage/rtc-pcf8563/description
 Kernel module for Philips PCF8563 RTC chip.
 The Epson RTC8564 should work as well.
endef

$(eval $(call KernelPackage,rtc-pcf8563))


define KernelPackage/n810bm
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Nokia N810 battery management driver
  DEPENDS:=@TARGET_omap24xx
  KCONFIG:=CONFIG_N810BM
  FILES:=$(LINUX_DIR)/drivers/cbus/n810bm.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,01,n810bm)
endef

define KernelPackage/n810bm/description
  Nokia N810 battery management driver.
  Controls battery power management and battery charging.
endef

$(eval $(call KernelPackage,n810bm))
