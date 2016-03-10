#
# Copyright (C) 2012-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/sound-arm-bcm2835
  TITLE:=BCM2835 ALSA driver
  KCONFIG:= \
	CONFIG_SND_ARM=y \
	CONFIG_SND_BCM2835 \
	CONFIG_SND_ARMAACI=n
  FILES:= \
	$(LINUX_DIR)/sound/arm/snd-bcm2835.ko
  AUTOLOAD:=$(call AutoLoad,68,snd-bcm2835)
  DEPENDS:=@TARGET_brcm2708
  $(call AddDepends/sound)
endef

define KernelPackage/sound-arm-bcm2835/description
  This package contains the BCM2835 ALSA pcm card driver
endef

$(eval $(call KernelPackage,sound-arm-bcm2835))


define KernelPackage/sound-soc-bcm2835-i2s
  TITLE:=SoC Audio support for the Broadcom 2835 I2S module
  KCONFIG:= \
	CONFIG_SND_BCM2835_SOC_I2S \
	CONFIG_SND_SOC_DMAENGINE_PCM=y \
	CONFIG_SND_SOC_GENERIC_DMAENGINE_PCM=y
  FILES:= \
	$(LINUX_DIR)/sound/soc/bcm/snd-soc-bcm2835-i2s.ko
  AUTOLOAD:=$(call AutoLoad,68,snd-soc-bcm2835-i2s)
  DEPENDS:=@TARGET_brcm2708 +kmod-regmap +kmod-sound-soc-core
  $(call AddDepends/sound)
endef

define KernelPackage/sound-soc-bcm2835-i2s/description
  This package contains support for codecs attached to the Broadcom 2835 I2S interface
endef

$(eval $(call KernelPackage,sound-soc-bcm2835-i2s))

define KernelPackage/sound-soc-hifiberry-dac
  TITLE:=Support for HifiBerry DAC
  KCONFIG:= \
	CONFIG_SND_BCM2708_SOC_HIFIBERRY_DAC \
	CONFIG_SND_SOC_PCM5102A
  FILES:= \
	$(LINUX_DIR)/sound/soc/bcm/snd-soc-hifiberry-dac.ko \
	$(LINUX_DIR)/sound/soc/codecs/snd-soc-pcm5102a.ko
  AUTOLOAD:=$(call AutoLoad,68,snd-soc-pcm5102a snd-soc-hifiberry-dac)
  DEPENDS:= \
	kmod-sound-soc-bcm2835-i2s \
	+kmod-i2c-bcm2708
  $(call AddDepends/sound)
endef

define KernelPackage/sound-soc-hifiberry-dac/description
  This package contains support for HifiBerry DAC
endef

$(eval $(call KernelPackage,sound-soc-hifiberry-dac))

define KernelPackage/sound-soc-hifiberry-dacplus
  TITLE:=Support for HifiBerry DAC+ / DAC+ Pro
  KCONFIG:= \
	CONFIG_SND_BCM2708_SOC_HIFIBERRY_DACPLUS \
	CONFIG_SND_SOC_PCM512x
  FILES:= \
	$(LINUX_DIR)/drivers/clk/clk-hifiberry-dacpro.ko \
	$(LINUX_DIR)/sound/soc/bcm/snd-soc-hifiberry-dacplus.ko \
	$(LINUX_DIR)/sound/soc/codecs/snd-soc-pcm512x.ko
  AUTOLOAD:=$(call AutoLoad,68,clk-hifiberry-dacpro snd-soc-pcm512x snd-soc-hifiberry-dacplus)
  DEPENDS:= \
	kmod-sound-soc-bcm2835-i2s \
	+kmod-i2c-bcm2708
  $(call AddDepends/sound)
endef

define KernelPackage/sound-soc-hifiberry-dacplus/description
  This package contains support for HifiBerry DAC+ / DAC+ Pro
endef

$(eval $(call KernelPackage,sound-soc-hifiberry-dacplus))

