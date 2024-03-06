# SPDX-License-Identifier: GPL-2.0-only

define Device/longcheer-l8150
	DEVICE_VENDOR := Longcheer
	DEVICE_MODEL := L8150
	SOC := msm8916
	ROOT_BLKDEV := "/dev/mmcblk0p31"
endef
TARGET_DEVICES += longcheer-l8150

define Device/thwc-uf896
	DEVICE_VENDOR := THWC
	DEVICE_MODEL := uf896
	SOC := msm8916
	ROOT_BLKDEV := "/dev/mmcblk0p27"
endef
TARGET_DEVICES += thwc-uf896
