TPM_MENU:=TPM

define KernelPackage/tpm
  TITLE:= tpm support
  SUBMENU:=$(TPM_MENU)
  KCONFIG:= \
    CONFIG_TCG_TPM=y
  FILES:= \
    $(LINUX_DIR)/drivers/char/tpm/tpm.ko
  AUTOLOAD:= $(call AutoProbe, tpm.ko)
  DEPENDS:=kmod-i2c-core
endef

define KernelPackage/tpm/description
  TPM support
endef

$(eval $(call KernelPackage,tpm))


define KernelPackage/tpm-i2c-infineon
  TITLE:= infineon i2c tpm driver
  SUBMENU:=$(TPM_MENU)
  KCONFIG:= \
    CONFIG_TCG_TIS_I2C_INFINEON=y
  FILES:= \
    $(LINUX_DIR)/drivers/char/tpm/tpm_i2c_infineon.ko
  AUTOLOAD:= $(call AutoProbe, tpm_i2c_infineon.ko)
  DEPENDS:=+kmod-tpm
endef

define KernelPackage/tpm-i2c-infineon/description
  TPM support for infineon i2c devices
endef

$(eval $(call KernelPackage,tpm-i2c-infineon))
