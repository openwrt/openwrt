platform_check_image() {
	true
}

platform_copy_config() {
	local partdev

	if export_partdevice partdev -1; then
		mount -t vfat -o rw,noatime "/dev/$partdev" /mnt
		cp -af "$CONF_TAR" /mnt/
		umount /mnt
	fi
}

platform_do_upgrade() {
	local diskdev partdev ibs diff

	if export_bootdevice && export_partdevice diskdev -2; then
		sync
		if [ "$SAVE_PARTITIONS" = "1" ]; then
			get_partitions "/dev/$diskdev" bootdisk

			#get block size
			if [ -f "/sys/block/$diskdev/queue/physical_block_size" ]; then
				ibs="$(cat "/sys/block/$diskdev/queue/physical_block_size")"
			else
				ibs=512
			fi

			#extract the boot sector from the image
			get_image "$@" | dd of=/tmp/image.bs count=1 bs=512b

			get_partitions /tmp/image.bs image

			#compare tables
			diff="$(grep -F -x -v -f /tmp/partmap.bootdisk /tmp/partmap.image)"
			if [ -n "$diff" ]; then
				echo "Partition layout is changed.  Full image will be written."
				ask_bool 0 "Abort" && exit

				get_image "$@" | dd of="/dev/$diskdev" bs=4096 conv=fsync
				return 0
			fi

			#write uboot image
			get_image "$@" | dd of="$diskdev" bs=1024 skip=8 seek=8 conv=notrunc
			#iterate over each partition from the image and write it to the boot disk
			while read part start size; do
				part="$(($part - 2))"
				if export_partdevice partdev $part; then
					echo "Writing image to /dev/$partdev..."
					get_image "$@" | dd of="/dev/$partdev" ibs="$ibs" obs=1M skip="$start" count="$size" conv=fsync
				else
					echo "Unable to find partition $part device, skipped."
				fi
			done < /tmp/partmap.image

			#copy partition uuid
			echo "Writing new UUID to /dev/$diskdev..."
			get_image "$@" | dd of="/dev/$diskdev" bs=1 skip=440 count=4 seek=440 conv=fsync
		else
			get_image "$@" | dd of="/dev/$diskdev" bs=4096 conv=fsync
		fi

		sleep 1
	fi
}
