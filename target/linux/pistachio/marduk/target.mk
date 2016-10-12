#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

BOARDNAME:=marduk

DEFAULT_PACKAGES+=kmod-leds-gpio kmod-ledtrig-heartbeat kmod-i2c-core i2c-tools \
                  kmod-sound-pistachio-soc alsa-lib alsa-utils alsa-utils-tests \
                  kmod-uccp420wlan kmod-cfg80211 fping iw hostapd wpa-cli wpa-supplicant \
                  kmod-tpm-i2c-infineon kmod-random-tpm tpm-tools \
                  uhttpd uboot-envtools tcpdump board-test proddata \
                  luci kmod-bluetooth kmod-ieee802154

define Profile/marduk/default
	$(1)_DEVICE_DTS:=$(2)
ifeq ($(3),)
        $(1)_UBIFS_OPTS:="-m 4096 -e 253952 -c 1580"
else
	$(1)_UBIFS_OPTS:=$(3)
endif
ifeq ($(4),)
        $(1)_UBI_OPTS:="-m 4096 -p 262144 -s 4096"
else
	$(1)_UBI_OPTS:=$(4)
endif
	$(eval $(call Profile,$(1)))
endef


define Target/Description
        Marduk
endef
