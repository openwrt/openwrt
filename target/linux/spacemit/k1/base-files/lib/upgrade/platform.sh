REQUIRE_IMAGE_METADATA=1

#iterate over each partition from the bootdisk and find 'bootfs'
find_partn_by_name() {
	local name="$1" var="$2" part start size partdev

	while read part start size; do
		if export_partdevice partdev $part; then
			. "/sys/class/block/$partdev/uevent"
			if [ "$PARTNAME" = "$name" ]; then
				export "$var=$PARTN"
				return 0
			fi
		fi
	done < /tmp/partmap.bootdisk

	return 1
}

platform_check_image() {
	local diskdev diff bootn partdev
	local board=$(board_name)
	[ "$#" -gt 1 ] && return 1

	v "Running on board: ${board}"

	export_bootdevice && export_partdevice diskdev 0 || {
		v "ERROR: Unable to determine upgrade device"
		return 1
	}

	get_partitions "/dev/$diskdev" bootdisk

	v "Extracting the boot sector from the image"
	get_image_dd "$1" of=/tmp/image.bs count=63 bs=512b

	get_partitions /tmp/image.bs image
	rm -f /tmp/image.bs

	if [ "$SAVE_PARTITIONS" = "1" ]; then
		#compare tables
		diff="$(grep -F -x -v -f /tmp/partmap.bootdisk /tmp/partmap.image)"

		if [ -n "$diff" ]; then
			v "The partition layout has changed!"
			v "The full image needs to be written and additional partitions will be lost!"
			v "Please confirm with '-p'."
			ask_bool 1 "Abort" && exit 1
			return 0
		fi
	else
		v "The full image will be written to /dev/$diskdev"
	fi

	if [ "$SAVE_CONFIG" = "1" ]; then
		find_partn_by_name "bootfs" bootn && export_partdevice partdev "$bootn"
		if [ -z "$partdev" ]; then
			v "The bootfs partition is missing, can't store the configuration"
			return 1
		fi
		v "The configuration will be stored on /dev/$partdev"
	fi

	rm -f /tmp/partmap.bootdisk /tmp/partmap.image
}

platform_copy_config() {
	local bootn partdev

	if find_partn_by_name "bootfs" bootn && export_partdevice partdev "$bootn"; then
		v "Storing the configuration on /dev/$partdev"
		mount -o rw,noatime "/dev/$partdev" /mnt
		cp -af "$UPGRADE_BACKUP" "/mnt/$BACKUP_FILE"
		umount /mnt
	else
		v "ERROR: Unable to find the partition to store the configuration on"
	fi
}

platform_do_upgrade() {
	local diskdev partdev diff

	export_bootdevice && export_partdevice diskdev 0 || {
		v "ERROR: Unable to determine upgrade device"
		return 1
	}

	sync

	if [ "$UPGRADE_OPT_SAVE_PARTITIONS" = "1" ]; then
		get_partitions "/dev/$diskdev" bootdisk

		v "Extracting the boot sector from the image"
		get_image_dd "$1" of=/tmp/image.bs count=63 bs=512b

		get_partitions /tmp/image.bs image

		#compare tables
		diff="$(grep -F -x -v -f /tmp/partmap.bootdisk /tmp/partmap.image)"
	else
		diff=1
	fi

	# Only change the partition table if sysupgrade -p is set,
	# otherwise doing so could interfere with embedded "single storage"
	# (e.g SoC boot from SD card) setups, as well as other user
	# created storage (like uvol)
	if [ -n "$diff" ]; then
		# Need to remove partitions before dd, otherwise the partitions
		# that are added after will have minor numbers offset
		partx -d - "/dev/$diskdev"

		v "Writing full image to /dev/$diskdev ..."
		get_image_dd "$1" of="/dev/$diskdev" bs=4096 conv=fsync

		# Separate removal and addtion is necessary; otherwise, partition 1
		# will be missing if it overlaps with the old partition 2
		partx -a - "/dev/$diskdev"

		return 0
	fi

	#iterate over each partition from the image and write it to the boot disk
	while read part start size; do
		if export_partdevice partdev $part; then
			v "Writing image partition to /dev/$partdev ..."
			get_image_dd "$1" of="/dev/$partdev" ibs=512 obs=1M skip="$start" \
				count="$size" conv=fsync
		else
			v "Unable to find partition $part device, skipped."
		fi
	done < /tmp/partmap.image
}
