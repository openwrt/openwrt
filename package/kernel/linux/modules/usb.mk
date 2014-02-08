#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

USB_MENU:=USB Support

USBNET_DIR:=net/usb
USBHID_DIR?=hid/usbhid
USBINPUT_DIR?=input/misc

define KernelPackage/usb-core
  SUBMENU:=$(USB_MENU)
  TITLE:=Support for USB
  DEPENDS:=@USB_SUPPORT
  KCONFIG:=CONFIG_USB CONFIG_XPS_USB_HCD_XILINX=n CONFIG_USB_FHCI_HCD=n
  FILES:= \
	$(LINUX_DIR)/drivers/usb/core/usbcore.ko \
	$(LINUX_DIR)/drivers/usb/usb-common.ko
  AUTOLOAD:=$(call AutoLoad,20,usb-common usbcore,1)
  $(call AddDepends/nls)
endef

define KernelPackage/usb-core/description
 Kernel support for USB
endef

$(eval $(call KernelPackage,usb-core))


define AddDepends/usb
  SUBMENU:=$(USB_MENU)
  DEPENDS+=+kmod-usb-core $(1)
endef


define KernelPackage/usb-gadget
  TITLE:=USB Gadget support
  KCONFIG:=CONFIG_USB_GADGET
  FILES:=$(LINUX_DIR)/drivers/usb/gadget/udc-core.ko
  AUTOLOAD:=$(call AutoLoad,45,udc-core)
  DEPENDS:=@USB_GADGET_SUPPORT
  $(call AddDepends/usb)
endef

define KernelPackage/usb-gadget/description
 Kernel support for USB Gadget mode
endef

$(eval $(call KernelPackage,usb-gadget))

define KernelPackage/usb-lib-composite
  TITLE:=USB lib composite
  KCONFIG:=CONFIG_USB_LIBCOMPOSITE
  DEPENDS:=+kmod-usb-gadget +kmod-fs-configfs @!LINUX_3_3 @!LINUX_3_6
  FILES:=$(LINUX_DIR)/drivers/usb/gadget/libcomposite.ko
  AUTOLOAD:=$(call AutoLoad,50,libcomposite)
  $(call AddDepends/usb)
endef

define KernelPackage/usb-lib-composite/description
 Lib Composite
endef

$(eval $(call KernelPackage,usb-lib-composite))


define KernelPackage/usb-eth-gadget
  TITLE:=USB Ethernet Gadget support
  KCONFIG:= \
	CONFIG_USB_ETH \
	CONFIG_USB_ETH_RNDIS=y \
	CONFIG_USB_ETH_EEM=y
  DEPENDS:=+kmod-usb-gadget +(!LINUX_3_3&&!LINUX_3_6):kmod-usb-lib-composite
ifneq ($(wildcard $(LINUX_DIR)/drivers/usb/gadget/u_ether.ko),)
  FILES:= \
	$(LINUX_DIR)/drivers/usb/gadget/u_ether.ko \
	$(LINUX_DIR)/drivers/usb/gadget/u_rndis.ko \
	$(LINUX_DIR)/drivers/usb/gadget/usb_f_rndis.ko \
	$(LINUX_DIR)/drivers/usb/gadget/usb_f_eem.ko \
	$(LINUX_DIR)/drivers/usb/gadget/g_ether.ko
  AUTOLOAD:=$(call AutoLoad,52,g_ether)
else
  FILES:=$(LINUX_DIR)/drivers/usb/gadget/g_ether.ko
  AUTOLOAD:=$(call AutoLoad,52,g_ether)
endif
  $(call AddDepends/usb)
endef

define KernelPackage/usb-eth-gadget/description
 Kernel support for USB Ethernet Gadget
endef

$(eval $(call KernelPackage,usb-eth-gadget))


define KernelPackage/usb-uhci
  TITLE:=Support for UHCI controllers
  KCONFIG:= \
	CONFIG_USB_UHCI_ALT \
	CONFIG_USB_UHCI_HCD
  FILES:=$(LINUX_DIR)/drivers/usb/host/uhci-hcd.ko
  AUTOLOAD:=$(call AutoLoad,50,uhci-hcd,1)
  $(call AddDepends/usb)
endef

define KernelPackage/usb-uhci/description
 Kernel support for USB UHCI controllers
endef

$(eval $(call KernelPackage,usb-uhci,1))


define KernelPackage/usb-ohci
  TITLE:=Support for OHCI controllers
  DEPENDS:=+TARGET_brcm47xx:kmod-usb-brcm47xx
  KCONFIG:= \
	CONFIG_USB_OHCI \
	CONFIG_USB_OHCI_HCD \
	CONFIG_USB_OHCI_ATH79=y \
	CONFIG_USB_OHCI_BCM63XX=y \
	CONFIG_USB_OCTEON_OHCI=y \
	CONFIG_USB_OHCI_HCD_OMAP3=y \
	CONFIG_USB_OHCI_HCD_PLATFORM=y
  FILES:=$(LINUX_DIR)/drivers/usb/host/ohci-hcd.ko
  AUTOLOAD:=$(call AutoLoad,50,ohci-hcd,1)
  $(call AddDepends/usb)
endef

define KernelPackage/usb-ohci/description
 Kernel support for USB OHCI controllers
endef

$(eval $(call KernelPackage,usb-ohci,1))


define KernelPackage/usb2-fsl
  TITLE:=Support for Freescale USB2 controllers
  DEPENDS:=@TARGET_mpc85xx
  KCONFIG:=\
	CONFIG_USB_FSL_MPH_DR_OF \
  	CONFIG_USB_EHCI_FSL=y
  FILES:=$(LINUX_DIR)/drivers/usb/host/fsl-mph-dr-of.ko
  AUTOLOAD:=$(call AutoLoad,39,fsl-mph-dr-of,1)
  $(call AddDepends/usb)
