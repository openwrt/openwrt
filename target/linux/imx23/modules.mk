#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

define KernelPackage/wdt-stmp3xxx
    SUBMENU:=$(OTHER_MENU)
    TITLE:=STMP3xxx Watchdog timer
    DEPENDS:=@(TARGET_imx23)
    KCONFIG:=CONFIG_STMP3XXX_RTC_WATCHDOG
    FILES:=$(LINUX_DIR)/drivers/$(WATCHDOG_DIR)/stmp3xxx_rtc_wdt.ko
    AUTOLOAD:=$(call AutoLoad,50,stmp3xxx_rtc_wdt)
endef

define KernelPackage/wdt-stmp3xxx/description
    Kernel module for STMP3xxx watchdog timer.
endef

$(eval $(call KernelPackage,wdt-stmp3xxx))
define KernelPackage/usb-chipidea
    TITLE:=Support for ChipIdea controllers
    DEPENDS:=+kmod-usb2 +kmod-usb-mxs-phy
    KCONFIG:= \
	CONFIG_USB_CHIPIDEA \
	CONFIG_USB_CHIPIDEA_HOST=y \
	CONFIG_USB_CHIPIDEA_DEBUG=y
    FILES:=\
	$(LINUX_DIR)/drivers/usb/chipidea/ci_hdrc.ko \
	$(LINUX_DIR)/drivers/usb/chipidea/ci13xxx_imx.ko
    AUTOLOAD:=$(call AutoLoad,51,ci_hdrc ci13xxx_imx,1)
    $(call AddDepends/usb)
endef

define KernelPackage/usb-chipidea/description
    Kernel support for USB ChipIdea controllers
endef

$(eval $(call KernelPackage,usb-chipidea,1))

define KernelPackage/usb-mxs-phy
    TITLE:=Support for Freescale MXS USB PHY controllers
    DEPENDS:=+kmod-usb2
    KCONFIG:= \
	CONFIG_USB_MXS_PHY
	FILES:=$(LINUX_DIR)/drivers/usb/otg/mxs-phy.ko
    AUTOLOAD:=$(call AutoLoad,50,mxs-phy,1)
    $(call AddDepends/usb)
endef

define KernelPackage/usb-mxs-phy/description
    Kernel support for Freescale MXS USB PHY controllers
endef

$(eval $(call KernelPackage,usb-mxs-phy,1))

define KernelPackage/usb-net-smsc95xx
    TITLE:=SMSC95xx USB/2.0 Ethernet driver
    KCONFIG:=CONFIG_USB_NET_SMSC95XX
    FILES:=$(LINUX_DIR)/drivers/net/usb/smsc95xx.ko
    AUTOLOAD:=$(call AutoLoad,64,smsc95xx)
    $(call AddDepends/usb-net)
endef

define KernelPackage/usb-net-smsc95xx/description
    Kernel support for SMSC95xx USB/2.0 Ethernet driver
endef

$(eval $(call KernelPackage,usb-net-smsc95xx))


