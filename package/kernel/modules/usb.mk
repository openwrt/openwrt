# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

USBMENU:=USB Support

# This is 2.6.22 specific
#ifeq ($(KERNEL),2.4)
	USBNET_DIR=usb/net
#else
#	USBNET_DIR=net/usb
#endif

define usbdep
  SUBMENU:=$(USBMENU)
  DEPENDS:=kmod-usb-core $(1)
endef

define KernelPackage/usb-core
  TITLE:=Support for USB
  DESCRIPTION:=Kernel support for USB
  DEPENDS:=@USB_SUPPORT
  KCONFIG:=$(CONFIG_USB)
  SUBMENU:=$(USBMENU)
  AUTOLOAD:=$(call AutoLoad,20,usbcore)
endef

define KernelPackage/usb-core/2.4
  FILES:=$(LINUX_DIR)/drivers/usb/usbcore.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/usb-core/2.6
  FILES:=$(LINUX_DIR)/drivers/usb/core/usbcore.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,usb-core))


define KernelPackage/usb-uhci
  $(call usbdep,)
  TITLE:=Support for UHCI controllers
  DESCRIPTION:=Kernel support for USB UHCI controllers
endef

define KernelPackage/usb-uhci/2.4
  FILES:=$(LINUX_DIR)/drivers/usb/host/uhci.o
  KCONFIG:=$(CONFIG_USB_UHCI_ALT)
  AUTOLOAD:=$(call AutoLoad,50,uhci)
endef

define KernelPackage/usb-uhci/2.6
  FILES:=$(LINUX_DIR)/drivers/usb/host/uhci-hcd.ko
  KCONFIG:=$(CONFIG_USB_UHCI_HCD)
  AUTOLOAD:=$(call AutoLoad,50,uhci-hcd)
endef
$(eval $(call KernelPackage,usb-uhci))

define KernelPackage/usb-uhci-iv
  $(call usbdep,@LINUX_2_4)
  TITLE:=Support for Intel/VIA UHCI controllers 
  DESCRIPTION:=Kernel support for Intel/VIA USB UHCI controllers
endef 

define KernelPackage/usb-uhci-iv/2.4
  FILES:=$(LINUX_DIR)/drivers/usb/host/usb-uhci.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=$(CONFIG_USB_UHCI)
  AUTOLOAD:=$(call AutoLoad,50,usb-uhci) 
endef 
$(eval $(call KernelPackage,usb-uhci-iv)) 

define KernelPackage/usb-ohci
  $(call usbdep,)
  TITLE:=Support for OHCI controllers
  DESCRIPTION:=Kernel support for USB OHCI controllers
endef

define KernelPackage/usb-ohci/2.4
  FILES:=$(LINUX_DIR)/drivers/usb/host/usb-ohci.o
  KCONFIG:=$(CONFIG_USB_OHCI)
  AUTOLOAD:=$(call AutoLoad,50,usb-ohci)
endef

define KernelPackage/usb-ohci/2.6
  FILES:=$(LINUX_DIR)/drivers/usb/host/ohci-hcd.ko
  KCONFIG:=$(CONFIG_USB_OHCI_HCD)
  AUTOLOAD:=$(call AutoLoad,50,ohci-hcd)
endef
$(eval $(call KernelPackage,usb-ohci))

define KernelPackage/usb2
  $(call usbdep,)
  TITLE:=Support for USB2 controllers
  DESCRIPTION:=Kernel support for USB2 (EHCI) controllers
  FILES:=$(LINUX_DIR)/drivers/usb/host/ehci-hcd.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=$(CONFIG_USB_EHCI_HCD)
  AUTOLOAD:=$(call AutoLoad,50,ehci-hcd)
endef
$(eval $(call KernelPackage,usb2))


define KernelPackage/usb-acm
  $(call usbdep,)
  TITLE:=Support for modems/isdn controllers
  DESCRIPTION:=Kernel support for USB ACM devices (modems/isdn controllers)
  KCONFIG:=$(CONFIG_USB_ACM)
endef

define KernelPackage/usb-acm/2.4
  FILES:=$(LINUX_DIR)/drivers/usb/acm.o
  AUTOLOAD:=$(call AutoLoad,60,acm)
endef

