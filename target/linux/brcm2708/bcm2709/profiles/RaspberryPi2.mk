#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/RaspberryPi_2
  NAME:=Raspberry Pi 2 Model B
endef
define Profile/RaspberryPi_2/Description
  Raspberry Pi 2 Model B
endef
$(eval $(call Profile,RaspberryPi_2))
