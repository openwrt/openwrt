#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Broadcom
  NAME:=Broadcom BCM43xx WiFi
  PACKAGES:=kmod-net-bcm43xx
endef
$(eval $(call Profile,Broadcom))

