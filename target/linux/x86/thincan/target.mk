DEVICE_TYPE:=terminal
BOARDNAME:=Artec ThinCan

CFLAGS := -O2 -pipe -march=geode -mtune=geode -funit-at-a-time

DEFAULT_PACKAGES:=\
	base-files \
	busybox \
	hotplug2 \
	libc \
	libgcc \
	opkg \
	uci \
	udevtrigger \
	kmod-fs-msdos \
	kmod-fs-vfat \
	kmod-fs-ext2 \
	kmod-fs-ext3 \
	kmod-fs-ext4 \
	kmod-usb-core \
	kmod-usb-ohci \
	kmod-usb-hid \
	kmod-usb-storage \
	kmod-usb2 \
	kmod-sound-core \
	kmod-sound-cs5535audio \
	kmod-crypto-core \
	kmod-crypto-hw-geode \
	kmod-crypto-ocf \
	kmod-crypto-aes

define Target/Description
	Build firmware images for Artec ThinCan

	Note that NBD, NFS, DHCP and ethernet module are built into kernel
endef

