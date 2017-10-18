#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

IIO_MENU:=Industrial I/O Modules


define KernelPackage/iio-core
  SUBMENU:=$(IIO_MENU)
  TITLE:=Industrial IO core
  KCONFIG:= \
	CONFIG_IIO \
	CONFIG_IIO_BUFFER=y \
	CONFIG_IIO_KFIFO_BUF \
	CONFIG_IIO_TRIGGER=y \
	CONFIG_IIO_TRIGGERED_BUFFER
  FILES:= \
	$(LINUX_DIR)/drivers/iio/industrialio.ko \
	$(if $(CONFIG_IIO_TRIGGERED_BUFFER),$(LINUX_DIR)/drivers/iio/industrialio-triggered-buffer.ko@lt4.4) \
	$(if $(CONFIG_IIO_TRIGGERED_BUFFER),$(LINUX_DIR)/drivers/iio/buffer/industrialio-triggered-buffer.ko@ge4.4) \
	$(LINUX_DIR)/drivers/iio/kfifo_buf.ko@lt4.4 \
	$(LINUX_DIR)/drivers/iio/buffer/kfifo_buf.ko@ge4.4
  AUTOLOAD:=$(call AutoLoad,55,industrialio kfifo_buf industrialio-triggered-buffer)
endef

define KernelPackage/iio-core/description
 The industrial I/O subsystem provides a unified framework for
 drivers for many different types of embedded sensors using a
 number of different physical interfaces (i2c, spi, etc)
endef

$(eval $(call KernelPackage,iio-core))


define KernelPackage/iio-ad799x
  SUBMENU:=$(IIO_MENU)
  DEPENDS:=+kmod-i2c-core +kmod-iio-core
  TITLE:=Analog Devices AD799x ADC driver
  KCONFIG:= \
	CONFIG_AD799X_RING_BUFFER=y \
	CONFIG_AD799X
  FILES:=$(LINUX_DIR)/drivers/iio/adc/ad799x.ko
  AUTOLOAD:=$(call AutoLoad,56,ad799x)
endef

define KernelPackage/iio-ad799x/description
 support for Analog Devices:
 ad7991, ad7995, ad7999, ad7992, ad7993, ad7994, ad7997, ad7998
 i2c analog to digital converters (ADC).
endef

$(eval $(call KernelPackage,iio-ad799x))


define KernelPackage/iio-dht11
  SUBMENU:=$(IIO_MENU)
  DEPENDS:=+kmod-iio-core @GPIO_SUPPORT @USES_DEVICETREE
  TITLE:=DHT11 (and compatible) humidity and temperature sensors
  KCONFIG:= \
	CONFIG_DHT11
  FILES:=$(LINUX_DIR)/drivers/iio/humidity/dht11.ko
  AUTOLOAD:=$(call AutoLoad,56,dht11)
endef

define KernelPackage/iio-dht11/description
 support for DHT11 and DHT22 digitial humidity and temperature sensors
 attached at GPIO lines. You will need a custom device tree file to
 specify the GPIO line to use.
endef

$(eval $(call KernelPackage,iio-dht11))
