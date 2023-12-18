# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

MENU_TITLE:=Multiplexer Support

define KernelPackage/mux-core
  SUBMENU:=$(MENU_TITLE)
  TITLE:=Multiplexer Support
  KCONFIG:=CONFIG_MULTIPLEXER
  FILES:=$(LINUX_DIR)/drivers/mux/mux-core.ko
  AUTOLOAD:=$(call AutoLoad,25,mux-core,1)
endef

define KernelPackage/mux-core/description
  Kernel module for multiplexer support
endef

$(eval $(call KernelPackage,mux-core))

define KernelPackage/mux-gpio
  SUBMENU:=$(MENU_TITLE)
  TITLE:=GPIO-controlled Multiplexer controller
  KCONFIG:=CONFIG_MUX_GPIO
  DEPENDS:=@GPIO_SUPPORT kmod-mux-core
  FILES:=$(LINUX_DIR)/drivers/mux/mux-gpio.ko
  AUTOLOAD:=$(call AutoLoad,25,mux-gpio,1)
endef

define KernelPackage/mux-gpio/description
  Kernel modules for GPIO-controlled Multiplexer controller
endef

$(eval $(call KernelPackage,mux-gpio))

define KernelPackage/mux-mmio
  SUBMENU:=$(OTHER_MENU)
  TITLE:=MMIO/Regmap register bitfield-controlled Multiplexer controller
  DEPENDS:=+kmod-mux-core
  KCONFIG:=CONFIG_MUX_MMIO
  FILES:=$(LINUX_DIR)/drivers/mux/mux-mmio.ko
  AUTOLOAD:=$(call AutoProbe,mux-mmio)
endef

define KernelPackage/mux-mmio/description
  Kernel module for the MMIO/Regmap multiplexer driver.
endef

$(eval $(call KernelPackage,mux-mmio))
