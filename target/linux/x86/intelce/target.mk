BOARD_NAME :=intelce
CPU_TYPE   :=pentium4
# override the features with the set of available ones on these targets
FEATURES   :=fpu pci pcie usb gpio squashfs ext4 targz boot-part rootfs-part

DEFAULT_PACKAGES += \
	cfdisk kmod-e1000 kmod-ath kmod-ath9k kmod-ath9k-common \
	kmod-cfg80211 kmod-mac80211 ath10k-board-qca988x ath10k-firmware-qca988x \
	kmod-button-hotplug \
	wireless-regdb wpa-supplicant hostapd iperf3 \
	grub2-bios-setup \
	luci-light luci-mod-dashboard luci-app-opkg

define Target/Description
	Build firmware images for devices using the CE2600 SoC, including the
	Cisco/Technicolor DPC3848(VE) and others.
endef