endef

define KernelPackage/usb2-fsl/description
 Kernel support for Freescale USB2 (EHCI) controllers
endef

$(eval $(call KernelPackage,usb2-fsl))


define KernelPackage/usb2
  TITLE:=Support for USB2 controllers
  DEPENDS:=\
	+TARGET_brcm47xx:kmod-usb-brcm47xx \
	+TARGET_mpc85xx:kmod-usb2-fsl
  KCONFIG:=\
	CONFIG_USB_EHCI_HCD \
	CONFIG_USB_EHCI_ATH79=y \
	CONFIG_USB_EHCI_BCM63XX=y \
	CONFIG_USB_IMX21_HCD=y \
	CONFIG_USB_EHCI_MXC=y \
	CONFIG_USB_OCTEON_EHCI=y \
	CONFIG_USB_EHCI_HCD_ORION=y \
	CONFIG_USB_EHCI_HCD_PLATFORM=y
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,3.8.0)),1)
  FILES:= \
	$(LINUX_DIR)/drivers/usb/host/ehci-hcd.ko \
	$(LINUX_DIR)/drivers/usb/host/ehci-platform.ko
  AUTOLOAD:=$(call AutoLoad,40,ehci-hcd ehci-platform,1)
else
  FILES:=$(LINUX_DIR)/drivers/usb/host/ehci-hcd.ko
  AUTOLOAD:=$(call AutoLoad,40,ehci-hcd,1)
endif
  $(call AddDepends/usb)
endef

define KernelPackage/usb2/description
 Kernel support for USB2 (EHCI) controllers
endef

$(eval $(call KernelPackage,usb2))


define KernelPackage/usb2-pci
  TITLE:=Support for PCI USB2 controllers
  DEPENDS:=@PCI_SUPPORT @(!LINUX_3_3&&!LINUX_3_6) +kmod-usb2
  KCONFIG:=CONFIG_USB_EHCI_PCI
  FILES:=$(LINUX_DIR)/drivers/usb/host/ehci-pci.ko
  AUTOLOAD:=$(call AutoLoad,42,ehci-pci,1)
  $(call AddDepends/usb)
endef

define KernelPackage/usb2-pci/description
 Kernel support for PCI USB2 (EHCI) controllers
endef

$(eval $(call KernelPackage,usb2-pci))


define KernelPackage/usb-dwc2
  TITLE:=DWC2 USB controller driver
  DEPENDS:=@(!LINUX_3_3&&!LINUX_3_6&&!LINUX_3_8&&!LINUX_3_9)
  KCONFIG:= \
	CONFIG_USB_DWC2 \
	CONFIG_USB_DWC2_DEBUG=n \
	CONFIG_USB_DWC2_VERBOSE=n \
	CONFIG_USB_DWC2_TRACK_MISSED_SOFS=n
  FILES:= \
	$(LINUX_DIR)/drivers/staging/dwc2/dwc2.ko \
	$(LINUX_DIR)/drivers/staging/dwc2/dwc2_platform.ko
  AUTOLOAD:=$(call AutoLoad,54,dwc2 dwc2_platform,1)
  $(call AddDepends/usb)
endef

define KernelPackage/usb-dwc2/description
 This driver provides USB Device Controller support for the
 Synopsys DesignWare USB OTG Core
endef

$(eval $(call KernelPackage,usb-dwc2))


define KernelPackage/usb-acm
  TITLE:=Support for modems/isdn controllers
  KCONFIG:=CONFIG_USB_ACM
  FILES:=$(LINUX_DIR)/drivers/usb/class/cdc-acm.ko
  AUTOLOAD:=$(call AutoProbe,cdc-acm)
$(call AddDepends/usb)
endef

define KernelPackage/usb-acm/description
 Kernel support for USB ACM devices (modems/isdn controllers)
endef

$(eval $(call KernelPackage,usb-acm))


define KernelPackage/usb-wdm
  TITLE:=USB Wireless Device Management
  KCONFIG:=CONFIG_USB_WDM
  FILES:=$(LINUX_DIR)/drivers/usb/class/cdc-wdm.ko
  AUTOLOAD:=$(call AutoProbe,cdc-wdm)
$(call AddDepends/usb)
$(call AddDepends/usb-net)
endef

define KernelPackage/usb-wdm/description
 USB Wireless Device Management support
endef

$(eval $(call KernelPackage,usb-wdm))


define KernelPackage/usb-audio
  TITLE:=Support for USB audio devices
  KCONFIG:= \
	CONFIG_USB_AUDIO \
	CONFIG_SND_USB_AUDIO
  $(call AddDepends/usb)
  $(call AddDepends/sound)
# For Linux 2.6.35+
ifneq ($(wildcard $(LINUX_DIR)/sound/usb/snd-usbmidi-lib.ko),)
  FILES:= \
	$(LINUX_DIR)/sound/usb/snd-usbmidi-lib.ko \
	$(LINUX_DIR)/sound/usb/snd-usb-audio.ko
  AUTOLOAD:=$(call AutoProbe,snd-usbmidi-lib snd-usb-audio)
else
  FILES:= \
	$(LINUX_DIR)/sound/usb/snd-usb-lib.ko \
	$(LINUX_DIR)/sound/usb/snd-usb-audio.ko
  AUTOLOAD:=$(call AutoProbe,snd-usb-lib snd-usb-audio)
endif
endef

define KernelPackage/usb-audio/description
 Kernel support for USB audio devices
endef

$(eval $(call KernelPackage,usb-audio))


