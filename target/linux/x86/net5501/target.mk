LINUX_VERSION:=3.2.2
BOARDNAME:=Soekris net5501
FEATURES:=squashfs jffs2 ext4 pci usb gpio
DEFAULT_PACKAGES += kmod-crypto-hw-geode kmod-crypto-ocf \
			kmod-gpio-cs5535-new kmod-gpio-nsc \
			kmod-wdt-geode kmod-cs5535-clockevt kmod-cs5535-mfgpt \
			kmod-cs5536 \
			kmod-hwmon-core kmod-hwmon-pc87360 kmod-gpio-pc8736x \
			kmod-via-rhine kmod-solos-pci \
			kmod-i2c-core kmod-i2c-gpio \
			kmod-i2c-algo-bit kmod-i2c-algo-pca kmod-i2c-algo-pcf \
			kmod-i2c-scx200-acb \
			kmod-usb-core kmod-usb2 kmod-usb-ohci \
			kmod-cfg80211 kmod-mac80211 \
			kmod-mppe kmod-pppoe kmod-pppoa kmod-pppo2ltp \
			kmod-ath kmod-ath5k kmod-ath9k \
			kmod-leds-gpio kmod-input-gpio-keys-polled \
			kmod-button-hotplug \
			kmod-ledtrig-heartbeat kmod-ledtrig-gpio \
			kmod-ledtrig-netdev \
			kmod-cpu-msr \
			hwclock wpad

CS5535_MASK:=0x01000040

CFLAGS += -Os -pipe -march=k6-2 -fno-align-functions -fno-align-loops -fno-align-jumps \
	  -fno-align-labels

define Target/Description
	Build firmware images for Soekris net5501 board
endef

define KernelPackage/gpio-cs5535-new/install
	sed -i -r -e 's/$$$$$$$$/ mask=$(CS5535_MASK)/' $(1)/etc/modules.d/??-gpio-cs5535-new
endef
