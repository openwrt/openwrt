#
# Copyright (C) 2014-2015 OpenWrt.org
#

BOOTPART=/dev/mmcblk0p2
CFGPART=/dev/mmcblk0p3

identify_magic() {
	local magic=$1
	case "$magic" in
		"55424923")
			echo "ubi"
			;;
		"31181006")
			echo "ubifs"
			;;
		"68737173")
			echo "squashfs"
			;;
		"d00dfeed")
			echo "fit"
			;;
		"00000000")
			echo "ext4"
			;;
		"4349"*)
			echo "combined"
			;;
		*)
			echo "unknown $magic"
			;;
	esac
}

get_magic_long_tar() {
	( tar xf $1 $2 -O | dd bs=4 count=1 | hexdump -v -n 4 -e '1/1 "%02x"') 2> /dev/null
}

identify_tar() {
	identify_magic $(get_magic_long_tar "$1" "$2")
}

platform_do_check_sockit() {
	local board=$(socfpga_board_name)
	local magic_long="$(get_magic_long "$1")"
	echo "magic = $magic_long"

	if [ "$magic_long" = "73797375" ] ; then
		local rootfs_type="$(identify_tar "$1" sysupgrade-$board/root)"
		if [ "$rootfs_type" = "ubifs" ] ; then
			nand_do_platform_check $board $1
			return 0;
		fi
		[ "$rootfs_type" = "ext4" ] && return 0

		echo "Unknown rootfs type $rootfs_type !"
	fi

	return 1
}

platform_do_upgrade_sockit() {
	local board=$(socfpga_board_name)
	local magic_long="$(get_magic_long "$1")"
	echo "magic = $magic_long"

	if [ "$magic_long" = "73797375" ] ; then
		local rootfs_type="$(identify_tar "$1" sysupgrade-$board/root)"
		if [ "$rootfs_type" = "ext4" ] ; then
			sync
			tar xf "$1" sysupgrade-$board/root -O | \
				dd of="$BOOTPART" bs=512 conv=fsync
			return 0
		fi

		echo "Unknown rootfs type $rootfs_type !"
	fi

	return 1
}

platform_copy_config() {
	if [ -b "$CFGPART" ]; then
		mount -o rw,noatime "$CFGPART" /mnt
		cp -af "$CONF_TAR" /mnt/
		umount /mnt
	fi
}
