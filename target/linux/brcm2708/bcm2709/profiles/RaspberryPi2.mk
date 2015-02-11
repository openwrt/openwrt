#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/RaspberryPi2
  NAME:=Raspberry Pi 2
endef

define Profile/RaspberryPi2/Description
	Raspberry Pi 2 board
endef

$(eval $(call Profile,RaspberryPi2))
