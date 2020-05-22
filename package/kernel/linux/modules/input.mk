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
  DEPENDS:=+kmod-input-core +kmod-input-evdev
  KCONFIG:=CONFIG_HID CONFIG_HIDRAW=y CONFIG_HID_BATTERY_STRENGTH=y
  FILES:=$(LINUX_DIR)/drivers/hid/hid.ko
  AUTOLOAD:=$(call AutoLoad,61,hid)
endef

define KernelPackage/hid/description
 Kernel modules for HID devices
endef

$(eval $(call KernelPackage,hid))

define KernelPackage/hid-generic
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=Generic HID device support
  DEPENDS:=+kmod-hid
  KCONFIG:=CONFIG_HID_GENERIC
  FILES:=$(LINUX_DIR)/drivers/hid/hid-generic.ko
  AUTOLOAD:=$(call AutoProbe,hid-generic)
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
  DEPENDS:=+kmod-input-core
  KCONFIG:=CONFIG_INPUT_EVDEV
  FILES:=$(LINUX_DIR)/drivers/input/evdev.ko
  AUTOLOAD:=$(call AutoLoad,60,evdev)
endef

define KernelPackage/input-evdev/description
 Kernel modules for support of input device events
endef

$(eval $(call KernelPackage,input-evdev))


define KernelPackage/input-gpio-keys
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=GPIO key support
  DEPENDS:= @GPIO_SUPPORT +kmod-input-core
  KCONFIG:= \
	CONFIG_KEYBOARD_GPIO \
	CONFIG_INPUT_KEYBOARD=y
  FILES:=$(LINUX_DIR)/drivers/input/keyboard/gpio_keys.ko
  AUTOLOAD:=$(call AutoProbe,gpio_keys,1)
endef

define KernelPackage/input-gpio-keys/description
 This driver implements support for buttons connected
 to GPIO pins of various CPUs (and some other chips).

 See also gpio-button-hotplug which is an alternative, lower overhead
 implementation that generates uevents instead of kernel input events.
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
endef

define KernelPackage/input-gpio-keys-polled/description
 Kernel module for support polled GPIO keys input device

 See also gpio-button-hotplug which is an alternative, lower overhead
 implementation that generates uevents instead of kernel input events.
endef

$(eval $(call KernelPackage,input-gpio-keys-polled))


define KernelPackage/input-gpio-encoder
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=GPIO rotary encoder
  DEPENDS:=@GPIO_SUPPORT +kmod-input-core
  KCONFIG:=CONFIG_INPUT_GPIO_ROTARY_ENCODER
  FILES:=$(LINUX_DIR)/drivers/input/misc/rotary_encoder.ko
  AUTOLOAD:=$(call AutoProbe,rotary_encoder)
endef

define KernelPackage/input-gpio-encoder/description
 Kernel module to use rotary encoders connected to GPIO pins
endef

$(eval $(call KernelPackage,input-gpio-encoder))


define KernelPackage/input-joydev
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=Joystick device support
  DEPENDS:=+kmod-input-core
  KCONFIG:=CONFIG_INPUT_JOYDEV
  FILES:=$(LINUX_DIR)/drivers/input/joydev.ko
  AUTOLOAD:=$(call AutoProbe,joydev)
endef

define KernelPackage/input-joydev/description
 Kernel module for joystick support
endef

$(eval $(call KernelPackage,input-joydev))


define KernelPackage/input-polldev
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=Polled Input device support
  DEPENDS:=+kmod-input-core
  KCONFIG:=CONFIG_INPUT_POLLDEV
  FILES:=$(LINUX_DIR)/drivers/input/input-polldev.ko
endef

define KernelPackage/input-polldev/description
 Kernel module for support of polled input devices
endef

$(eval $(call KernelPackage,input-polldev))


define KernelPackage/input-matrixkmap
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=Input matrix devices support
  DEPENDS:=+kmod-input-core
  KCONFIG:=CONFIG_INPUT_MATRIXKMAP
  FILES:=$(LINUX_DIR)/drivers/input/matrix-keymap.ko
  AUTOLOAD:=$(call AutoProbe,matrix-keymap)
endef

define KernelPackage/input-matrixkmap/description
 Kernel module support for input matrix devices
endef

$(eval $(call KernelPackage,input-matrixkmap))


define KernelPackage/input-touchscreen-ads7846
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=ADS7846/TSC2046/AD7873 and AD(S)7843 based touchscreens
  DEPENDS:=+kmod-hwmon-core +kmod-input-core +kmod-spi-bitbang
  KCONFIG:= \
	CONFIG_INPUT_TOUCHSCREEN=y \
	CONFIG_TOUCHSCREEN_PROPERTIES=y \
	CONFIG_TOUCHSCREEN_ADS7846
  FILES:=$(LINUX_DIR)/drivers/input/touchscreen/ads7846.ko \
	$(LINUX_DIR)/drivers/input/touchscreen/of_touchscreen.ko@ge5.4
  AUTOLOAD:=$(call AutoProbe,ads7846)
