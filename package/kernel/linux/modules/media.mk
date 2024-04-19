#
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

MEDIA_MENU:=Multimedia support

V4L2_I2C_DIR=i2c

define AddDepends/camera-i2c
  SUBMENU:=$(MEDIA_MENU)
  DEPENDS=kmod-video-core +kmod-i2c-core $(1)
endef


define KernelPackage/cam-os04a10
  TITLE:=OmniVision OS04A10 sensor
  KCONFIG:= \
	CONFIG_VIDEO_OS04A10
  FILES:=$(LINUX_DIR)/drivers/media/$(V4L2_I2C_DIR)/os04a10.ko
  AUTOLOAD:=$(call AutoProbe,os04a10)
  $(call AddDepends/camera-i2c)
endef

define KernelPackage/cam-os04a10/description
  OminiVision OS04A10 camera sensor driver based on i2c.
endef

$(eval $(call KernelPackage,cam-os04a10))


define KernelPackage/cam-ov2640
  TITLE:=OmniVision OV2640 sensor
  KCONFIG:= \
	CONFIG_VIDEO_OV2640
  FILES:=$(LINUX_DIR)/drivers/media/$(V4L2_I2C_DIR)/ov2640.ko
  AUTOLOAD:=$(call AutoProbe,ov2640)
  $(call AddDepends/camera-i2c)
endef

define KernelPackage/cam-ov2640/description
  OminiVision OV2640 camera sensor driver based on i2c.
endef

$(eval $(call KernelPackage,cam-ov2640))


define KernelPackage/cam-ov5640
  TITLE:=OmniVision OV5640 sensor
  KCONFIG:= \
	CONFIG_VIDEO_OV5640
  FILES:=$(LINUX_DIR)/drivers/media/$(V4L2_I2C_DIR)/ov5640.ko
  AUTOLOAD:=$(call AutoProbe,ov5640)
  $(call AddDepends/camera-i2c)
endef

define KernelPackage/cam-ov5640/description
  OminiVision OV5640 camera sensor driver based on i2c.
endef

$(eval $(call KernelPackage,cam-ov5640))


define KernelPackage/cam-ov7640
  TITLE:=OmniVision OV7640 sensor
  KCONFIG:= \
	CONFIG_VIDEO_OV7640
  FILES:=$(LINUX_DIR)/drivers/media/$(V4L2_I2C_DIR)/ov7640.ko
  AUTOLOAD:=$(call AutoProbe,ov7640)
  $(call AddDepends/camera-i2c)
endef

define KernelPackage/cam-ov7640/description
  OminiVision OV7640 camera sensor driver based on i2c.
endef

$(eval $(call KernelPackage,cam-ov7640))

define KernelPackage/cam-ov9640
  TITLE:=OmniVision OV9640 sensor
  KCONFIG:= \
	CONFIG_VIDEO_OV9640
  FILES:=$(LINUX_DIR)/drivers/media/$(V4L2_I2C_DIR)/ov9640.ko
  AUTOLOAD:=$(call AutoProbe,ov9640)
  $(call AddDepends/camera-i2c)
endef

define KernelPackage/cam-ov9640/description
  OminiVision OV9640 camera sensor driver based on i2c.
endef

$(eval $(call KernelPackage,cam-ov9640))


define KernelPackage/cam-sc2336
  TITLE:=SmartSens SC2336 sensor
  KCONFIG:= \
	CONFIG_VIDEO_SC2336
  FILES:=$(LINUX_DIR)/drivers/media/$(V4L2_I2C_DIR)/sc2336.ko
  AUTOLOAD:=$(call AutoProbe,sc2336)
  $(call AddDepends/camera-i2c)
endef

define KernelPackage/cam-sc2336/description
  SmartSens SC2336 camera sensor driver based on i2c.
endef

$(eval $(call KernelPackage,cam-sc2336))


define KernelPackage/cam-sc3336
  TITLE:=SmartSens SC3336 sensor
  KCONFIG:= \
	CONFIG_VIDEO_SC3336
  FILES:=$(LINUX_DIR)/drivers/media/$(V4L2_I2C_DIR)/sc3336.ko
  AUTOLOAD:=$(call AutoProbe,sc3336)
  $(call AddDepends/camera-i2c)
endef

define KernelPackage/cam-sc3336/description
  SmartSens SC3336 camera sensor driver based on i2c.
endef

$(eval $(call KernelPackage,cam-sc3336))


define KernelPackage/cam-sc4336
  TITLE:=SmartSens SC4336 sensor
  KCONFIG:= \
	CONFIG_VIDEO_SC4336
  FILES:=$(LINUX_DIR)/drivers/media/$(V4L2_I2C_DIR)/sc4336.ko
  AUTOLOAD:=$(call AutoProbe,sc4336)
  $(call AddDepends/camera-i2c)
endef

define KernelPackage/cam-sc4336/description
  SmartSens SC4336 camera sensor driver based on i2c.
endef

$(eval $(call KernelPackage,cam-sc4336))


