#
# Copyright (C) 2018 Jianhui Zhao <jianhuizhao329@gmail.com>
# Copyright (C) 2019 Samik Gupta <samik.gupta@broadcom.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

ARCH:=aarch64
BOARD:=brcmiproc
BOARDNAME:=Broadcom iProc
FEATURES:=fpu pci rtc usb ext4 targz
SUBTARGETS:=arm64
MAINTAINER:=Samik Gupta <samik.gupta@broadcom.com>

KERNEL_PATCHVER:=4.19

define Target/Description
	Build firmware images for Broadcom iProc SoC based boards.
endef

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))