define KernelPackage/sound-soc-hifiberry-digi
  TITLE:=Support for HifiBerry Digi
  KCONFIG:= \
	CONFIG_SND_BCM2708_SOC_HIFIBERRY_DIGI \
	CONFIG_SND_SOC_WM8804
  FILES:= \
	$(LINUX_DIR)/sound/soc/bcm/snd-soc-hifiberry-digi.ko \
	$(LINUX_DIR)/sound/soc/codecs/snd-soc-wm8804.ko
  AUTOLOAD:=$(call AutoLoad,68,snd-soc-wm8804 snd-soc-hifiberry-digi)
  DEPENDS:= \
	kmod-sound-soc-bcm2835-i2s \
	+kmod-i2c-bcm2708
  $(call AddDepends/sound)
endef

define KernelPackage/sound-soc-hifiberry-digi/description
  This package contains support for HifiBerry Digi
endef

$(eval $(call KernelPackage,sound-soc-hifiberry-digi))

define KernelPackage/sound-soc-hifiberry-amp
  TITLE:=Support for HifiBerry Amp
  KCONFIG:= \
	CONFIG_SND_BCM2708_SOC_HIFIBERRY_AMP \
	CONFIG_SND_SOC_TAS5713
  FILES:= \
	$(LINUX_DIR)/sound/soc/bcm/snd-soc-hifiberry-amp.ko \
	$(LINUX_DIR)/sound/soc/codecs/snd-soc-tas5713.ko
  AUTOLOAD:=$(call AutoLoad,68,snd-soc-tas5713 snd-soc-hifiberry-amp)
  DEPENDS:= \
	kmod-sound-soc-bcm2835-i2s \
	+kmod-i2c-bcm2708
  $(call AddDepends/sound)
endef

define KernelPackage/sound-soc-hifiberry-amp/description
  This package contains support for HifiBerry Amp
endef

$(eval $(call KernelPackage,sound-soc-hifiberry-amp))

define KernelPackage/sound-soc-rpi-dac
  TITLE:=Support for RPi-DAC
  KCONFIG:= \
	CONFIG_SND_BCM2708_SOC_RPI_DAC \
	CONFIG_SND_SOC_PCM1794A
  FILES:= \
	$(LINUX_DIR)/sound/soc/bcm/snd-soc-rpi-dac.ko \
	$(LINUX_DIR)/sound/soc/codecs/snd-soc-pcm1794a.ko
  AUTOLOAD:=$(call AutoLoad,68,snd-soc-pcm1794a snd-soc-rpi-dac)
  DEPENDS:= \
	kmod-sound-soc-bcm2835-i2s \
	+kmod-i2c-bcm2708
  $(call AddDepends/sound)
endef

define KernelPackage/sound-soc-rpi-dac/description
  This package contains support for RPi-DAC
endef

$(eval $(call KernelPackage,sound-soc-rpi-dac))

define KernelPackage/sound-soc-rpi-proto
  TITLE:=Support for RPi-PROTO
  KCONFIG:= \
	CONFIG_SND_BCM2708_SOC_RPI_PROTO \
	CONFIG_SND_SOC_WM8731
  FILES:= \
	$(LINUX_DIR)/sound/soc/bcm/snd-soc-rpi-proto.ko \
	$(LINUX_DIR)/sound/soc/codecs/snd-soc-wm8731.ko
  AUTOLOAD:=$(call AutoLoad,68,snd-soc-wm8731 snd-soc-rpi-proto)
  DEPENDS:= \
	kmod-sound-soc-bcm2835-i2s \
	+kmod-i2c-bcm2708
  $(call AddDepends/sound)
endef

define KernelPackage/sound-soc-rpi-proto/description
  This package contains support for RPi-PROTO
endef

$(eval $(call KernelPackage,sound-soc-rpi-proto))

define KernelPackage/sound-soc-iqaudio-dac
  TITLE:=Support for IQaudIO-DAC
  KCONFIG:= \
	CONFIG_SND_BCM2708_SOC_IQAUDIO_DAC \
	CONFIG_SND_SOC_PCM512x \
	CONFIG_SND_SOC_PCM512x_I2C
  FILES:= \
	$(LINUX_DIR)/sound/soc/bcm/snd-soc-iqaudio-dac.ko \
	$(LINUX_DIR)/sound/soc/codecs/snd-soc-pcm512x.ko \
	$(LINUX_DIR)/sound/soc/codecs/snd-soc-pcm512x-i2c.ko
  AUTOLOAD:=$(call AutoLoad,68,snd-soc-pcm512x snd-soc-pcm512x-i2c snd-soc-iqaudio-dac)
  DEPENDS:= \
	kmod-sound-soc-bcm2835-i2s \
	+kmod-i2c-bcm2708
  $(call AddDepends/sound)
