include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Device/Default
	PROFILES := Default
	DEVICE_DTS := alpine-$(lastword $(subst _, ,$(1)))
	DEVICE_DTS_DIR := ../dts
	IMAGES := sysupgrade.bin
	IMAGE/sysupgrade.bin = sysupgrade-tar | append-metadata
	IMAGE/sysupgrade.bin/squashfs :=
endef

include $(SUBTARGET).mk

$(eval $(call BuildImage))
