#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/DSL2640B_B
  NAME:=D-Link DSL-2640B rev B2
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/DSL2640B_B/Description
  Package set optimized for DSL-2640B rev B2.
endef
$(eval $(call Profile,DSL2640B_B))

define Profile/DSL2650U
  NAME:=D-Link DSL-2650U
  PACKAGES:=kmod-b43 wpad-mini\
	kmod-usb2 kmod-usb-ohci
endef
define Profile/DSL2650U/Description
  Package set optimized for DSL-2650U.
endef
$(eval $(call Profile,DSL2650U))

define Profile/DSL274XB_C
  NAME:=D-Link DSL-2740B/DSL-2741B rev C2/C3
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/DSL274XB_C/Description
  Package set optimized for DSL-2740B/DSL-2741B rev C2/C3.
endef
$(eval $(call Profile,DSL274XB_C))

define Profile/DSL274XB_F
  NAME:=D-Link DSL-2740B/DSL-2741B rev F1
  PACKAGES:=kmod-ath9k wpad-mini
endef
define Profile/DSL274XB_F/Description
  Package set optimized for DSL-2740B/DSL-2741B rev F1.
endef
$(eval $(call Profile,DSL274XB_F))

define Profile/DVAG3810BN
  NAME:=D-Link DVA-G3810BN/TL
  PACKAGES:=kmod-b43 wpad-mini \
	kmod-usb2 kmod-usb-ohci
endef
define Profile/DVAG3810BN/Description
  Package set optimized for DVA-G3810BN/TL.
endef
$(eval $(call Profile,DVAG3810BN))
