include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=alpine
BOARDNAME:=Annapurna Labs Alpine
FEATURES:=squashfs nand fpu ramdisk usb pci pcie
CPU_TYPE:=cortex-a15
CPU_SUBTYPE:=neon-vfpv4
SUBTARGETS:=mikrotik

KERNEL_PATCHVER:=6.18

KERNELNAME:=zImage Image dtbs

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	kmod-leds-gpio kmod-gpio-button-hotplug \
	uboot-envtools

$(eval $(call BuildTarget))
