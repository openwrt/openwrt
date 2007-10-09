#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

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


define KernelPackage/eeprom-93cx6
  SUBMENU:=$(OTHER_MENU)
  TITLE:=EEPROM 93CX6 support
  DEPENDS:=@LINUX_2_6
  KCONFIG:=CONFIG_EEPROM_93CX6
  FILES:=$(LINUX_DIR)/drivers/misc/eeprom_93cx6.$(LINUX_KMOD_SUFFIX)
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


define KernelPackage/pcmcia-core
  SUBMENU:=$(OTHER_MENU)
  TITLE:=PCMCIA/CardBus support
  DEPENDS:=@PCMCIA_SUPPORT
  KCONFIG:= \
	CONFIG_PCMCIA \
	CONFIG_CARDBUS
	CONFIG_PCCARD \
	CONFIG_YENTA \
	CONFIG_PCCARD_NONSTATIC \
	PCMCIA_DEBUG=n
endef

define KernelPackage/pcmcia-core/2.4
#  KCONFIG:= \
#	CONFIG_PCMCIA \
#	CONFIG_CARDBUS
  FILES:= \
	$(LINUX_DIR)/drivers/pcmcia/pcmcia_core.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/pcmcia/ds.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/pcmcia/yenta_socket.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,pcmcia_core yenta_socket ds)
endef

define KernelPackage/pcmcia-core/2.6
#  KCONFIG:= \
#	CONFIG_PCCARD \
#	CONFIG_PCMCIA \
#	CONFIG_YENTA \
#	CONFIG_PCCARD_NONSTATIC \
#	PCMCIA_DEBUG=n
  FILES:= \
	$(LINUX_DIR)/drivers/pcmcia/pcmcia_core.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/pcmcia/pcmcia.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/pcmcia/rsrc_nonstatic.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/pcmcia/yenta_socket.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,pcmcia_core pcmcia rsrc_nonstatic yenta_socket)
endef

define KernelPackage/pcmcia-core/au1000-2.6
  FILES:= \
	$(LINUX_DIR)/drivers/pcmcia/pcmcia_core.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/pcmcia/pcmcia.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/pcmcia/rsrc_nonstatic.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/pcmcia/au1x00_ss.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,pcmcia_core pcmcia rsrc_nonstatic au1x00_ss)
endef

define KernelPackage/pcmcia-core/description
 Kernel support for PCMCIA/CardBus controllers
endef

$(eval $(call KernelPackage,pcmcia-core))


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


# XXX: remove @!TARGET_* later when we have USB support properly detected on all targets
define KernelPackage/bluetooth
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Bluetooth support
  DEPENDS:=@USB_SUPPORT @!TARGET_atheros @!TARGET_uml
  KCONFIG:= \
	CONFIG_BLUEZ \
	CONFIG_BLUEZ_L2CAP \
	CONFIG_BLUEZ_SCO \
	CONFIG_BLUEZ_RFCOMM \
	CONFIG_BLUEZ_BNEP \
	CONFIG_BLUEZ_HCIUART \
	CONFIG_BLUEZ_HCIUSB \
	CONFIG_BT \
	CONFIG_BT_L2CAP \
	CONFIG_BT_SCO \
	CONFIG_BT_RFCOMM \
	CONFIG_BT_BNEP \
	CONFIG_BT_HCIUSB \
	CONFIG_BT_HCIUART
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
  FILES:= \
	$(LINUX_DIR)/net/bluetooth/bluetooth.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/l2cap.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/sco.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/rfcomm/rfcomm.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/bnep/bnep.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/bluetooth/hci_uart.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/bluetooth/hci_usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,bluetooth l2cap sco rfcomm bnep hci_uart hci_usb)
endef

define KernelPackage/bluetooth/description
 Kernel support for Bluetooth devices
endef

$(eval $(call KernelPackage,bluetooth))


define KernelPackage/mmc
  SUBMENU:=$(OTHER_MENU)
  TITLE:=MMC/SD Card Support
  DEPENDS:=@TARGET_at91
  KCONFIG:= \
	CONFIG_MMC \
	CONFIG_MMC_BLOCK \
	CONFIG_MMC_AT91
  FILES:= \
	$(LINUX_DIR)/drivers/mmc/mmc_core.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/mmc/mmc_block.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/mmc/at91_mci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,mmc_core mmc_block at91_mci)
endef

define KernelPackage/mmc/description
 Kernel support for MMC/SD cards
endef

$(eval $(call KernelPackage,mmc))


define KernelPackage/softdog
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Software watchdog driver
  KCONFIG:=CONFIG_SOFT_WATCHDOG
  AUTOLOAD:=$(call AutoLoad,50,softdog)
endef

define KernelPackage/softdog/2.4
  FILES:=$(LINUX_DIR)/drivers/char/softdog.o
endef

define KernelPackage/softdog/2.6
  FILES:=$(LINUX_DIR)/drivers/char/watchdog/softdog.ko
endef

define KernelPackage/softdog/description
 Software watchdog driver
endef

$(eval $(call KernelPackage,softdog))


define KernelPackage/leds-adm5120
  SUBMENU:=$(OTHER_MENU)
  TITLE:=ADM5120 LED support
  DEPENDS:=@TARGET_adm5120
  KCONFIG:=CONFIG_LEDS_ADM5120
  FILES:=$(LINUX_DIR)/drivers/leds/leds-adm5120.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,leds-adm5120)
endef

define KernelPackage/leds-adm5120/description
 Kernel module for LEDs on ADM5120 based boards
endef

$(eval $(call KernelPackage,leds-adm5120))


define KernelPackage/leds-net48xx
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Soekris Net48xx LED support
  DEPENDS:=@TARGET_x86
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
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_LEDS_WRAP
  FILES:=$(LINUX_DIR)/drivers/leds/leds-wrap.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,leds-wrap)
endef

define KernelPackage/leds-wrap/description
 Kernel module for PCengines WRAP LEDs
endef

$(eval $(call KernelPackage,leds-wrap))


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
  DEPENDS:=@TARGET_x86 kmod-nsc-gpio
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
  KCONFIG:=CONFIG_SC1200_WDT
  FILES:=$(LINUX_DIR)/drivers/char/watchdog/scx200_wdt.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,scx200_wdt)
endef

define KernelPackage/scx200-wdt/description
 Kernel module for SCX200 Watchdog
endef

$(eval $(call KernelPackage,scx200-wdt))


define KernelPackage/input-core
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Input device core
  DEPENDS:=@LINUX_2_6
  KCONFIG:=CONFIG_INPUT
  FILES:=$(LINUX_DIR)/drivers/input/input-core.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,input-core)
endef

define KernelPackage/input-core/description
 Kernel modules for support of input device
endef

$(eval $(call KernelPackage,input-core))


define KernelPackage/input-evdev
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Input even device
  DEPENDS:=+kmod-input-core
  KCONFIG:=CONFIG_INPUT_EVDEV
  FILES:=$(LINUX_DIR)/drivers/input/evdev.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,evdev)
endef

define KernelPackage/input-evdev/description
 Kernel modules for support of input device events
endef

$(eval $(call KernelPackage,input-evdev))
