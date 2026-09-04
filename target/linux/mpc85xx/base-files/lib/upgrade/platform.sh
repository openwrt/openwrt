#
# Copyright (C) 2011 OpenWrt.org
#

PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

. /lib/functions/turris1x.sh

turris1x_nand_do_upgrade() {
	local tar_file="$1"
	local mtd

	mtd="$(find_mtd_index "$CI_UBIPART")"
	[ -n "$mtd" ] || {
		echo "NAND partition \"$CI_UBIPART\" not found"
		return 1
	}

	# The generic helpers attach without a VID header offset, so attach
	# here and let them find the device that is already up. nand_find_ubi
	# reports "not found" by printing nothing, not through its exit status.
	if [ -z "$(nand_find_ubi "$CI_UBIPART")" ]; then
		ubiattach -m "$mtd" -d 0 -O "$TURRIS1X_UBI_VID_OFFSET" || {
			echo "could not attach UBI on mtd$mtd"
			return 1
		}
	fi

	nand_upgrade_tar "$tar_file"
}

turris1x_sdcard_do_upgrade() {
	local tar_file="$1"
	local board_dir bootdev rootdev blocks ret rootfs_size part_size

	export_bootdevice && export_partdevice bootdev 1 &&
		export_partdevice rootdev 2 || {
		echo "Unable to determine upgrade device"
		return 1
	}
	board_dir=$(tar tf "$tar_file" | grep -m 1 '^sysupgrade-.*/$')
	board_dir=${board_dir%/}
	[ -n "$board_dir" ] || {
		echo "no sysupgrade directory in $tar_file"
		return 1
	}

	# Unpack the kernel first, so a broken image is caught while the card
	# still boots.
	tar xOf "$tar_file" "$board_dir/kernel" > /tmp/turris1x_kernel || return 1
	[ -s /tmp/turris1x_kernel ] || {
		echo "no kernel in $tar_file"
		return 1
	}

	# Refuse rather than truncate, the caller reboots whatever we return.
	rootfs_size=$(tar xOf "$tar_file" "$board_dir/root" | wc -c)
	part_size=$(($(cat "/sys/class/block/$rootdev/size") * 512))
	[ "$rootfs_size" -gt 0 ] && [ $((rootfs_size + 65536)) -le "$part_size" ] || {
		echo "the root filesystem does not fit /dev/$rootdev"
		return 1
	}

	# Rename over the old kernel, so an interrupted upgrade leaves the card
	# bootable.
	mkdir -p /tmp/turris1x_boot
	mount -t vfat -o rw,noatime "/dev/$bootdev" /tmp/turris1x_boot || return 1
	cp /tmp/turris1x_kernel /tmp/turris1x_boot/zImage.new && sync &&
		mv /tmp/turris1x_boot/zImage.new /tmp/turris1x_boot/zImage
	ret=$?
	sync
	umount /tmp/turris1x_boot
	rmdir /tmp/turris1x_boot
	[ "$ret" = 0 ] || {
		echo "could not write the kernel to /dev/$bootdev"
		return 1
	}

	tar xOf "$tar_file" "$board_dir/root" > "/dev/$rootdev" || {
		echo "could not write the root filesystem to /dev/$rootdev"
		return 1
	}
	sync
	blocks=$(((rootfs_size + 511) / 512))

	# Clear the old overlay, which libfstools looks for at the next
	# ROOTDEV_OVERLAY_ALIGN boundary. The config backup is on the FAT
	# partition, so this cannot lose it.
	dd if=/dev/zero of="/dev/$rootdev" bs=512 \
		seek=$(((blocks + 127) & ~127)) count=8 2>/dev/null

	return 0
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
	local bootdev rootdev

	case "$board" in
	cznic,turris1x)
		# Fail here rather than after the switch to ramfs, where
		# do_stage2 ignores the result and reboots regardless.
		turris1x_boots_from_nand || {
			export_bootdevice && export_partdevice bootdev 1 &&
				export_partdevice rootdev 2
		} || {
			echo "Unable to determine upgrade device"
			return 1
		}

		# One image serves both media. Its tar carries
		# sysupgrade-cznic_turris1x/CONTROL, so nand_do_platform_check
		# takes the tar branch, and REQUIRE_IMAGE_METADATA above already
		# rejects anything whose metadata does not name this board.
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
		export CI_UBIPART="ubi"
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
