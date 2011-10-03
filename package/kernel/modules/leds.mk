#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

LEDS_MENU:=LED modules


ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),lt,2.6.39)),1)
define KernelPackage/leds-alix
  SUBMENU:=$(LEDS_MENU)
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
endif


define KernelPackage/leds-gpio
  SUBMENU:=$(LEDS_MENU)
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
  SUBMENU:=$(LEDS_MENU)
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
  SUBMENU:=$(LEDS_MENU)
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
  SUBMENU:=$(LEDS_MENU)
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
  SUBMENU:=$(LEDS_MENU)
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
  SUBMENU:=$(LEDS_MENU)
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


define KernelPackage/ledtrig-heartbeat
  SUBMENU:=$(LEDS_MENU)
  TITLE:=LED Heartbeat Trigger
  KCONFIG:=CONFIG_LEDS_TRIGGER_HEARTBEAT
  FILES:=$(LINUX_DIR)/drivers/leds/ledtrig-heartbeat.ko
  AUTOLOAD:=$(call AutoLoad,50,ledtrig-heartbeat)
endef

define KernelPackage/ledtrig-gpio/description
 Kernel module that allows LEDs to blink like heart beat
endef

$(eval $(call KernelPackage,ledtrig-heartbeat))


define KernelPackage/ledtrig-gpio
  SUBMENU:=$(LEDS_MENU)
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
  SUBMENU:=$(LEDS_MENU)
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
  SUBMENU:=$(LEDS_MENU)
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
  SUBMENU:=$(LEDS_MENU)
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
  SUBMENU:=$(LEDS_MENU)
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
