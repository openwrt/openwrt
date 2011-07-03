#
# Copyright (C) 2006-2011 OpenWrt.org
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
	CONFIG_BT_HIDP \
	CONFIG_HID_SUPPORT=y
  $(call AddDepends/crc16)
  $(call AddDepends/hid)
  $(call AddDepends/rfkill)
  FILES:= \
	$(LINUX_DIR)/net/bluetooth/bluetooth.ko \
	$(LINUX_DIR)/net/bluetooth/rfcomm/rfcomm.ko \
	$(LINUX_DIR)/net/bluetooth/bnep/bnep.ko \
	$(LINUX_DIR)/net/bluetooth/hidp/hidp.ko \
	$(LINUX_DIR)/drivers/bluetooth/hci_uart.ko \
	$(LINUX_DIR)/drivers/bluetooth/btusb.ko
  ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.39)),1)
    AUTOLOAD:=$(call AutoLoad,90,bluetooth rfcomm bnep hidp hci_uart btusb)
  else
    FILES+= \
	$(LINUX_DIR)/net/bluetooth/l2cap.ko \
	$(LINUX_DIR)/net/bluetooth/sco.ko
    AUTOLOAD:=$(call AutoLoad,90,bluetooth l2cap sco rfcomm bnep hidp hci_uart btusb)
  endif
endef

define KernelPackage/bluetooth/description
 Kernel support for Bluetooth devices
endef

$(eval $(call KernelPackage,bluetooth))


define KernelPackage/bluetooth-hci-h4p
  SUBMENU:=$(OTHER_MENU)
  TITLE:=HCI driver with H4 Nokia extensions
  DEPENDS:=@TARGET_omap24xx +kmod-bluetooth
  KCONFIG:=CONFIG_BT_HCIH4P
  FILES:=$(LINUX_DIR)/drivers/bluetooth/hci_h4p/hci_h4p.ko
  AUTOLOAD:=$(call AutoLoad,91,hci_h4p)
endef

define KernelPackage/bluetooth-hci-h4p/description
  HCI driver with H4 Nokia extensions
endef

$(eval $(call KernelPackage,bluetooth-hci-h4p))


define KernelPackage/cpu-msr
  SUBMENU:=$(OTHER_MENU)
  TITLE:=x86 CPU MSR support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_X86_MSR
  FILES:=$(LINUX_DIR)/arch/x86/kernel/msr.ko
  AUTOLOAD:=$(call AutoLoad,20,msr)
endef

define KernelPackage/cpu-msr/description
 Kernel module for Model Specific Registers support in x86 CPUs
endef

$(eval $(call KernelPackage,cpu-msr))


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
  DEPENDS:=@TARGET_x86 @LINUX_2_6_30||LINUX_2_6_31||LINUX_2_6_32||LINUX_2_6_35||LINUX_2_6_36||LINUX_2_6_37
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
  DEPENDS:=@TARGET_x86 +kmod-cs5535-mfd @!(LINUX_2_6_30||LINUX_2_6_31||LINUX_2_6_32)
  KCONFIG:=CONFIG_GPIO_CS5535
  FILES:=$(LINUX_DIR)/drivers/gpio/cs5535-gpio.ko
  AUTOLOAD:=$(call AutoLoad,50,cs5535-gpio)
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


define KernelPackage/input-gpio-keys-polled
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Polled GPIO key support
  DEPENDS:=@GPIO_SUPPORT @!(LINUX_2_6_30||LINUX_2_6_31||LINUX_2_6_32||LINUX_2_6_34||LINUX_2_6_35||LINUX_2_6_36) +kmod-input-polldev
  KCONFIG:= \
	CONFIG_KEYBOARD_GPIO_POLLED \
	CONFIG_INPUT_KEYBOARD=y
  FILES:=$(LINUX_DIR)/drivers/input/keyboard/gpio_keys_polled.ko
  AUTOLOAD:=$(call AutoLoad,62,gpio_keys_polled)
  $(call AddDepends/input)
