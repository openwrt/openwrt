#
# Copyright (C) 2013-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/370-DB
  NAME:=Marvell Armada 370 DB (DB-88F6710-BP-DDR3)
  PACKAGES:= \
	kmod-mmc kmod-mvsdio kmod-usb2 kmod-usb-storage \
	kmod-of-i2c kmod-i2c-core kmod-i2c-mv64xxx \
	kmod-ata-core kmod-ata-marvell-sata \
	kmod-rtc-marvell kmod-thermal-armada
endef

define Profile/370-DB/Description
 Package set compatible with the Armada 370 evaluation board (DB-88F6710-BP-DDR3).
endef

$(eval $(call Profile,370-DB))

define Profile/370-RD
  NAME:=Marvell Armada 370 RD (RD-88F6710-A1)
  PACKAGES:= \
	kmod-mmc kmod-mvsdio kmod-usb2 kmod-usb-storage \
	kmod-of-i2c kmod-i2c-core kmod-i2c-mv64xxx \
	kmod-ata-core kmod-ata-marvell-sata \
	kmod-rtc-marvell kmod-thermal-armada
endef

define Profile/370-RD/Description
 Package set compatible with the Armada 370 reference design board (RD-88F6710-A1).
endef

$(eval $(call Profile,385-RD))

define Profile/385-RD
  NAME:=Marvell Armada 385 RD (RD-88F6820-AP)
  PACKAGES:= \
	kmod-mmc kmod-mvsdio kmod-usb3 kmod-usb-storage \
	kmod-of-i2c kmod-i2c-core kmod-i2c-mv64xxx \
	kmod-thermal-armada
endef

define Profile/385-RD/Description
 Package set compatible with the Armada 385 reference design board (RD-88F6820-AP).
endef

$(eval $(call Profile,385-RD))

define Profile/385-DB-AP
  NAME:=Marvell Armada 385 DB AP (DB-88F6820-AP)
  PACKAGES:= \
	kmod-usb3 kmod-usb-storage \
	kmod-of-i2c kmod-i2c-core kmod-i2c-mv64xxx \
	kmod-ata-core kmod-ata-marvell-sata \
	kmod-thermal-armada
endef

define Profile/385-DB-AP/Description
 Package set compatible with the Armada 385 access point development board (DB-88F6820-AP).
endef

$(eval $(call Profile,385-DB-AP))

define Profile/XP-DB
  NAME:=Marvell Armada XP DB (DB-78460-BP)
  PACKAGES:= \
	kmod-mmc kmod-mvsdio kmod-usb2 kmod-usb-storage \
	kmod-of-i2c kmod-i2c-core kmod-i2c-mv64xxx \
	kmod-ata-core kmod-ata-marvell-sata \
	kmod-rtc-marvell kmod-thermal-armada
endef

define Profile/XP-DB/Description
 Package set compatible with the Marvell Armada XP evaluation board (DB-78460-BP).
endef

$(eval $(call Profile,XP-DB))

define Profile/XP-GP
  NAME:=Marvell Armada XP GP (DB-MV784MP-GP)
  PACKAGES:= \
	kmod-mmc kmod-mvsdio kmod-usb2 kmod-usb-storage \
	kmod-of-i2c kmod-i2c-core kmod-i2c-mv64xxx \
	kmod-ata-core kmod-ata-marvell-sata \
	kmod-rtc-marvell kmod-thermal-armada
endef

define Profile/XP-GP/Description
 Package set compatible with the Armada XP development board (DB-MV784MP-GP).
endef

$(eval $(call Profile,XP-GP))
