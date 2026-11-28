include $(TOPDIR)/rules.mk

ARCH:=aarch64
BOARD:=qualcommax
BOARDNAME:=Qualcomm Atheros 802.11ax WiSoC-s
FEATURES:=squashfs ramdisk fpu nand rtc emmc
KERNELNAME:=Image
CPU_TYPE:=cortex-a53
SUBTARGETS:=ipq807x ipq60xx ipq50xx ipq50xx32

KERNEL_PATCHVER:=6.18

include $(INCLUDE_DIR)/target.mk
ifeq ($(SUBTARGET),ipq50xx32)
DEFAULT_PACKAGES += kmod-leds-gpio kmod-gpio-button-hotplug
else
DEFAULT_PACKAGES += \
	kmod-usb3 kmod-usb-dwc3 kmod-usb-dwc3-qcom \
	kmod-leds-gpio kmod-gpio-button-hotplug \
	kmod-ath11k-ahb \
	wpad-basic-mbedtls uboot-envtools \
	e2fsprogs kmod-fs-ext4 losetup
endif

$(eval $(call BuildTarget))
