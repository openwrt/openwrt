#
# Copyright (C) 2013-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

define KernelPackage/rtc-stmp3xxx
  SUBMENU:=$(OTHER_MENU)
  TITLE:=STMP3xxx SoC built-in RTC support
  DEPENDS:=@TARGET_mxs
  $(call AddDepends/rtc)
  KCONFIG:= \
	CONFIG_RTC_CLASS=y \
	CONFIG_RTC_DRV_STMP=m
  FILES:=$(LINUX_DIR)/drivers/rtc/rtc-stmp3xxx.ko
  AUTOLOAD:=$(call AutoLoad,50,rtc-stmp3xxx)
endef

$(eval $(call KernelPackage,rtc-stmp3xxx))

define KernelPackage/wdt-stmp3xxx
  SUBMENU:=$(OTHER_MENU)
  TITLE:=STMP3xxx Watchdog timer
  DEPENDS:=kmod-rtc-stmp3xxx
  KCONFIG:=CONFIG_STMP3XXX_RTC_WATCHDOG
  FILES:=$(LINUX_DIR)/drivers/$(WATCHDOG_DIR)/stmp3xxx_rtc_wdt.ko
  AUTOLOAD:=$(call AutoLoad,51,stmp3xxx_rtc_wdt)
endef

define KernelPackage/wdt-stmp3xxx/description
  Kernel module for STMP3xxx watchdog timer.
endef

$(eval $(call KernelPackage,wdt-stmp3xxx))

define KernelPackage/sound-soc-mxs
  TITLE:=Freescale i.MX23/i.MX28 built-in SoC sound support
  KCONFIG:= \
	CONFIG_SND_SOC_MXS_BUILTIN_CODEC \
	CONFIG_SND_MXS_SOC_BUILTIN
  FILES:= \
	$(LINUX_DIR)/sound/soc/mxs/snd-soc-mxs-builtin-audio.ko \
	$(LINUX_DIR)/sound/soc/mxs/snd-soc-mxs-builtin-dai.ko \
	$(LINUX_DIR)/sound/soc/mxs/snd-soc-mxs-builtin-pcm.ko \
	$(LINUX_DIR)/sound/soc/codecs/snd-soc-mxs-builtin-codec.ko
  AUTOLOAD:=$(call AutoLoad,65,snd-soc-mxs-builtin-pcm snd-soc-mxs-builtin-dai snd-soc-mxs-builtin-codec snd-soc-mxs-builtin-audio)
  DEPENDS:=@TARGET_mxs +kmod-sound-soc-core
  $(call AddDepends/sound)
endef

define KernelPackage/sound-soc-mxs/description
  Kernel support for Freescale i.MX23/i.MX28 built-in SoC audio
endef

$(eval $(call KernelPackage,sound-soc-mxs))

define KernelPackage/iio-mxs-lradc
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Freescale i.MX23/28 LRADC driver
  DEPENDS:=@TARGET_mxs +kmod-iio-core
  KCONFIG:=CONFIG_MXS_LRADC
  FILES:=$(LINUX_DIR)/drivers/staging/iio/adc/mxs-lradc.ko 
  AUTOLOAD:=$(call AutoLoad,70,mxs-lradc)
endef

define KernelPackage/iio-mxs-lradc/description
  Kernel module for Freescale i.MX23/28 LRADC driver
endef

$(eval $(call KernelPackage,iio-mxs-lradc))

define KernelPackage/crypto-hw-dcp
  TITLE:=Freescale i.MX23/28 DCP hardware crypto module
  DEPENDS:=@TARGET_mxs
  KCONFIG:=CONFIG_CRYPTO_DEV_MXS_DCP
  FILES:=$(LINUX_DIR)/drivers/crypto/mxs-dcp.ko
  AUTOLOAD:=$(call AutoLoad,90,mxs-dcp)
  $(call AddDepends/crypto,+kmod-crypto-authenc +kmod-crypto-des)
endef

define KernelPackage/crypto-hw-dcp/description
  Kernel support for Freescale i.MX23/28 DCP crypto engine
endef

$(eval $(call KernelPackage,crypto-hw-dcp))

define KernelPackage/spi-mxs
  SUBMENU:=$(SPI_MENU)
  TITLE:=Freescale i.MX23/28 SPI driver
  DEPENDS:=@TARGET_mxs
  KCONFIG:=CONFIG_SPI_MXS
  FILES:=$(LINUX_DIR)/drivers/spi/spi-mxs.ko
  AUTOLOAD:=$(call AutoProbe,spi-mxs)
endef

define KernelPackage/spi-mxs/description
  Kernel module for Freescale i.MX23/28 SPI controller
endef

$(eval $(call KernelPackage,spi-mxs))

I2C_MXS_MODULES:= \
  CONFIG_I2C_MXS:drivers/i2c/busses/i2c-mxs

define KernelPackage/i2c-mxs
  $(call i2c_defaults,$(I2C_MXS_MODULES),55)
  TITLE:=Freescale i.MX23/28 I2C driver
  DEPENDS:=@TARGET_mxs +kmod-i2c-core
endef

define KernelPackage/i2c-mxs/description
  Kernel module for Freescale i.MX23/28 I2C controller
endef

$(eval $(call KernelPackage,i2c-mxs))
