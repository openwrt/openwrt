#
# Copyright (C) 2016 OpenWrt.org
#

platform_do_upgrade() {
	local tar_file="$1"
	local board="$(cat /tmp/sysinfo/board_name)"

	echo "flashing kernel"
	tar xf $tar_file sysupgrade-$board/kernel -O | mtd write - kernel

	echo "flashing rootfs"
	tar xf $tar_file sysupgrade-$board/root -O | mtd write - rootfs

	return 0
}

platform_check_image() {
	local tar_file="$1"
	local board=$(cat /tmp/sysinfo/board_name)

	case "$board" in
	NAND | \
	eMMC)
		local kernel_length=`(tar xf $tar_file sysupgrade-$board/kernel -O | wc -c) 2> /dev/null`
		local rootfs_length=`(tar xf $tar_file sysupgrade-$board/root -O | wc -c) 2> /dev/null`
		;;

	*)
		echo "Sysupgrade is not supported on your board yet."
		return 1
		;;
	esac

	[ "$kernel_length" = 0 -o "$rootfs_length" = 0 ] && {
		echo "The upgarde image is corrupt."
		return 1
	}

	return 0
}
