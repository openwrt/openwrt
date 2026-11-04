include $(TOPDIR)/rules.mk

BOARD:=airoha
BOARDNAME:=Airoha
SUBTARGETS:=en751221 en751627 en7528 en7580 en7523 an7552 an7581 an7583
FEATURES:=dt squashfs nand ramdisk gpio

KERNEL_PATCHVER:=6.18

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += \
	kmod-leds-gpio kmod-gpio-button-hotplug nand-utils \
	xpon-agent

$(eval $(call BuildTarget))