define KernelPackage/usb-printer
  TITLE:=Support for printers
  KCONFIG:=CONFIG_USB_PRINTER
  FILES:=$(LINUX_DIR)/drivers/usb/class/usblp.ko
  AUTOLOAD:=$(call AutoProbe,usblp)
  $(call AddDepends/usb)
endef

define KernelPackage/usb-printer/description
 Kernel support for USB printers
endef

$(eval $(call KernelPackage,usb-printer))


define KernelPackage/usb-serial
  TITLE:=Support for USB-to-Serial converters
  KCONFIG:=CONFIG_USB_SERIAL
  FILES:=$(LINUX_DIR)/drivers/usb/serial/usbserial.ko
  AUTOLOAD:=$(call AutoProbe,usbserial)
  $(call AddDepends/usb)
endef

define KernelPackage/usb-serial/description
 Kernel support for USB-to-Serial converters
endef

$(eval $(call KernelPackage,usb-serial))


define AddDepends/usb-serial
  SUBMENU:=$(USB_MENU)
  DEPENDS+=kmod-usb-serial $(1)
endef


define KernelPackage/usb-serial-belkin
  TITLE:=Support for Belkin devices
  KCONFIG:=CONFIG_USB_SERIAL_BELKIN
  FILES:=$(LINUX_DIR)/drivers/usb/serial/belkin_sa.ko
  AUTOLOAD:=$(call AutoProbe,belkin_sa)
  $(call AddDepends/usb-serial)
endef

define KernelPackage/usb-serial-belkin/description
 Kernel support for Belkin USB-to-Serial converters
endef

$(eval $(call KernelPackage,usb-serial-belkin))


define KernelPackage/usb-serial-ch341
  TITLE:=Support for CH341 devices
  KCONFIG:=CONFIG_USB_SERIAL_CH341
  FILES:=$(LINUX_DIR)/drivers/usb/serial/ch341.ko
  AUTOLOAD:=$(call AutoProbe,ch341)
  $(call AddDepends/usb-serial)
endef

define KernelPackage/usb-serial-ch341/description
 Kernel support for Winchiphead CH341 USB-to-Serial converters
endef

$(eval $(call KernelPackage,usb-serial-ch341))


define KernelPackage/usb-serial-ftdi
  TITLE:=Support for FTDI devices
  KCONFIG:=CONFIG_USB_SERIAL_FTDI_SIO
  FILES:=$(LINUX_DIR)/drivers/usb/serial/ftdi_sio.ko
  AUTOLOAD:=$(call AutoProbe,ftdi_sio)
  $(call AddDepends/usb-serial)
endef

define KernelPackage/usb-serial-ftdi/description
 Kernel support for FTDI USB-to-Serial converters
endef

$(eval $(call KernelPackage,usb-serial-ftdi))


define KernelPackage/usb-serial-ti-usb
  TITLE:=Support for TI USB 3410/5052
  KCONFIG:=CONFIG_USB_SERIAL_TI
  FILES:=$(LINUX_DIR)/drivers/usb/serial/ti_usb_3410_5052.ko
  AUTOLOAD:=$(call AutoProbe,ti_usb_3410_5052)
  $(call AddDepends/usb-serial)
endef

define KernelPackage/usb-serial-ti-usb/description
 Kernel support for TI USB 3410/5052 devices
endef

$(eval $(call KernelPackage,usb-serial-ti-usb))


define KernelPackage/usb-serial-ipw
  TITLE:=Support for IPWireless 3G devices
  KCONFIG:=CONFIG_USB_SERIAL_IPW
  FILES:=$(LINUX_DIR)/drivers/usb/serial/ipw.ko
  AUTOLOAD:=$(call AutoProbe,ipw)
  $(call AddDepends/usb-serial,+kmod-usb-serial-wwan)
endef

$(eval $(call KernelPackage,usb-serial-ipw))


define KernelPackage/usb-serial-mct
  TITLE:=Support for Magic Control Tech. devices
  KCONFIG:=CONFIG_USB_SERIAL_MCT_U232
  FILES:=$(LINUX_DIR)/drivers/usb/serial/mct_u232.ko
  AUTOLOAD:=$(call AutoProbe,mct_u232)
  $(call AddDepends/usb-serial)
endef

define KernelPackage/usb-serial-mct/description
 Kernel support for Magic Control Technology USB-to-Serial converters
endef

$(eval $(call KernelPackage,usb-serial-mct))


define KernelPackage/usb-serial-mos7720
  TITLE:=Support for Moschip MOS7720 devices
  KCONFIG:=CONFIG_USB_SERIAL_MOS7720
  FILES:=$(LINUX_DIR)/drivers/usb/serial/mos7720.ko
  AUTOLOAD:=$(call AutoProbe,mos7720)
  $(call AddDepends/usb-serial)
endef

define KernelPackage/usb-serial-mos7720/description
 Kernel support for Moschip MOS7720 USB-to-Serial converters
endef

$(eval $(call KernelPackage,usb-serial-mos7720))


define KernelPackage/usb-serial-pl2303
  TITLE:=Support for Prolific PL2303 devices
  KCONFIG:=CONFIG_USB_SERIAL_PL2303
  FILES:=$(LINUX_DIR)/drivers/usb/serial/pl2303.ko
  AUTOLOAD:=$(call AutoProbe,pl2303)
  $(call AddDepends/usb-serial)
endef

define KernelPackage/usb-serial-pl2303/description
 Kernel support for Prolific PL2303 USB-to-Serial converters
endef

$(eval $(call KernelPackage,usb-serial-pl2303))


define KernelPackage/usb-serial-cp210x
  TITLE:=Support for Silicon Labs cp210x devices
  KCONFIG:=CONFIG_USB_SERIAL_CP210X
  FILES:=$(LINUX_DIR)/drivers/usb/serial/cp210x.ko
  AUTOLOAD:=$(call AutoProbe,cp210x)
  $(call AddDepends/usb-serial)
