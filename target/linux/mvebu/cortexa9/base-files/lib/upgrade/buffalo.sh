. /lib/functions.sh

buffalo_terastation_check_image() {
	local file="$1"
	local gz="$(identify_if_gzip "$file")"
	local board_dir="$(tar t${gz}f "$1" | grep -m 1 '^sysupgrade-.*/$')"
	local rootdev_mjmn=$(grep "/dev/root" /proc/self/mountinfo | cut -d' ' -f3)
	local kerndev rootdev diskdev
	local kerndev_size kernbin_size rootdev_size rootbin_size

	board_dir="${board_dir%/}"

	if [ "$(rootfs_type)" = "tmpfs" ]; then
		v "sysupgrade on initramfs image is not supported"
		return 1
	fi

	nand_verify_tar_file "$file" "$gz" || return 1

	rootdev=$(basename $(readlink /sys/dev/block/$rootdev_mjmn))
	if [ -z "$rootdev" ]; then
		v "Failed to get root partition."
		return 1
	fi

	diskdev=${rootdev%%[1-9]}
	kerndev=$(grep "PARTNAME=kernel" /sys/class/block/${diskdev}[1-9]/uevent | \
			sed "s/.*\/\(${diskdev}[0-9]\)\/uevent:.*/\1/")
	if [ -z "$kerndev" ]; then
		v "Failed to get kernel partition."
		return 1
	fi

	kerndev_size=$(cat /sys/class/block/${kerndev}/size)
	kerndev_size=$((kerndev_size * 512))
	rootdev_size=$(cat /sys/class/block/${rootdev}/size)
	rootdev_size=$((rootdev_size * 512))
	kernbin_size=$( (tar xO${gz}f "$1" "$board_dir/kernel" | wc -c) 2>/dev/null)
	rootbin_size=$( (tar xO${gz}f "$1" "$board_dir/root" | wc -c) 2>/dev/null)

	# check kernel binary size (smaller than partsize - 1MB(for ext3 fs)?)
	if [ "$kernbin_size" -gt "$((kerndev_size - 1024 * 1024))" ]; then
		v "new kernel is larger than kernel partition!"
		return 1
	fi

	# check rootfs binary size
	if [ "$rootbin_size" -gt "$rootdev_size" ]; then
		v "new rootfs is larger than rootfs partition!"
		return 1
	fi

	echo ${kerndev} > /tmp/sysupgrade_kerndev.txt
	echo ${rootdev}:${rootdev_size} > /tmp/sysupgrade_rootdev.txt
}

buffalo_terastation_do_upgrade() {
	local gz="$(identify_if_gzip "$1")"
	local board_dir="$(tar t${gz}f "$1" | grep -m 1 '^sysupgrade-.*/$')"
	local kerndev=$(cat /tmp/sysupgrade_kerndev.txt)
	local rootdev=$(cat /tmp/sysupgrade_rootdev.txt | cut -d':' -f1)
	local rootdev_size=$(cat /tmp/sysupgrade_rootdev.txt | cut -d':' -f2)
	local rootbin_size rootpad_size

	board_dir="${board_dir%/}"

	rootbin_size=$( (tar xO${gz}f "$1" "$board_dir/root" | wc -c) 2> /dev/null)
	rootpad_size=$((rootdev_size - rootbin_size))

	mkdir /boot

	v "Updating kernel binary in $kerndev"
	mount -t ext4 /dev/$kerndev /boot
	tar xO${gz}f "$1" "$board_dir/kernel" > /boot/uImage.buffalo
	umount /boot

	v "Updating rootfs partition ($rootdev)"
	tar xO${gz}f "$1" "$board_dir/root" > /dev/$rootdev
	[ "$rootdev_size" -gt "$rootbin_size" ] &&
		dd if=/dev/zero of=/dev/$rootdev \
			iflag=count_bytes oflag=seek_bytes \
			count=$rootpad_size seek=$rootbin_size conv=notrunc
}

buffalo_terastation_copy_config() {
	local rootdev=$(cat /tmp/sysupgrade_rootdev.txt | cut -d':' -f1)

	mount -t ext4 /dev/$rootdev /boot && {
		v "Saving configurations to new rootfs ($rootdev)"
		cp "$UPGRADE_BACKUP" "/boot/$BACKUP_FILE"
		umount /boot
	}
}
