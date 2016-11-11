#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

BOARDNAME:=marduk

DEFAULT_PACKAGES+=kmod-leds-gpio kmod-ledtrig-heartbeat kmod-i2c-core i2c-tools \
                  kmod-sound-pistachio-soc alsa-lib alsa-utils alsa-utils-tests \
                  fping iw hostapd wpa-cli wpa-supplicant \
                  kmod-tpm-i2c-infineon kmod-random-tpm tpm-tools \
                  uhttpd uboot-envtools tcpdump board-test proddata \
                  luci kmod-bluetooth kmod-mac802154

define Target/Description
        Marduk
endef
