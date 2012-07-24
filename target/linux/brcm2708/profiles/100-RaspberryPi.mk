#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/RaspberryPi
  NAME:=RaspberryPi
endef

define Profile/RaspberryPi/Description
	RaspberryPi board
endef

$(eval $(call Profile,RaspberryPi))

