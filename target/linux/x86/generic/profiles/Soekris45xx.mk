#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Soekris45xx
  NAME:=Soekris Net45xx
  PACKAGES:=kmod-natsemi kmod-leds-net48xx kmod-sc520-wdt
endef

define Profile/Soekris45xx/Description
        Package set compatible with the Soekris Net45xx. Net4511 and 4521 need PCMCIA Cardbus support. Please add manually.
endef
$(eval $(call Profile,Soekris45xx))
