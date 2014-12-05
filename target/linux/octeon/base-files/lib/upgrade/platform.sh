#
# Copyright (C) 2014 OpenWrt.org
#

. /lib/functions/octeon.sh

platform_get_rootfs() {
	local rootfsdev

	if read cmdline < /proc/cmdline; then
		case "$cmdline" in
			*block2mtd=*)
				rootfsdev="${cmdline##*block2mtd=}"
				rootfsdev="${rootfsdev%%,*}"
			;;
			*root=*)
				rootfsdev="${cmdline##*root=}"
				rootfsdev="${rootfsdev%% *}"
			;;
		esac

		echo "${rootfsdev}"
	fi
}

platform_copy_config() {
	local board="$(octeon_board_name)"
	local rootfs="$(platform_get_rootfs)"

	mount -t ext4 -o rw,noatime "${rootfs}" /mnt
	cp -af "$CONF_TAR" /mnt/
	umount /mnt
}

platform_do_upgrade() {
	local board=$(octeon_board_name)
	local rootfs="$(platform_get_rootfs)"

	[ -b "${rootfs}" ] || return 1

	case "$board" in
	erlite)
		local tar_file="$1"
		local kernel_length=`(tar xf $tar_file sysupgrade-erlite/kernel -O | wc -c) 2> /dev/null`
		local rootfs_length=`(tar xf $tar_file sysupgrade-erlite/root -O | wc -c) 2> /dev/null`

		[ -f /boot/vmlinux.64 -a ! -L /boot/vmlinux.64 ] && {
			mv /boot/vmlinux.64 /boot/vmlinux.64.previous
			mv /boot/vmlinux.64.md5 /boot/vmlinux.64.md5.previous
		}

		mkdir -p /boot
		mount -t vfat /dev/sda1 /boot
		tar xf $tar_file sysupgrade-erlite/kernel -O > /boot/vmlinux.64
		md5sum /boot/vmlinux.64 | cut -f1 -d " " > /boot/vmlinux.64.md5
		tar xf $tar_file sysupgrade-erlite/root -O | dd of="${rootfs}" bs=4096
		sync
		umount /mnt
		return 0
		;;
	esac

	return 1
	
}

platform_check_image() {
	local board=$(octeon_board_name)

	case "$board" in
	erlite)
		local tar_file="$1"
		local kernel_length=`(tar xf $tar_file sysupgrade-erlite/kernel -O | wc -c) 2> /dev/null`
		local rootfs_length=`(tar xf $tar_file sysupgrade-erlite/root -O | wc -c) 2> /dev/null`
		[ "$kernel_length" = 0 -o "$rootfs_length" = 0 ] && {
			echo "The upgarde image is corrupt."
			return 1
		}
		return 0
		;;
	esac

	echo "Sysupgrade is not yet supported on $board."
	return 1
}