define KernelPackage/cam-sc530ai
  TITLE:=SmartSens SC530AI sensor
  KCONFIG:= \
	CONFIG_VIDEO_SC530AI
  FILES:=$(LINUX_DIR)/drivers/media/$(V4L2_I2C_DIR)/sc530ai.ko
  AUTOLOAD:=$(call AutoProbe,sc530ai)
  $(call AddDepends/camera-i2c)
endef

define KernelPackage/cam-sc530ai/description
  SmartSens SC530AI camera sensor driver based on i2c.
endef

$(eval $(call KernelPackage,cam-sc530ai))


define KernelPackage/cam-s5k3l6xx
  TITLE:=Samsung S5K3L6XX sensor
  KCONFIG:= \
	CONFIG_VIDEO_S5K3L6XX 
  FILES:=$(LINUX_DIR)/drivers/media/$(V4L2_I2C_DIR)/s5k3l6xx.ko
  AUTOLOAD:=$(call AutoProbe,s5k3l6xx)
  $(call AddDepends/camera-i2c)
endef

define KernelPackage/cam-s5k3l6xx/description
  Samsung S5K3L6XX camera sensor driver based on i2c.
endef

$(eval $(call KernelPackage,cam-s5k3l6xx))


define KernelPackage/cam-s5k3l6xx
  TITLE:=Samsung S5K3L6XX sensor
  KCONFIG:= \
	CONFIG_VIDEO_S5K3L6XX 
  FILES:=$(LINUX_DIR)/drivers/media/$(V4L2_I2C_DIR)/s5k3l6xx.ko
  AUTOLOAD:=$(call AutoProbe,s5k3l6xx)
  $(call AddDepends/camera-i2c)
endef

define KernelPackage/cam-s5k3l6xx/description
  Samsung S5K3L6XX camera sensor driver based on i2c.
endef

$(eval $(call KernelPackage,cam-s5k3l6xx))


define KernelPackage/cam-s5k6aafx
  TITLE:=Samsung S5K6AAFX sensor
  KCONFIG:= \
	CONFIG_VIDEO_S5K6AA 
  FILES:=$(LINUX_DIR)/drivers/media/$(V4L2_I2C_DIR)/s5k6aa.ko
  AUTOLOAD:=$(call AutoProbe,s5k6aa)
  $(call AddDepends/camera-i2c)
endef

define KernelPackage/cam-s5k6aafx/description
  Samsung S5K6AAFX camera sensor (with ISP) driver based on i2c.
endef

$(eval $(call KernelPackage,cam-s5k6aafx))


define KernelPackage/cam-s5k6a3
  TITLE:=Samsung S5K6A3 sensor
  KCONFIG:= \
	CONFIG_VIDEO_S5K6A3 
  FILES:=$(LINUX_DIR)/drivers/media/$(V4L2_I2C_DIR)/s5k6a3.ko
  AUTOLOAD:=$(call AutoProbe,s5k6a3)
  $(call AddDepends/camera-i2c)
endef

define KernelPackage/cam-s5k6a3/description
  Samsung S5K6A3 camera sensor driver based on i2c.
endef

$(eval $(call KernelPackage,cam-s5k6a3))


define KernelPackage/cam-s5k4ecgx
  TITLE:=Samsung S5K4ECGX sensor
  KCONFIG:= \
	CONFIG_VIDEO_S5K4ECGX 
  FILES:=$(LINUX_DIR)/drivers/media/$(V4L2_I2C_DIR)/s5k4ecgx.ko
  AUTOLOAD:=$(call AutoProbe,s5k4ecgx)
  $(call AddDepends/camera-i2c)
endef

define KernelPackage/cam-s5k4ecgx/description
  Samsung S5K4ECGX camera sensor (with ISP) driver based on i2c.
endef

$(eval $(call KernelPackage,cam-s5k4ecgx))


define KernelPackage/cam-s5k5baf
  TITLE:=Samsung S5K5BAF sensor
  KCONFIG:= \
	CONFIG_VIDEO_S5K5BAF 
  FILES:=$(LINUX_DIR)/drivers/media/$(V4L2_I2C_DIR)/s5k5baf.ko
  AUTOLOAD:=$(call AutoProbe,s5k5baf)
  $(call AddDepends/camera-i2c)
endef

define KernelPackage/cam-s5k5baf/description
  Samsung S5K5BAF camera sensor (with ISP) driver based on i2c.
endef

$(eval $(call KernelPackage,cam-s5k5baf))


define KernelPackage/cam-s5kjn1
  TITLE:=Samsung S5KJN1 sensor
  KCONFIG:= \
	CONFIG_VIDEO_S5KJN1 
  FILES:=$(LINUX_DIR)/drivers/media/$(V4L2_I2C_DIR)/s5kjn1.ko
  AUTOLOAD:=$(call AutoProbe,s5kjn1)
  $(call AddDepends/camera-i2c)
endef

define KernelPackage/cam-s5kjn1/description
  Samsung S5KJN1 camera sensor driver based on i2c.
endef

$(eval $(call KernelPackage,cam-s5kjn1))
