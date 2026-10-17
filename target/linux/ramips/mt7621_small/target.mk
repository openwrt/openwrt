#
# Copyright (C) 2009 OpenWrt.org
#

SUBTARGET:=mt7621_small
BOARDNAME:=MT7621 based boards (small flash)
FEATURES+=nand ramdisk rtc usb minor small_flash
CPU_TYPE:=24kc
KERNELNAME:=vmlinux vmlinuz
# make Kernel/CopyImage use $LINUX_DIR/vmlinuz
IMAGES_DIR:=../../..

DEFAULT_PACKAGES += wpad-basic-mbedtls uboot-envtools kmod-crypto-hw-eip93

define Target/Description
	Build firmware images for Ralink MT7621 based boards with small flash.
endef