endef

define KernelPackage/usb-serial-cp210x/description
 Kernel support for Silicon Labs cp210x USB-to-Serial converters
endef

$(eval $(call KernelPackage,usb-serial-cp210x))


define KernelPackage/usb-serial-ark3116
  TITLE:=Support for ArkMicroChips ARK3116 devices
  KCONFIG:=CONFIG_USB_SERIAL_ARK3116
  FILES:=$(LINUX_DIR)/drivers/usb/serial/ark3116.ko
  AUTOLOAD:=$(call AutoProbe,ark3116)
  $(call AddDepends/usb-serial)
endef

define KernelPackage/usb-serial-ark3116/description
 Kernel support for ArkMicroChips ARK3116 USB-to-Serial converters
endef

$(eval $(call KernelPackage,usb-serial-ark3116))


define KernelPackage/usb-serial-oti6858
  TITLE:=Support for Ours Technology OTI6858 devices
  KCONFIG:=CONFIG_USB_SERIAL_OTI6858
  FILES:=$(LINUX_DIR)/drivers/usb/serial/oti6858.ko
  AUTOLOAD:=$(call AutoProbe,oti6858)
  $(call AddDepends/usb-serial)
endef

define KernelPackage/usb-serial-oti6858/description
 Kernel support for Ours Technology OTI6858 USB-to-Serial converters
endef

$(eval $(call KernelPackage,usb-serial-oti6858))


define KernelPackage/usb-serial-sierrawireless
  TITLE:=Support for Sierra Wireless devices
  KCONFIG:=CONFIG_USB_SERIAL_SIERRAWIRELESS
  FILES:=$(LINUX_DIR)/drivers/usb/serial/sierra.ko
  AUTOLOAD:=$(call AutoProbe,sierra)
  $(call AddDepends/usb-serial)
endef

define KernelPackage/usb-serial-sierrawireless/description
 Kernel support for Sierra Wireless devices
endef

$(eval $(call KernelPackage,usb-serial-sierrawireless))


define KernelPackage/usb-serial-motorola-phone
  TITLE:=Support for Motorola usb phone
  KCONFIG:=CONFIG_USB_SERIAL_MOTOROLA
  FILES:=$(LINUX_DIR)/drivers/usb/serial/moto_modem.ko
  AUTOLOAD:=$(call AutoProbe,moto_modem)
  $(call AddDepends/usb-serial)
endef

define KernelPackage/usb-serial-motorola-phone/description
 Kernel support for Motorola usb phone
endef

$(eval $(call KernelPackage,usb-serial-motorola-phone))


define KernelPackage/usb-serial-visor
  TITLE:=Support for Handspring Visor devices
  KCONFIG:=CONFIG_USB_SERIAL_VISOR
  FILES:=$(LINUX_DIR)/drivers/usb/serial/visor.ko
  AUTOLOAD:=$(call AutoProbe,visor)
  $(call AddDepends/usb-serial)
endef

define KernelPackage/usb-serial-visor/description
 Kernel support for Handspring Visor PDAs
endef

$(eval $(call KernelPackage,usb-serial-visor))


define KernelPackage/usb-serial-cypress-m8
  TITLE:=Support for CypressM8 USB-Serial
  KCONFIG:=CONFIG_USB_SERIAL_CYPRESS_M8
  FILES:=$(LINUX_DIR)/drivers/usb/serial/cypress_m8.ko
  AUTOLOAD:=$(call AutoProbe,cypress_m8)
  $(call AddDepends/usb-serial)
endef

define KernelPackage/usb-serial-cypress-m8/description
 Kernel support for devices with Cypress M8 USB to Serial chip
 (for example, the Delorme Earthmate LT-20 GPS)
 Supported microcontrollers in the CY4601 family are:
 CY7C63741 CY7C63742 CY7C63743 CY7C64013
endef

$(eval $(call KernelPackage,usb-serial-cypress-m8))


define KernelPackage/usb-serial-keyspan
  TITLE:=Support for Keyspan USB-to-Serial devices
  KCONFIG:= \
	CONFIG_USB_SERIAL_KEYSPAN \
	CONFIG_USB_SERIAL_KEYSPAN_USA28 \
	CONFIG_USB_SERIAL_KEYSPAN_USA28X \
	CONFIG_USB_SERIAL_KEYSPAN_USA28XA \
	CONFIG_USB_SERIAL_KEYSPAN_USA28XB \
	CONFIG_USB_SERIAL_KEYSPAN_USA19 \
	CONFIG_USB_SERIAL_KEYSPAN_USA18X \
	CONFIG_USB_SERIAL_KEYSPAN_USA19W \
	CONFIG_USB_SERIAL_KEYSPAN_USA19QW \
	CONFIG_USB_SERIAL_KEYSPAN_USA19QI \
	CONFIG_USB_SERIAL_KEYSPAN_MPR \
	CONFIG_USB_SERIAL_KEYSPAN_USA49W \
	CONFIG_USB_SERIAL_KEYSPAN_USA49WLC
  FILES:= \
	$(LINUX_DIR)/drivers/usb/serial/keyspan.ko \
	$(wildcard $(LINUX_DIR)/drivers/usb/misc/ezusb.ko)
  AUTOLOAD:=$(call AutoProbe,ezusb keyspan)
  $(call AddDepends/usb-serial)
endef

define KernelPackage/usb-serial-keyspan/description
 Kernel support for Keyspan USB-to-Serial devices
endef

$(eval $(call KernelPackage,usb-serial-keyspan))


