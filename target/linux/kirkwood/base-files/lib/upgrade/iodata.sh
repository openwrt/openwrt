iodata_check_image() {
	local tar_file="$1"
	local board_dir=$(tar tf "$tar_file" | grep -m 1 '^sysupgrade-.*/$')
	local found_tools

	local kernsize rootsize kerndev_size rootdev_size datadev_size
	local data_min_size="$((64 * 1024 * 1024))"
	local kerndev="$(find_scsi_part kernel)"
	local rootdev="$(find_scsi_part rootfs)"
	local datadev="$(find_scsi_part rootfs_data)"

	if [ -z "$kerndev" -o -z "$rootdev" -o -z "$datadev" ]; then
		echo "partition layout is wong, please check"
		echo "required partitions for sysupgrade:"
		echo "  \"kernel\" -> $kerndev"
		echo "  \"rootfs\" -> $rootdev"
		echo "  \"rootfs_data\" -> $datadev"
		return 1
	fi

	if ! found_tools="$(which fdisk mkfs.ext3 mkfs.ext4)"; then
		echo "sysupgrade requires some commands, fdisk, mkfs.ext3 and mkfs.ext4"
		echo "found:"
		echo "$found_tools"
		return 1
	fi

	kernsize=$( (tar xf "$tar_file" ${board_dir}/kernel -O | wc -c) 2> /dev/null)
	rootsize=$( (tar xf "$tar_file" ${board_dir}/root -O | wc -c) 2> /dev/null)

	kerndev_size=$( (fdisk -l $kerndev | head -n1 | \
		grep -o "[0-9]\{1,16\} bytes" | cut -d' ' -f1) 2>/dev/null)
	rootdev_size=$( (fdisk -l $rootdev | head -n1 | \
		grep -o "[0-9]\{1,16\} bytes" | cut -d' ' -f1) 2>/dev/null)
	datadev_size=$( (fdisk -l $datadev | head -n1 | \
		grep -o "[0-9]\{1,16\} bytes" | cut -d' ' -f1) 2>/dev/null)


	# calcuate mergin for filesystem in kernel
	if [ "$kernsize" -ge "$((kerndev_size - 8 * 1024 * 1024))" -o \
	     "$rootsize" -ge "$rootdev_size" ]; then
		echo "current partition layout is not suitable for new image"
		echo "new image : kernel-> ${kernsize} bytes, rootfs-> ${rootsize}"
		echo "partitions: kernel-> ${kerndev_size} bytes, rootfs-> ${rootdev_size}"
		return 1
	fi

	# check size of rootfs_data partition
	if [ "$datadev_size" -lt "$data_min_size" ]; then
		echo "rootfs_data partition is smaller than ${data_min_size} bytes"
		return 1
	fi

	return 0
}

iodata_do_upgrade() {
	local uimage_suffix="$1"
	local tar_file="$2"
	local board_dir=$(tar tf "$tar_file" | grep -m 1 '^sysupgrade-.*/$')

	local kerndev="$(find_scsi_part kernel)"
	local rootdev="$(find_scsi_part rootfs)"
	local datadev="$(find_scsi_part rootfs_data)"

	if [ -z "$uimage_suffix" ]; then
		echo "no uImage name specified, exit..."
		exit 1
	fi

	# update kernel
	mkfs.ext3 -F $kerndev
	mkdir /mnt/$board_dir

	if ! mount -t ext3 $kerndev /mnt/$board_dir; then
		echo "failed to mount ${kerndev}, exit..."
		exit 1
	fi

	tar xf "$tar_file" ${board_dir}kernel -O \
		> /mnt/${board_dir}uImage.${uimage_suffix}

	# create dummy initrd
	dd if=/dev/zero bs=4 count=1 \
		of=/mnt/${board_dir}initrd.${uimage_suffix}

	sync
	if ! umount /mnt/${board_dir}; then
		echo "failed to unmount ${kerndev}, exit..."
		exit 1
	fi

	# update rootfs
	tar xf "$tar_file" ${board_dir}root -O > "$rootdev"

	# prepare rootfs_data
	mkfs.ext4 -F $datadev

	if [ -n "$UPGRADE_BACKUP" ]; then
		if ! mount -t ext4 $datadev /mnt/$board_dir; then
			echo "failed to mount ${datadev}, exit..."
			exit 1
		fi

		cp "$UPGRADE_BACKUP" /mnt/${board_dir}
	fi

	echo "sysupgrade successful"

	sync
	umount -a
	reboot -f
}
