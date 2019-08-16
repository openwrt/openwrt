#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/broadcom-mips74k-brcmsmac
  NAME:=Broadcom SoC, BCM43xx WiFi (brcmsmac)
  PACKAGES:=kmod-brcmsmac
endef

define Profile/broadcom-mips74k-brcmsmac/Description
	Package set for devices with BCM43xx WiFi including mac80211 and
	brcmsmac driver.
endef

$(eval $(call Profile,broadcom-mips74k-brcmsmac))

