define LegacyDevice/TD_O3V2
	DEVICE_TITLE := TENDA TD-O3V2 (8MB)
	DEVICE_PACKAGES := rssileds kmod-leds-gpio 	kmod-fs-ext4 kmod-fs-nfs-v3 kmod-fs-nfs-v4 luci luci-app-commands bridge openssh-sftp-server aircrack-ng 
endef
LEGACY_DEVICES += TD_O3V2
