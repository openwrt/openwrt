include $(TOPDIR)/rules.mk

ARCH:=aarch64
BOARD:=ipq95xx
BOARDNAME:=Qualcomm Technologies, Inc. IPQ95XX
FEATURES:=squashfs fpu ramdisk nand pcie
CPU_TYPE:=cortex-a73
CPU_SUBTYPE:=neon-vfpv4
SUBTARGETS:=generic


KERNELNAME:=Image dtbs
KERNEL_PATCHVER:=6.1

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	e2fsprogs uboot-envtools kmod-leds-gpio kmod-gpio-button-hotplug kmod-button-hotplug

$(eval $(call BuildTarget))
