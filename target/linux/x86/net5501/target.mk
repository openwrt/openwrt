BOARDNAME:=Soekris net5501
DEFAULT_PACKAGES += kmod-ata-via-sata kmod-crypto-hw-geode \
			kmod-cs5535-gpio kmod-nsc-gpio kmod-pc8736x-gpio \
			kmod-geodewdt kmod-hwmon-core kmod-hwmon-pc87360 \
			kmod-via-rhine \
			kmod-i2c-core kmod-i2c-gpio \
			kmod-i2c-algo-bit kmod-i2c-algo-pca kmod-i2c-algo-pcf \
			kmod-usb-core kmod-usb2 kmod-usb-hid kmod-usb-uhci
CFLAGS += -march=k6-2 -fno-align-functions -fno-align-loops -fno-align-jumps \
	  -fno-align-labels

define Target/Description
	Build firmware images for Soekris net5501 board
endef

