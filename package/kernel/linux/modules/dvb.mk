#
# Copyright (C) 2018 CZ.NIC, z. s. p. o. <https://www.nic.cz/>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

DVB_MENU:=DVB Support
VIDEO_MENU:=Video Support

CURRENTLY_UNSUPPORTED:= \
CONFIG_VIDEO_AU0828=n \
CONFIG_DVB_USB_UMT_010=n \
CONFIG_DVB_USB_M920X=n \
CONFIG_DVB_USB_DIGITV=n \
CONFIG_DVB_USB_VP7045=n \
CONFIG_DVB_USB_VP702X=n \
CONFIG_DVB_USB_GP8PSK=n \
CONFIG_DVB_USB_NOVA_T_USB2=n \
CONFIG_DVB_USB_DTT200U=n \
CONFIG_DVB_USB_OPERA1=n \
CONFIG_DVB_USB_AF9005=n \
CONFIG_DVB_USB_PCTV452E=n \
CONFIG_DVB_USB_DW2102=n \
CONFIG_DVB_USB_CINERGY_T2=n \
CONFIG_DVB_USB_DTV5100=n \
CONFIG_DVB_USB_FRIIO=n \
CONFIG_DVB_USB_AZ6027=n \
CONFIG_DVB_USB_TECHNISAT_USB2=n \
CONFIG_DVB_USB_AU6610=n \
CONFIG_DVB_USB_CE6230=n \
CONFIG_DVB_USB_EC168=n \
CONFIG_DVB_USB_GL861=n \
CONFIG_DVB_USB_LME2510=n \
CONFIG_DVB_USB_MXL111SF=n \
CONFIG_DVB_USB_ZD1301=n \
CONFIG_DVB_TTUSB_BUDGET=n \
CONFIG_DVB_TTUSB_DEC=n \
CONFIG_SMS_USB_DRV=n \
CONFIG_DVB_B2C2_FLEXCOP_USB=n \
CONFIG_DVB_AS102=n \
CONFIG_MEDIA_TUNER_M88RS6000T=n \
CONFIG_MEDIA_TUNER_MXL301RF=n \
CONFIG_MEDIA_TUNER_QM1D1C0042=n \
CONFIG_DVB_STB0899=n \
CONFIG_DVB_STB6100=n \
CONFIG_DVB_STV090x=n \
CONFIG_DVB_STV0910=n \
CONFIG_DVB_STV6110x=n \
CONFIG_DVB_STV6111=n \
CONFIG_DVB_MXL5XX=n \
CONFIG_DVB_TDA18271C2DD=n \
CONFIG_DVB_SI2165=n \
CONFIG_DVB_CX24110=n \
CONFIG_DVB_CX24123=n \
CONFIG_DVB_MT312=n \
CONFIG_DVB_ZL10036=n \
CONFIG_DVB_ZL10039=n \
CONFIG_DVB_S5H1420=n \
CONFIG_DVB_STV0288=n \
CONFIG_DVB_STB6000=n \
CONFIG_DVB_STV0299=n \
CONFIG_DVB_STV6110=n \
CONFIG_DVB_STV0900=n \
CONFIG_DVB_TDA8083=n \
CONFIG_DVB_TDA10086=n \
CONFIG_DVB_TDA8261=n \
CONFIG_DVB_VES1X93=n \
CONFIG_DVB_TUNER_ITD1000=n \
CONFIG_DVB_TUNER_CX24113=n \
CONFIG_DVB_TDA826X=n \
CONFIG_DVB_TUA6100=n \
CONFIG_DVB_CX24116=n \
CONFIG_DVB_CX24117=n \
CONFIG_DVB_CX24120=n \
CONFIG_DVB_SI21XX=n \
CONFIG_DVB_DS3000=n \
CONFIG_DVB_MB86A16=n \
CONFIG_DVB_TDA10071=n \
CONFIG_DVB_SP8870=n \
CONFIG_DVB_SP887X=n \
CONFIG_DVB_CX22700=n \
CONFIG_DVB_CX22702=n \
CONFIG_DVB_S5H1432=n \
CONFIG_DVB_DRXD=n \
CONFIG_DVB_L64781=n \
CONFIG_DVB_TDA1004X=n \
CONFIG_DVB_NXT6000=n \
CONFIG_DVB_MT352=n \
CONFIG_DVB_EC100=n \
CONFIG_DVB_STV0367=n \
CONFIG_DVB_CXD2820R=n \
CONFIG_DVB_CXD2841ER=n \
CONFIG_DVB_ZD1301_DEMOD=n \
CONFIG_DVB_VES1820=n \
CONFIG_DVB_TDA10021=n \
CONFIG_DVB_STV0297=n \
CONFIG_DVB_NXT200X=n \
CONFIG_DVB_OR51211=n \
CONFIG_DVB_OR51132=n \
CONFIG_DVB_BCM3510=n \
CONFIG_DVB_LGDT330X=n \
CONFIG_DVB_LGDT3305=n \
CONFIG_DVB_LGDT3306A=n \
CONFIG_DVB_LG2160=n \
CONFIG_DVB_S5H1409=n \
CONFIG_DVB_AU8522_DTV=n \
CONFIG_DVB_S5H1411=n \
CONFIG_DVB_S921=n \
CONFIG_DVB_MB86A20S=n \
CONFIG_DVB_TC90522=n \
CONFIG_DVB_DRX39XYJ=n \
CONFIG_DVB_LNBH25=n \
CONFIG_DVB_LNBP21=n \
CONFIG_DVB_LNBP22=n \
CONFIG_DVB_ISL6405=n \
CONFIG_DVB_ISL6421=n \
CONFIG_DVB_ISL6423=n \
CONFIG_DVB_A8293=n \
CONFIG_DVB_SP2=n \
CONFIG_DVB_LGS8GL5=n \
CONFIG_DVB_LGS8GXX=n \
CONFIG_DVB_ATBM8830=n \
CONFIG_DVB_TDA665x=n \
CONFIG_DVB_IX2505V=n \
CONFIG_DVB_M88RS2000=n \
CONFIG_DVB_HORUS3A=n \
CONFIG_DVB_ASCOT2E=n \
CONFIG_DVB_HELENE=n \
CONFIG_DVB_FIREDTV=n \
CONFIG_MEDIA_TUNER_MSI001=n \
CONFIG_SMS_SDIO_DRV=n

