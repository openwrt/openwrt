include $(TOPDIR)/rules.mk

BOARD:=airoha
BOARDNAME:=Airoha / Econet
SUBTARGETS:=en7523 an7581 an7583 en751221 en751627 en7528
FEATURES:=dt squashfs nand ramdisk gpio

KERNEL_PATCHVER:=6.18

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += \
	kmod-leds-gpio kmod-gpio-button-hotplug nand-utils

$(eval $(call BuildTarget))
