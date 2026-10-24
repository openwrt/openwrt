# SPDX-License-Identifier: GPL-2.0-only

REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	local diskdev partdev diff

	[ "$#" -gt 1 ] && return 1

	export_bootdevice && export_partdevice diskdev 0 || {
		v "Unable to determine upgrade device"
		return 1
	}

	get_partitions "/dev/$diskdev" bootdisk

	# Extract partition table from image
	get_image "$@" | dd of=/tmp/image.bs count=1 bs=512b 2>/dev/null
	get_partitions /tmp/image.bs image

	# Compare partition tables (checks partition index, start offset, and size)
	diff="$(grep -F -x -v -f /tmp/partmap.bootdisk /tmp/partmap.image 2>/dev/null)"

	rm -f /tmp/image.bs /tmp/partmap.bootdisk /tmp/partmap.image

	if [ -n "$diff" ]; then
		if [ "$UPGRADE_OPT_SAVE_PARTITIONS" = "1" ]; then
			v "Partition layout has changed. Cannot preserve partitions."
			return 1
		fi
		v "Partition layout has changed. Full image will be written."
		ask_bool 0 "Abort" && return 1
		return 0
	fi

	return 0
}

platform_do_upgrade() {
	local diskdev partdev diff

	export_bootdevice && export_partdevice diskdev 0 || {
		v "Unable to determine upgrade device"
		return 1
	}

	sync

	if [ "$UPGRADE_OPT_SAVE_PARTITIONS" = "1" ]; then
		get_partitions "/dev/$diskdev" bootdisk

		# Extract partition table from new image
		get_image "$@" | dd of=/tmp/image.bs count=1 bs=512b 2>/dev/null
		get_partitions /tmp/image.bs image

		# Compare partition tables (checks partition index, start offset, and size)
		diff="$(grep -F -x -v -f /tmp/partmap.bootdisk /tmp/partmap.image 2>/dev/null)"
	else
		diff=1
	fi

	if [ -n "$diff" ]; then
		v "Writing full image to /dev/$diskdev..."
		get_image "$@" | dd of="/dev/$diskdev" bs=4096 conv=fsync

		# Inform kernel of partition table changes
		partx -d - "/dev/$diskdev" 2>/dev/null || true
		partx -a - "/dev/$diskdev" 2>/dev/null || true

		rm -f /tmp/image.bs /tmp/partmap.bootdisk /tmp/partmap.image
		return 0
	fi

	# Update the whole area in front of the first partition (sectors 1..8191):
	# BL1, BL2, U-Boot and TZSW, and reset the U-Boot environment (sector 2560)
	# to the defaults of the new U-Boot
	v "Updating bootloader components..."
	get_image "$@" | dd of="/dev/$diskdev" bs=512 skip=1 seek=1 count=8191 conv=fsync,notrunc

	# Copy partition UUID (bytes 440..443 of MBR)
	v "Updating disk UUID..."
	get_image "$@" | dd of="/dev/$diskdev" bs=1 skip=440 count=4 seek=440 conv=fsync,notrunc

	# Write individual partitions (boot + rootfs)
	while read part start size; do
		if export_partdevice partdev $part; then
			v "Writing partition $part to /dev/$partdev..."
			get_image "$@" | dd of="/dev/$partdev" ibs=512 obs=1M skip="$start" count="$size" conv=fsync
		else
			v "Unable to find partition $part device, skipped."
		fi
	done < /tmp/partmap.image

	rm -f /tmp/image.bs /tmp/partmap.bootdisk /tmp/partmap.image
}

platform_copy_config() {
	local partdev

	if export_partdevice partdev 1; then
		mount -t ext4 -o rw,noatime "/dev/$partdev" /mnt
		cp -af "$UPGRADE_BACKUP" "/mnt/$BACKUP_FILE"
		umount /mnt
	fi
}
