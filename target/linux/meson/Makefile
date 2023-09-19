# SPDX-License-Identifier: GPL-2.0-or-later
#
# Copyright (C) 2017-2023 shiyu1314 <1052672227@qq.com>

include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=meson
BOARDNAME:=Amlogic Meson family
FEATURES:=audio boot-part ext4 fpu legacy-sdcard squashfs usbgadget
MAINTAINER:=shiyu1314 <1052672227@qq.com>
SUBTARGETS:=meson8b

KERNEL_PATCHVER:=5.15
KERNEL_TESTING_PATCHVER:=6.1

define Target/Description
	Build firmware image for Amlogic Meson SoC devices.
	Currently produces SD Card/USB disk image.
endef

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	e2fsprogs \
	kmod-sound-core \
	kmod-usb-hid \
	mkf2fs \
	automount \
	resize2fs \
	partx-utils

KERNELNAME:=Image dtbs

$(eval $(call BuildTarget))