define KernelPackage/usb-acm/2.6
  FILES:=$(LINUX_DIR)/drivers/usb/class/cdc-acm.ko
  AUTOLOAD:=$(call AutoLoad,60,cdc-acm)
endef
$(eval $(call KernelPackage,usb-acm))


define KernelPackage/usb-audio
  $(call usbdep,+kmod-soundcore)
  TITLE:=Support for audio devices
  DESCRIPTION:=Kernel support for USB audio devices
endef

define KernelPackage/usb-audio/2.4
  KCONFIG:=$(CONFIG_USB_AUDIO)
  FILES:=$(LINUX_DIR)/drivers/usb/audio.o
  AUTOLOAD:=$(call AutoLoad,60,audio)
endef

define KernelPackage/usb-audio/2.6
  KCONFIG:=$(CONFIG_SND_USB_AUDIO)
  FILES:= \
	$(LINUX_DIR)/sound/usb/snd-usb-lib.ko \
	$(LINUX_DIR)/sound/usb/snd-usb-audio.ko
  AUTOLOAD:=$(call AutoLoad,60,snd-usb-lib snd-usb-audio)
endef
$(eval $(call KernelPackage,usb-audio))


define KernelPackage/usb-printer
  $(call usbdep,)
  TITLE:=Support for printers
  DESCRIPTION:=Kernel support for USB printers
  KCONFIG:=$(CONFIG_USB_PRINTER)
endef

define KernelPackage/usb-printer/2.4
  FILES:=$(LINUX_DIR)/drivers/usb/printer.o
  AUTOLOAD:=$(call AutoLoad,60,printer)
endef

define KernelPackage/usb-printer/2.6
  FILES:=$(LINUX_DIR)/drivers/usb/class/usblp.ko
  AUTOLOAD:=$(call AutoLoad,60,usblp)
endef
$(eval $(call KernelPackage,usb-printer))


define KernelPackage/usb-serial
  $(call usbdep,)
  TITLE:=Support for USB-to-Serial converters
  DESCRIPTION:=Kernel support for USB-to-Serial converters
  KCONFIG:=$(CONFIG_USB_SERIAL)
  FILES:=$(LINUX_DIR)/drivers/usb/serial/usbserial.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,usbserial)
endef
$(eval $(call KernelPackage,usb-serial))

define KernelPackage/usb-serial-airprime
  $(call usbdep,kmod-usb-serial)
  TITLE:=Support for Airprime (EVDO) 
  DESCRIPTION:=Kernel support for Airprime (EVDO) 
  KCONFIG:=$(CONFIG_USB_SERIAL_AIRPRIME)
  FILES:=$(LINUX_DIR)/drivers/usb/serial/airprime.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,65,airprime)
endef
$(eval $(call KernelPackage,usb-serial-airprime))

define KernelPackage/usb-serial-belkin
  $(call usbdep,kmod-usb-serial)
  TITLE:=Support for Belkin devices
  DESCRIPTION:=Kernel support for Belkin USB-to-Serial converters
  KCONFIG:=$(CONFIG_USB_SERIAL_BELKIN)
  FILES:=$(LINUX_DIR)/drivers/usb/serial/belkin_sa.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,65,belkin_sa)
endef
$(eval $(call KernelPackage,usb-serial-belkin))


define KernelPackage/usb-serial-ftdi
  $(call usbdep,kmod-usb-serial)
  TITLE:=Support for FTDI devices
  DESCRIPTION:=Kernel support for FTDI USB-to-Serial converters
  KCONFIG:=$(CONFIG_USB_SERIAL_FTDI_SIO)
  FILES:=$(LINUX_DIR)/drivers/usb/serial/ftdi_sio.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,65,ftdi_sio)
endef
$(eval $(call KernelPackage,usb-serial-ftdi))


define KernelPackage/usb-serial-mct
  $(call usbdep,kmod-usb-serial)
  TITLE:=Support for Magic Control Tech. devices
  DESCRIPTION:=Kernel support for Magic Control Technology USB-to-Serial converters
  KCONFIG:=$(CONFIG_USB_SERIAL_MCT_U232)
  FILES:=$(LINUX_DIR)/drivers/usb/serial/mct_u232.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,65,mct_u232)
