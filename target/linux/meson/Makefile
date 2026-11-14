# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

ARCH:=aarch64
BOARD:=meson
BOARDNAME:=Amlogic Meson SoCs
FEATURES:=audio boot-part display ext4 fpu gpio legacy-sdcard rootfs-part squashfs usb
SUBTARGETS:=gxbb

KERNEL_PATCHVER:=6.18

define Target/Description
	Build firmware images for Amlogic Meson SoC devices.
endef

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=Image dtbs

DEFAULT_PACKAGES += e2fsprogs mkf2fs video-support

$(eval $(call BuildTarget))
