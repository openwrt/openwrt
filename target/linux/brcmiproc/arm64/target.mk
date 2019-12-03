#
# Copyright (C) 2018 Jianhui Zhao <jianhuizhao329@gmail.com>
# Copyright (C) 2019 Samik Gupta <samik.gupta@broadcom.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

SUBTARGET:=arm64
BOARDNAME:=64-bit iProc based boards
KERNELNAME:=Image

define Target/Description
	Build image for boards based on 64-bit Broadcom iProc devices
endef
