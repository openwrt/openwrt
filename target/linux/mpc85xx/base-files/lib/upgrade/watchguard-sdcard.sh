# SPDX-License-Identifier: GPL-2.0-only
#
# WatchGuard T30-W SD card upgrade script
#
# SD Card Layout:
#   p1: 1MB placeholder (reserved)
#   p2: squashfs root (kernel boots with root=/dev/mmcblk0p2)
#   p3: ext2 boot partition (U-Boot loads kernel from here)
#   p4: ext4 rootfs_data (overlay/config - NEVER touch during upgrade)
#
# During sysupgrade:
#   - p2: Extract new squashfs root
#   - p3: Write new kernel image
#   - p4: PRESERVED - never touched
#

watchguard_get_rootfs() {
	local cmdline rootfsdev

	if read -r cmdline < /proc/cmdline; then
		case "$cmdline" in
			*root=*)
				rootfsdev="${cmdline##*root=}"
				rootfsdev="${rootfsdev%% *}"
				;;
		esac

		[ -n "$rootfsdev" ] || {
			echo "watchguard-sdcard: cannot determine root device from /proc/cmdline" >&2
			return 1
		}
		echo "$rootfsdev"
	fi
}

watchguard_cleanup() {
	umount /tmp/new_bootfs >/dev/null 2>&1
	rmdir /tmp/new_bootfs >/dev/null 2>&1
}

# Get the squashfs root and boot partition devices
# Input: current root device (e.g., /dev/mmcblk0p2)
# Output: rootfs bootfs (space-separated)
watchguard_get_partitions() {
	local rootfsdev="$1"
	local diskdev

	rootfsdev="${rootfsdev#/dev/}"

	# Validate that rootfsdev ends in pN (e.g. mmcblk0p2)
	case "$rootfsdev" in
		*p[0-9]|*p[0-9][0-9])
			;;
		*)
			echo "watchguard-sdcard: unexpected root device format: /dev/$rootfsdev" >&2
			return 1
			;;
	esac

	diskdev="${rootfsdev%p*}"

	# SD card layout:
	# p1 = placeholder
	# p2 = squashfs root
	# p3 = boot partition
	# p4 = rootfs_data (overlay - never touch)
	#
	# U-Boot loads kernel from p3, kernel mounts p2 as root
	# p4 contains rootfs_data overlay and must be preserved

	echo "/dev/${diskdev}p2 /dev/${diskdev}p3"
}

watchguard_do_upgrade() {
	local tar_file="$1"
	local rootfs="$2"
	local bootfs="$3"
	local board_dir

	board_dir="$(tar tf "$tar_file" | grep -m 1 '^sysupgrade-.*/$')"
	board_dir="${board_dir%/}"
	[ -n "$board_dir" ] || return 1

	watchguard_cleanup
	trap 'watchguard_cleanup' EXIT

	# Write new kernel to boot partition (p3)
	# U-Boot loads kernel from this ext2 partition
	tar xf "$tar_file" "$board_dir/kernel" -O | dd of="$bootfs" bs=512k conv=fsync || return 1

	# Write new squashfs root (p2) — check it fits in the partition first
	local rootfs_part_sectors rootfs_part_bytes root_size
	rootfs_part_sectors="$(cat /sys/class/block/$(basename "$rootfs")/size 2>/dev/null || echo 0)"
	rootfs_part_bytes=$((rootfs_part_sectors * 512))
	root_size="$(tar tvf "$tar_file" "$board_dir/root" 2>/dev/null | awk '{print $3}' | head -1)"
	root_size="${root_size:-0}"
	if [ "$rootfs_part_bytes" -gt 0 ] && [ "$root_size" -gt "$rootfs_part_bytes" ]; then
		echo "watchguard-sdcard: new root ($root_size bytes) exceeds partition $rootfs ($rootfs_part_bytes bytes)" >&2
		return 1
	fi
	# This replaces the base OS while preserving p4 overlay
	tar xf "$tar_file" "$board_dir/root" -O | dd of="$rootfs" bs=512k conv=fsync || return 1
	# p4 (rootfs_data) is NOT touched - user config is preserved

	# Handle config backup to boot partition
	if [ -f "$UPGRADE_BACKUP" ]; then
		mkdir -p /tmp/new_bootfs
		mount -t ext2 "$bootfs" /tmp/new_bootfs || return 1
		cp "$UPGRADE_BACKUP" "/tmp/new_bootfs/$BACKUP_FILE" || return 1
		watchguard_cleanup
	fi

	trap - EXIT
	sync
	reboot -f
}

watchguard_sdcard_do_upgrade() {
	local tar_file="$1"
	local rootfsdev parts rootfs bootfs

	rootfsdev="$(watchguard_get_rootfs)"
	[ -n "$rootfsdev" ] || return 1

	parts="$(watchguard_get_partitions "$rootfsdev")"
	rootfs="${parts%% *}"
	bootfs="${parts##* }"
	[ -b "$rootfs" ] || return 1
	[ -b "$bootfs" ] || return 1

	# p4 (rootfs_data) is NOT referenced here - it's never touched
	# This preserves user config, packages, and any expanded partition size
	watchguard_do_upgrade "$tar_file" "$rootfs" "$bootfs"
}