endef

define KernelPackage/input-gpio-keys-polled/description
 Kernel module for support polled GPIO keys input device
endef

$(eval $(call KernelPackage,input-gpio-keys-polled))


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
  FILES:=$(LINUX_DIR)/arch/$(LINUX_KARCH)/oprofile/oprofile.ko
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


define KernelPackage/wdt-geode
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Geode/LX Watchdog timer
  DEPENDS:=@TARGET_x86 +kmod-cs5535-mfgpt
  KCONFIG:=CONFIG_GEODE_WDT
  FILES:=$(LINUX_DIR)/drivers/$(WATCHDOG_DIR)/geodewdt.ko
  AUTOLOAD:=$(call AutoLoad,50,geodewdt)
endef

define KernelPackage/wdt-geode/description
  Kernel module for Geode watchdog timer.
endef

$(eval $(call KernelPackage,wdt-geode))


define KernelPackage/cs5535-clockevt
  SUBMENU:=$(OTHER_MENU)
  TITLE:=CS5535/CS5536 high-res timer (MFGPT) events
  DEPENDS:=@TARGET_x86 +kmod-cs5535-mfgpt
  KCONFIG:=CONFIG_CS5535_CLOCK_EVENT_SRC
  FILES:=$(LINUX_DIR)/drivers/clocksource/cs5535-clockevt.ko
  AUTOLOAD:=$(call AutoLoad,50,cs5535-clockevt)
endef

define KernelPackage/cs5535-clockevt/description
  Kernel module for CS5535/6 high-res clock event source
endef

$(eval $(call KernelPackage,cs5535-clockevt))


define KernelPackage/cs5535-mfgpt
  SUBMENU:=$(OTHER_MENU)
  TITLE:=CS5535/6 Multifunction General Purpose Timer
  DEPENDS:=@TARGET_x86 +kmod-cs5535-mfd
  KCONFIG:=CONFIG_CS5535_MFGPT
  FILES:=$(LINUX_DIR)/drivers/misc/cs5535-mfgpt.ko
  AUTOLOAD:=$(call AutoLoad,45,cs5535-mfgpt)
endef

define KernelPackage/cs5535-mfgpt/description
  Kernel module for CS5535/6 multifunction general purpose timer.
endef

$(eval $(call KernelPackage,cs5535-mfgpt))


define KernelPackage/cs5535-mfd
  SUBMENU:=$(OTHER_MENU)
  TITLE:=CS5535/6 Multifunction General Purpose Driver
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_MFD_CS5535
  FILES:= \
  	$(LINUX_DIR)/drivers/mfd/mfd-core.ko \
  	$(LINUX_DIR)/drivers/mfd/cs5535-mfd.ko 
  AUTOLOAD:=$(call AutoLoad,44,mfd-core cs5535-mfd)
endef

define KernelPackage/cs5535-mfd/description
  Core driver for CS5535/CS5536 MFD functions.
endef

$(eval $(call KernelPackage,cs5535-mfd))


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
  DEPENDS:=@(!LINUX_3_0||BROKEN)
  TITLE:=Real Time Clock class support
  KCONFIG:=CONFIG_RTC_CLASS
  FILES:=$(LINUX_DIR)/drivers/rtc/rtc-core.ko
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
  FILES:=$(LINUX_DIR)/drivers/rtc/rtc-pcf8563.ko
  AUTOLOAD:=$(call AutoLoad,60,rtc-pcf8563)
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
  FILES:=$(LINUX_DIR)/drivers/cbus/n810bm.ko
  AUTOLOAD:=$(call AutoLoad,01,n810bm)
endef

define KernelPackage/n810bm/description
  Nokia N810 battery management driver.
  Controls battery power management and battery charging.
endef

$(eval $(call KernelPackage,n810bm))
