#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/RaspberryPi_3
  NAME:=Raspberry Pi 3 Model B
  PACKAGES:=brcmfmac43430-firmware-sdio kmod-brcmfmac wpad-mini
endef
define Profile/RaspberryPi_3/Description
  Raspberry Pi 3 Model B
endef
$(eval $(call Profile,RaspberryPi_3))
