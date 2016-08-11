#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/RaspberryPi
  NAME:=Raspberry Pi Models B/B+/CM
endef
define Profile/RaspberryPi/Description
  Raspberry Pi Models B/B+/CM
endef
$(eval $(call Profile,RaspberryPi))
