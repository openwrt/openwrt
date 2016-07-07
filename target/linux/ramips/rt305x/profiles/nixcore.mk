#
# Copyright (C) 2016 lede-project.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/NIXCORE
    NAME:=NixcoreX1
    PACKAGES:=\
        kmod-usb-core kmod-usb-ohci kmod-usb2 \
        kmod-i2c-core kmod-i2c-ralink \
        kmod-spi-dev
endef

define Profile/NIXCORE/Description
    Package set for Nixcore X1 board
endef

$(eval $(call Profile,NIXCORE))
