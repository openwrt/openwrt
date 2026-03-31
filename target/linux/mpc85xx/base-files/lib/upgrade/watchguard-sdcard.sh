# SPDX-License-Identifier: GPL-2.0-only

watchguard_get_rootfs() {
	local cmdline rootfsdev

	if read -r cmdline < /proc/cmdline; then
		case "$cmdline" in
			*root=*)
				rootfsdev="${cmdline##*root=}"
				rootfsdev="${rootfsdev%% *}"
				;;
		esac

		echo "$rootfsdev"
	fi
}

watchguard_cleanup_flash() {
	umount /tmp/new_bootfs >/dev/null 2>&1
	rmdir /tmp/new_bootfs >/dev/null 2>&1
}

watchguard_get_partitions() {
	local rootfsdev="$1"
	local diskdev

	rootfsdev="${rootfsdev#/dev/}"
	diskdev="${rootfsdev%p*}"

	echo "/dev/${diskdev}p3 /dev/${diskdev}p4"
}

watchguard_do_flash() {
	local tar_file="$1"
	local bootfs="$2"
	local rootfs="$3"
	local board_dir

	board_dir="$(tar tf "$tar_file" | grep -m 1 '^sysupgrade-.*/$')"
	board_dir="${board_dir%/}"
	[ -n "$board_dir" ] || return 1

	watchguard_cleanup_flash
	trap 'watchguard_cleanup_flash' EXIT

	tar xf "$tar_file" "$board_dir/kernel" -O | dd of="$bootfs" bs=512k conv=fsync || return 1
	dd if=/dev/zero of="$rootfs" bs=1M > /dev/null 2>&1
	tar xf "$tar_file" "$board_dir/root" -O | dd of="$rootfs" bs=512k conv=fsync || return 1

	if [ -f "$UPGRADE_BACKUP" ]; then
		mkdir -p /tmp/new_bootfs
		mount -t ext2 "$bootfs" /tmp/new_bootfs || return 1
		cp "$UPGRADE_BACKUP" "/tmp/new_bootfs/$BACKUP_FILE" || return 1
		watchguard_cleanup_flash
	fi

	trap - EXIT
	sync
	reboot -f
}

watchguard_sdcard_do_upgrade() {
	local tar_file="$1"
	local rootfsdev parts bootfs rootfs

	rootfsdev="$(watchguard_get_rootfs)"
	[ -n "$rootfsdev" ] || return 1

	parts="$(watchguard_get_partitions "$rootfsdev")"
	bootfs="${parts%% *}"
	rootfs="${parts##* }"
	[ -b "$bootfs" ] || return 1
	[ -b "$rootfs" ] || return 1

	watchguard_do_flash "$tar_file" "$bootfs" "$rootfs"
}