# ------------------------------ core drivers ---------------------------------

define KernelPackage/rc-core
  SUBMENU:=$(DVB_MENU)
  TITLE:=Remote Controller support
  KCONFIG:= \
	CONFIG_MEDIA_SUPPORT=m \
	CONFIG_MEDIA_RC_SUPPORT=y \
	CONFIG_RC_DEVICES=y \
	CONFIG_IR_SERIAL=n \
	CONFIG_IR_SIR=n \
	CONFIG_RC_CORE
  FILES:=$(LINUX_DIR)/drivers/media/rc/rc-core.ko
  AUTOLOAD:=$(call AutoProbe,rc-core)
  DEPENDS:=+kmod-input-core
endef

define KernelPackage/rc-core/description
 Enable support for Remote Controllers on Linux. This is
 needed in order to support several video capture adapters,
 standalone IR receivers/transmitters, and RF receivers.

 Enable this option if you have a video capture board even
 if you don't need IR, as otherwise, you may not be able to
 compile the driver for your adapter.
endef

$(eval $(call KernelPackage,rc-core))

define KernelPackage/dvb-core
  SUBMENU:=$(DVB_MENU)
  TITLE:=DVB core support
  KCONFIG:= $(CURRENTLY_UNSUPPORTED) \
	CONFIG_MEDIA_SUPPORT=m \
	CONFIG_MEDIA_DIGITAL_TV_SUPPORT=y \
	CONFIG_DVB_NET=y \
	CONFIG_DVB_MAX_ADAPTERS=8 \
	CONFIG_DVB_CORE \
	CONFIG_DVB_DYNAMIC_MINORS=y \
	CONFIG_DVB_DEMUX_SECTION_LOSS_LOG=y \
	CONFIG_DVB_PLATFORM_DRIVERS=n \
	CONFIG_DVB_USB_DEBUG=n
  FILES:=$(LINUX_DIR)/drivers/media/dvb-core/dvb-core.ko
  AUTOLOAD:=$(call AutoProbe,dvb-core)
endef

define KernelPackage/dvb-core/description
 Kernel modules for DVB support.
endef

$(eval $(call KernelPackage,dvb-core))

define AddDepends/dvb-core
  SUBMENU:=$(DVB_MENU)
  DEPENDS+=+kmod-dvb-core $1
endef

# ----------------------------- utility drivers -------------------------------

define KernelPackage/cypress-firmware
  SUBMENU:=$(DVB_MENU)
  TITLE:=Cypress firmware helper routines
  KCONFIG:=CONFIG_CYPRESS_FIRMWARE
  FILES:=$(LINUX_DIR)/drivers/media/common/cypress_firmware.ko
  DEPENDS:=+kmod-usb-core
  AUTOLOAD:=$(call AutoProbe,cypress-firmware)
endef

define KernelPackage/cypress-firmware/description
 Helper module for Cypress firmware download.
endef

$(eval $(call KernelPackage,cypress-firmware))

# ----------------------------- DVB USB drivers -------------------------------

define KernelPackage/dvb-usb
  SUBMENU:=$(DVB_MENU)
  TITLE:=Support for various USB DVB devices
  KCONFIG:= \
	CONFIG_MEDIA_SUPPORT=m \
	CONFIG_MEDIA_DIGITAL_TV_SUPPORT=y \
	CONFIG_MEDIA_USB_SUPPORT=y \
	CONFIG_DVB_USB
  FILES:=$(LINUX_DIR)/drivers/media/usb/dvb-usb/dvb-usb.ko
  DEPENDS:=+kmod-usb-core +kmod-i2c-core +kmod-rc-core +kmod-dvb-core
  AUTOLOAD:=$(call AutoProbe,dvb-usb)
endef

define KernelPackage/dvb-usb/description
 By enabling this you will be able to choose the various supported
 USB1.1 and USB2.0 DVB devices.

 Almost every USB device needs a firmware.

 For a complete list of supported USB devices see the LinuxTV DVB Wiki:
 <http://www.linuxtv.org/wiki/index.php/DVB_USB>
endef

$(eval $(call KernelPackage,dvb-usb))

define AddDepends/dvb-usb
  SUBMENU:=$(DVB_MENU)
  DEPENDS+=+kmod-dvb-usb $1
endef

define KernelPackage/dvb-usb-dib0700
  TITLE:=DiBcom DiB0700 USB DVB devices
  KCONFIG:=CONFIG_DVB_USB_DIB0700
  FILES:=$(LINUX_DIR)/drivers/media/usb/dvb-usb/dvb-usb-dib0700.ko
  AUTOLOAD:=$(call AutoProbe,dvb-usb-dib0700)
  DEPENDS:= \
	+PACKAGE_kmod-dvb-tuner-dib0070:kmod-dvb-tuner-dib0070 \
	+PACKAGE_kmod-dvb-tuner-dib0090:kmod-dvb-tuner-dib0090 \
	@(PACKAGE_kmod-dvb-dib3000mc||PACKAGE_kmod-dvb-dib7000m||PACKAGE_kmod-dvb-dib7000p||PACKAGE_kmod-dvb-dib8000||PACKAGE_kmod-dvb-dib9000) \
	+PACKAGE_kmod-dvb-dib3000mc:kmod-dvb-dib3000mc \
	+PACKAGE_kmod-dvb-dib7000m:kmod-dvb-dib7000m \
	+PACKAGE_kmod-dvb-dib7000p:kmod-dvb-dib7000p \
	+PACKAGE_kmod-dvb-dib8000:kmod-dvb-dib8000 \
	+PACKAGE_kmod-dvb-dib9000:kmod-dvb-dib9000
  $(call AddDepends/dvb-usb)
