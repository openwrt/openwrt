#
# Copyright (C) 2011 OpenWrt.org
#

PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='fw_setenv'
RAMFS_COPY_DATA='/etc/fw_env.config'

platform_check_image() {
	local board=$(board_name)

	case "$board" in
	nec,wg600hp|\
	nec,wr8750n|\
	nec,wr9500n)
		local uboot_mtd=$(find_mtd_part "bootloader")

		# check "U-Boot <year>.<month>" string in the "bootloader" partition
		if ! grep -q "U-Boot [0-9]\{4\}\.[0-9]\{2\}" $uboot_mtd; then
			v "The bootloader doesn't seem to be replaced to U-Boot!"
			return 1
		fi
		;;
	*)
		return 0
	esac
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	engenius,eap350-v1|\
	engenius,ecb350-v1|\
	engenius,enh202-v1)
		ENV_SCRIPT="/tmp/fw_env"
		IMAGE_LIST="tar tzf $1"
		IMAGE_CMD="tar xzOf $1"
		KERNEL_PART="loader"
		ROOTFS_PART="fwconcat0"
		KERNEL_FILE="uImage-lzma.bin"
		ROOTFS_FILE="root.squashfs"
		platform_do_upgrade_failsafe_datachk "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
