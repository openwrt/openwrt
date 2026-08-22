#
# Copyright (C) 2011 OpenWrt.org
#

PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

# Match the VID header offset the factory bootloader expects.
TURRIS1X_UBI_VID_OFFSET=2048

turris1x_boots_from_nand() {
	# Check the kernel command line instead of /proc/mounts,
	# which changes after sysupgrade switches to ramfs.
	grep -q 'root=/dev/ubiblock' /proc/cmdline
}

turris1x_nand_do_upgrade() {
	local tar_file="$1"
	local mtd

	mtd="$(find_mtd_index "$CI_UBIPART")"
	[ -n "$mtd" ] || {
		echo "NAND partition \"$CI_UBIPART\" not found"
		return 1
	}

	# The generic helpers attach without a VID header offset, so attach
	# here and let them find the device that is already up.
	nand_find_ubi "$CI_UBIPART" >/dev/null ||
		ubiattach -m "$mtd" -d 0 -O "$TURRIS1X_UBI_VID_OFFSET" || {
			echo "could not attach UBI on mtd$mtd"
			return 1
		}

	nand_upgrade_tar "$tar_file"
}

turris1x_sdcard_do_upgrade() {
	local tar_file="$1"
	local board_dir bootdev rootdev blocks ret

	export_bootdevice || return 1
	export_partdevice bootdev 1 || return 1
	export_partdevice rootdev 2 || return 1
	board_dir=$(tar tf "$tar_file" | grep -m 1 '^sysupgrade-.*/$')
	board_dir=${board_dir%/}
	[ -n "$board_dir" ] || {
		echo "no sysupgrade directory in $tar_file"
		return 1
	}

	# Unpack the kernel before the root filesystem is overwritten, so
	# that a broken image is caught while the card is still bootable.
	tar xOf "$tar_file" "$board_dir/kernel" > /tmp/turris1x_kernel || return 1
	[ -s /tmp/turris1x_kernel ] || {
		echo "no kernel in $tar_file"
		return 1
	}

	# Count the written blocks the same way emmc_upgrade_tar() does.
	blocks="$(($(tar xOf "$tar_file" "$board_dir/root" |
		dd of="/dev/$rootdev" bs=512 2>&1 |
		grep "records out" | cut -d' ' -f1)))"
	sync
	[ "$blocks" -gt 0 ] || {
		echo "could not write the root filesystem to /dev/$rootdev"
		return 1
	}

	# Clear the old overlay, which libfstools looks for at the next
	# ROOTDEV_OVERLAY_ALIGN boundary. The configuration backup is kept on
	# the FAT partition, so unlike on eMMC this cannot throw it away.
	dd if=/dev/zero of="/dev/$rootdev" bs=512 \
		seek=$(((blocks + 127) & ~127)) count=8 2>/dev/null

	mkdir -p /tmp/turris1x_boot
	mount -t vfat -o rw,noatime "/dev/$bootdev" /tmp/turris1x_boot || return 1
	cp /tmp/turris1x_kernel /tmp/turris1x_boot/zImage
	ret=$?
	sync
	umount /tmp/turris1x_boot
	rmdir /tmp/turris1x_boot

	return $ret
}

platform_copy_config() {
	case "$(board_name)" in
	cznic,turris1x)
		if turris1x_boots_from_nand; then
			nand_restore_config "$UPGRADE_BACKUP"
		else
			# preinit picks the backup up from the FAT partition,
			# see base-files/lib/preinit/79_move_config
			legacy_sdcard_copy_config
		fi
		;;
	esac
}

platform_check_image() {
	local board=$(board_name)

	case "$board" in
	cznic,turris1x)
		# One sysupgrade image serves both media, platform_do_upgrade
		# writes it to the one the system is running from.
		nand_do_platform_check "$board" "$1"
		return $?
		;;
	esac

	return 0
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	cznic,turris1x)
		export CI_UBIPART="rootfs"
		# the NOR has an MTD partition called "kernel" as well, and
		# that one holds the factory rescue system
		export CI_SKIP_KERNEL_MTD=1
		if turris1x_boots_from_nand; then
			turris1x_nand_do_upgrade "$1"
		else
			turris1x_sdcard_do_upgrade "$1"
		fi
		;;
	hpe,msm460|\
	ocedo,panda|\
	sophos,red-15w-rev1|\
	watchguard,firebox-t10|\
	watchguard,firebox-t15|\
	watchguard,xtm330)
		nand_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