endef
$(eval $(call KernelPackage,usb-serial-mct))

define KernelPackage/usb-serial-pl2303
  $(call usbdep,kmod-usb-serial)
  TITLE:=Support for Prolific PL2303 devices
  DESCRIPTION:=Kernel support for Prolific PL2303 USB-to-Serial converters
  KCONFIG:=$(CONFIG_USB_SERIAL_PL2303)
  FILES:=$(LINUX_DIR)/drivers/usb/serial/pl2303.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,65,pl2303)
endef
$(eval $(call KernelPackage,usb-serial-pl2303))


define KernelPackage/usb-serial-sierrawireless
  $(call usbdep,kmod-usb-serial)
  TITLE:=Support for Sierra Wireless devices
  DESCRIPTION:=Kernel support for Sierra Wireless devices
  KCONFIG:=$(CONFIG_USB_SERIAL_SIERRAWIRELESS)
  FILES:=$(LINUX_DIR)/drivers/usb/serial/sierra.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,65,sierra)
endef
$(eval $(call KernelPackage,usb-serial-sierrawireless))


define KernelPackage/usb-serial-visor
  $(call usbdep,kmod-usb-serial)
  TITLE:=Support for Handspring Visor devices
  DESCRIPTION:=Kernel support for Handspring Visor PDAs
  KCONFIG:=$(CONFIG_USB_SERIAL_VISOR)
  FILES:=$(LINUX_DIR)/drivers/usb/serial/visor.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,65,visor)
endef
$(eval $(call KernelPackage,usb-serial-visor))


define KernelPackage/usb-storage
  $(call usbdep,+kmod-scsi-core)
  TITLE:=USB Storage support
  DESCRIPTION:=Kernel support for USB Mass Storage devices
  KCONFIG:=$(CONFIG_USB_STORAGE)
  FILES:= \
	$(LINUX_DIR)/drivers/usb/storage/usb-storage.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,scsi_mod sd_mod usb-storage)
endef
$(eval $(call KernelPackage,usb-storage))

define KernelPackage/usb-atm
  $(call usbdep,@LINUX_2_6 kmod-atm)
  TITLE:=Support for ATM on USB bus
  DESCRIPTION:=Kernel support for USB DSL modems
  KCONFIG:=$(CONFIG_USB_ATM)
  FILES:=$(LINUX_DIR)/drivers/usb/atm/usbatm.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,usbatm)
endef
$(eval $(call KernelPackage,usb-atm))

define KernelPackage/usb-speedtouch
  $(call usbdep,@LINUX_2_6 kmod-atm)
  TITLE:=Support for SpeedTouch ADSL modems
  DESCRIPTION:=Kernel support for SpeedTouch USB ADSL modems
  KCONFIG:=$(CONFIG_USB_SPEEDTOUCH)
  FILES:=$(LINUX_DIR)/drivers/usb/atm/speedtch.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,70,speedtch)
endef
$(eval $(call KernelPackage,usb-speedtouch))

define KernelPackage/ueagle-atm
  $(call usbdep,@LINUX_2_6 kmod-atm)
  TITLE:=Driver for Eagle 8051 based USB ADSL modems
  DESCRIPTION:=Kernel support for Eagle 8051 based USB ADSL modems
  FILES:=$(LINUX_DIR)/drivers/usb/atm/ueagle-atm.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=$(CONFIG_USB_UEAGLEATM)
  AUTOLOAD:=$(call AutoLoad,70,ueagle-atm)
endef
$(eval $(call KernelPackage,ueagle-atm))

define KernelPackage/usb-pwc
  $(call usbdep,+kmod-videodev)
  TITLE:=Philips WebCam driver
  DESCRIPTION:=Kernel modules for supporting Philips WebCam USB devices
  KCONFIG:=$(CONFIG_USB_PWC)
  AUTOLOAD:=$(call AutoLoad,70,pwc)
endef

define KernelPackage/usb-pwc/2.4
  FILES:=$(LINUX_DIR)/drivers/usb/pwc.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/usb-pwc/2.6
  FILES:=$(LINUX_DIR)/drivers/media/video/pwc/pwc.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,usb-pwc))

