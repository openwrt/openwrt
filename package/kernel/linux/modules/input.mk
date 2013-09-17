#
# Copyright (C) 2006-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

INPUT_MODULES_MENU:=Input modules

define KernelPackage/hid
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=HID Devices
  KCONFIG:=CONFIG_HID CONFIG_HIDRAW=y
  FILES:=$(LINUX_DIR)/drivers/hid/hid.ko
  AUTOLOAD:=$(call AutoLoad,61,hid)
  $(call AddDepends/input,+kmod-input-evdev)
endef

define KernelPackage/hid/description
 Kernel modules for HID devices
endef

$(eval $(call KernelPackage,hid))

define KernelPackage/hid-generic
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=Generic HID device support
  KCONFIG:=CONFIG_HID_GENERIC
  FILES:=$(LINUX_DIR)/drivers/hid/hid-generic.ko
  AUTOLOAD:=$(call AutoProbe,hid-generic)
  $(call AddDepends/hid)
endef

define KernelPackage/hid/description
 Kernel modules for generic HID device (e.g. keyboards and mice) support
endef

$(eval $(call KernelPackage,hid-generic))

define KernelPackage/input-core
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=Input device core
  KCONFIG:=CONFIG_INPUT
  FILES:=$(LINUX_DIR)/drivers/input/input-core.ko
endef

define KernelPackage/input-core/description
 Kernel modules for support of input device
endef

$(eval $(call KernelPackage,input-core))


define KernelPackage/input-evdev
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=Input event device
  KCONFIG:=CONFIG_INPUT_EVDEV
  FILES:=$(LINUX_DIR)/drivers/input/evdev.ko
  AUTOLOAD:=$(call AutoLoad,60,evdev)
  $(call AddDepends/input)
endef

define KernelPackage/input-evdev/description
 Kernel modules for support of input device events
endef

$(eval $(call KernelPackage,input-evdev))


define KernelPackage/input-gpio-keys
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=GPIO key support
  DEPENDS:= @GPIO_SUPPORT
  KCONFIG:= \
	CONFIG_KEYBOARD_GPIO \
	CONFIG_INPUT_KEYBOARD=y
  FILES:=$(LINUX_DIR)/drivers/input/keyboard/gpio_keys.ko
  AUTOLOAD:=$(call AutoProbe,gpio_keys)
  $(call AddDepends/input)
endef

define KernelPackage/input-gpio-keys/description
 This driver implements support for buttons connected
 to GPIO pins of various CPUs (and some other chips).
endef

$(eval $(call KernelPackage,input-gpio-keys))


define KernelPackage/input-gpio-keys-polled
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=Polled GPIO key support
  DEPENDS:=@GPIO_SUPPORT +kmod-input-polldev
  KCONFIG:= \
	CONFIG_KEYBOARD_GPIO_POLLED \
	CONFIG_INPUT_KEYBOARD=y
  FILES:=$(LINUX_DIR)/drivers/input/keyboard/gpio_keys_polled.ko
  AUTOLOAD:=$(call AutoProbe,gpio_keys_polled,1)
  $(call AddDepends/input)
endef

define KernelPackage/input-gpio-keys-polled/description
 Kernel module for support polled GPIO keys input device
endef

$(eval $(call KernelPackage,input-gpio-keys-polled))


define KernelPackage/input-gpio-encoder
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=GPIO rotay encoder
  KCONFIG:=CONFIG_INPUT_GPIO_ROTARY_ENCODER
  FILES:=$(LINUX_DIR)/drivers/input/misc/rotary_encoder.ko
  AUTOLOAD:=$(call AutoProbe,rotary_encoder)
  $(call AddDepends/input,@GPIO_SUPPORT)
endef

define KernelPackage/gpio-encoder/description
 Kernel module to use rotary encoders connected to GPIO pins
endef

$(eval $(call KernelPackage,input-gpio-encoder))


define KernelPackage/input-joydev
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=Joystick device support
  KCONFIG:=CONFIG_INPUT_JOYDEV
  FILES:=$(LINUX_DIR)/drivers/input/joydev.ko
  AUTOLOAD:=$(call AutoProbe,joydev)
  $(call AddDepends/input)
endef

define KernelPackage/input-joydev/description
 Kernel module for joystick support
endef

$(eval $(call KernelPackage,input-joydev))


define KernelPackage/input-polldev
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=Polled Input device support
  KCONFIG:=CONFIG_INPUT_POLLDEV
  FILES:=$(LINUX_DIR)/drivers/input/input-polldev.ko
  $(call AddDepends/input)
endef

define KernelPackage/input-polldev/description
 Kernel module for support of polled input devices
endef

$(eval $(call KernelPackage,input-polldev))


define KernelPackage/input-matrixkmap
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=Input matrix devices support
  KCONFIG:=CONFIG_INPUT_MATRIXKMAP
  DEPENDS:=@!LINUX_3_3
  FILES:=$(LINUX_DIR)/drivers/input/matrix-keymap.ko
  AUTOLOAD:=$(call AutoLoad,20,matrix-keymap)
  $(call AddDepends/input)
endef

define KernelPackage/input-matrix/description
 Kernel module support for input matrix devices
endef

$(eval $(call KernelPackage,input-matrixkmap))


define KernelPackage/acpi-button
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=ACPI Button Support
  DEPENDS:=@(TARGET_x86_generic||TARGET_x86_kvm_guest||TARGET_x86_xen_domu) +kmod-input-evdev
  KCONFIG:=CONFIG_ACPI_BUTTON
  FILES:=$(LINUX_DIR)/drivers/acpi/button.ko
  AUTOLOAD:=$(call AutoLoad,06,button)
endef

define KernelPackage/acpi-button/description
 Kernel module for ACPI Button support
endef

$(eval $(call KernelPackage,acpi-button))
