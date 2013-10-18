#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/IMX6Q_GW5400_A
  NAME:=Gateworks Ventana GW5400-A
  PACKAGES:= \
	kmod-thermal-imx kmod-usb-chipidea kmod-usb-mxs-phy
endef

define Profile/IMX6Q_GW5400_A/Description
 The Gateworks GW5400-A family of products is based on the Freescale i.MX6Q SoC
 and offers a large variety of peripherals such as:

  * DDR3
  * SPI FLASH
  * 6x PCIe (3x with USB)
  * Optional expansion for additional USB/PCI based periperhals
  * 1x USB EHCI
  * 1x USB OTG
  * HDMI Audio/Video in/out
  * Analog Video in/out
  * Analog Audio in/out
  * Gateworks System Controller
endef

$(eval $(call Profile,IMX6Q_GW5400_A))

define Profile/IMX6Q_GW54XX
  NAME:=Gateworks Ventana GW54XX
  PACKAGES:= \
	kmod-thermal-imx kmod-usb-chipidea kmod-usb-mxs-phy
endef

define Profile/IMX6Q_GW54XX/Description
 The Gateworks GW54xx family of products is based on the Freescale i.MX6Q SoC
 and offers a large variety of peripherals such as:

  * DDR3
  * NAND FLASH
  * 6x PCIe (3x with USB)
  * Optional expansion for additional USB/PCI based periperhals
  * 1x USB EHCI
  * 1x USB OTG
  * HDMI Audio/Video in/out
  * Analog Video in/out
  * Analog Audio in/out
  * Gateworks System Controller
endef

IMX6Q_GW54XX_UBIFS_OPTS:="-m 2048 -e 124KiB -c 1912"
IMX6Q_GW54XX_UBI_OPTS:="-m 2048 -p 128KiB -s 2048"

$(eval $(call Profile,IMX6Q_GW54XX))