define KernelPackage/usb-serial-wwan
  TITLE:=Support for GSM and CDMA modems
  KCONFIG:=CONFIG_USB_SERIAL_WWAN
  FILES:=$(LINUX_DIR)/drivers/usb/serial/usb_wwan.ko
  AUTOLOAD:=$(call AutoProbe,usb_wwan)
  $(call AddDepends/usb-serial)
endef

define KernelPackage/usb-serial-wwan/description
 Kernel support for USB GSM and CDMA modems
endef

$(eval $(call KernelPackage,usb-serial-wwan))


define KernelPackage/usb-serial-option
  TITLE:=Support for Option HSDPA modems
  DEPENDS:=+kmod-usb-serial-wwan
  KCONFIG:=CONFIG_USB_SERIAL_OPTION
  FILES:=$(LINUX_DIR)/drivers/usb/serial/option.ko
  AUTOLOAD:=$(call AutoProbe,option)
  $(call AddDepends/usb-serial)
endef

define KernelPackage/usb-serial-option/description
 Kernel support for Option HSDPA modems
endef

$(eval $(call KernelPackage,usb-serial-option))


define KernelPackage/usb-serial-qualcomm
  TITLE:=Support for Qualcomm USB serial
  KCONFIG:=CONFIG_USB_SERIAL_QUALCOMM
  FILES:=$(LINUX_DIR)/drivers/usb/serial/qcserial.ko
  AUTOLOAD:=$(call AutoProbe,qcserial)
  $(call AddDepends/usb-serial,+kmod-usb-serial-wwan)
endef

define KernelPackage/usb-serial-qualcomm/description
 Kernel support for Qualcomm USB Serial devices (Gobi)
endef

$(eval $(call KernelPackage,usb-serial-qualcomm))


define KernelPackage/usb-storage
  TITLE:=USB Storage support
  DEPENDS:= +kmod-scsi-core
  KCONFIG:=CONFIG_USB_STORAGE
  FILES:=$(LINUX_DIR)/drivers/usb/storage/usb-storage.ko
  AUTOLOAD:=$(call AutoProbe,usb-storage,1)
  $(call AddDepends/usb)
endef

define KernelPackage/usb-storage/description
 Kernel support for USB Mass Storage devices
endef

$(eval $(call KernelPackage,usb-storage))


define KernelPackage/usb-storage-extras
  SUBMENU:=$(USB_MENU)
  TITLE:=Extra drivers for usb-storage
  DEPENDS:=+kmod-usb-storage
  KCONFIG:= \
	CONFIG_USB_STORAGE_ALAUDA \
	CONFIG_USB_STORAGE_CYPRESS_ATACB \
	CONFIG_USB_STORAGE_DATAFAB \
	CONFIG_USB_STORAGE_FREECOM \
	CONFIG_USB_STORAGE_ISD200 \
	CONFIG_USB_STORAGE_JUMPSHOT \
	CONFIG_USB_STORAGE_KARMA \
	CONFIG_USB_STORAGE_SDDR09 \
	CONFIG_USB_STORAGE_SDDR55 \
	CONFIG_USB_STORAGE_USBAT
  FILES:= \
	$(LINUX_DIR)/drivers/usb/storage/ums-alauda.ko \
	$(LINUX_DIR)/drivers/usb/storage/ums-cypress.ko \
	$(LINUX_DIR)/drivers/usb/storage/ums-datafab.ko \
	$(LINUX_DIR)/drivers/usb/storage/ums-freecom.ko \
	$(LINUX_DIR)/drivers/usb/storage/ums-isd200.ko \
	$(LINUX_DIR)/drivers/usb/storage/ums-jumpshot.ko \
	$(LINUX_DIR)/drivers/usb/storage/ums-karma.ko \
	$(LINUX_DIR)/drivers/usb/storage/ums-sddr09.ko \
	$(LINUX_DIR)/drivers/usb/storage/ums-sddr55.ko \
	$(LINUX_DIR)/drivers/usb/storage/ums-usbat.ko
  AUTOLOAD:=$(call AutoProbe,ums-alauda ums-cypress ums-datafab \
				ums-freecom ums-isd200 ums-jumpshot \
				ums-karma ums-sddr09 ums-sddr55 ums-usbat)
endef

define KernelPackage/usb-storage-extras/description
 Say Y here if you want to have some more drivers,
 such as for SmartMedia card readers
endef

$(eval $(call KernelPackage,usb-storage-extras))


define KernelPackage/usb-atm
  TITLE:=Support for ATM on USB bus
  DEPENDS:=+kmod-atm
  KCONFIG:=CONFIG_USB_ATM
  FILES:=$(LINUX_DIR)/drivers/usb/atm/usbatm.ko
  AUTOLOAD:=$(call AutoProbe,usbatm)
  $(call AddDepends/usb)
endef

define KernelPackage/usb-atm/description
 Kernel support for USB DSL modems
endef

$(eval $(call KernelPackage,usb-atm))


define AddDepends/usb-atm
  SUBMENU:=$(USB_MENU)
  DEPENDS+=kmod-usb-atm $(1)
endef


define KernelPackage/usb-atm-speedtouch
  TITLE:=SpeedTouch USB ADSL modems support
  KCONFIG:=CONFIG_USB_SPEEDTOUCH
  FILES:=$(LINUX_DIR)/drivers/usb/atm/speedtch.ko
  AUTOLOAD:=$(call AutoProbe,speedtch)
  $(call AddDepends/usb-atm)
endef

define KernelPackage/usb-atm-speedtouch/description
 Kernel support for SpeedTouch USB ADSL modems
endef

$(eval $(call KernelPackage,usb-atm-speedtouch))