endef

define KernelPackage/sound-soc-iqaudio-dac/description
  This package contains support for IQaudIO-DAC
endef

$(eval $(call KernelPackage,sound-soc-iqaudio-dac))

define KernelPackage/sound-soc-raspidac3
  TITLE:=Support for RaspiDAC Rev.3x
  KCONFIG:= \
	CONFIG_SND_BCM2708_SOC_RASPIDAC3 \
	CONFIG_SND_SOC_PCM512x \
	CONFIG_SND_SOC_PCM512x_I2C \
	CONFIG_SND_SOC_TPA6130A2
  FILES:= \
	$(LINUX_DIR)/sound/soc/bcm/snd-soc-raspidac3.ko \
	$(LINUX_DIR)/sound/soc/codecs/snd-soc-pcm512x.ko \
	$(LINUX_DIR)/sound/soc/codecs/snd-soc-pcm512x-i2c.ko \
	$(LINUX_DIR)/sound/soc/codecs/snd-soc-tpa6130a2.ko
  AUTOLOAD:=$(call AutoLoad,68,snd-soc-pcm512x snd-soc-pcm512x-i2c snd-soc-tpa6130a2 snd-soc-raspidac3)
  DEPENDS:= \
	kmod-sound-soc-bcm2835-i2s \
	+kmod-i2c-bcm2708
  $(call AddDepends/sound)
endef

define KernelPackage/sound-soc-raspidac3/description
  This package contains support for RaspiDAC Rev.3x
endef

$(eval $(call KernelPackage,sound-soc-raspidac3))


define KernelPackage/random-bcm2835
  SUBMENU:=$(OTHER_MENU)
  TITLE:=BCM2835 HW Random Number Generator
  KCONFIG:=CONFIG_HW_RANDOM_BCM2835
  FILES:=$(LINUX_DIR)/drivers/char/hw_random/bcm2835-rng.ko
  AUTOLOAD:=$(call AutoLoad,11,bcm2835-rng)
  DEPENDS:=@TARGET_brcm2708 +kmod-random-core
endef

define KernelPackage/random-bcm2835/description
  This package contains the Broadcom 2835 HW random number generator driver
endef

$(eval $(call KernelPackage,random-bcm2835))


define KernelPackage/smi-bcm2835
  SUBMENU:=$(OTHER_MENU)
  TITLE:=BCM2835 SMI driver
  KCONFIG:=CONFIG_BCM2835_SMI
  FILES:=$(LINUX_DIR)/drivers/misc/bcm2835_smi.ko
  AUTOLOAD:=$(call AutoLoad,20,bcm2835_smi)
  DEPENDS:=@TARGET_brcm2708
endef

define KernelPackage/smi-bcm2835/description
  This package contains the Character device driver for Broadcom Secondary
  Memory Interface
endef

$(eval $(call KernelPackage,smi-bcm2835))

define KernelPackage/smi-bcm2835-dev
  SUBMENU:=$(OTHER_MENU)
  TITLE:=BCM2835 SMI device driver
  KCONFIG:=CONFIG_BCM2835_SMI_DEV
  FILES:=$(LINUX_DIR)/drivers/char/broadcom/bcm2835_smi_dev.ko
  AUTOLOAD:=$(call AutoLoad,21,bcm2835_smi_dev)
  DEPENDS:=@TARGET_brcm2708 +kmod-smi-bcm2835
endef

define KernelPackage/smi-bcm2835-dev/description
  This driver provides a character device interface (ioctl + read/write) to
  Broadcom's Secondary Memory interface. The low-level functionality is provided
  by the SMI driver itself.
endef

$(eval $(call KernelPackage,smi-bcm2835-dev))