endef

define KernelPackage/input-touchscreen-ads7846/description
  Kernel module for ADS7846/TSC2046/AD7873 and AD(S)7843 based touchscreens
endef

$(eval $(call KernelPackage,input-touchscreen-ads7846))


define KernelPackage/multimedia-input
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=Multimedia input support
  KCONFIG:=CONFIG_RC_CORE \
	CONFIG_LIRC=y \
	CONFIG_RC_DECODERS=y \
	CONFIG_RC_DEVICES=y
  FILES:=$(LINUX_DIR)/drivers/media/rc/rc-core.ko
  AUTOLOAD:=$(call AutoProbe,rc-core)
endef

define KernelPackage/multimedia-input/description
 Enable multimedia input.
endef

$(eval $(call KernelPackage,multimedia-input))


define KernelPackage/infrared-sensor
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=infrared sensor support
  DEPENDS:=+kmod-multimedia-input
  KCONFIG:= \
	CONFIG_GPIOLIB=y \
	CONFIG_SPI=y \
	CONFIG_PWM=y \
	CONFIG_IR_GPIO_CIR \
	CONFIG_BPF_LIRC_MODE2 \
	CONFIG_IR_SANYO_DECODER \
	CONFIG_IR_SHARP_DECODER \
	CONFIG_IR_MCE_KBD_DECODER \
	CONFIG_IR_NEC_DECODER \
	CONFIG_IR_XMP_DECODER \
	CONFIG_IR_IMON_DECODER \
	CONFIG_IR_RCMM_DECODER \
	CONFIG_IR_IMON_RAW \
	CONFIG_IR_SPI \
	CONFIG_IR_GPIO_TX \
	CONFIG_IR_PWM_TX \
	CONFIG_IR_SERIAL \
	CONFIG_IR_SERIAL_TRANSMITTER=y \
	CONFIG_IR_SIR \
	CONFIG_RC_MAP \
	CONFIG_RC_XBOX_DVD
  FILES:= \
	$(LINUX_DIR)/drivers/media/rc/gpio-ir-recv.ko \
	$(LINUX_DIR)/drivers/media/rc/gpio-ir-tx.ko \
	$(LINUX_DIR)/drivers/media/rc/imon_raw.ko \
	$(LINUX_DIR)/drivers/media/rc/ir-imon-decoder.ko \
	$(LINUX_DIR)/drivers/media/rc/ir-mce_kbd-decoder.ko \
	$(LINUX_DIR)/drivers/media/rc/ir-nec-decoder.ko \
	$(LINUX_DIR)/drivers/media/rc/ir-rcmm-decoder.ko \
	$(LINUX_DIR)/drivers/media/rc/ir-sanyo-decoder.ko \
	$(LINUX_DIR)/drivers/media/rc/ir-sharp-decoder.ko \
	$(LINUX_DIR)/drivers/media/rc/ir-spi.ko \
	$(LINUX_DIR)/drivers/media/rc/ir-xmp-decoder.ko \
	$(LINUX_DIR)/drivers/media/rc/pwm-ir-tx.ko \
	$(LINUX_DIR)/drivers/media/rc/serial_ir.ko \
	$(LINUX_DIR)/drivers/media/rc/sir_ir.ko \
	$(LINUX_DIR)/drivers/media/rc/xbox_remote.ko
  AUTOLOAD:=$(call AutoProbe,gpio-ir-recv gpio-ir-tx imon_raw ir-imon-decoder \
	ir-mce_kbd-decoder ir-nec-decoder ir-rcmm-decoder ir-sanyo-decoder \
	ir-sharp-decoder ir-spi ir-xmp-decoder pwm-ir-tx sir_ir xbox_remote)
endef

define KernelPackage/infrared-sensor/description
 Enable support for infrared sensors.
endef

$(eval $(call KernelPackage,infrared-sensor))


define KernelPackage/keyboard-imx
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=IMX keypad support
  DEPENDS:=@(TARGET_mxs||TARGET_imx6) +kmod-input-matrixkmap
  KCONFIG:= \
	CONFIG_KEYBOARD_IMX \
	CONFIG_INPUT_KEYBOARD=y
  FILES:=$(LINUX_DIR)/drivers/input/keyboard/imx_keypad.ko
  AUTOLOAD:=$(call AutoProbe,imx_keypad)
endef

define KernelPackage/keyboard-imx/description
 Enable support for IMX keypad port.
endef

$(eval $(call KernelPackage,keyboard-imx))

define KernelPackage/input-uinput
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=user input module
  DEPENDS:=+kmod-input-core
  KCONFIG:= \
	CONFIG_INPUT_MISC=y \
	CONFIG_INPUT_UINPUT
  FILES:=$(LINUX_DIR)/drivers/input/misc/uinput.ko
  AUTOLOAD:=$(call AutoProbe,uinput)
endef

define KernelPackage/input-uinput/description
  user input modules needed for bluez
endef

$(eval $(call KernelPackage,input-uinput))
