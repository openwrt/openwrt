REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	local diskdev partdev diff extra_parts

	export_bootdevice && export_partdevice diskdev 0 || {
		echo "Unable to determine upgrade device"
		return 1
	}

	get_partitions "/dev/$diskdev" bootdisk

	#extract the boot sector from the image
	get_image "$@" | dd of=/tmp/image.bs count=1 bs=512b 2>/dev/null

	get_partitions /tmp/image.bs image

	#compare tables
	diff="$(grep -F -x -v -f /tmp/partmap.bootdisk /tmp/partmap.image)"

	# Detect partitions beyond the two defaults
	extra_parts="$(awk '$1 > 2 { printf "%s ", $1 }' /tmp/partmap.bootdisk)"

	rm -f /tmp/image.bs /tmp/partmap.bootdisk /tmp/partmap.image

	if [ -n "$diff" ]; then
		# Applying a changed layout requires rewriting the whole disk. If
		# non-default partitions exist, that destroys them, so refuse
		# unless the user opted in with -p (SAVE_PARTITIONS=0). The refusal
		# marks the image invalid-but-forceable, so sysupgrade aborts
		# unless -p or -F/--force is given.
		#
		# SAVE_PARTITIONS is exported by sysupgrade so it reaches this
		# (validate_firmware_image child) process. If it is unset, this is
		# not a sysupgrade run (e.g. a GUI image check), so default to the
		# safe choice and refuse: "$SAVE_PARTITIONS" != "0" is then true.
		if [ -n "$extra_parts" ] && [ "$SAVE_PARTITIONS" != "0" ]; then
			# Report the reason to GUI/RPC callers, which only see the
			# JSON test results and not the messages printed below.
			notify_firmware_test_result "partition_layout" 0
			v "Partition layout changed; refusing to erase non-default partition(s): ${extra_parts}"
			echo "The new image uses a different partition layout than the one"       >&2
			echo "currently installed. Applying it requires rewriting the partition"  >&2
			echo "table, which will DESTROY these non-default partition(s) and all"   >&2
			echo "data on them: ${extra_parts}"                                        >&2
			echo "Re-run sysupgrade with -p to proceed and erase them."                >&2
			return 1
		fi
		# Either only the default partitions are present or the user opted in with -p.
		notify_firmware_test_result "partition_layout" 1
		v "Partition layout has changed, full image will be written."
	fi

	return 0
}

platform_copy_config() {
	local partdev

	if export_partdevice partdev 1; then
		mount -o rw,noatime "/dev/$partdev" /mnt
		cp -af "$UPGRADE_BACKUP" "/mnt/$BACKUP_FILE"
		umount /mnt
	fi
}

platform_do_upgrade() {
	local diskdev partdev diff

	export_bootdevice && export_partdevice diskdev 0 || {
		echo "Unable to determine upgrade device"
		return 1
	}

	sync

	if [ "$UPGRADE_OPT_SAVE_PARTITIONS" = "1" ]; then
		get_partitions "/dev/$diskdev" bootdisk

		#extract the boot sector from the image
		get_image "$@" | dd of=/tmp/image.bs count=1 bs=512b

		get_partitions /tmp/image.bs image

		#compare tables
		diff="$(grep -F -x -v -f /tmp/partmap.bootdisk /tmp/partmap.image)"
	else
		diff=1
	fi

	if [ -n "$diff" ]; then
		get_image "$@" | dd of="/dev/$diskdev" bs=4096 conv=fsync

		# Separate removal and addtion is necessary; otherwise, partition 1
		# will be missing if it overlaps with the old partition 2
		partx -d - "/dev/$diskdev"
		partx -a - "/dev/$diskdev"

		return 0
	fi

	#iterate over each partition from the image and write it to the boot disk
	while read part start size; do
		if export_partdevice partdev $part; then
			echo "Writing image to /dev/$partdev..."
			get_image "$@" | dd of="/dev/$partdev" ibs="512" obs=1M skip="$start" count="$size" conv=fsync
		else
			echo "Unable to find partition $part device, skipped."
		fi
	done < /tmp/partmap.image

	#copy partition uuid
	echo "Writing new UUID to /dev/$diskdev..."
	get_image "$@" | dd of="/dev/$diskdev" bs=1 skip=440 count=4 seek=440 conv=fsync
}
