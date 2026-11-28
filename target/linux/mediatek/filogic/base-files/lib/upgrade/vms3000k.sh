#!/bin/sh

if [ "$(board_name)" = tenbay,v-ms3000k ]; then
	RAMFS_COPY_BIN="$RAMFS_COPY_BIN sha256sum head readlink mktemp tr ubiblock ubiupdatevol"
fi
VMS_KERNEL_BYTES=8126464
VMS_ROOTFS_BYTES=9142272

vms_slot_ready() {
	local u=/sys/class/ubi/ubi0 m=/sys/class/mtd/mtd5 v
	[ "$(cat /tmp/sysinfo/board_name)" = tenbay,v-ms3000k ] &&
	grep -q 'ubi.mtd=ubi1 root=/dev/ubiblock0_1' /proc/cmdline &&
	[ "$(cat "$u/mtd_num")" = 5 ] && [ "$(cat "$u/ro_mode")" = 0 ] &&
	[ ! -e /sys/class/ubi/ubi1 ] && [ "$(cat "$u/volumes_count")" = 3 ] &&
	[ "$(cat "$m/name")" = ubi1 ] && [ "$(cat "$m/offset")" = 56098816 ] &&
	[ "$(cat "$m/size")" = 50331648 ] && [ "$(cat "$m/flags")" = 0x400 ] &&
	[ "$(cat /sys/class/mtd/mtd4/name)" = ubi ] && [ "$(cat /sys/class/mtd/mtd4/flags)" = 0x0 ] &&
	[ "$(cat "$m/ecc_failures")" = 0 ] && [ "$(cat "$u/bad_peb_count")" = 0 ] &&
	[ "$(cat "${u}_0/name")" = kernel ] && [ "$(cat "${u}_0/reserved_ebs")" = 64 ] &&
	[ "$(cat "${u}_1/name")" = rootfs ] && [ "$(cat "${u}_1/reserved_ebs")" = 72 ] &&
	[ "$(cat "${u}_2/name")" = rootfs_data ] && [ "$(cat "${u}_2/reserved_ebs")" = 225 ] || return 1
	for v in 0 1 2; do
		[ -c "/dev/ubi0_$v" ] && [ "$(cat "${u}_$v/type")" = dynamic ] && [ "$(cat "${u}_$v/usable_eb_size")" = 126976 ] &&
		[ "$(cat "${u}_$v/upd_marker")$(cat "${u}_$v/corrupted")" = 00 ] || return 1
	done
}

vms_unpack() {
	local image="$1" dir="$2" listing
	[ -f "$image" ] && [ "$(wc -c < "$image")" -le 17300000 ] || return 1
	listing="$(tar tf "$image")" || return 1
	[ "$listing" = "format
kernel
rootfs
sha256" ] || return 1
	tar xOf "$image" format > "$dir/format" &&
	[ "$(cat "$dir/format")" = 'tenbay,v-ms3000k:ubi1:v2' ] &&
	tar xOf "$image" kernel > "$dir/kernel" && tar xOf "$image" rootfs > "$dir/rootfs" &&
	tar xOf "$image" sha256 > "$dir/sha256" || return 1
	[ "$(wc -c < "$dir/kernel")" -le "$VMS_KERNEL_BYTES" ] &&
	[ "$(wc -c < "$dir/rootfs")" -le "$VMS_ROOTFS_BYTES" ] &&
	[ "$(hexdump -n4 -e '4/1 "%02x"' "$dir/kernel")" = d00dfeed ] &&
	[ "$(hexdump -n4 -e '4/1 "%02x"' "$dir/rootfs")" = 68737173 ] || return 1
	(cd "$dir" && sha256sum kernel rootfs > calculated &&
		[ "$(cat calculated)" = "$(cat sha256)" ])
}

vms_check_image() {
	local dir rc=0
	[ "${SAVE_CONFIG:-1}" = 1 ] && [ "${FORCE:-0}" = 0 ] &&
		[ "${FWTOOL_DEVICE_MATCH:-1}" = 0 ] || return 74
	vms_slot_ready || {
		echo "V-MS3000K requires the 64/72/225-LEB ubi1 layout. Migrate from RAM before upgrading." >&2
		return 74
	}
	grep -q ' /overlay ubifs rw' /proc/mounts &&
		grep -qx 'tenbay,v-ms3000k:ubi1:v2' /overlay/.vms-openwrt-overlay || return 74
	dir="$(mktemp -d /tmp/vms-check.XXXXXX)" || return 74
	vms_unpack "$1" "$dir" || rc=74
	rm -r "$dir"
	return "$rc"
}

vms_write_volume() {
	local volume="$1" image="$2" bytes="$3" verify="$4" length
	length="$(wc -c < "$image")" || return 1
	vms_slot_ready && ubiupdatevol "$volume" "$image" &&
		cat "$volume" > "$verify" && [ "$(wc -c < "$verify")" = "$bytes" ] &&
		[ "$(head -c "$length" "$verify" | sha256sum)" = "$(sha256sum < "$image")" ] &&
		[ "$(tail -c "+$((length + 1))" "$verify" | tr -d '\377' | wc -c)" = 0 ] &&
		vms_slot_ready
}

# fstools moves the archive to the overlay upper directory at boot.
vms_save_config() {
	local volume="$1" backup="$2" mountpoint status=1
	mountpoint="$(mktemp -d /tmp/vms-config.XXXXXX)" || return 1
	if mount -t ubifs "$volume" "$mountpoint"; then
		if grep -qx 'tenbay,v-ms3000k:ubi1:v2' "$mountpoint/.vms-openwrt-overlay" &&
			[ "$(readlink "$mountpoint/.fs_state")" = 2 ] &&
			[ ! -e "$mountpoint/sysupgrade.tgz" ] &&
			cp "$backup" "$mountpoint/sysupgrade.tgz.new" &&
			[ "$(sha256sum < "$backup")" = "$(sha256sum < "$mountpoint/sysupgrade.tgz.new")" ] &&
			mv "$mountpoint/sysupgrade.tgz.new" "$mountpoint/sysupgrade.tgz" && sync; then
			status=0
		fi
		umount "$mountpoint" || return 1
	fi
	rmdir "$mountpoint"
	return "$status"
}

vms_write_upgrade() {
	local dir
	[ -n "$UPGRADE_BACKUP" ] && [ -f "$UPGRADE_BACKUP" ] || return 1
	[ "$(rootfs_type)" = tmpfs ] || return 1
	! grep -q ' /overlay ubifs ' /proc/mounts || return 1
	vms_slot_ready || return 1
	dir="$(mktemp -d /tmp/vms-write.XXXXXX)" || return 1
	vms_unpack "$1" "$dir" || return 1
	if [ -e /dev/ubiblock0_1 ]; then
		ubiblock -r /dev/ubi0_1 || return 1
	fi
	[ ! -e /sys/class/block/ubiblock0_1 ] || return 1
	vms_write_volume /dev/ubi0_1 "$dir/rootfs" "$VMS_ROOTFS_BYTES" "$dir/readback" &&
		vms_write_volume /dev/ubi0_0 "$dir/kernel" "$VMS_KERNEL_BYTES" "$dir/readback" &&
		vms_save_config /dev/ubi0_2 "$UPGRADE_BACKUP" && sync
}

vms_do_upgrade() {
	vms_write_upgrade "$1" && return 0
	v "V-MS3000K upgrade stopped. Inspect UART before rebooting."
	while :; do sleep 60; done
}
