MMEDIA_MENU:=Multimedia

define KernelPackage/multimedia
  SUBMENU:=$(MMEDIA_MENU)
  TITLE:=Multimedia
  KCONFIG:= \
	CONFIG_MEDIA_SUPPORT=m \
	CONFIG_MEDIA_SUPPORT_FILTER=y \
	CONFIG_MEDIA_USB_SUPPORT=y
  DEPENDS:=@USB_SUPPORT
  FILES:=$(LINUX_DIR)/drivers/media/mc/mc.ko
  AUTOLOAD:=$(call AutoLoad,80,mc)
endef
define KernelPackage/multimedia/description
  Kernel module for Multimedia support (cameras, tv tuners)
endef
$(eval $(call KernelPackage,multimedia))

define KernelPackage/dvb-usb-v2
  SUBMENU:=$(MMEDIA_MENU)
  TITLE:=USB DVB support
  KCONFIG:= \
	CONFIG_MEDIA_DIGITAL_TV_SUPPORT=y \
	CONFIG_MEDIA_ANALOG_TV_SUPPORT=y \
	CONFIG_DVB_CORE=m \
	CONFIG_DVB_USB_V2=m
  DEPENDS:=+kmod-i2c-mux +kmod-usb-core +kmod-multimedia
  FILES:= \
	$(LINUX_DIR)/drivers/media/dvb-core/dvb-core.ko \
	$(LINUX_DIR)/drivers/media/usb/dvb-usb-v2/dvb_usb_v2.ko
  AUTOLOAD:=$(call AutoLoad,81,dvb-core dvb_usb_v2)
endef
define KernelPackage/dvb-usb-v2/description
  Kernel module for USB DVB support
endef
$(eval $(call KernelPackage,dvb-usb-v2))

define KernelPackage/dvb-usb-rtl28xx
  SUBMENU:=$(MMEDIA_MENU)
  TITLE:=DVB USB Realtek RTL28xx
  KCONFIG:= \
	CONFIG_DVB_USB_RTL28XXU=m \
	CONFIG_DVB_RTL2830=m \
	CONFIG_DVB_RTL2832=m
  DEPENDS:=+kmod-regmap-i2c +kmod-dvb-usb-v2
  FILES:= \
	$(LINUX_DIR)/drivers/media/usb/dvb-usb-v2/dvb-usb-rtl28xxu.ko \
	$(LINUX_DIR)/drivers/media/dvb-frontends/rtl2830.ko \
	$(LINUX_DIR)/drivers/media/dvb-frontends/rtl2832.ko
  AUTOLOAD:=$(call AutoLoad,82,dvb-usb-rtl28xxu rtl2830 rtl2832)
endef
define KernelPackage/dvb-usb-rtl28xx/description
  Kernel modules for RTL28xxU USB DVB demod and RTL2830 & RTL2832 DVB-T frontends
endef
$(eval $(call KernelPackage,dvb-usb-rtl28xx))

define KernelPackage/dvb-tuner-r820t
  SUBMENU:=$(MMEDIA_MENU)
  TITLE:=DVB Tuner Rafael Micro R820T
  KCONFIG:=CONFIG_MEDIA_TUNER_R820T=m
  DEPENDS:=+kmod-i2c-core +kmod-multimedia
  FILES:=$(LINUX_DIR)/drivers/media/tuners/r820t.ko
  AUTOLOAD:=$(call AutoLoad,82,r820t)
endef
define KernelPackage/dvb-tuner-r820t/description
  Kernel module for Rafael Micro R820T tuner
endef
$(eval $(call KernelPackage,dvb-tuner-r820t))

define KernelPackage/dvb-frontend-mn88472
  SUBMENU:=$(MMEDIA_MENU)
  TITLE:=DVB-T Panasonic MN88472 (no firmware)
  KCONFIG:=CONFIG_DVB_MN88472=m
  DEPENDS:=+kmod-regmap-i2c +kmod-dvb-usb-v2
  FILES:=$(LINUX_DIR)/drivers/media/dvb-frontends/mn88472.ko
  AUTOLOAD:=$(call AutoLoad,82,mn88472)
endef
define KernelPackage/dvb-frontend-mn88472/description
  Kernel module for Panasonic MN88472 DVB-T/T2/C frontend

  This chip requires firmware! It is not included due to missing licence.
  Firmware file: dvb-demod-mn88472-02.fw
  Install in: /lib/firmware
endef
$(eval $(call KernelPackage,dvb-frontend-mn88472))

define KernelPackage/dvb-frontend-mn88473
  SUBMENU:=$(MMEDIA_MENU)
  TITLE:=DVB-T Panasonic MN88473 (no firmware)
  KCONFIG:=CONFIG_DVB_MN88473=m
  DEPENDS:=+kmod-regmap-i2c +kmod-dvb-usb-v2
  FILES:=$(LINUX_DIR)/drivers/media/dvb-frontends/mn88473.ko
  AUTOLOAD:=$(call AutoLoad,82,mn88473)
endef
define KernelPackage/dvb-frontend-mn88473/description
  Kernel module for Panasonic MN88473 DVB-T/T2/C frontend

  This chip requires firmware! It is not included due to missing licence.
  Firmware file: dvb-demod-mn88473-01.fw
  Install in: /lib/firmware
endef
$(eval $(call KernelPackage,dvb-frontend-mn88473))

define KernelPackage/dvb-frontend-cxd2841er
  SUBMENU:=$(MMEDIA_MENU)
  TITLE:=DVB-C Sony CXD2841ER
  KCONFIG:=CONFIG_DVB_CXD2841ER=m
  DEPENDS:=+kmod-i2c-core +kmod-dvb-usb-v2
  FILES:=$(LINUX_DIR)/drivers/media/dvb-frontends/cxd2841er.ko
  AUTOLOAD:=$(call AutoLoad,82,cxd2841er)
endef
define KernelPackage/dvb-frontend-cxd2841er/description
  Kernel module for Sony CXD2841ER DVB-C frontend
endef
$(eval $(call KernelPackage,dvb-frontend-cxd2841er))
