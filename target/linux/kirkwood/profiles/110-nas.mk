#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/DOCKSTAR
  NAME:=Seagate FreeAgent Dockstar
  PACKAGES:= \
	kmod-usb2 kmod-usb-storage
endef

define Profile/DOCKSTAR/Description
 Package set compatible with Seagate FreeAgent Dockstar board.
endef

$(eval $(call Profile,DOCKSTAR))

define Profile/GOFLEXHOME
  NAME:=Seagate GoFlexHome
  PACKAGES:= \
	kmod-ata-core kmod-ata-marvell-sata \
	kmod-usb2 kmod-usb-storage \
	uboot-envtools
endef

define Profile/GOFLEXHOME/Description
 Package set compatible with Seagate GoFlexHome
endef

$(eval $(call Profile,GOFLEXHOME))

define Profile/GOFLEXNET
  NAME:=Seagate GoFlexNet
  PACKAGES:= \
	kmod-ata-core kmod-ata-marvell-sata \
	kmod-usb2 kmod-usb-storage
endef

define Profile/GOFLEXNET/Description
 Package set compatible with Seagate GoFlexNet
endef

$(eval $(call Profile,GOFLEXNET))

define Profile/IB62X0
  NAME:=RaidSonic ICY BOX IB-NAS62x0
  PACKAGES:= \
	kmod-ata-core kmod-ata-marvell-sata \
	kmod-usb2 kmod-usb-storage
endef

define Profile/IB62X0/Description
 Package set compatible with RaidSonic ICY BOX IB-NAS62x0 board.
endef

$(eval $(call Profile,IB62X0))

define Profile/ICONNECT
  NAME:=Iomega Iconnect
  PACKAGES:= \
	kmod-usb2 kmod-usb-storage
endef

define Profile/ICONNECT/Description
 Package set compatible with Iomega Iconnect board.
endef

$(eval $(call Profile,ICONNECT))

define Profile/IOMEGA_IX2_200
  NAME:=Iomega StorCenter ix2-200
  PACKAGES:= \
	kmod-ata-core kmod-ata-marvell-sata \
	kmod-usb2 kmod-usb-storage
endef

define Profile/IOMEGA_IX2_200/Description
 Package set compatible with Iomega StorCenter ix2-200 board.
endef

$(eval $(call Profile,IOMEGA_IX2_200))

define Profile/NSA310S
  NAME:=ZyXEL NSA310S
  PACKAGES:= \
	kmod-ata-core kmod-ata-marvell-sata \
	kmod-usb2 kmod-usb-storage
endef

define Profile/NSA310S/Description
 Package set compatible with ZyXEL NSA310S board.
endef

$(eval $(call Profile,NSA310S))

define Profile/POGOE02
  NAME:=Cloud Engines Pogoplug E02
  PACKAGES:= \
	kmod-usb2 kmod-usb-storage
endef

define Profile/POGOE02/Description
 Package set compatible with Cloud Engines Pogoplug E02 board.
endef

$(eval $(call Profile,POGOE02))