define KernelPackage/usb-atm-ueagle
  TITLE:=Eagle 8051 based USB ADSL modems support
  FILES:=$(LINUX_DIR)/drivers/usb/atm/ueagle-atm.ko
  KCONFIG:=CONFIG_USB_UEAGLEATM
  AUTOLOAD:=$(call AutoProbe,ueagle-atm)
  $(call AddDepends/usb-atm)
endef

define KernelPackage/usb-atm-ueagle/description
 Kernel support for Eagle 8051 based USB ADSL modems
endef

$(eval $(call KernelPackage,usb-atm-ueagle))


define KernelPackage/usb-atm-cxacru
  TITLE:=cxacru
  FILES:=$(LINUX_DIR)/drivers/usb/atm/cxacru.ko
  KCONFIG:=CONFIG_USB_CXACRU
  AUTOLOAD:=$(call AutoProbe,cxacru)
  $(call AddDepends/usb-atm)
endef

define KernelPackage/usb-atm-cxacru/description
 Kernel support for cxacru based USB ADSL modems
endef

$(eval $(call KernelPackage,usb-atm-cxacru))


define KernelPackage/usb-net
  TITLE:=Kernel modules for USB-to-Ethernet convertors
  KCONFIG:=CONFIG_USB_USBNET CONFIG_MII=y
  AUTOLOAD:=$(call AutoProbe,usbnet)
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),lt,3.12.0)),1)
  FILES:=$(LINUX_DIR)/drivers/$(USBNET_DIR)/usbnet.ko
else
  FILES:=\
	$(LINUX_DIR)/drivers/$(USBNET_DIR)/usbnet.ko \
	$(LINUX_DIR)/drivers/net/mii.ko
endif
  $(call AddDepends/usb)
endef

define KernelPackage/usb-net/description
 Kernel modules for USB-to-Ethernet convertors
endef

$(eval $(call KernelPackage,usb-net))


define AddDepends/usb-net
  SUBMENU:=$(USB_MENU)
  DEPENDS+=kmod-usb-net $(1)
endef


define KernelPackage/usb-net-asix
  TITLE:=Kernel module for USB-to-Ethernet Asix convertors
  DEPENDS:=+!LINUX_3_3:kmod-libphy
  KCONFIG:=CONFIG_USB_NET_AX8817X
  FILES:=$(LINUX_DIR)/drivers/$(USBNET_DIR)/asix.ko
  AUTOLOAD:=$(call AutoProbe,asix)
  $(call AddDepends/usb-net)
endef

define KernelPackage/usb-net-asix/description
 Kernel module for USB-to-Ethernet Asix convertors
endef

$(eval $(call KernelPackage,usb-net-asix))


define KernelPackage/usb-net-hso
  TITLE:=Kernel module for Option USB High Speed Mobile Devices
  KCONFIG:=CONFIG_USB_HSO
  FILES:= \
	$(LINUX_DIR)/drivers/$(USBNET_DIR)/hso.ko
  AUTOLOAD:=$(call AutoProbe,hso)
  $(call AddDepends/usb-net)
  $(call AddDepends/rfkill)
endef

define KernelPackage/usb-net-hso/description
 Kernel module for Option USB High Speed Mobile Devices
endef

$(eval $(call KernelPackage,usb-net-hso))


define KernelPackage/usb-net-kaweth
  TITLE:=Kernel module for USB-to-Ethernet Kaweth convertors
  KCONFIG:=CONFIG_USB_KAWETH
  FILES:=$(LINUX_DIR)/drivers/$(USBNET_DIR)/kaweth.ko
  AUTOLOAD:=$(call AutoProbe,kaweth)
  $(call AddDepends/usb-net)
endef

define KernelPackage/usb-net-kaweth/description
 Kernel module for USB-to-Ethernet Kaweth convertors
endef

$(eval $(call KernelPackage,usb-net-kaweth))


define KernelPackage/usb-net-pegasus
  TITLE:=Kernel module for USB-to-Ethernet Pegasus convertors
  KCONFIG:=CONFIG_USB_PEGASUS
  FILES:=$(LINUX_DIR)/drivers/$(USBNET_DIR)/pegasus.ko
  AUTOLOAD:=$(call AutoProbe,pegasus)
  $(call AddDepends/usb-net)
endef

define KernelPackage/usb-net-pegasus/description
 Kernel module for USB-to-Ethernet Pegasus convertors
endef

$(eval $(call KernelPackage,usb-net-pegasus))


define KernelPackage/usb-net-mcs7830
  TITLE:=Kernel module for USB-to-Ethernet MCS7830 convertors
  KCONFIG:=CONFIG_USB_NET_MCS7830
  FILES:=$(LINUX_DIR)/drivers/$(USBNET_DIR)/mcs7830.ko
  AUTOLOAD:=$(call AutoProbe,mcs7830)
  $(call AddDepends/usb-net)
endef

define KernelPackage/usb-net-mcs7830/description
 Kernel module for USB-to-Ethernet MCS7830 convertors
endef

$(eval $(call KernelPackage,usb-net-mcs7830))


define KernelPackage/usb-net-dm9601-ether
  TITLE:=Support for DM9601 ethernet connections
  KCONFIG:=CONFIG_USB_NET_DM9601
  FILES:=$(LINUX_DIR)/drivers/$(USBNET_DIR)/dm9601.ko
  AUTOLOAD:=$(call AutoProbe,dm9601)
  $(call AddDepends/usb-net)
endef

define KernelPackage/usb-net-dm9601-ether/description
 Kernel support for USB DM9601 devices
endef

$(eval $(call KernelPackage,usb-net-dm9601-ether))

define KernelPackage/usb-net-cdc-ether
  TITLE:=Support for cdc ethernet connections
  KCONFIG:=CONFIG_USB_NET_CDCETHER
  FILES:=$(LINUX_DIR)/drivers/$(USBNET_DIR)/cdc_ether.ko
  AUTOLOAD:=$(call AutoProbe,cdc_ether)
  $(call AddDepends/usb-net)
