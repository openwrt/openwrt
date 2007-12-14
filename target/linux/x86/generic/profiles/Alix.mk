#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Alix
  NAME:=PCEngines Alix
  PACKAGES:=kmod-via-rhine kmod-leds-alix
endef

define Profile/Alix/Description
        Package set compatible with the PCEngines Alix. Contains VIA Rhine III VT6105M and LED support
endef
$(eval $(call Profile,Alix))
