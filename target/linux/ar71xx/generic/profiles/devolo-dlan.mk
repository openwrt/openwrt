#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/dLAN_pro_500_wp
	NAME:=devolo dLAN pro 500 Wireless+
	PACKAGES:=open-plc-utils open-plc-utils-chkpib open-plc-utils-getpib open-plc-utils-modpib \
		open-plc-utils-setpib open-plc-utils-hpavkey open-plc-utils-amphost \
		open-plc-utils-plctool open-plc-utils-pibdump open-plc-utils-plcstat \
		ebtables hostapd-utils wifitoggle wpad
endef

define Profile/dLAN_pro_500_wp/Description
	Package set optimized for the devolo dLAN pro 500 Wireless+.
endef
$(eval $(call Profile,dLAN_pro_500_wp))

define Profile/dLAN_pro_1200_ac
	NAME:=devolo dLAN pro 1200+ WiFi ac
	PACKAGES:=open-plc-utils open-plc-utils-chkpib open-plc-utils-getpib open-plc-utils-modpib \
		open-plc-utils-setpib open-plc-utils-hpavkey open-plc-utils-plchost \
		open-plc-utils-plctool open-plc-utils-pibdump open-plc-utils-plcstat \
		ebtables hostapd-utils wifitoggle wpad ip-full kmod-ath10k kmod-leds-gpio \
		kmod-ledtrig-gpio
endef

define Profile/dLAN_pro_1200_ac/Description
	Package set optimized for the devolo dLAN pro 1200+ WiFi ac.
endef
$(eval $(call Profile,dLAN_pro_1200_ac))
