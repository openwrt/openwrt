#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/AR5381u
  NAME:=Comtrend AR-5381u
  PACKAGES:=kmod-b43 wpad-mini \
	kmod-usb2 kmod-usb-ohci
endef
define Profile/AR5381u/Description
  Package set optimized for AR-5381u.
endef
$(eval $(call Profile,AR5381u))

define Profile/AR5387un
  NAME:=Comtrend AR-5387un
  PACKAGES:=kmod-b43 wpad-mini \
	kmod-usb2 kmod-usb-ohci
endef
define Profile/AR5387un/Description
  Package set optimized for AR-5387un.
endef
$(eval $(call Profile,AR5387un))

define Profile/CT536_CT5621
  NAME:=Comtrend CT-536+/CT-5621
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/CT536_CT5621/Description
  Package set optimized for CT-536+/CT-5621.
endef
$(eval $(call Profile,CT536_CT5621))

define Profile/CT5365
  NAME:=Comtrend CT-5365
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/CT5365/Description
  Package set optimized for CT-5365.
endef
$(eval $(call Profile,CT5365))

define Profile/CT6373
  NAME:=Comtrend CT-6373
  PACKAGES:=kmod-b43 wpad-mini \
	kmod-usb2 kmod-usb-ohci
endef
define Profile/CT6373/Description
  Package set optimized for CT-6373.
endef
$(eval $(call Profile,CT6373))

define Profile/VR3025u
  NAME:=Comtrend VR-3025u
  PACKAGES:=kmod-b43 wpad-mini \
	kmod-usb2 kmod-usb-ohci
endef
define Profile/VR3025u/Description
  Package set optimized for VR-3025u.
endef
$(eval $(call Profile,VR3025u))

define Profile/VR3025un
  NAME:=Comtrend VR-3025un
  PACKAGES:=kmod-b43 wpad-mini \
	kmod-usb2 kmod-usb-ohci
endef
define Profile/VR3025un/Description
  Package set optimized for VR-3025un.
endef
$(eval $(call Profile,VR3025un))

define Profile/WAP5813n
  NAME:=Comtrend WAP-5813n
  PACKAGES:=kmod-b43 wpad-mini \
	kmod-usb2 kmod-usb-ohci
endef
define Profile/WAP5813n/Description
  Package set optimized for WAP-5813n.
endef
$(eval $(call Profile,WAP5813n))
