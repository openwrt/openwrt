define KernelPackage/i2c-bcm-iproc
  TITLE:=Broadcom iProc I2C controller
  KCONFIG:=CONFIG_I2C_BCM_IPROC
  DEPENDS:=@TARGET_bcm5862x +kmod-i2c-core
  SUBMENU:=$(I2C_MENU)
  FILES:=$(LINUX_DIR)/drivers/i2c/busses/i2c-bcm-iproc.ko
  AUTOLOAD:=$(call AutoLoad,59,i2c-bcm-iproc,1)
endef

define KernelPackage/i2c-bcm-iproc/description
 Kernel module for the Broadcom iProc I2C controller.
endef

$(eval $(call KernelPackage,i2c-bcm-iproc))

define KernelPackage/hwmon-bcm59111
  TITLE:=Broadcom BCM59111 PSE support
  KCONFIG:=CONFIG_SENSORS_BCM59111
  DEPENDS:=@TARGET_bcm5862x +kmod-i2c-core
  SUBMENU:=$(HWMON_MENU)
  FILES:=$(LINUX_DIR)/drivers/hwmon/bcm_poe.ko
  AUTOLOAD:=$(call AutoLoad,60,bcm_poe,1)
endef

define KernelPackage/hwmon-bcm59111/description
  Kernel module for the Broadcom BCM59111 PSE controller.
endef

$(eval $(call KernelPackage,hwmon-bcm59111))
