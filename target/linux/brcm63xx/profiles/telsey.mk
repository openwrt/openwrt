#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/CPVA502PLUS
  NAME:=Telsey CPVA502+
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/CPVA502PLUS/Description
  Package set optimized for CPVA502+.
endef
$(eval $(call Profile,CPVA502PLUS))

define Profile/CPVA642
  NAME:=Telsey CPVA642-type (CPA-ZNTE60T)
  PACKAGES:=kmod-rt61-pci wpad-mini\
	kmod-usb2 kmod-usb-ohci
endef
define Profile/CPVA642/Description
  Package set optimized for CPVA642-type.
endef
$(eval $(call Profile,CPVA642))

define Profile/MAGIC
  NAME:=Telsey MAGIC (Alice W-Gate)
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/MAGIC/Description
  Package set optimized for Telsey MAGIC (Alice W-Gate)
endef
$(eval $(call Profile,CPVA502PLUS))