define KernelPackage/spi-bcm2835
  SUBMENU:=$(SPI_MENU)
  TITLE:=BCM2835 SPI controller driver
  KCONFIG:=\
    CONFIG_SPI=y \
    CONFIG_SPI_BCM2835 \
    CONFIG_SPI_MASTER=y
  FILES:=$(LINUX_DIR)/drivers/spi/spi-bcm2835.ko
  AUTOLOAD:=$(call AutoLoad,89,spi-bcm2835)
  DEPENDS:=@TARGET_brcm2708
endef

define KernelPackage/spi-bcm2835/description
  This package contains the Broadcom 2835 SPI master controller driver
endef

$(eval $(call KernelPackage,spi-bcm2835))

define KernelPackage/spi-bcm2835-aux
  SUBMENU:=$(SPI_MENU)
  TITLE:=BCM2835 Aux SPI controller driver
  KCONFIG:=\
    CONFIG_SPI=y \
    CONFIG_SPI_BCM2835AUX \
    CONFIG_SPI_MASTER=y
  FILES:=$(LINUX_DIR)/drivers/spi/spi-bcm2835aux.ko
  AUTOLOAD:=$(call AutoLoad,89,spi-bcm2835aux)
  DEPENDS:=@TARGET_brcm2708
endef

define KernelPackage/spi-bcm2835-aux/description
  This package contains the Broadcom 2835 Aux SPI master controller driver
endef

$(eval $(call KernelPackage,spi-bcm2835-aux))


define KernelPackage/hwmon-bcm2835
  TITLE:=BCM2835 HWMON driver
  KCONFIG:=CONFIG_SENSORS_BCM2835
  FILES:=$(LINUX_DIR)/drivers/hwmon/bcm2835-hwmon.ko
  AUTOLOAD:=$(call AutoLoad,60,bcm2835-hwmon)
  $(call AddDepends/hwmon,@TARGET_brcm2708)
endef

define KernelPackage/hwmon-bcm2835/description
  Kernel module for BCM2835 thermal monitor chip
endef

$(eval $(call KernelPackage,hwmon-bcm2835))


I2C_BCM2708_MODULES:=\
  CONFIG_I2C_BCM2708:drivers/i2c/busses/i2c-bcm2708

define KernelPackage/i2c-bcm2708
  $(call i2c_defaults,$(I2C_BCM2708_MODULES),59)
  TITLE:=Broadcom BCM2708 I2C master controller driver
  KCONFIG+= \
	CONFIG_I2C_BCM2708_BAUDRATE=100000
  DEPENDS:=@TARGET_brcm2708 +kmod-i2c-core
endef

define KernelPackage/i2c-bcm2708/description
  This package contains the Broadcom 2708 I2C master controller driver
endef

$(eval $(call KernelPackage,i2c-bcm2708))

I2C_BCM2835_MODULES:=\
  CONFIG_I2C_BCM2835:drivers/i2c/busses/i2c-bcm2835

define KernelPackage/i2c-bcm2835
  $(call i2c_defaults,$(I2C_BCM2835_MODULES),59)
  TITLE:=Broadcom BCM2835 I2C master controller driver
  DEPENDS:=@TARGET_brcm2708 +kmod-i2c-core
endef

define KernelPackage/i2c-bcm2835/description
  This package contains the Broadcom 2835 I2C master controller driver
endef

$(eval $(call KernelPackage,i2c-bcm2835))


define KernelPackage/video-bcm2835
  TITLE:=Broadcom BCM2835 camera interface driver
  KCONFIG:= \
	CONFIG_VIDEO_BCM2835=y \
	CONFIG_VIDEO_BCM2835_MMAL
  FILES:= $(LINUX_DIR)/drivers/media/platform/bcm2835/bcm2835-v4l2.ko
  AUTOLOAD:=$(call AutoLoad,65,bcm2835-v4l2)
  $(call AddDepends/video,@TARGET_brcm2708 +kmod-video-videobuf2)
endef

define KernelPackage/video-bcm2835/description
  This is a V4L2 driver for the Broadcom 2835 MMAL camera host interface
endef

$(eval $(call KernelPackage,video-bcm2835))
