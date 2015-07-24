BOARDNAME:=AMD Geode based systems
FEATURES:=squashfs ext4 pci usb gpio
DEFAULT_PACKAGES += \
			kmod-crypto-hw-geode kmod-crypto-ocf kmod-crypto-cbc \
			kmod-ath5k kmod-ath9k \
			kmod-button-hotplug \
			kmod-ledtrig-heartbeat kmod-ledtrig-gpio \
			kmod-ledtrig-netdev hwclock wpad-mini

CPU_TYPE := geode

define Target/Description
	Build firmware images for AMD Geode GX/LX based systems (net5501, alix, geos)
endef
