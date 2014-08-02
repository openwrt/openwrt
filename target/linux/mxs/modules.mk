#
# Copyright (C) 2013 OpenWrt.org
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

define KernelPackage/usb-chipidea-imx
    TITLE:=Support for ChipIdea controllers on Freescale i.MX SoCs
    DEPENDS:=+kmod-usb-chipidea @TARGET_mxs
    FILES:= \
	$(LINUX_DIR)/drivers/usb/chipidea/ci_hdrc_imx.ko \
	$(LINUX_DIR)/drivers/usb/chipidea/usbmisc_imx.ko
    AUTOLOAD:=$(call AutoLoad,52,usbmisc_imx ci_hdrc_imx,1)
    $(call AddDepends/usb)
endef

define KernelPackage/usb-chipidea-imx/description
    Kernel support for USB ChipIdea controllers on Freescale i.MX SoCs
endef

$(eval $(call KernelPackage,usb-chipidea-imx,1))

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
    TITLE:=LRADC driver for i.MX23/28
    DEPENDS:=@TARGET_mxs
    KCONFIG:=CONFIG_MXS_LRADC
    FILES:=$(LINUX_DIR)/drivers/staging/iio/adc/mxs-lradc.ko \
	$(LINUX_DIR)/drivers/iio/industrialio-triggered-buffer.ko
    AUTOLOAD:=$(call AutoLoad,70,industrialio-triggered-buffer mxs-lradc)
endef

define KernelPackage/iio-mxs-lradc/description
    Kernel module for i.MX23/28 LRADC driver
endef

$(eval $(call KernelPackage,iio-mxs-lradc))

define KernelPackage/crypto-hw-dcp
    TITLE:=i.MX23/28 DCP hardware crypto module
    DEPENDS:=@TARGET_mxs
    KCONFIG:=CONFIG_CRYPTO_DEV_MXS_DCP
    FILES:=$(LINUX_DIR)/drivers/crypto/mxs-dcp.ko
    AUTOLOAD:=$(call AutoLoad,90,mxs-dcp)
    $(call AddDepends/crypto,+kmod-crypto-authenc +kmod-crypto-des)
endef

define KernelPackage/crypto-hw-dcp/description
    Kernel support for the i.MX23/28 DCP crypto engine
endef

$(eval $(call KernelPackage,crypto-hw-dcp))
