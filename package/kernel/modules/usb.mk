# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

USBMENU:=USB Support

define KernelPackage/usb-core
  TITLE:=Support for USB
  DESCRIPTION:=Kernel support for USB
  DEPENDS:=@USB_SUPPORT
  KCONFIG:=$(CONFIG_USB)
  SUBMENU:=$(USBMENU)
  AUTOLOAD:=$(call AutoLoad,20,usbcore)
endef

define KernelPackage/usb-core/2.4
  FILES:=$(MODULES_DIR)/kernel/drivers/usb/usbcore.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/usb-core/2.6
  FILES:=$(MODULES_DIR)/kernel/drivers/usb/core/usbcore.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,usb-core))


define KernelPackage/usb-uhci
  TITLE:=Support for UHCI controllers
  DESCRIPTION:=Kernel support for USB UHCI controllers
  DEPENDS:=kmod-usb-core
  PROVIDES:=usb-controller
  SUBMENU:=$(USBMENU)
endef

define KernelPackage/usb-uhci/2.4
  FILES:=$(MODULES_DIR)/kernel/drivers/usb/host/uhci.o
  KCONFIG:=$(CONFIG_USB_UHCI_ALT)
  AUTOLOAD:=$(call AutoLoad,50,uhci)
endef

define KernelPackage/usb-uhci/2.6
  FILES:=$(MODULES_DIR)/kernel/drivers/usb/host/uhci-hcd.ko
  KCONFIG:=$(CONFIG_USB_UHCI_HCD)
  AUTOLOAD:=$(call AutoLoad,50,uhci-hcd)
endef
$(eval $(call KernelPackage,usb-uhci))


define KernelPackage/usb-ohci
  TITLE:=Support for OHCI controllers
  DESCRIPTION:=Kernel support for USB OHCI controllers
  DEPENDS:=kmod-usb-core
  PROVIDES:=usb-controller
  SUBMENU:=$(USBMENU)
endef

define KernelPackage/usb-ohci/2.4
  FILES:=$(MODULES_DIR)/kernel/drivers/usb/host/usb-ohci.o
  KCONFIG:=$(CONFIG_USB_OHCI)
  AUTOLOAD:=$(call AutoLoad,50,usb-ohci)
endef

define KernelPackage/usb-ohci/2.6
  FILES:=$(MODULES_DIR)/kernel/drivers/usb/host/ohci-hcd.ko
  KCONFIG:=$(CONFIG_USB_OHCI_HCD)
  AUTOLOAD:=$(call AutoLoad,50,ohci-hcd)
endef
$(eval $(call KernelPackage,usb-ohci))


define KernelPackage/usb2
  TITLE:=Support for USB2 controllers
  DESCRIPTION:=Kernel support for USB2 (EHCI) controllers
  DEPENDS:=kmod-usb-core
  PROVIDES:=usb-controller
  KCONFIG:=$(CONFIG_USB)
  SUBMENU:=$(USBMENU)
  FILES:=$(MODULES_DIR)/kernel/drivers/usb/host/ehci-hcd.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=$(CONFIG_USB_EHCI_HCD)
  AUTOLOAD:=$(call AutoLoad,50,ehci-hcd)
endef
$(eval $(call KernelPackage,usb2))


define usbdep
  SUBMENU:=$(USBMENU)
  DEPENDS:=kmod-usb-core usb-controller $(1)
endef


define KernelPackage/usb-acm
  $(call usbdep,)
  TITLE:=Support for modems/isdn controllers
  DESCRIPTION:=Kernel support for USB ACM devices (modems/isdn controllers)
  KCONFIG:=$(CONFIG_USB_ACM)
endef

define KernelPackage/usb-acm/2.4
  FILES:=$(MODULES_DIR)/kernel/drivers/usb/acm.o
  AUTOLOAD:=$(call AutoLoad,60,acm)
endef

define KernelPackage/usb-acm/2.6
  FILES:=$(MODULES_DIR)/kernel/drivers/usb/class/cdc-acm.ko
  AUTOLOAD:=$(call AutoLoad,60,cdc-acm)
endef
$(eval $(call KernelPackage,usb-acm))


define KernelPackage/usb-audio
  $(call usbdep,+kmod-soundcore)
  TITLE:=Support for audio devices
  DESCRIPTION:=Kernel support for USB audio devices
  KCONFIG:=$(CONFIG_USB_AUDIO)
  AUTOLOAD:=$(call AutoLoad,60,audio)
endef

define KernelPackage/usb-audio/2.4
  FILES:=$(MODULES_DIR)/kernel/drivers/usb/audio.o
endef

define KernelPackage/usb-audio/2.6
  FILES:=$(MODULES_DIR)/kernel/drivers/usb/class/audio.ko
endef
$(eval $(call KernelPackage,usb-audio))


define KernelPackage/usb-printer
  $(call usbdep,)
  TITLE:=Support for printers
  DESCRIPTION:=Kernel support for USB printers
  KCONFIG:=$(CONFIG_USB_PRINTER)
endef

define KernelPackage/usb-printer/2.4
  FILES:=$(MODULES_DIR)/kernel/drivers/usb/printer.o
  AUTOLOAD:=$(call AutoLoad,60,printer)