endef

define KernelPackage/usb-net-cdc-ether/description
 Kernel support for USB CDC Ethernet devices
endef

$(eval $(call KernelPackage,usb-net-cdc-ether))


define KernelPackage/usb-net-qmi-wwan
  TITLE:=QMI WWAN driver
  KCONFIG:=CONFIG_USB_NET_QMI_WWAN
  FILES:= $(LINUX_DIR)/drivers/$(USBNET_DIR)/qmi_wwan.ko
  AUTOLOAD:=$(call AutoProbe,qmi_wwan)
  $(call AddDepends/usb-net,+kmod-usb-wdm)
endef

define KernelPackage/usb-net-qmi-wwan/description
 QMI WWAN driver for Qualcomm MSM based 3G and LTE modems
endef

$(eval $(call KernelPackage,usb-net-qmi-wwan))


define KernelPackage/usb-net-rndis
  TITLE:=Support for RNDIS connections
  KCONFIG:=CONFIG_USB_NET_RNDIS_HOST
  FILES:= $(LINUX_DIR)/drivers/$(USBNET_DIR)/rndis_host.ko
  AUTOLOAD:=$(call AutoProbe,rndis_host)
  $(call AddDepends/usb-net,+kmod-usb-net-cdc-ether)
endef

define KernelPackage/usb-net-rndis/description
 Kernel support for RNDIS connections
endef

$(eval $(call KernelPackage,usb-net-rndis))


define KernelPackage/usb-net-cdc-mbim
  SUBMENU:=$(USB_MENU)
  TITLE:=Kernel module for MBIM Devices
  KCONFIG:=CONFIG_USB_NET_CDC_MBIM
  FILES:= \
   $(LINUX_DIR)/drivers/$(USBNET_DIR)/cdc_mbim.ko
  AUTOLOAD:=$(call AutoProbe,cdc_mbim)
  $(call AddDepends/usb-net,+kmod-usb-wdm +kmod-usb-net-cdc-ncm)
endef

define KernelPackage/usb-net-cdc-mbim/description
 Kernel module for Option USB High Speed Mobile Devices
endef

$(eval $(call KernelPackage,usb-net-cdc-mbim))


define KernelPackage/usb-net-cdc-ncm
  TITLE:=Support for CDC NCM connections
  KCONFIG:=CONFIG_USB_NET_CDC_NCM
  FILES:= $(LINUX_DIR)/drivers/$(USBNET_DIR)/cdc_ncm.ko
  AUTOLOAD:=$(call AutoProbe,cdc_ncm)
  $(call AddDepends/usb-net)
endef

define KernelPackage/usb-net-cdc-ncm/description
 Kernel support for CDC NCM connections
endef

$(eval $(call KernelPackage,usb-net-cdc-ncm))


define KernelPackage/usb-net-sierrawireless
  TITLE:=Support for Sierra Wireless devices
  KCONFIG:=CONFIG_USB_SIERRA_NET
  FILES:=$(LINUX_DIR)/drivers/net/usb/sierra_net.ko
  AUTOLOAD:=$(call AutoProbe,sierra_net)
  $(call AddDepends/usb-net)
endef

define KernelPackage/usb-net-sierrawireless/description
 Kernel support for Sierra Wireless devices
endef

$(eval $(call KernelPackage,usb-net-sierrawireless))


define KernelPackage/usb-net-ipheth
  TITLE:=Apple iPhone USB Ethernet driver
  KCONFIG:=CONFIG_USB_IPHETH
  FILES:=$(LINUX_DIR)/drivers/net/usb/ipheth.ko
  AUTOLOAD:=$(call AutoProbe,ipheth)
  $(call AddDepends/usb-net)
endef

define KernelPackage/usb-net-ipheth/description
 Kernel support for Apple iPhone USB Ethernet driver
endef

$(eval $(call KernelPackage,usb-net-ipheth))


define KernelPackage/usb-hid
  TITLE:=Support for USB Human Input Devices
  KCONFIG:=CONFIG_HID_SUPPORT=y CONFIG_USB_HID CONFIG_USB_HIDDEV=y
  FILES:=$(LINUX_DIR)/drivers/$(USBHID_DIR)/usbhid.ko
  AUTOLOAD:=$(call AutoProbe,usbhid)
  $(call AddDepends/usb)
  $(call AddDepends/hid)
  $(call AddDepends/input,+kmod-input-evdev)
endef

define KernelPackage/usb-hid/description
 Kernel support for USB HID devices such as keyboards and mice
endef

$(eval $(call KernelPackage,usb-hid))


define KernelPackage/usb-yealink
  TITLE:=USB Yealink VOIP phone
  KCONFIG:=CONFIG_USB_YEALINK CONFIG_INPUT_YEALINK CONFIG_INPUT=m CONFIG_INPUT_MISC=y
  FILES:=$(LINUX_DIR)/drivers/$(USBINPUT_DIR)/yealink.ko
  AUTOLOAD:=$(call AutoProbe,yealink)
  $(call AddDepends/usb)
  $(call AddDepends/input,+kmod-input-evdev)
endef

define KernelPackage/usb-yealink/description
 Kernel support for Yealink VOIP phone
endef

$(eval $(call KernelPackage,usb-yealink))


define KernelPackage/usb-cm109
  TITLE:=Support for CM109 device
  KCONFIG:=CONFIG_USB_CM109 CONFIG_INPUT_CM109 CONFIG_INPUT=m CONFIG_INPUT_MISC=y
  FILES:=$(LINUX_DIR)/drivers/$(USBINPUT_DIR)/cm109.ko
  AUTOLOAD:=$(call AutoProbe,cm109)
  $(call AddDepends/usb)
  $(call AddDepends/input,+kmod-input-evdev)
