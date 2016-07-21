#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/axs103
	NAME:=Synopsys DesignWare AXS103
	PACKAGES:= kmod-usb-core kmod-usb-ohci kmod-mmc kmod-sdhci kmod-ath9k-htc wpad-mini
endef

define Profile/axs103/Description
	Package set compatible with hardware using Synopsys DesignWare AXS103 boards.
endef
$(eval $(call Profile,axs103))
