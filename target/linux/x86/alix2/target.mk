BOARDNAME:=PCEngines alix2
FEATURES:=squashfs ext4 pci usb gpio
DEFAULT_PACKAGES += \
			kmod-crypto-hw-geode kmod-crypto-ocf kmod-crypto-aes kmod-crypto-cbc \
			kmod-gpio-cs5535-new kmod-gpio-nsc \
			kmod-wdt-geode kmod-cs5535-clockevt kmod-cs5535-mfgpt \
			kmod-cs5536 \
			kmod-hwmon-core kmod-hwmon-lm90 \
			kmod-via-rhine \
			kmod-i2c-core kmod-i2c-gpio \
			kmod-i2c-algo-bit kmod-i2c-algo-pca kmod-i2c-algo-pcf \
			kmod-i2c-scx200-acb \
			kmod-usb-core kmod-usb2 kmod-usb2-pci kmod-usb-ohci \
			kmod-cfg80211 kmod-mac80211 \
			kmod-mppe kmod-pppoe kmod-pppol2tp \
			kmod-ath5k kmod-ath9k \
			kmod-leds-gpio kmod-input-gpio-keys-polled \
			kmod-button-hotplug \
			kmod-ledtrig-heartbeat kmod-ledtrig-gpio \
			kmod-ledtrig-netdev \
			kmod-cpu-msr hwclock wpad
LINUX_VERSION:=3.10.15
CS5535_MASK:=0x0b000042

CPU_TYPE := geode

define Target/Description
	Build firmware images for PCEngines alix2 board
endef

define KernelPackage/gpio-cs5535-new/install
     sed -i -r -e 's/$$$$$$$$/ mask=$(CS5535_MASK)/' $(1)/etc/modules.d/??-gpio-cs5535-new
endef
