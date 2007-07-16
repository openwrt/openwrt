#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/s100
  NAME:=T-Vision S-100
  LINUX_CONFIG:=$(CURDIR)/config/profile-s100
  FEATURES+=usb
endef

define Profile/s100/Description
	T-Online streaming client.
endef
$(eval $(call Profile,s100))
