platform_export_bootpart() {
	local cmdline uuid disk

	if read cmdline < /proc/cmdline; then
		case "$cmdline" in
			*block2mtd=*)
				disk="${cmdline##*block2mtd=}"
				disk="${disk%%,*}"
			;;
			*root=*)
				disk="${cmdline##*root=}"
				disk="${disk%% *}"
			;;
		esac

		case "$disk" in
			PARTUUID=[a-f0-9][a-f0-9][a-f0-9][a-f0-9][a-f0-9][a-f0-9][a-f0-9][a-f0-9]-02)
				uuid="${disk#PARTUUID=}"
				uuid="${uuid%-02}"
				for disk in /dev/*; do
					[ -b "$disk" ] || continue
					set -- $(dd if=$disk bs=1 skip=440 count=4 2>/dev/null | hexdump -v -e '4/1 "%02x "')
					if [ "$4$3$2$1" = "$uuid" ]; then
						export BOOTPART="${disk}1"
						return 0
					fi
				done
			;;
			/dev/*)
				export BOOTPART="${disk%[0-9]}1"
				return 0
			;;
		esac
	fi

	return 1
}

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
	if [ -b "$BOOTPART" ]; then
		mount -t ext4 -o rw,noatime "$BOOTPART" /mnt
		cp -af "$CONF_TAR" /mnt/
		umount /mnt
	fi
}

get_partitions() { # <device> <filename>
	local disk="$1"
	local filename="$2"

	if [ -b "$disk" -o -f "$disk" ]; then
		echo "Reading partition table from $filename..."

		local magic="$(hexdump -v -n 2 -s 0x1FE -e '1/2 "0x%04X"' "$disk")"
		if [ "$magic" != 0xAA55 ]; then
			echo "Invalid partition table on $disk"
			exit
		fi

		rm -f "/tmp/partmap.$filename"

		local part
		for part in 1 2 3 4; do
			set -- $(hexdump -v -n 12 -s "$((0x1B2 + $part * 16))" -e '3/4 "0x%08X "' "$disk")

			local type="$(($1 % 256))"
			local lba="$(($2))"
			local num="$(($3))"

			[ $type -gt 0 ] || continue

			printf "%2d %5d %7d\n" $part $lba $num >> "/tmp/partmap.$filename"
		done
	fi
}

platform_do_upgrade() {
	platform_export_bootpart
	disk="${BOOTPART%[0-9]}"

	if [ -b "$disk" ]; then
		sync
		if [ "$SAVE_PARTITIONS" = "1" ]; then
			get_partitions "$disk" bootdisk


			#get block size
			if [ -f "/sys/block/${disk##*/}/queue/physical_block_size" ]; then
				ibs="$(cat "/sys/block/${disk##*/}/queue/physical_block_size")"
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

				get_image "$@" | dd of="$disk" bs=4096 conv=fsync
				return 0
			fi

			#iterate over each partition from the image and write it to the boot disk
			while read part start size; do
			echo "Writing image to $disk$part..."
				get_image "$@" | dd of="$disk$part" ibs="$ibs" obs=1M skip="$start" count="$size" conv=fsync
			done < /tmp/partmap.image

			#copy partition uuid
			echo "Writing new UUID to $disk$part..."
			get_image "$@" | dd of="$disk" bs=1 skip=440 count=4 seek=440 conv=fsync
		else
			get_image "$@" | dd of="$disk" bs=4096 conv=fsync
		fi

		sleep 1
	fi
}
