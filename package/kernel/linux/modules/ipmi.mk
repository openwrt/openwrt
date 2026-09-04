#
# Copyright (C) 2022 TDT AG <development@tdt.de>
#
# This is free software, licensed under the GNU General Public License v2.
# See https://www.gnu.org/licenses/gpl-2.0.txt for more information
#

IPMI_MENU:=IPMI system interface support

define KernelPackage/ipmi-handler
  SUBMENU:=$(IPMI_MENU)
  TITLE:=IPMI top-level message handler
  KCONFIG:=CONFIG_IPMI_HANDLER \
           CONFIG_ACPI_IPMI \
           CONFIG_IPMI_PANIC_EVENT=y \
           CONFIG_IPMI_PANIC_STRING=y \
           CONFIG_IPMI_DEVICE_INTERFACE=n \
           CONFIG_IPMI_SI=n \
           CONFIG_IPMI_SSIF=n \
           CONFIG_IPMI_WATCHDOG=n \
           CONFIG_IPMI_POWEROFF=n \
           CONFIG_SENSORS_IBMAEM=n \
           CONFIG_SENSORS_IBMPEX=n
  FILES:=$(LINUX_DIR)/drivers/char/ipmi/ipmi_msghandler.ko
  AUTOLOAD:=$(call AutoProbe,ipmi_msghandler)
endef

define KernelPackage/ipmi-handler/description
  This enables the central IPMI message handler, required for IPMI to work.

  IPMI is a standard for managing sensors in a system.
  (e.g. temperature, voltage, power supply, etc.)
endef

$(eval $(call KernelPackage,ipmi-handler))


define KernelPackage/ipmi-si
  SUBMENU:=$(IPMI_MENU)
  TITLE:=IPMI System Interface handler
  DEPENDS:=+kmod-ipmi-handler
  KCONFIG:=CONFIG_IPMI_SI
  FILES:=$(LINUX_DIR)/drivers/char/ipmi/ipmi_si.ko
  AUTOLOAD:=$(call AutoProbe,ipmi_si)
endef

define KernelPackage/ipmi-si/description
  Provides a driver for System Interfaces.
  If you are using IPMI, you should probably say "y" here.
endef

$(eval $(call KernelPackage,ipmi-si))


define KernelPackage/ipmi-device-interface
  SUBMENU:=$(IPMI_MENU)
  TITLE:=Device interface for IPMI
  DEPENDS:=+kmod-ipmi-handler
  KCONFIG:=CONFIG_IPMI_DEVICE_INTERFACE
  FILES:=$(LINUX_DIR)/drivers/char/ipmi/ipmi_devintf.ko
  AUTOLOAD:=$(call AutoProbe,ipmi_devintf)
endef

define KernelPackage/ipmi-device-interface/description
  This provides an IOCTL interface to the IPMI message handler so
  userland processes may use IPMI.
endef

$(eval $(call KernelPackage,ipmi-device-interface))
