include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=i2c-aw9523-expander
PKG_RELEASE:=2

include $(INCLUDE_DIR)/package.mk

define KernelPackage/i2c-leds-aw9523-expander
  SUBMENU:=LED modules
  TITLE:=Custom LED I2C AW9523 device
  DEPENDS:=+kmod-i2c-core
  FILES:=$(PKG_BUILD_DIR)/i2c-leds-aw9523.ko
  AUTOLOAD:=$(call AutoLoad,40,i2c-leds-aw9523,1)
endef

define KernelPackage/i2c-leds-aw9523-expander/description
 Kernel module for AW9523 i2c-led expander platform device.
endef
EXTRA_KCONFIG := CONFIG_I2C_LEDS_AW9523=m

EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG)))) \

MAKE_OPTS:= \
	$(KERNEL_MAKE_FLAGS) \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	$(EXTRA_KCONFIG)

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,i2c-leds-aw9523-expander))
