. /lib/functions.sh

REQUIRE_IMAGE_METADATA=1

meson_partmap_unchanged() {
	local diskdev="$1"
	shift

	rm -f /tmp/image.bs /tmp/partmap.bootdisk /tmp/partmap.image

	get_image "$@" | dd of=/tmp/image.bs count=1 bs=512b 2>/dev/null

	( get_partitions "/dev/$diskdev" bootdisk )
	( get_partitions /tmp/image.bs image )

	rm -f /tmp/image.bs

	[ -s /tmp/partmap.bootdisk ] && [ -s /tmp/partmap.image ] &&
		[ "$(cat /tmp/partmap.bootdisk)" = "$(cat /tmp/partmap.image)" ]
}

meson_dd_complete() {
	local log="$1"
	local count="$2"

	grep -q "^$count+0 records in$" "$log"
}

platform_check_image() {
	local diskdev

	export_bootdevice && export_partdevice diskdev 0 || {
		echo "Unable to determine upgrade device"
		return 1
	}

	if ! meson_partmap_unchanged "$diskdev" "$@"; then
		echo "Partition layout has changed. Full image will be written."
		ask_bool 0 "Abort" && exit 1
	fi

	rm -f /tmp/partmap.bootdisk /tmp/partmap.image
	return 0
}

platform_copy_config() {
	local partdev

	if export_partdevice partdev 1; then
		mount -t ext4 -o rw,noatime "/dev/$partdev" /mnt
		cp -af "$UPGRADE_BACKUP" "/mnt/$BACKUP_FILE"
		umount /mnt
	fi
}

platform_do_upgrade() {
	local diskdev partdev part start size

	export_bootdevice && export_partdevice diskdev 0 || {
		echo "Unable to determine upgrade device"
		return 1
	}

	sync

	if [ "$UPGRADE_OPT_SAVE_PARTITIONS" != "1" ] ||
	   ! meson_partmap_unchanged "$diskdev" "$@"; then
		get_image "$@" | dd of="/dev/$diskdev" bs=4096 conv=fsync || {
			echo "Failed to write image to /dev/$diskdev"
			return 1
		}

		partx -d - "/dev/$diskdev"
		partx -a - "/dev/$diskdev"

		return 0
	fi

	while read part start size; do
		export_partdevice partdev "$part" || {
			echo "Unable to find partition $part device"
			return 1
		}

		echo "Writing image to /dev/$partdev..."
		get_image "$@" | dd of="/dev/$partdev" ibs=512 obs=1M skip="$start" count="$size" \
			conv=fsync 2>/tmp/meson-dd.log &&
			meson_dd_complete /tmp/meson-dd.log "$size" || {
			cat /tmp/meson-dd.log
			echo "Failed to write image to /dev/$partdev"
			return 1
		}
	done < /tmp/partmap.image

	echo "Writing new UUID to /dev/$diskdev..."
	get_image "$@" | dd of="/dev/$diskdev" bs=1 skip=440 count=4 seek=440 conv=fsync \
		2>/tmp/meson-dd.log &&
		meson_dd_complete /tmp/meson-dd.log 4 || {
		cat /tmp/meson-dd.log
		echo "Failed to write disk identifier to /dev/$diskdev"
		return 1
	}
}