endef

define KernelPackage/dvb-usb-dib0700/description
 Support for USB2.0/1.1 DVB receivers based on the DiB0700 USB bridge. The
 USB bridge is also present in devices having the DiB7700 DVB-T-USB
 silicon. This chip can be found in devices offered by Hauppauge,
 Avermedia and other big and small companies.

 For an up-to-date list of devices supported by this driver, have a look
 on the LinuxTV Wiki at www.linuxtv.org.

 Say Y if you own such a device and want to use it. You should build it as
 a module.
endef

$(eval $(call KernelPackage,dvb-usb-dib0700))

define KernelPackage/dvb-usb-dibusb-common
  TITLE:=Common library for DiBcom USB drivers
  FILES:=$(LINUX_DIR)/drivers/media/usb/dvb-usb/dvb-usb-dibusb-common.ko \
         $(LINUX_DIR)/drivers/media/usb/dvb-usb/dvb-usb-dibusb-mc-common.ko
  AUTOLOAD:=$(call AutoProbe,dvb-usb-dibusb-common dvb-usb-dibusb-mc-common)
  $(call AddDepends/dvb-usb,+PACKAGE_kmod-dvb-dib3000mc:kmod-dvb-dib3000mc)
endef

$(eval $(call KernelPackage,dvb-usb-dibusb-common))

define KernelPackage/dvb-usb-a800
  TITLE:=AVerMedia AverTV DVB-T USB 2.0 (A800)
  KCONFIG:=CONFIG_DVB_USB_A800
  FILES:=$(LINUX_DIR)/drivers/media/usb/dvb-usb/dvb-usb-a800.ko
  AUTOLOAD:=$(call AutoProbe,dvb-usb-a800)
  $(call AddDepends/dvb-usb,+kmod-dvb-dib3000mc +kmod-dvb-usb-dibusb-common)
endef

define KernelPackage/dvb-usb-a800/description
 Say Y here to support the AVerMedia AverTV DVB-T USB 2.0 (A800) receiver.
endef

$(eval $(call KernelPackage,dvb-usb-a800))

define KernelPackage/dvb-usb-dibusb-mb
  TITLE:=DiBcom USB DVB-T devices (DiB3000M-B based)
  KCONFIG:= \
	CONFIG_DVB_USB_DIBUSB_MB_FAULTY=y \
	CONFIG_DVB_USB_DIBUSB_MB
  FILES:=$(LINUX_DIR)/drivers/media/usb/dvb-usb/dvb-usb-dibusb-mb.ko
  AUTOLOAD:=$(call AutoProbe,dvb-usb-dibusb-mb)
  $(call AddDepends/dvb-usb,+kmod-dvb-dib3000mb +kmod-dvb-usb-dibusb-common)
endef

