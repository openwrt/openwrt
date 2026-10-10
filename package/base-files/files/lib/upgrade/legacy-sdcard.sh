legacy_sdcard_get_partitions() { # <image> <diskdev>
	# bs=512b reads up to 256 KiB, enough for an MBR or a GPT header and entries
	v "Extract boot sector from the image"
	get_image_dd "$1" of=/tmp/image.bs count=1 bs=512b

	get_partitions /tmp/image.bs image
	rm -f /tmp/image.bs

	get_partitions "/dev/$2" bootdisk
}

# partitions on the disk which the image does not have
legacy_sdcard_extra_partitions() {
	awk 'NR == FNR { img[$1]; next } !($1 in img) { printf "%s ", $1 }' \
		/tmp/partmap.image /tmp/partmap.bootdisk
}

legacy_sdcard_check_image() {
	local file="$1"
	local diskdev diff extra

	export_bootdevice && export_partdevice diskdev 0 || {
		v "Unable to determine upgrade device"
	return 1
	}

	legacy_sdcard_get_partitions "$file" "$diskdev"

	#compare tables
	diff="$(grep -F -x -v -f /tmp/partmap.bootdisk /tmp/partmap.image)"
	extra="$(legacy_sdcard_extra_partitions)"

	rm -f /tmp/partmap.bootdisk /tmp/partmap.image

	# Writing the full image erases the extra partitions, so
	# legacy_sdcard_do_upgrade refuses that without sysupgrade -p. Report
	# it here already when sysupgrade itself runs the check, and return 74
	# so --force cannot get past it. procd and LuCI validate without
	# SAVE_PARTITIONS and must pass, or sysupgrade -p could never work.
	if [ -n "$extra" ]; then
		if [ "$SAVE_PARTITIONS" = "0" ] && [ -z "$FAILSAFE" ]; then
			echo "sysupgrade -p writes the full image, which erases partition(s)" >&2
			echo "${extra}on /dev/$diskdev." >&2
		elif [ -n "$diff" ]; then
			if [ -n "$FAILSAFE" ]; then
				notify_firmware_test_result "partition_layout" 0
				echo "Partition layout has changed. Writing the full image would erase" >&2
				echo "partition(s) ${extra}on /dev/$diskdev. sysupgrade -p is not supported" >&2
				echo "in failsafe mode, remove the partition(s) first." >&2
				return 74
			fi
			if [ "$SAVE_PARTITIONS" = "1" ]; then
				notify_firmware_test_result "partition_layout" 0
				echo "Partition layout has changed. Writing the full image would erase" >&2
				echo "partition(s) ${extra}on /dev/$diskdev. Use sysupgrade -p to erase them." >&2
				return 74
			fi
			return 0
		fi
	fi

	[ -n "$diff" ] || return 0

	notify_firmware_test_result "partition_layout" 1
	v "Partition layout has changed. Full image will be written."
	ask_bool 0 "Abort" && exit 1
	return 0
}

# Prints how to write the image: "full", "partitions" or "refuse"
legacy_sdcard_upgrade_mode() { # <image> <diskdev>
	local diff=1 extra

	# get_partitions exits on an unreadable table, so read in a subshell
	rm -f /tmp/partmap.bootdisk /tmp/partmap.image
	( legacy_sdcard_get_partitions "$1" "$2" ) >&2

	[ -f /tmp/partmap.image ] || {
		v "Unable to read the partition table of the image"
		echo refuse
		return
	}

	if [ -f /tmp/partmap.bootdisk ]; then
		#compare tables
		diff="$(grep -F -x -v -f /tmp/partmap.bootdisk /tmp/partmap.image)"
		extra="$(legacy_sdcard_extra_partitions)"
	fi

	if [ "$UPGRADE_OPT_SAVE_PARTITIONS" = "0" ]; then
		# sysupgrade -p
		echo full
	elif [ -n "$extra" ]; then
		if [ -n "$diff" ]; then
			v "Partition layout has changed. Refusing to erase partition(s) ${extra}on /dev/$2, use sysupgrade -p to erase them."
			echo refuse
		else
			echo partitions
		fi
	elif [ -n "$diff" ] || [ -z "$UPGRADE_OPT_SAVE_PARTITIONS" ]; then
		# failsafe passes no options and writes the full image
		echo full
	else
		echo partitions
	fi
}

legacy_sdcard_do_upgrade() {
	local diskdev partdev mode

	export_bootdevice && export_partdevice diskdev 0 || {
		v "Unable to determine upgrade device"
	return 1
	}

	sync

	mode="$(legacy_sdcard_upgrade_mode "$1" "$diskdev")"

	case "$mode" in
	full)
		get_image_dd "$1" of="/dev/$diskdev" bs=4096 conv=fsync

		# Separate removal and addtion is necessary; otherwise, partition 1
		# will be missing if it overlaps with the old partition 2
		partx -d - "/dev/$diskdev"
		partx -a - "/dev/$diskdev"
		;;
	partitions)
		#iterate over each partition from the image and write it to the boot disk
		while read part start size; do
			if export_partdevice partdev $part; then
				v "Writing image to /dev/$partdev..."
				get_image_dd "$1" of="/dev/$partdev" ibs="512" obs=1M skip="$start" count="$size" conv=fsync
			else
				v "Unable to find partition $part device, skipped."
			fi
		done < /tmp/partmap.image

		v "Writing new UUID to /dev/$diskdev..."
		get_image_dd "$1" of="/dev/$diskdev" bs=1 skip=440 count=4 seek=440 conv=fsync

		if type 'platform_legacy_sdcard_post_upgrade' >/dev/null 2>/dev/null; then
			platform_legacy_sdcard_post_upgrade "$diskdev"
		fi
		;;
	*)
		return 1
		;;
	esac

	sleep 1
}

legacy_sdcard_copy_config() {
	local partdev

	if export_partdevice partdev 1; then
		mkdir -p /boot
		[ -f /boot/kernel.img ] || mount -o rw,noatime /dev/$partdev /boot
		cp -af "$UPGRADE_BACKUP" "/boot/$BACKUP_FILE"
		sync
		umount /boot
	fi
}
