#
# Copyright (C) 2013-2016 OpenWrt.org
# Copyright (C) 2016 Yousong Zhou
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
ifeq ($(SUBTARGET),cortexa53)

define Device/sun50i-h5-nanopi-neo-plus2
  $(call Device/FitImageLzma64)
  DEVICE_TITLE:=Nanopi NEO Plus2 (H5)
  SUPPORTED_DEVICES:=nanopi-neo-plus2
endef

TARGET_DEVICES += sun50i-h5-nanopi-neo-plus2

define Device/sun50i-h5-nanopi-neo2
  $(call Device/FitImageLzma64)
  DEVICE_TITLE:=Nanopi NEO2 (H5)
  SUPPORTED_DEVICES:=nanopi-neo2
endef

TARGET_DEVICES += sun50i-h5-nanopi-neo2

define Device/sun50i-a64-pine64-plus
  $(call Device/FitImageLzma64)
  DEVICE_TITLE:=Pine64 Plus A64
  SUPPORTED_DEVICES:=pine64,pine64-plus
endef

TARGET_DEVICES += sun50i-a64-pine64-plus

define Device/sun50i-a64-sopine-baseboard
  $(call Device/FitImageLzma64)
  DEVICE_TITLE:=Pine64 Sopine
  SUPPORTED_DEVICES:=pine64,sopine-baseboard
endef

TARGET_DEVICES += sun50i-a64-sopine-baseboard

define Device/sun50i-h5-orangepi-zero-plus
  $(call Device/FitImageLzma64)
  DEVICE_TITLE:=Xunlong Orange Pi Zero Plus
  SUPPORTED_DEVICES:=xunlong,orangepi-zero-plus
endef

TARGET_DEVICES += sun50i-h5-orangepi-zero-plus

define Device/sun50i-h5-orangepi-pc2
  $(call Device/FitImageLzma64)
  DEVICE_TITLE:=Xunlong Orange Pi PC2
  SUPPORTED_DEVICES:=xunlong,orangepi-pc2
endef

TARGET_DEVICES += sun50i-h5-orangepi-pc2

endif
