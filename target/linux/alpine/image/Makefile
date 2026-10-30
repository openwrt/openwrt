include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Device/Default
	PROFILES := Default
	DEVICE_DTS_DIR := ../dts
	IMAGES := sysupgrade.bin
	IMAGE/sysupgrade.bin = sysupgrade-tar | append-metadata
endef

include $(SUBTARGET).mk

$(eval $(call BuildImage))
