include $(TOPDIR)/rules.mk

ARCH:=aarch64
BOARD:=qualcommbe
BOARDNAME:=Qualcomm Atheros 802.11be WiSoC-s
FEATURES:=squashfs ramdisk fpu nand rtc emmc source-only
KERNELNAME:=Image
CPU_TYPE:=cortex-a73
SUBTARGETS:=ipq95xx

KERNEL_PATCHVER:=6.18

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += \
	kmod-usb3 kmod-usb-dwc3 kmod-usb-dwc3-qcom \
	kmod-leds-gpio kmod-gpio-button-hotplug \
	kmod-qcom-ppe \
	wpad-basic-mbedtls \
	e2fsprogs kmod-fs-ext4 losetup \
	uboot-envtools

$(eval $(call BuildTarget))
