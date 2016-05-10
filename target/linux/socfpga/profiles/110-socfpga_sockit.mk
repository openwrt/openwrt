#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/SOCFPGA_SOCKIT
	NAME:=Terasic SoCKit
	PACKAGES:=uboot-socfpga-socfpga_cyclone5_sockit
	DEPENDS:=+@TARGET_ROOTFS_INCLUDE_KERNEL +@TARGET_ROOTFS_INCLUDE_DTB
endef

define Profile/SOCFPGA_SOCKIT/Description
 The Terasic SoCKit is based on the Altera Cyclone V SoC
 and offers a large variety of peripherals such as:

  * DDR3
  * NAND or SPI flash
  * USB EHCI
  * USB OTG
  * User GPIO
endef

SOCFPGA_SOCKIT_DTS := \
	socfpga_cyclone5_sockit

$(eval $(call Profile,SOCFPGA_SOCKIT))