endef

define KernelPackage/usb-cm109/description
 Kernel support for CM109 VOIP phone
endef

$(eval $(call KernelPackage,usb-cm109))


define KernelPackage/usb-test
  TITLE:=USB Testing Driver
  DEPENDS:=@DEVEL
  KCONFIG:=CONFIG_USB_TEST
  FILES:=$(LINUX_DIR)/drivers/usb/misc/usbtest.ko
  $(call AddDepends/usb)
endef

define KernelPackage/usb-test/description
 Kernel support for testing USB Host Controller software
endef

$(eval $(call KernelPackage,usb-test))


define KernelPackage/usbip
  TITLE := USB-over-IP kernel support
  KCONFIG:= \
	CONFIG_USBIP_CORE \
	CONFIG_USBIP_DEBUG=n
  FILES:=$(LINUX_DIR)/drivers/staging/usbip/usbip-core.ko
  AUTOLOAD:=$(call AutoProbe,usbip-core)
  $(call AddDepends/usb)
endef

$(eval $(call KernelPackage,usbip))


define KernelPackage/usbip-client
  TITLE := USB-over-IP client driver
  DEPENDS := +kmod-usbip
  KCONFIG := CONFIG_USBIP_VHCI_HCD
  FILES := $(LINUX_DIR)/drivers/staging/usbip/vhci-hcd.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD := $(call AutoProbe,vhci-hcd)
  $(call AddDepends/usb)
endef

$(eval $(call KernelPackage,usbip-client))


define KernelPackage/usbip-server
$(call KernelPackage/usbip/Default)
  TITLE := USB-over-IP host driver
  DEPENDS := +kmod-usbip
  KCONFIG := CONFIG_USBIP_HOST
  FILES := $(LINUX_DIR)/drivers/staging/usbip/usbip-host.ko
  AUTOLOAD := $(call AutoProbe,usbip-host)
  $(call AddDepends/usb)
endef

$(eval $(call KernelPackage,usbip-server))


define KernelPackage/usb-chipidea
  TITLE:=Support for ChipIdea controllers
  DEPENDS:=+kmod-usb2
  KCONFIG:=\
	CONFIG_USB_CHIPIDEA \
	CONFIG_USB_CHIPIDEA_HOST=y \
	CONFIG_USB_CHIPIDEA_UDC=n \
	CONFIG_USB_CHIPIDEA_DEBUG=y
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),lt,3.11.0)),1)
  FILES:=\
	$(LINUX_DIR)/drivers/usb/chipidea/ci_hdrc.ko \
	$(if $(CONFIG_OF_DEVICE),$(LINUX_DIR)/drivers/usb/chipidea/ci13xxx_imx.ko) \
	$(if $(CONFIG_OF_DEVICE),$(LINUX_DIR)/drivers/usb/chipidea/usbmisc_imx$(if $(call kernel_patchver_le,3.9),6q).ko)
  AUTOLOAD:=$(call AutoLoad,51,ci_hdrc $(if $(CONFIG_OF_DEVICE),ci13xxx_imx usbmisc_imx$(if $(call kernel_patchver_le,3.9),6q)),1)
else
  FILES:=\
	$(LINUX_DIR)/drivers/usb/chipidea/ci_hdrc.ko \
	$(if $(CONFIG_OF),$(LINUX_DIR)/drivers/usb/chipidea/ci_hdrc_imx.ko) \
	$(if $(CONFIG_OF),$(LINUX_DIR)/drivers/usb/chipidea/usbmisc_imx.ko)
  AUTOLOAD:=$(call AutoLoad,51,ci_hdrc $(if $(CONFIG_OF),ci_hdrc_imx usbmisc_imx),1)
endif
  $(call AddDepends/usb)
endef
  
define KernelPackage/usb-chipidea/description
 Kernel support for USB ChipIdea controllers
endef

$(eval $(call KernelPackage,usb-chipidea,1))


define KernelPackage/usb-mxs-phy
  TITLE:=Support for Freescale MXS USB PHY
  DEPENDS:=@TARGET_imx6
  KCONFIG:=CONFIG_USB_MXS_PHY
  FILES:=\
	$(LINUX_DIR)/drivers/usb/phy/phy-mxs-usb.ko
  AUTOLOAD:=$(call AutoLoad,52,phy-mxs-usb,1)
  $(call AddDepends/usb)
endef

define KernelPackage/usb-mxs-phy/description
 Kernel support for Freescale MXS USB PHY
endef

$(eval $(call KernelPackage,usb-mxs-phy,1))


define KernelPackage/usbmon
  TITLE:=USB traffic monitor
  KCONFIG:=CONFIG_USB_MON
  $(call AddDepends/usb)
  FILES:=$(LINUX_DIR)/drivers/usb/mon/usbmon.ko
  AUTOLOAD:=$(call AutoProbe,usbmon)
endef

define KernelPackage/usbmon/description
 Kernel support for USB traffic monitoring
endef

$(eval $(call KernelPackage,usbmon))


define KernelPackage/usb3
  TITLE:=Support for USB3 controllers
  KCONFIG:= \
	CONFIG_USB_XHCI_HCD \
	CONFIG_USB_XHCI_HCD_DEBUGGING=n
  FILES:= \
	$(LINUX_DIR)/drivers/usb/host/xhci-hcd.ko
  AUTOLOAD:=$(call AutoLoad,54,xhci-hcd,1)
  $(call AddDepends/usb)
endef

define KernelPackage/usb3/description
 Kernel support for USB3 (XHCI) controllers
endef

$(eval $(call KernelPackage,usb3))