define KernelPackage/cpia2
  $(call usbdep,+kmod-videodev)
  TITLE:=CPIA2 video driver
  DESCRIPTION:=Kernel modules for CPIA2 WebCam devices
  KCONFIG:=$(CONFIG_VIDEO_CPIA2)
  AUTOLOAD:=$(call AutoLoad,70,cpia2)
endef

define KernelPackage/cpia2/2.4
  FILES:=$(LINUX_DIR)/drivers/usb/cpia2.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/cpia2/2.6
  FILES:=$(LINUX_DIR)/drivers/media/video/cpia2/cpia2.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,cpia2))

define KernelPackage/usb-net
  $(call usbdep,)
  TITLE:=Kernel modules for USB-to-Ethernet convertors
  DESCRIPTION:=Kernel modules for USB-to-Ethernet convertors
  KCONFIG:=$(CONFIG_USB_USBNET)
  AUTOLOAD:=$(call Autoload,60, usbnet)
endef

define KernelPackage/usb-net/2.4
  FILES:=$(LINUX_DIR)/drivers/usb/usbnet.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/usb-net/2.6
  FILES:=$(LINUX_DIR)/drivers/$(USBNET_DIR)/usbnet.$(LINUX_KMOD_SUFFIX)
endef

$(eval $(call KernelPackage,usb-net))

define KernelPackage/usb-net-asix
  $(call usbdep,kmod-usbnet)
  TITLE:=Kernel module for USB-to-Ethernet Asix convertors
  DESCRIPTION:=Kernel module for USB-to-Ethernet Asix convertors
  KCONFIG:=$(CONFIG_USB_NET_AX8817X)
endef

define KernelPackage/usb-net-asix/2.4
  FILES:=$(LINUX_DIR)/drivers/usb/ax8817x.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call Autoload,61,ax8817x)
endef

define KernelPackage/usb-net-asix/2.6
  FILES:=$(LINUX_DIR)/drivers/$(USBNET_DIR)/asix.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call Autoload,61,asix)
endef

$(eval $(call KernelPackage,usb-net-asix))

define KernelPackage/usb-net-kaweth
  $(call usbdep,kmod-usbnet)
  TITLE:=Kernel module for USB-to-Ethernet Kaweth convertors
  DESCRIPTION:=Kernel module for USB-to-Ethernet Kaweth convertors
  KCONFIG:=$(CONFIG_USB_KAWETH)
  FILES:=$(LINUX_DIR)/drivers/$(USBNET_DIR)/kaweth.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call Autoload,61,kaweth)
endef

$(eval $(call KernelPackage,usb-net-kaweth))

define KernelPackage/usb-net-pegasus
  $(call usbdep,kmod-usbnet)
  TITLE:=Kernel module for USB-to-Ethernet Pegasus convertors
  DESCRIPTION:=Kernel module for USB-to-Ethernet Pegasus convertors
  KCONFIG:=$(CONFIG_USB_PEGASUS)
  FILES:=$(LINUX_DIR)/drivers/$(USBNET_DIR)/pegasus.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call Autoload,61,pegasus)
endef

$(eval $(call KernelPackage,usb-net-pegasus))

define KernelPackage/usb-hid
  $(call usbdep,@LINUX_2_6 +kmod-input-core +kmod-input-evdev)
  TITLE:=Support for USB Human Input Devices
  DESCRIPTION:=Kernel support for USB HID devices such as keyboards and mice
  KCONFIG:=$(CONFIG_USB_HID)
  AUTOLOAD:=$(call AutoLoad,70,usbhid)
endef

define KernelPackage/usb-hid/2.6
  FILES:=$(LINUX_DIR)/drivers/usb/input/usbhid.ko
endef
$(eval $(call KernelPackage,usb-hid))

define KernelPackage/usb-yealink
  $(call usbdep,@LINUX_2_6 +kmod-input-core +kmod-input-evdev)
  TITLE:=USB Yealink VOIP phone
  DESCRIPTION:=Kernel support for Yealink VOIP phone
  KCONFIG:=$(CONFIG_USB_YEALINK)
  FILES:=$(LINUX_DIR)/drivers/usb/input/yealink.ko
  AUTOLOAD:=$(call AutoLoad,70,yealink)
endef
$(eval $(call KernelPackage,usb-yealink))


