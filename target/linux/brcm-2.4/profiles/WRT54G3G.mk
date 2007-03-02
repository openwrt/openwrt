#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WRT54G3G
  NAME:=Linksys WRT54G3G
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-pcmcia-core kmod-pcmcia-serial kmod-nozomi comgt wlc nas kmod-wlcompat
endef

define Profile/WRT54G3G/Description
	Package set optimized for the WRT54G3G with UMTS support
endef
$(eval $(call Profile,WRT54G3G))

