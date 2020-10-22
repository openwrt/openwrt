#
# Copyright (C) 2014 OpenWrt.org
#

platform_octeon_copy_config() {
	local device="$1"
	[ -n "$device" ] && [ -b "$device" ] && {
		mount -t "vfat" "$device" "/mnt"
		cp -af "$UPGRADE_BACKUP" "/mnt/$BACKUP_FILE"
		umount "/mnt"
	}
}

platform_copy_config() {
	case "$(board_name)" in
	erlite)
		platform_octeon_copy_config "/dev/sda1"
		;;
	itus,shield-router)
		platform_octeon_copy_config "/dev/mmcblk1p1"
		;;
	esac
}

platform_do_flash() {
	local sysupgrade_file="$1"
	local kernel_file="$2"
	local kernel_checksum_file="$kernel_file.md5"
	local boot_device="$3"
	local root_device="$4"
	local sysupgrade_directory
	# find a sysupgrade directory by looking inside sysupgrade file
	sysupgrade_directory=$(tar tf "$sysupgrade_file" | awk -F '/' '/^sysupgrade/ {print $1; exit}')
	[ -n "$sysupgrade_directory" ] || return 1

	if [ ! -f "$sysupgrade_file" ] && \
		[ ! -b "$boot_device" ] && \
		[ ! -b "$root_device" ] ; then
		return 1
	fi

	local boot_directory="/boot"
	mkdir -p "$boot_directory"
	mount -t "vfat" "$boot_device" "$boot_directory"

	[ ! -e "$boot_directory/$kernel_checksum_file" ] && {
		md5sum "$kernel_file" | cut -f1 -d " " > "$kernel_checksum_file"
	}
	# backups current kernel file to a kernel file with .previous postfix
	if [ -f "$boot_directory/$kernel_file" ] && \
		[ ! -L "$boot_directory/$kernel_file" ] && \
		[ -f "$boot_directory/$kernel_checksum_file" ] && \
		[ ! -L "$boot_directory/$kernel_checksum_file" ] ; then
		mv "$kernel_file" "$kernel_file.previous"
		mv "$kernel_checksum_file" "$kernel_checksum_file.previous"
	fi

	echo "Copying kernel to $boot_directory/$kernel_file"
	tar xf "$sysupgrade_file" "$sysupgrade_directory/kernel" -O > "$kernel_file"
	md5sum "$kernel_file" | cut -f1 -d " " > "$kernel_checksum_file"

	echo "Flashing rootfs to $root_device"
	tar xf "$sysupgrade_file" "$sysupgrade_directory/root" -O | dd of="$root_device" bs=4096

	sync
	umount "$boot_directory"
	return 0
}

platform_do_upgrade() {
	local sysupgrade_file="$1"
	local kernel_file="vmlinux.64"
	local boot_device
	local root_device

	case "$(board_name)" in
		er)
			boot_device="/dev/mmcblk0p1"
			root_device="/dev/mmcblk0p2"
			;;
		erlite)
			boot_device="/dev/sda1"
			root_device="/dev/sda2"
			;;
		itus,shield-router)
			kernel_file="ItusrouterImage"
			boot_device="/dev/mmcblk1p1"
			root_device="/dev/mmcblk1p2"
			;;
		*)
			return 1
	esac

	if [ -n "$boot_device" ] && \
		[ -b "$boot_device" ] && \
		[ -n "$root_device" ] && \
		[ -b "$root_device" ] ; then
		platform_do_flash "$sysupgrade_file" "$kernel_file" "$boot_device" "$root_device"
	fi

	return 0
}

platform_check_image() {
	local sysupgrade_file="$1"
	# find a sysupgrade directory by looking inside sysupgrade file
	local sysupgrade_directory
	sysupgrade_directory=$(tar tf "$sysupgrade_file" | awk -F '/' '/^sysupgrade/ {print $1; exit}')
	[ -n "$sysupgrade_directory" ] || return 1

	case "$(board_name)" in
	er | \
	erlite | \
	itus,shield-router)
		local kernel_length
		local rootfs_length
		kernel_length=$(tar xf "$sysupgrade_file" "$sysupgrade_directory/kernel" -O | wc -c 2> /dev/null)
		rootfs_length=$(tar xf "$sysupgrade_file" "$sysupgrade_directory/root" -O | wc -c 2> /dev/null)
		[ "$kernel_length" = 0 ] || [ "$rootfs_length" = 0 ] && {
			echo "The upgrade image is corrupt."
			return 1
		}
		return 0
		;;
	esac

	echo "Sysupgrade is not yet supported on $(board_name)."
	return 1
}