endef

define KernelPackage/usb-printer/2.6
  FILES:=$(MODULES_DIR)/kernel/drivers/usb/class/usblp.ko
  AUTOLOAD:=$(call AutoLoad,60,usblp)
endef
$(eval $(call KernelPackage,usb-printer))


define KernelPackage/usb-serial
  $(call usbdep,)
  TITLE:=Support for USB-to-Serial converters
  DESCRIPTION:=Kernel support for USB-to-Serial converters
  KCONFIG:=$(CONFIG_USB_SERIAL)
  FILES:=$(MODULES_DIR)/kernel/drivers/usb/serial/usbserial.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,usbserial)
endef
$(eval $(call KernelPackage,usb-serial))


define KernelPackage/usb-serial-belkin
  TITLE:=Support for Belkin devices
  DESCRIPTION:=Kernel support for Belkin USB-to-Serial converters
  DEPENDS:=kmod-usb-serial
  SUBMENU:=$(USBMENU)
  KCONFIG:=$(CONFIG_USB_SERIAL_BELKIN)
  FILES:=$(MODULES_DIR)/kernel/drivers/usb/serial/belkin_sa.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,65,belkin_sa)
endef
$(eval $(call KernelPackage,usb-serial-belkin))


define KernelPackage/usb-serial-ftdi
  TITLE:=Support for FTDI devices
  DESCRIPTION:=Kernel support for FTDI USB-to-Serial converters
  DEPENDS:=kmod-usb-serial
  SUBMENU:=$(USBMENU)
  KCONFIG:=$(CONFIG_USB_SERIAL_FTDI_SIO)
  FILES:=$(MODULES_DIR)/kernel/drivers/usb/serial/ftdi_sio.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,65,ftdi_sio)
endef
$(eval $(call KernelPackage,usb-serial-ftdi))


define KernelPackage/usb-serial-mct
  TITLE:=Support for Magic Control Tech. devices
  DESCRIPTION:=Kernel support for Magic Control Technology USB-to-Serial converters
  DEPENDS:=kmod-usb-serial
  SUBMENU:=$(USBMENU)
  KCONFIG:=$(CONFIG_USB_SERIAL_MCT_U232)
  FILES:=$(MODULES_DIR)/kernel/drivers/usb/serial/mct_u232.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,65,mct_u232)
endef
$(eval $(call KernelPackage,usb-serial-mct))

define KernelPackage/usb-serial-pl2303
  TITLE:=Support for Prolific PL2303 devices
  DESCRIPTION:=Kernel support for Prolific PL2303 USB-to-Serial converters
  DEPENDS:=kmod-usb-serial
  SUBMENU:=$(USBMENU)
  KCONFIG:=$(CONFIG_USB_SERIAL_PL2303)
  FILES:=$(MODULES_DIR)/kernel/drivers/usb/serial/pl2303.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,65,pl2303)
endef
$(eval $(call KernelPackage,usb-serial-pl2303))


define KernelPackage/usb-serial-visor
  TITLE:=Support for Handspring Visor devices
  DESCRIPTION:=Kernel support for Handspring Visor PDAs
  DEPENDS:=kmod-usb-serial
  SUBMENU:=$(USBMENU)
  KCONFIG:=$(CONFIG_USB_SERIAL_VISOR)
  FILES:=$(MODULES_DIR)/kernel/drivers/usb/serial/visor.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,65,visor)
endef
$(eval $(call KernelPackage,usb-serial-visor))


define KernelPackage/usb-storage
  $(call usbdep,)
  TITLE:=USB Storage support
  DESCRIPTION:=Kernel support for USB Mass Storage devices
  KCONFIG:=$(CONFIG_USB_STORAGE)
  FILES:= \
	$(MODULES_DIR)/kernel/drivers/scsi/scsi_mod.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/scsi/sd_mod.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/usb/storage/usb-storage.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,scsi_mod sd_mod usb-storage)
endef
$(eval $(call KernelPackage,usb-storage))



define KernelPackage/usb-speedtouch
  $(call usbdep,@LINUX_2_6 kmod-atm)
  TITLE:=Support for SpeedTouch ADSL modems
  DESCRIPTION:=Kernel support for SpeedTouch USB ADSL modems
  KCONFIG:=$(CONFIG_USB_SPEEDTOUCH)
  FILES:= \
	$(MODULES_DIR)/kernel/drivers/usb/atm/usbatm.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/usb/atm/speedtch.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,70,usbatm speedtch)
endef
$(eval $(call KernelPackage,usb-speedtouch))


define KernelPackage/usb-pwc
  $(call usbdep,+kmod-videodev)
  TITLE:=Philips WebCam driver
  DESCRIPTION:=Kernel modules for supporting Philips WebCam USB devices
  KCONFIG:=$(CONFIG_USB_PWC)
  AUTOLOAD:=$(call AutoLoad,70,pwc)
endef

define KernelPackage/usb-pwc/2.4
  FILES:=$(MODULES_DIR)/kernel/drivers/usb/pwc.$(LINUX_KMOD_SUFFIX)
endef

$(eval $(call KernelPackage,usb-pwc))




