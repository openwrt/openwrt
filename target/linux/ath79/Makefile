include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=ath79
BOARDNAME:=Atheros ATH79
CPU_TYPE:=24kc
SUBTARGETS:=generic mikrotik nand tiny

FEATURES:=ramdisk squashfs usbgadget

KERNEL_PATCHVER:=6.6

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	kmod-gpio-button-hotplug swconfig \
	kmod-ath9k uboot-envtools \
	kmod-switch-ar8xxx kmod-swconfig kmod-ag71xx-legacy kmod-phy-at803x \
	kmod-mdio-devres kmod-mdio-gpio

$(eval $(call BuildTarget))
