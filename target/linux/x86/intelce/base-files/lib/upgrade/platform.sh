
platform_check_image() {
	local diskdev partdev diff
	[ "$#" -gt 1 ] && return 1

	case "$(get_magic_word "$1")" in
		eb48|eb63) ;;
		*)
			v "Invalid image type"
			return 1
		;;
	esac

	export_bootdevice && export_partdevice diskdev 0 || {
		v "Unable to determine upgrade device"
		return 1
	}

	rm -f /tmp/image.bs /tmp/partmap.bootdisk /tmp/partmap.image

}

platform_copy_config() {
	local partdev parttype=ext4 partnum=2

	if export_partdevice partdev $partnum; then
		part_magic_fat "/dev/$partdev" && parttype=vfat
		mount -t $parttype -o rw,noatime "/dev/$partdev" /mnt
		cp -af "$UPGRADE_BACKUP" "/mnt/$BACKUP_FILE"
		umount /mnt
	fi
}

platform_do_bootloader_upgrade() {
	local diskdev="$1"

	v "Bootloader can't be upgraded, sadly..."
}

platform_do_upgrade() {
	local diskdev partdev diff

	export_bootdevice && export_partdevice diskdev 0 || {
		v "Unable to determine upgrade device"
		return 1
	}

	sync

	v "Extract boot sector from the source image"
	get_image_dd "$1" of=/tmp/image.bs count=63 bs=512b
	# get the partition list from the source image's boot sector
	get_partitions /tmp/image.bs image
	# iterate over each partition from the image and write it to the boot disk
	while read part start size; do
		if [ $part -eq 1 ]; then
			# first partition on the image is the kernel + grub stuff
			if export_partdevice partdev 1; then
				v "Extracting kernel from boot partition from image"
				local boot boot_mnt parttype=ext4

				boot=/tmp/boot.img
				boot_mnt=/mnt
				boot_mnt_kernel=$boot_mnt/boot/vmlinuz
				mkdir -p $boot_mnt

				# the first part on a sysupgrade image is a fat part with the kernel on it
				get_image_dd "$1" of="$boot" ibs=512 obs=1M skip="$start" count="$size" conv=fsync
				# mount it and just take the kernel.
				part_magic_fat "$boot" && parttype=vfat
				mount -t $parttype -o ro,noatime "$boot" $boot_mnt
				v "Kernel md5sum is $(md5sum $boot_mnt_kernel)"
				# dd the kernel into the first mmcblk partition
				v "Writing kernel to /dev/$partdev..."
				get_image_dd "$boot_mnt_kernel" of="/dev/$partdev" conv=fsync
			else
				v "Unable to find kernel partition (1), skipped."
			fi
			umount $boot_mnt
			rm $boot
		elif [ $part -eq 2 ]; then
			# second partition on the image is the rootfs
			if export_partdevice partdev 3; then
				v "Writing image to /dev/$partdev..."
				get_image_dd "$1" of="/dev/$partdev" ibs=512 obs=1M skip="$start" count="$size" conv=fsync
			else
				v "Unable to find rootfs partition (3), skipped."
			fi
		else
			# other partitions on the image are not supported
			v "Partition index $part is not supported on this target, skipped."
		fi
	done < /tmp/partmap.image
}
