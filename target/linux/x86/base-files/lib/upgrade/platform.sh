platform_check_image() {
	[ "$#" -gt 1 ] && return 1

	case "$(get_magic_word "$1")" in
		eb48|eb63) return 0;;
		*)
			echo "Invalid image type"
			return 1
		;;
	esac
}

platform_copy_config() {
	local partdev

	if export_partdevice partdev 1; then
		mount -t ext4 -o rw,noatime "/dev/$partdev" /mnt
		cp -af "$CONF_TAR" /mnt/
		umount /mnt
	fi
}

platform_do_upgrade() {
	local diskdev partdev diff

	if export_bootdevice && export_partdevice diskdev 0; then
		sync
		if [ "$SAVE_PARTITIONS" = "1" ]; then
			get_partitions "/dev/$diskdev" bootdisk

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
		else
			get_image "$@" | dd of="/dev/$diskdev" bs=4096 conv=fsync
		fi

		sleep 1
	fi
}
