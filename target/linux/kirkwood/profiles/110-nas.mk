#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/DOCKSTAR
  NAME:=Seagate FreeAgent Dockstar
  PACKAGES:= \
	kmod-ata-core kmod-ata-marvell-sata \
	kmod-rtc-marvell kmod-usb2 kmod-usb-storage \
	uboot-envtools
endef

define Profile/DOCKSTAR/Description
 Package set compatible with Seagate FreeAgent Dockstar board.
endef

DOCKSTAR_UBIFS_OPTS:="-m 2048 -e 126KiB -c 4096"
DOCKSTAR_UBI_OPTS:="-m 2048 -p 128KiB -s 512"

$(eval $(call Profile,DOCKSTAR))

define Profile/GOFLEXNET
  NAME:=Seagate GoFlexNet
  PACKAGES:= \
	kmod-ata-core kmod-ata-marvell-sata \
	kmod-rtc-marvell kmod-usb2 kmod-usb-storage \
	uboot-envtools
endef

define Profile/GOFLEXNET/Description
 Package set compatible with Seagate GoFlexNet
endef

GOFLEXNET_UBIFS_OPTS:="-m 2048 -e 126KiB -c 4096"
GOFLEXNET_UBI_OPTS:="-m 2048 -p 128KiB -s 512"

$(eval $(call Profile,GOFLEXNET))

define Profile/IB62X0
  NAME:=RaidSonic ICY BOX IB-NAS62x0
  PACKAGES:= \
	kmod-ata-core kmod-ata-marvell-sata \
	kmod-rtc-marvell kmod-usb2 kmod-usb-storage \
	uboot-envtools
endef

define Profile/IB62X0/Description
 Package set compatible with RaidSonic ICY BOX IB-NAS62x0 board.
endef

IB62X0_UBIFS_OPTS:="-m 2048 -e 126KiB -c 4096"
IB62X0_UBI_OPTS:="-m 2048 -p 128KiB -s 512"

$(eval $(call Profile,IB62X0))

define Profile/ICONNECT
  NAME:=Iomega Iconnect
  PACKAGES:= \
	kmod-usb2 kmod-usb-storage
endef

define Profile/ICONNECT/Description
 Package set compatible with Iomega Iconnect board.
endef

ICONNECT_UBIFS_OPTS:="-m 2048 -e 126KiB -c 4096"
ICONNECT_UBI_OPTS:="-m 2048 -p 128KiB -s 512"

$(eval $(call Profile,ICONNECT))
