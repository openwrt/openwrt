gpt_check_image() {
	local file="$1"
	local i syspart_max="${2:-2}"
	local diskdev partdev diff clean

	export_bootdevice && export_partdevice diskdev 0 || {
		v "Unable to determine upgrade device"
		return 1
	}

	[ "$UPGRADE_OPT_SAVE_PARTITIONS" != "1" ] &&
		return 0

	get_partitions "/dev/$diskdev" bootdisk

	v "Extract boot sector from the image"
	get_image_dd "$file" of=/tmp/image.bs count=63 bs=512b

	get_partitions /tmp/image.bs image

	# compare tables
	diff="$(grep -F -x -v -f /tmp/partmap.bootdisk /tmp/partmap.image)"

	rm -f /tmp/image.bs /tmp/partmap.bootdisk /tmp/partmap.image

	# lookup system partitions of OpenWrt
	for i in $(seq 1 $syspart_max); do
		printf "$diff" | grep -q "^$i " &&
			clean=1
	done

	if [ -n "$clean" ]; then
		v "Layout of system partition(s) has changed. Full image will be written and user partition(s) will be removed."
		ask_bool 0 "Abort" && exit 1
		return 0
	fi
}

gpt_update_crc32() {
	local disk="$1"
	local offset="$2"
	local target="$3"
	local bs=4

	dd if=/dev/zero of="$disk" bs=$bs count=1 seek=$((offset / bs)) \
		conv=fsync,notrunc 2>/dev/null
	dd if="$disk" bs=$bs \
		skip=$((${target#*@} / bs)) \
		count=$((${target%@*} / bs)) 2>/dev/null | \
			gzip -c | tail -c8 | head -c4 > /tmp/crc32.bin
	dd if=/tmp/crc32.bin of="$disk" bs=$bs count=1 seek=$((offset / bs)) \
		conv=fsync,notrunc 2>/dev/null
}

gpt_do_upgrade() {
	local file="$1"
	local i syspart_max="${2:-2}"
	local diskdev partdev diff clean

	export_bootdevice && export_partdevice diskdev 0 || {
		v "Unable to determine upgrade device"
		return 1
	}

	sync

	if [ "$UPGRADE_OPT_SAVE_PARTITIONS" = "1" ]; then
		get_partitions "/dev/$diskdev" bootdisk

		v "Extract boot sector from the image"
		get_image_dd "$file" of=/tmp/image.bs count=63 bs=512b

		get_partitions /tmp/image.bs image

		# compare tables
		diff="$(grep -F -x -v -f /tmp/partmap.bootdisk /tmp/partmap.image)"

		# lookup system partitions of OpenWrt
		for i in $(seq 1 $syspart_max); do
			printf "$diff" | grep -q "^$i " &&
				clean=1
		done
	else
		clean=1
	fi

	if [ "$clean" = "1" ]; then
		get_image_dd "$file" of="/dev/$diskdev" bs=4096 conv=fsync

		# Separate removal and addtion is necessary; otherwise, partition 1
		# will be missing if it overlaps with the old partition 2
		partx -d - "/dev/$diskdev"
		partx -a - "/dev/$diskdev"

		return 0
	fi

	v "Writing new GPT header to /dev/$diskdev..."
	get_image_dd "$file" of="/dev/$diskdev" bs=512 skip=1 count=1 seek=1 conv=fsync

	#iterate over each partition from the image and write it to the boot disk
	while read part start size; do
		if export_partdevice partdev $part; then
			# update partition entries for partition GUID and others
			v "Writing new GPT partition entry of $partdev to /dev/$diskdev..."
			get_image_dd "$file" of="/dev/$diskdev" bs=128 count=1 skip=$((8 + (part - 1))) seek=$((8 + (part - 1))) conv=fsync
			v "Writing image to /dev/$partdev..."
			get_image_dd "$file" of="/dev/$partdev" ibs=512 obs=1M skip="$start" count="$size" conv=fsync
		else
			v "Unable to find partition $part device, skipped."
		fi
	done < /tmp/partmap.image

	v "Updating CRC32 values in GPT header..."
	gpt_update_crc32 "/dev/$diskdev" 0x258 "0x4000@0x400" # entry crc32
	gpt_update_crc32 "/dev/$diskdev" 0x210 "0x5c@0x200"   # header crc32

	v "Writing new UUID to /dev/$diskdev..."
	get_image_dd "$file" of="/dev/$diskdev" bs=1 skip=440 count=4 seek=440 conv=fsync
}

iodata_nas_check_image() {
	if part_magic_efi "$1"; then
		gpt_check_image "$1"
	else
		legacy_sdcard_check_image "$1"
	fi
}

iodata_nas_do_upgrade() {
	if part_magic_efi "$1"; then
		gpt_do_upgrade "$1"
	else
		legacy_sdcard_do_upgrade "$1"
	fi
}

iodata_nas_copy_config() {
	# the same way can be used for GPT
	legacy_sdcard_copy_config
}
