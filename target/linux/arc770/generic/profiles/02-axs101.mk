#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/axs101
	NAME:=Synopsys DesignWare AXS101
	PACKAGES:= kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-mmc kmod-sdhci
endef

define Profile/axs101/Description
	Package set compatible with hardware using Synopsys DesignWare AXS101 boards.
endef
$(eval $(call Profile,axs101))
