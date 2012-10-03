#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Gumstix
  NAME:=Gumstix
  PACKAGES:=uboot-pxa-gumstix
endef

define Profile/Atheros-ath5k/Description
	Package set compatible with the Gumstix boards
endef
$(eval $(call Profile,Gumstix))

