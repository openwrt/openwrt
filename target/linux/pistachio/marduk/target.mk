#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

BOARDNAME:=marduk

DEFAULT_PACKAGES+=kmod-leds-gpio kmod-ledtrig-heartbeat kmod-i2c-core i2c-tools \
                  kmod-sound-pistachio-soc alsa-lib alsa-utils alsa-utils-tests \
                  uhttpd uboot-envtools tcpdump luci

define Target/Description
        Marduk
endef
