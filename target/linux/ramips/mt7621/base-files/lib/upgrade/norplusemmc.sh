#
# Copyright (C) 2021 X-WRT
#

. /lib/functions.sh

norplusemmc_check_image() {
	[ "$#" -gt 1 ] && return 1

	local tar_file="$1"

	local board_dir=$(tar tf "$tar_file" | grep -m 1 '^sysupgrade-.*/$')
	board_dir=${board_dir%/}

	local magic=$(tar xf "$tar_file" ${board_dir}/root -O | zcat | dd bs=2 count=1 | hexdump -v -n 2 -e '1/1 "%02x"')

	case "$magic" in
		eb48|eb63) ;;
		*)
			v "Invalid image type"
			return 1
		;;
	esac

	local diskdev partdev diff
	export_bootdevice && export_partdevice diskdev 0 || {
		v "Unable to determine upgrade device"
		return 1
	}

	get_partitions "/dev/$diskdev" bootdisk

	v "Extract boot sector from the image"
	tar xf "$tar_file" ${board_dir}/root -O | zcat | dd of=/tmp/image.bs count=63 bs=512b

	get_partitions /tmp/image.bs image

	#compare tables
	diff="$(grep -F -x -v -f /tmp/partmap.bootdisk /tmp/partmap.image)"

	rm -f /tmp/image.bs /tmp/partmap.bootdisk /tmp/partmap.image

	if [ -n "$diff" ]; then
		v "Partition layout has changed. Full image will be written."
		ask_bool 0 "Abort" && exit 1
		return 0
	fi
}

norplusemmc_copy_config() {
	local partdev parttype=ext4

	if export_partdevice partdev 1; then
		part_magic_fat "/dev/$partdev" && parttype=vfat
		mount -t $parttype -o rw,noatime "/dev/$partdev" /mnt
		cp -af "$UPGRADE_BACKUP" "/mnt/$BACKUP_FILE"
		umount /mnt
	fi
}

norplusemmc_do_upgrade() {
	CI_KERNPART="firmware"

	local tar_file="$1"
	local kernel_mtd="$(find_mtd_index $CI_KERNPART)"

	local board_dir=$(tar tf "$tar_file" | grep -m 1 '^sysupgrade-.*/$')
	board_dir=${board_dir%/}

	local diskdev partdev diff
	export_bootdevice && export_partdevice diskdev 0 || {
		v "Unable to determine upgrade device"
		return 1
	}

	sync

	kernel_length=$( (tar xf "$tar_file" ${board_dir}/kernel -O | wc -c) 2> /dev/null)
	[ "$kernel_length" != 0 -a -n "$kernel_mtd" ] && {
		v "Writing kernel to $CI_KERNPART"
		tar xf "$tar_file" ${board_dir}/kernel -O | mtd write - $CI_KERNPART
	}

	if [ "$UPGRADE_OPT_SAVE_PARTITIONS" = "1" ]; then
		get_partitions "/dev/$diskdev" bootdisk

		v "Extract boot sector from the image"
		tar xf "$tar_file" ${board_dir}/root -O | zcat | dd of=/tmp/image.bs count=63 bs=512b

		get_partitions /tmp/image.bs image

		#compare tables
		diff="$(grep -F -x -v -f /tmp/partmap.bootdisk /tmp/partmap.image)"
	else
		diff=1
	fi

	if [ -n "$diff" ]; then
		tar xf "$tar_file" ${board_dir}/root -O | zcat | dd of="/dev/$diskdev" bs=4096 conv=fsync

		# Separate removal and addtion is necessary; otherwise, partition 1
		# will be missing if it overlaps with the old partition 2
		partx -d - "/dev/$diskdev"
		partx -a - "/dev/$diskdev"

		return 0
	fi

	#iterate over each partition from the image and write it to the boot disk
	while read part start size; do
		if export_partdevice partdev $part; then
			v "Writing image to /dev/$partdev..."
			tar xf "$tar_file" ${board_dir}/root -O | zcat | dd of="/dev/$partdev" ibs=512 obs=1M skip="$start" count="$size" conv=fsync
		else
			v "Unable to find partition $part device, skipped."
		fi
	done < /tmp/partmap.image

	v "Writing new UUID to /dev/$diskdev..."
	tar xf "$tar_file" ${board_dir}/root -O | zcat | dd of="/dev/$diskdev" bs=1 skip=440 count=4 seek=440 conv=fsync
}
