#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

ARCH:=aarch64
BOARDNAME:=layerscape 64b boards
CFLAGS:=-Os -pipe -fno-caller-saves

define Target/Description
	Build firmware images for $(BOARDNAME) SoC devices.
endef

