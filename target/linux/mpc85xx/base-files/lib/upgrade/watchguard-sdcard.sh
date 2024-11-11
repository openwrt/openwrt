#
# Copyright (C) 2016 lede-project.org
#

watchguard_get_rootfs() {
	local rootfsdev

	if read cmdline < /proc/cmdline; then
		case "$cmdline" in
			*root=*)
				rootfsdev="${cmdline##*root=}"
				rootfsdev="${rootfsdev%% *}"
			;;
		esac

		echo "${rootfsdev}"
	fi
}

watchguard_do_flash() {
	local tar_file=$1
	local rootfs=$2
	local kernel_filename=$3
	local dtb_filename=$4
	
	# keep sure its unbound
	losetup --detach-all || {
		echo Failed to detach all loop devices. Skip this try.
		reboot -f
	}

	# use the first found directory in the tar archive
	local board_dir=$(tar tf $tar_file | grep -m 1 '^sysupgrade-.*/$')
	board_dir=${board_dir%/}
	
	# Mount loop for rootfs
	mkfs.ext4 -t ext2 -F -L rootfs $rootfs
	
	mkdir /tmp/new_rootfs
	mount -t ext2 $rootfs /tmp/new_rootfs && {		
		echo "flashing rootfs to ${rootfs}"
		tar xf $tar_file ${board_dir}/root -O >/tmp/rootfs_image
		local rootfs_image_loopdev="$(losetup -f)"
		echo "Using $rootfs_image_loopdev to temp mount sysupgrade rootfs..."
		losetup $rootfs_image_loopdev /tmp/rootfs_image
		mkdir /tmp/rootfs_image_mount
		mount $rootfs_image_loopdev /tmp/rootfs_image_mount
		cp -rp /tmp/rootfs_image_mount/* /tmp/new_rootfs
		umount /tmp/rootfs_image_mount
		# copy in uImage and dtb
		echo "Copy in uImage and dtb into rootfs..." 
		tar xf $tar_file ${board_dir}/kernel -O >/tmp/new_rootfs/$kernel_filename
		tar xf $tar_file ${board_dir}/dtb -O >/tmp/new_rootfs/$dtb_filename
		
		cp -v "$UPGRADE_BACKUP" "/tmp/new_rootfs/$BACKUP_FILE"
		
		umount /tmp/new_rootfs
		losetup -d $rootfs_image_loopdev >/dev/null 2>&1
	}

	# Cleanup
	sync
	umount -a
	reboot -f
}

watchguard_sdcard_do_upgrade() {
	local tar_file="$1"
	local kernel_filename="$2"
	local dtb_filename="$3"
	local board=$(board_name)
	local rootfs="$(watchguard_get_rootfs)"
	[ -b "${rootfs}" ] || return 1

	watchguard_do_flash $tar_file $rootfs $kernel_filename $dtb_filename

	nand_do_upgrade "$1"

	return 0
}