define KernelPackage/dvb-usb-dibusb-mb/description
 Support for USB 1.1 and 2.0 DVB-T receivers based on reference designs made by
 DiBcom (<http://www.dibcom.fr>) equipped with a DiB3000M-B demodulator.

 For an up-to-date list of devices supported by this driver, have a look
 on the Linux-DVB Wiki at www.linuxtv.org.

 Say Y if you own such a device and want to use it. You should build it as
 a module.
endef

$(eval $(call KernelPackage,dvb-usb-dibusb-mb))

define KernelPackage/dvb-usb-dibusb-mc
  TITLE:=DiBcom USB DVB-T devices (DiB3000M-C/P based)
  KCONFIG:=CONFIG_DVB_USB_DIBUSB_MC
  FILES:=$(LINUX_DIR)/drivers/media/usb/dvb-usb/dvb-usb-dibusb-mc.ko
  AUTOLOAD:=$(call AutoProbe,dvb-usb-dibusb-mc)
  $(call AddDepends/dvb-usb,+kmod-dvb-dib3000mc +kmod-dvb-usb-dibusb-common)
endef

define KernelPackage/dvb-usb-dibusb-mc/description
 Support for USB2.0 DVB-T receivers based on reference designs made by
 DiBcom (<http://www.dibcom.fr>) equipped with a DiB3000M-C/P demodulator.

 For an up-to-date list of devices supported by this driver, have a look
 on the Linux-DVB Wiki at www.linuxtv.org.

 Say Y if you own such a device and want to use it. You should build it as
 a module.
endef

$(eval $(call KernelPackage,dvb-usb-dibusb-mc))

define KernelPackage/dvb-usb-ttusb2
  TITLE:=Pinnacle 400e DVB-S USB2.0 support
  KCONFIG:=CONFIG_DVB_USB_TTUSB2
  FILES:=$(LINUX_DIR)/drivers/media/usb/dvb-usb/dvb-usb-ttusb2.ko
  AUTOLOAD:=$(call AutoProbe,dvb-usb-ttusb2)
  $(call AddDepends/dvb-usb)
endef

define KernelPackage/dvb-usb-ttusb2/description
 Say Y here to support the Pinnacle 400e DVB-S USB2.0 receiver and
 the TechnoTrend CT-3650 CI DVB-C/T USB2.0 receiver. The
 firmware protocol used by this module is similar to the one used by the
 old ttusb-driver - that's why the module is called dvb-usb-ttusb2.
endef

$(eval $(call KernelPackage,dvb-usb-ttusb2))

define KernelPackage/dvb-usb-cxusb
  TITLE:=Conexant USB2.0 hybrid reference design support
  KCONFIG:=CONFIG_DVB_USB_CXUSB
  FILES:=$(LINUX_DIR)/drivers/media/usb/dvb-usb/dvb-usb-cxusb.ko
  AUTOLOAD:=$(call AutoProbe,dvb-usb-cxusb)
  $(call AddDepends/dvb-usb, +kmod-dvb-tuner-dib0070)
endef

define KernelPackage/dvb-usb-cxusb/description
 Conexant USB2.0 hybrid reference design support
endef

$(eval $(call KernelPackage,dvb-usb-cxusb))

# --------------------------- DVB USB v2 drivers ------------------------------

define KernelPackage/dvb-usb-v2
  TITLE:=Support for various USB DVB devices v2
  KCONFIG:= \
	CONFIG_MEDIA_USB_SUPPORT=y \
	CONFIG_DVB_USB_V2
  FILES:=$(LINUX_DIR)/drivers/media/usb/dvb-usb-v2/dvb_usb_v2.ko
  AUTOLOAD:=$(call AutoProbe,dvb_usb_v2)
  DEPENDS:=+PACKAGE_kmod-rc-core:kmod-rc-core
  $(call AddDepends/dvb-core,+kmod-usb-core)
endef

define KernelPackage/dvb-usb-v2/description
 By enabling this you will be able to choose the various supported
 USB1.1 and USB2.0 DVB devices.

 Almost every USB device needs a firmware.

 For a complete list of supported USB devices see the LinuxTV DVB Wiki:
 <http://www.linuxtv.org/wiki/index.php/DVB_USB>
endef

$(eval $(call KernelPackage,dvb-usb-v2))

define AddDepends/dvb-usb-v2
  SUBMENU:=$(DVB_MENU)
  DEPENDS+=+kmod-dvb-usb-v2 $1
endef

define KernelPackage/dvb-usb-rtl28xxu
  TITLE:=Realtek RTL28xxU DVB USB support
  KCONFIG:=CONFIG_DVB_USB_RTL28XXU
  FILES:=$(LINUX_DIR)/drivers/media/usb/dvb-usb-v2/dvb-usb-rtl28xxu.ko
  AUTOLOAD:=$(call AutoProbe,dvb-usb-rtl28xxu)
  $(call AddDepends/dvb-usb-v2,+kmod-dvb-rtl2830 +kmod-dvb-rtl2832)
endef

define KernelPackage/dvb-usb-rtl28xxu/description
 Realtek RTL28xxU DVB USB support
endef

$(eval $(call KernelPackage,dvb-usb-rtl28xxu))

define KernelPackage/dvb-usb-anysee
  TITLE:=Anysee DVB-T/C USB2.0 support
  KCONFIG:=CONFIG_DVB_USB_ANYSEE
  FILES:=$(LINUX_DIR)/drivers/media/usb/dvb-usb-v2/dvb-usb-anysee.ko
  AUTOLOAD:=$(call AutoProbe,dvb-usb-anysee)
  $(call AddDepends/dvb-usb-v2)
endef

define KernelPackage/dvb-usb-anysee/description
 Anysee DVB-T/C USB2.0 support
endef

$(eval $(call KernelPackage,dvb-usb-anysee))

define KernelPackage/dvb-usb-af9015
  TITLE:=Afatech AF9015 DVB-T USB2.0 support
  KCONFIG:=CONFIG_DVB_USB_AF9015
  FILES:=$(LINUX_DIR)/drivers/media/usb/dvb-usb-v2/dvb-usb-af9015.ko
  AUTOLOAD:=$(call AutoProbe,dvb-usb-af9015)
  $(call AddDepends/dvb-usb-v2,+kmod-dvb-af9013)
endef

define KernelPackage/dvb-usb-af9015/description
 Support for the Afatech AF9015 based DVB-T USB2.0 receiver.
endef

$(eval $(call KernelPackage,dvb-usb-af9015))

define KernelPackage/dvb-usb-af9035
  TITLE:=Afatech AF9035 DVB-T USB2.0 support
  KCONFIG:=CONFIG_DVB_USB_AF9035
  FILES:=$(LINUX_DIR)/drivers/media/usb/dvb-usb-v2/dvb-usb-af9035.ko
  AUTOLOAD:=$(call AutoProbe,dvb-usb-af9035)
  $(call AddDepends/dvb-usb-v2,+kmod-dvb-af9033)
endef

define KernelPackage/dvb-usb-af9035/description
 Say Y here to support the Afatech AF9035 based DVB USB receiver.
endef

$(eval $(call KernelPackage,dvb-usb-af9035))

define KernelPackage/dvb-usb-az6007
  TITLE:=AzureWave 6007 and clones DVB-T/C USB2.0 support
  KCONFIG:=CONFIG_DVB_USB_AZ6007
  FILES:=$(LINUX_DIR)/drivers/media/usb/dvb-usb-v2/dvb-usb-az6007.ko
  AUTOLOAD:=$(call AutoProbe,dvb-usb-az6007)
  $(call AddDepends/dvb-usb-v2,+kmod-cypress-firmware)
endef

define KernelPackage/dvb-usb-az6007/description
 Say Y here to support the AZ6007 receivers like Terratec H7.
endef

$(eval $(call KernelPackage,dvb-usb-az6007))

define KernelPackage/dvb-usb-dvbsky
  TITLE:=DVBSky USB support
  KCONFIG:=CONFIG_DVB_USB_DVBSKY
  FILES:=$(LINUX_DIR)/drivers/media/usb/dvb-usb-v2/dvb-usb-dvbsky.ko
  AUTOLOAD:=$(call AutoProbe,dvb-usb-dvbsky)
  $(call AddDepends/dvb-usb-v2,+kmod-dvb-si2168 +kmod-dvb-m88ds3103)
endef

define KernelPackage/dvb-usb-dvbsky/description
 Say Y here to support the USB receivers from DVBSky.
endef

$(eval $(call KernelPackage,dvb-usb-dvbsky))

# ------------------------------ DVB frontends --------------------------------

define DvbFrontend
  SUBMENU:=$(DVB_MENU)
  KCONFIG:= \
	$2
  DEPENDS:=+kmod-i2c-core +kmod-dvb-core +kmod-i2c-mux +kmod-regmap
  FILES:=$(LINUX_DIR)/drivers/media/dvb-frontends/$1.ko
  AUTOLOAD:=$(call AutoProbe,$1)
endef

define KernelPackage/dvb-drxk
  TITLE:=Micronas DRXK based
  $(call DvbFrontend,drxk,CONFIG_DVB_DRXK)
endef

define KernelPackage/dvb-drxk/description
 Micronas DRX-K DVB-C/T demodulator.
endef

$(eval $(call KernelPackage,dvb-drxk))

define KernelPackage/dvb-pll
  TITLE:=Generic I2C PLL based tuners
  $(call DvbFrontend,dvb-pll,CONFIG_DVB_PLL)
endef

define KernelPackage/dvb-pll/description
 This module drives a number of tuners based on PLL chips with a
 common I2C interface.
endef

$(eval $(call KernelPackage,dvb-pll))

define KernelPackage/dvb-rtl2830
  TITLE:=Realtek RTL2830 DVB-T
  $(call DvbFrontend,rtl2830,CONFIG_DVB_RTL2830)
endef

define KernelPackage/dvb-rtl2830/description
 Realtek RTL2830 DVB-T
endef

$(eval $(call KernelPackage,dvb-rtl2830))

define KernelPackage/dvb-rtl2832
  TITLE:=Realtek RTL2832 DVB-T
  $(call DvbFrontend,rtl2832,CONFIG_DVB_RTL2832)
  DEPENDS+=+kmod-i2c-mux
endef

define KernelPackage/dvb-rtl2832/description
 Realtek RTL2832 DVB-T
endef

$(eval $(call KernelPackage,dvb-rtl2832))

define KernelPackage/dvb-si2168
  TITLE:=Silicon Labs Si2168
  $(call DvbFrontend,si2168,CONFIG_DVB_SI2168)
  DEPENDS+=+kmod-i2c-mux
endef

define KernelPackage/dvb-si2168/description
 Silicon Labs Si2168
endef

$(eval $(call KernelPackage,dvb-si2168))

define KernelPackage/dvb-mn88472
  TITLE:=Panasonic MN88472
  $(call DvbFrontend,mn88472,CONFIG_DVB_MN88472)
  DEPENDS+=+kmod-i2c-mux
endef

define KernelPackage/dvb-mn88472/description
  Panasonic MN88472
endef

$(eval $(call KernelPackage,dvb-mn88472))

define KernelPackage/dvb-mn88473
  TITLE:=Panasonic MN88473
  $(call DvbFrontend,mn88473,CONFIG_DVB_MN88473)
  DEPENDS+=+kmod-i2c-mux
endef

define KernelPackage/dvb-mn88473/description
  Panasonic MN88473
endef

$(eval $(call KernelPackage,dvb-mn88473))

define KernelPackage/dvb-m88ds3103
  TITLE:=Montage Technology M88DS3103
  $(call DvbFrontend,m88ds3103,CONFIG_DVB_M88DS3103)
  DEPENDS+=+kmod-i2c-mux
endef

define KernelPackage/dvb-m88ds3103/description
  Montage Technology M88DS3103
endef

$(eval $(call KernelPackage,dvb-m88ds3103))

define KernelPackage/dvb-ts2020
  TITLE:=Montage Technology TS2020
  $(call DvbFrontend,ts2020,CONFIG_DVB_TS2020)
  DEPENDS+=+kmod-i2c-mux
endef

define KernelPackage/dvb-ts2020/description
  Montage Tehnology TS2020 based tuners
endef

$(eval $(call KernelPackage,dvb-ts2020))

define KernelPackage/dvb-zl10353
  TITLE:=Zarlink ZL10353 based tuner
  $(call DvbFrontend,zl10353,CONFIG_DVB_ZL10353)
endef

define KernelPackage/dvb-zl10353/description
 A DVB-T tuner module.
endef

$(eval $(call KernelPackage,dvb-zl10353))

define KernelPackage/dvb-tda10023
  TITLE:=Philips TDA10023 based tuner
  $(call DvbFrontend,tda10023,CONFIG_DVB_TDA10023)
endef

define KernelPackage/dvb-tda10023/description
 A DVB-C tuner module.
endef

$(eval $(call KernelPackage,dvb-tda10023))

define KernelPackage/dvb-tda10048
  TITLE:=Philips TDA10048HN based tuner
  $(call DvbFrontend,tda10048,CONFIG_DVB_TDA10048)
endef

define KernelPackage/dvb-tda10048/description
 A DVB-T tuner module.
endef

$(eval $(call KernelPackage,dvb-tda10048))

define KernelPackage/dvb-af9013
  TITLE:=Afatech AF9013 demodulator
  $(call DvbFrontend,af9013,CONFIG_DVB_AF9013)
endef

define KernelPackage/dvb-af9013/description
 Support for AF9013 DVB frontend.
endef

$(eval $(call KernelPackage,dvb-af9013))

define KernelPackage/dvb-af9033
  TITLE:=Afatech AF9033 DVB-T demodulator
  $(call DvbFrontend,af9033,CONFIG_DVB_AF9033)
endef

define KernelPackage/dvb-af9033/description
 Support for AF9033 DVB frontend.
endef

$(eval $(call KernelPackage,dvb-af9033))

define KernelPackage/dvb-tuner-dib0070
  TITLE:=DiBcom DiB0070 silicon base-band tuner
  $(call DvbFrontend,dib0070,CONFIG_DVB_TUNER_DIB0070)
endef

define KernelPackage/dvb-tuner-dib0070/description
 A driver for the silicon baseband tuner DiB0070 from DiBcom.
 This device is only used inside a SiP called together with a
 demodulator for now.
endef

$(eval $(call KernelPackage,dvb-tuner-dib0070))

define KernelPackage/dvb-tuner-cxusb
  TITLE:=Conexant USB2.0 hybrid reference design support
  $(call DvbFrontend,cxusb,CONFIG_DVB_USB_CXUSB)
endef

define KernelPackage/dvb-tuner-cxusb/description
 Support the Conexant USB2.0 hybrid reference design.
 Currently, only DVB and ATSC modes are supported, analog mode
 shall be added in the future.
endef

$(eval $(call KernelPackage,dvb-tuner-dib0070))

define KernelPackage/dvb-tuner-dib0090
  TITLE:=DiBcom DiB0090 silicon base-band tuner
  $(call DvbFrontend,dib0090,CONFIG_DVB_TUNER_DIB0090)
endef

define KernelPackage/dvb-tuner-dib0090/description
 A driver for the silicon baseband tuner DiB0090 from DiBcom.
 This device is only used inside a SiP called together with a
 demodulator for now.
endef

$(eval $(call KernelPackage,dvb-tuner-dib0090))

define KernelPackage/dvb-dib3000mb
  TITLE:=DiBcom 3000M-B
  $(call DvbFrontend,dib3000mb,CONFIG_DVB_DIB3000MB)
endef

define KernelPackage/dvb-dib3000mb/description
 A DVB-T tuner module. Designed for mobile usage. Say Y when you want
 to support this frontend.
endef

$(eval $(call KernelPackage,dvb-dib3000mb))

define KernelPackage/dvb-dibx000-common
  SUBMENU:=$(DVB_MENU)
  TITLE:=Common library for DiBX000 drivers
  FILES:=$(LINUX_DIR)/drivers/media/dvb-frontends/dibx000_common.ko
  AUTOLOAD:=$(call AutoProbe,dibx000_common)
endef

$(eval $(call KernelPackage,dvb-dibx000-common))

define KernelPackage/dvb-dib3000mc
  TITLE:=DiBcom 3000P/M-C
  $(call DvbFrontend,dib3000mc,CONFIG_DVB_DIB3000MC)
  DEPENDS+=+kmod-dvb-dibx000-common
endef

define KernelPackage/dvb-dib3000mc/description
 A DVB-T tuner module. Designed for mobile usage. Say Y when you want
 to support this frontend.
endef

$(eval $(call KernelPackage,dvb-dib3000mc))

define KernelPackage/dvb-dib7000m
  TITLE:=DiBcom 7000MA/MB/PA/PB/MC
  $(call DvbFrontend,dib7000m,CONFIG_DVB_DIB7000M)
  DEPENDS+=+kmod-dvb-dibx000-common
endef

define KernelPackage/dvb-dib7000m/description
 A DVB-T tuner module. Designed for mobile usage. Say Y when you want
 to support this frontend
endef

$(eval $(call KernelPackage,dvb-dib7000m))

define KernelPackage/dvb-dib7000p
  TITLE:=DiBcom 7000PC
  $(call DvbFrontend,dib7000p,CONFIG_DVB_DIB7000P)
  DEPENDS+=+kmod-dvb-dibx000-common
endef

define KernelPackage/dvb-dib7000p/description
 A DVB-T tuner module. Designed for mobile usage. Say Y when you want
 to support this frontend.
endef

$(eval $(call KernelPackage,dvb-dib7000p))

define KernelPackage/dvb-dib8000
  TITLE:=DiBcom 8000MB/MC
  $(call DvbFrontend,dib8000,CONFIG_DVB_DIB8000)
  DEPENDS+=+kmod-dvb-dibx000-common
endef

define KernelPackage/dvb-dib8000/description
 A driver for DiBcom's DiB8000 ISDB-T/ISDB-Tsb demodulator.
 Say Y when you want to support this frontend.
endef

$(eval $(call KernelPackage,dvb-dib8000))

define KernelPackage/dvb-dib9000
  TITLE:=DiBcom 9000
  $(call DvbFrontend,dib9000,CONFIG_DVB_DIB9000)
  DEPENDS+=+kmod-dvb-dibx000-common
endef

define KernelPackage/dvb-dib9000/description
 A DVB-T tuner module. Designed for mobile usage. Say Y when you want
 to support this frontend
endef

$(eval $(call KernelPackage,dvb-dib9000))

# -----------------------------------------------------------------------------
# ------------------------------- Media tuners --------------------------------
# -----------------------------------------------------------------------------

TUNER_MENU:=Media tuners

define MediaTuner
  SUBMENU:=$(TUNER_MENU)
  KCONFIG:= \
	CONFIG_MEDIA_SUPPORT=m \
	CONFIG_MEDIA_DIGITAL_TV_SUPPORT=m \
	$2
  DEPENDS:=+kmod-i2c-core
  FILES:=$(LINUX_DIR)/drivers/media/tuners/$1.ko
  AUTOLOAD:=$(call AutoProbe,$1)
endef

define KernelPackage/media-tuner-e4000
  TITLE:=Elonics E4000 silicon tuner
  $(call MediaTuner,e4000,CONFIG_MEDIA_TUNER_E4000)
  DEPENDS+=+kmod-regmap +kmod-video-core
endef

define KernelPackage/media-tuner-e4000/description
 Elonics E4000 silicon tuner driver.
endef

$(eval $(call KernelPackage,media-tuner-e4000))

define KernelPackage/media-tuner-fc0011
  TITLE:=Fitipower FC0011 silicon tuner
  $(call MediaTuner,fc0011,CONFIG_MEDIA_TUNER_FC0011)
endef

define KernelPackage/media-tuner-fc0011/description
 Fitipower FC0011 silicon tuner driver.
endef

$(eval $(call KernelPackage,media-tuner-fc0011))

define KernelPackage/media-tuner-fc0012
  TITLE:=Fitipower FC0012 silicon tuner
  $(call MediaTuner,fc0012,CONFIG_MEDIA_TUNER_FC0012)
endef

define KernelPackage/media-tuner-fc0012/description
 Fitipower FC0012 silicon tuner driver.
endef

$(eval $(call KernelPackage,media-tuner-fc0012))

define KernelPackage/media-tuner-fc0013
  TITLE:=Fitipower FC0013 silicon tuner
  $(call MediaTuner,fc0013,CONFIG_MEDIA_TUNER_FC0013)
endef

define KernelPackage/media-tuner-fc0013/description
 Fitipower FC0013 silicon tuner driver.
endef

$(eval $(call KernelPackage,media-tuner-fc0013))

define KernelPackage/media-tuner-fc2580
  TITLE:=FCI FC2580 silicon tuner
  $(call MediaTuner,fc2580,CONFIG_MEDIA_TUNER_FC2580)
  DEPENDS+=+kmod-regmap +kmod-video-core
endef

define KernelPackage/media-tuner-fc2580/description
 FCI FC2580 silicon tuner driver.
endef

$(eval $(call KernelPackage,media-tuner-fc2580))

define KernelPackage/media-tuner-it913x
  TITLE:=ITE Tech IT913x silicon tuner
  $(call MediaTuner,it913x,CONFIG_MEDIA_TUNER_IT913X)
  DEPENDS+=+kmod-regmap
endef

define KernelPackage/media-tuner-it913x/description
 ITE Tech IT913x silicon tuner driver.
endef

$(eval $(call KernelPackage,media-tuner-it913x))

define KernelPackage/media-tuner-si2157
  TITLE:=Silicon Labs Si2157 silicon tuner
  $(call MediaTuner,si2157,CONFIG_MEDIA_TUNER_SI2157)
  DEPENDS+=+kmod-regmap
endef

define KernelPackage/media-tuner-si2157/description
 Silicon Labs Si2157 silicon tuner driver.
endef

$(eval $(call KernelPackage,media-tuner-si2157))

define KernelPackage/media-tuner-max2165
  TITLE:=Maxim MAX2165 silicon tuner
  $(call MediaTuner,max2165,CONFIG_MEDIA_TUNER_MAX2165)
endef

define KernelPackage/media-tuner-max2165/description
 A driver for the silicon tuner MAX2165 from Maxim.
endef

$(eval $(call KernelPackage,media-tuner-max2165))

define KernelPackage/media-tuner-mc44s803
  TITLE:=Freescale MC44S803 Broadband tuners
  $(call MediaTuner,mc44s803,CONFIG_MEDIA_TUNER_MC44S803)
endef

define KernelPackage/media-tuner-mc44s803/description
 Say Y here to support the Freescale MC44S803 based tuners
endef

$(eval $(call KernelPackage,media-tuner-mc44s803))

define KernelPackage/media-tuner-mt2060
  TITLE:=Microtune MT2060 silicon IF tuner
  $(call MediaTuner,mt2060,CONFIG_MEDIA_TUNER_MT2060)
endef

define KernelPackage/media-tuner-mt2060/description
 A driver for the silicon IF tuner MT2060 from Microtune.
endef

$(eval $(call KernelPackage,media-tuner-mt2060))

define KernelPackage/media-tuner-mt2063
  TITLE:=Microtune MT2063 silicon IF tuner
  $(call MediaTuner,mt2063,CONFIG_MEDIA_TUNER_MT2063)
endef

define KernelPackage/media-tuner-mt2063/description
 A driver for the silicon IF tuner MT2063 from Microtune.
endef

$(eval $(call KernelPackage,media-tuner-mt2063))

define KernelPackage/media-tuner-mt20xx
  TITLE:=Microtune 2032 / 2050 tuners
  $(call MediaTuner,mt20xx,CONFIG_MEDIA_TUNER_MT20XX)
endef

define KernelPackage/media-tuner-mt20xx/description
 Say Y here to include support for the MT2032 / MT2050 tuner.
endef

$(eval $(call KernelPackage,media-tuner-mt20xx))

define KernelPackage/media-tuner-mt2131
  TITLE:=Microtune MT2131 silicon tuner
  $(call MediaTuner,mt2131,CONFIG_MEDIA_TUNER_MT2131)
endef

define KernelPackage/media-tuner-mt2131/description
 A driver for the silicon baseband tuner MT2131 from Microtune.
endef

$(eval $(call KernelPackage,media-tuner-mt2131))

define KernelPackage/media-tuner-mt2266
  TITLE:=Microtune MT2266 silicon tuner
  $(call MediaTuner,mt2266,CONFIG_MEDIA_TUNER_MT2266)
endef

define KernelPackage/media-tuner-mt2266/description
 A driver for the silicon baseband tuner MT2266 from Microtune.
endef

$(eval $(call KernelPackage,media-tuner-mt2266))

define KernelPackage/media-tuner-mxl5005s
  TITLE:=MaxLinear MSL5005S silicon tuner
  $(call MediaTuner,mxl5005s,CONFIG_MEDIA_TUNER_MXL5005S)
endef

define KernelPackage/media-tuner-mxl5005s/description
 A driver for the silicon tuner MXL5005S from MaxLinear.
endef

$(eval $(call KernelPackage,media-tuner-mxl5005s))

define KernelPackage/media-tuner-mxl5007t
  TITLE:=MaxLinear MxL5007T silicon tuner
  $(call MediaTuner,mxl5007t,CONFIG_MEDIA_TUNER_MXL5007T)
endef

define KernelPackage/media-tuner-mxl5007t/description
 A driver for the silicon tuner MxL5007T from MaxLinear.
endef

$(eval $(call KernelPackage,media-tuner-mxl5007t))

define KernelPackage/media-tuner-qt1010
  TITLE:=Quantek QT1010 silicon tuner
  $(call MediaTuner,qt1010,CONFIG_MEDIA_TUNER_QT1010)
endef

define KernelPackage/media-tuner-qt1010/description
 A driver for the silicon tuner QT1010 from Quantek.
endef

$(eval $(call KernelPackage,media-tuner-qt1010))

define KernelPackage/media-tuner-r820t
  TITLE:=Rafael Micro R820T silicon tuner
  $(call MediaTuner,r820t,CONFIG_MEDIA_TUNER_R820T)
endef

define KernelPackage/media-tuner-r820t/description
 Rafael Micro R820T silicon tuner driver.
endef

$(eval $(call KernelPackage,media-tuner-r820t))

define KernelPackage/media-tuner-simple
  SUBMENU:=$(TUNER_MENU)
  TITLE:=Simple tuner support
  KCONFIG:= \
	CONFIG_MEDIA_SUPPORT=m \
	CONFIG_MEDIA_TUNER_SIMPLE
  DEPENDS:=+kmod-i2c-core +kmod-media-tuner-tda9887
  FILES:= \
	$(LINUX_DIR)/drivers/media/tuners/tuner-simple.ko \
	$(LINUX_DIR)/drivers/media/tuners/tuner-types.ko
  AUTOLOAD:=$(call AutoProbe,tuner-simple tuner-types)
endef

define KernelPackage/media-tuner-simple/description
 Say Y here to include support for various simple tuners.
endef

$(eval $(call KernelPackage,media-tuner-simple))

define KernelPackage/media-tuner-tda18212
  TITLE:=NXP TDA18212 silicon tuner
  $(call MediaTuner,tda18212,CONFIG_MEDIA_TUNER_TDA18212)
  DEPENDS+=+kmod-regmap
endef

define KernelPackage/media-tuner-tda18212/description
 NXP TDA18212 silicon tuner driver.
endef

$(eval $(call KernelPackage,media-tuner-tda18212))

define KernelPackage/media-tuner-tda18218
  TITLE:=NXP TDA18218 silicon tuner
  $(call MediaTuner,tda18218,CONFIG_MEDIA_TUNER_TDA18218)
endef

define KernelPackage/media-tuner-tda18218/description
 NXP TDA18218 silicon tuner driver.
endef

$(eval $(call KernelPackage,media-tuner-tda18218))

define KernelPackage/media-tuner-tda18271
  TITLE:=NXP TDA18271 silicon tuner
  $(call MediaTuner,tda18271,CONFIG_MEDIA_TUNER_TDA18271)
endef

define KernelPackage/media-tuner-tda18271/description
 A silicon tuner module. Say Y when you want to support this tuner.
endef

$(eval $(call KernelPackage,media-tuner-tda18271))

define KernelPackage/media-tuner-tda827x
  TITLE:=Philips TDA827X silicon tuner
  $(call MediaTuner,tda827x,CONFIG_MEDIA_TUNER_TDA827X)
endef

define KernelPackage/media-tuner-tda827x/description
 A DVB-T silicon tuner module. Say Y when you want to support this tuner.
endef

$(eval $(call KernelPackage,media-tuner-tda827x))

define KernelPackage/media-tuner-tda8290
  TITLE:=TDA 8290/8295 + 8275(a)/18271 tuner combo
  DEPENDS:=+kmod-media-tuner-tda827x +kmod-media-tuner-tda18271
  $(call MediaTuner,tda8290,CONFIG_MEDIA_TUNER_TDA8290)
endef

define KernelPackage/media-tuner-tda8290/description
 Say Y here to include support for Philips TDA8290+8275(a) tuner.
endef

$(eval $(call KernelPackage,media-tuner-tda8290))

define KernelPackage/media-tuner-tda9887
  TITLE:=TDA 9885/6/7 analog IF demodulator
  $(call MediaTuner,tda9887,CONFIG_MEDIA_TUNER_TDA9887)
endef

define KernelPackage/media-tuner-tda9887/description
 Say Y here to include support for Philips TDA9885/6/7
 analog IF demodulator.
endef

$(eval $(call KernelPackage,media-tuner-tda9887))

define KernelPackage/media-tuner-tea5761
  TITLE:=TEA 5761 radio tuner
  $(call MediaTuner,tea5761,CONFIG_MEDIA_TUNER_TEA5761)
endef

define KernelPackage/media-tuner-tea5761/description
 Say Y here to include support for the Philips TEA5761 radio tuner.
endef

$(eval $(call KernelPackage,media-tuner-tea5761))

define KernelPackage/media-tuner-tea5767
  TITLE:=TEA 5767 radio tuner
  $(call MediaTuner,tea5767,CONFIG_MEDIA_TUNER_TEA5767)
endef

define KernelPackage/media-tuner-tea5767/description
 Say Y here to include support for the Philips TEA5767 radio tuner.
endef

$(eval $(call KernelPackage,media-tuner-tea5767))

define KernelPackage/media-tuner-tua9001
  TITLE:=Infineon TUA 9001 silicon tuner
  $(call MediaTuner,tua9001,CONFIG_MEDIA_TUNER_TUA9001)
  DEPENDS+=+kmod-regmap
endef

define KernelPackage/media-tuner-tua9001/description
 Infineon TUA 9001 silicon tuner driver.
endef

$(eval $(call KernelPackage,media-tuner-tua9001))

define KernelPackage/media-tuner-xc2028
  TITLE:=XCeive xc2028/xc3028 tuners
  $(call MediaTuner,tuner-xc2028,CONFIG_MEDIA_TUNER_XC2028)
endef

define KernelPackage/media-tuner-xc2028/description
 Support for the xc2028/xc3028 tuners.
endef

$(eval $(call KernelPackage,media-tuner-xc2028))

define KernelPackage/media-tuner-xc4000
  TITLE:=Xceive XC4000 silicon tuner
  $(call MediaTuner,xc4000,CONFIG_MEDIA_TUNER_XC4000)
endef

define KernelPackage/media-tuner-xc4000/description
 A driver for the silicon tuner XC4000 from Xceive.
endef

$(eval $(call KernelPackage,media-tuner-xc4000))

define KernelPackage/media-tuner-xc5000
  TITLE:=Xceive XC5000 silicon tuner
  $(call MediaTuner,xc5000,CONFIG_MEDIA_TUNER_XC5000)
endef

define KernelPackage/media-tuner-xc5000/description
 A driver for the silicon tuner XC5000 from Xceive.
 This device is only used inside a SiP called together with a
 demodulator for now.
endef

$(eval $(call KernelPackage,media-tuner-xc5000))

