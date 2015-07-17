#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Default
  NAME:=Raspberry Pi
endef
define Profile/Default/Description
  Raspberry Pi
endef
$(eval $(call Profile,Default))

define Profile/RaspberryPi_B
  NAME:=Raspberry Pi Model B
endef
define Profile/RaspberryPi_B/Description
  Raspberry Pi Model B
endef
$(eval $(call Profile,RaspberryPi_B))

define Profile/RaspberryPi_BPlus
  NAME:=Raspberry Pi Model B+
endef
define Profile/RaspberryPi_BPlus/Description
  Raspberry Pi Model B+
endef
$(eval $(call Profile,RaspberryPi_BPlus))

define Profile/RaspberryPi_CM
  NAME:=Raspberry Pi Compute Module
endef
define Profile/RaspberryPi_CM/Description
  Raspberry Pi Model Compute Module
endef
$(eval $(call Profile,RaspberryPi_CM))
