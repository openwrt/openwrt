# SPDX-License-Identifier: GPL-2.0-only
# RTX830: SPI bootloader stays unchanged. Update only the running internal SD.

rtx830_upgrade_error() {
	v "RTX830: $*"
	return 1
}

rtx830_require_block_devices() {
	[ -b "$RTX830_BOOTDEV" ] && [ -b "$RTX830_ROOTDEV" ]
}

# A buffered read immediately after fsync can still hit cached pages.
rtx830_drop_caches() {
	sync && printf '3\n' > /proc/sys/vm/drop_caches
}

rtx830_upgrade_devices() {
	local disk boot root path table
	[ "$(board_name)" = "yamaha,rtx830" ] || return 1
	[ "$(cmdline_get_var root)" = "PARTUUID=83002026-02" ] || return 1
	export_bootdevice && export_partdevice disk 0 &&
		export_partdevice boot 1 && export_partdevice root 2 || return 1
	# The GL834 reader is on the internal USB2 hub's port 2, never the USB socket.
	path="$(readlink -f "/sys/class/block/$disk/device")"
	case "$path" in
	*/f1058000.usb/usb*/*-1.2/*) ;;
	*) rtx830_upgrade_error "root device is not the internal SD reader"; return 1 ;;
	esac
	[ "$(cat "/sys/class/block/$boot/start")" = 2048 ] &&
	[ "$(cat "/sys/class/block/$boot/size")" = 131072 ] &&
	[ "$(cat "/sys/class/block/$root/start")" = 133120 ] &&
	[ "$(cat "/sys/class/block/$root/size")" = 1048576 ] || {
		rtx830_upgrade_error "unsupported SD partition layout"; return 1
	}
	[ "$(dd if="/dev/$disk" bs=1 skip=440 count=4 2>/dev/null | hexdump -v -e '4/1 "%02x"')" = 26200083 ] || return 1
	[ "$(dd if="/dev/$disk" bs=1 skip=510 count=2 2>/dev/null | hexdump -v -e '2/1 "%02x"')" = 55aa ] || return 1
	# Reject GPT, extra partitions, and filesystem types other than FAT32/ext4.
	table="$(hexdump -v -s 446 -n 64 -e '16/1 "%02x" "\n"' "/dev/$disk")"
	printf '%s\n' "$table" | awk '
		NR == 1 { if (substr($0,9,2) != "0c") exit 1 }
		NR == 2 { if (substr($0,9,2) != "83") exit 1 }
		NR > 2 { if ($0 != "00000000000000000000000000000000") exit 1 }
		END { if (NR != 4) exit 1 }
	' || return 1
	RTX830_BOOTDEV="/dev/$boot"
	RTX830_ROOTDEV="/dev/$root"
	rtx830_require_block_devices
}

rtx830_upgrade_cleanup() {
	[ -n "${RTX830_IMAGE_DIR:-}" ] && rm -rf "$RTX830_IMAGE_DIR"
	RTX830_IMAGE_DIR=
}

# Check both the producer and consumer; ash pipelines otherwise hide tar errors.
rtx830_root_stream() {
	local dir="$RTX830_IMAGE_DIR" producer rc
	mkfifo "$dir/root.pipe" || return 1
	tar -xOzf "$dir/image.tar.gz" sysupgrade-yamaha_rtx830/root > "$dir/root.pipe" &
	producer=$!
	"$@" < "$dir/root.pipe"
	rc=$?
	wait "$producer" || rc=1
	rm -f "$dir/root.pipe"
	return "$rc"
}

rtx830_upgrade_archive() {
	local image="$1" dir prefix=sysupgrade-yamaha_rtx830
	# Revalidate metadata even when sysupgrade -F was used.
	REQUIRE_IMAGE_METADATA=1 fwtool_check_image "$image" || return 1
	RTX830_IMAGE_DIR="$(mktemp -d /tmp/rtx830-upgrade.XXXXXX)" || return 1
	dir="$RTX830_IMAGE_DIR"
	# Strip fwtool metadata using the standard read-only stdout operation.
	# Keeping only this compressed copy avoids an uncompressed 512 MiB RAM file.
	fwtool -q -T -i /dev/null "$image" > "$dir/image.tar.gz" || return 1
	# One complete decompression validates both gzip CRC and tar listing.
	# Drain after tar EOF so the producer can validate its compressed footer.
	local producer rc
	mkfifo "$dir/list.pipe" || return 1
	busybox gunzip -c "$dir/image.tar.gz" > "$dir/list.pipe" &
	producer=$!
	{
		tar -tvf - > "$dir/list"
		rc=$?
		cat > /dev/null || rc=1
	} < "$dir/list.pipe"
	wait "$producer" || rc=1
	rm -f "$dir/list.pipe"
	[ "$rc" = 0 ] || return 1
	# Strict members, regular files, no duplicate names or link entries.
	awk -v p="$prefix" '
		{ name=$NF; type=substr($1,1,1) }
		name == p "/" { if (type != "d" || seen[name]++) exit 1; next }
		name == p "/CONTROL" || name == p "/kernel" ||
		name == p "/dtb" || name == p "/root" {
			if (type != "-" || seen[name]++) exit 1
			if (name == p "/root" && $3 != 536870912) exit 1
			if (name == p "/kernel" && ($3 < 1048576 || $3 > 16777216)) exit 1
			if (name == p "/dtb" && ($3 < 40 || $3 > 1048576)) exit 1
			if (name == p "/CONTROL" && $3 > 1024) exit 1
			files++
			next
		}
		{ exit 1 }
		END { if (files != 4 || !seen[p "/"]) exit 1 }
	' "$dir/list" || return 1
	# Extract the three small members together, never the 512 MiB root.
	tar -xzf "$dir/image.tar.gz" -C "$dir" \
		"$prefix/CONTROL" "$prefix/kernel" "$prefix/dtb" || return 1
	for name in CONTROL kernel dtb; do
		mv "$dir/$prefix/$name" "$dir/$name" || return 1
	done
	[ "$(cat "$dir/CONTROL")" = BOARD=yamaha_rtx830 ] || return 1
	[ "$(hexdump -v -s 36 -n 4 -e '4/1 "%02x"' "$dir/kernel")" = 18286f01 ] || return 1
	[ "$(hexdump -v -n 4 -e '4/1 "%02x"' "$dir/dtb")" = d00dfeed ] || return 1
	# The preceding DTB property-name offset can itself be a printable byte.
	strings "$dir/dtb" | grep 'yamaha,rtx830$' > /dev/null || return 1
	# Verify ext4 signature and filesystem size, not only the tar member length.
	tar -xOzf "$dir/image.tar.gz" "$prefix/root" 2>/dev/null | dd bs=1024 count=2 iflag=fullblock 2>/dev/null > "$dir/superblock"
	[ "$(hexdump -v -s 1080 -n 2 -e '2/1 "%02x"' "$dir/superblock")" = 53ef ] || return 1
	# The pinned image uses 4096-byte blocks: 131072 blocks == 512 MiB.
	[ "$(hexdump -v -s 1028 -n 4 -e '4/1 "%02x"' "$dir/superblock")" = 00000200 ] || return 1
	[ "$(hexdump -v -s 1048 -n 4 -e '4/1 "%02x"' "$dir/superblock")" = 02000000 ] || return 1
	[ "$(hexdump -v -s 1360 -n 4 -e '4/1 "%02x"' "$dir/superblock")" = 00000000 ] || return 1
}

# Also called before switching to the RAM root: -F must not bypass this.
rtx830_upgrade_running_root() {
	local mounted_root
	rtx830_upgrade_devices || return 1
	# The chosen SD must also be the current ext4 root, not merely share its UUID.
	mounted_root="$(awk '$5 == "/" { print $3; exit }' /proc/self/mountinfo)"
	[ "$mounted_root" = "$(cat "/sys/class/block/${RTX830_ROOTDEV##*/}/dev")" ]
}

rtx830_check_image() {
	local rc=0 mountpoint
	rtx830_upgrade_running_root || return 1
	rtx830_upgrade_archive "$1" || rc=1
	if [ "$rc" = 0 ]; then
		# Check FAT mounting before services stop; this also loads its modules.
		mountpoint="$RTX830_IMAGE_DIR/boot"
		mkdir "$mountpoint"
		mount -t vfat -o ro "$RTX830_BOOTDEV" "$mountpoint" || rc=1
		[ "$rc" != 0 ] || umount "$mountpoint" || rc=1
	fi
	rtx830_upgrade_cleanup
	[ "$rc" = 0 ] || rtx830_upgrade_error "image or SD validation failed"
}

# Standard upgraded owns PID1 during stage2, so a normal poweroff cannot be
# delivered there. Retain supply and finish the update before the final reboot.
rtx830_defer_poweroff() {
	[ -x /bin/true ] && [ -w /proc/sys/kernel/poweroff_cmd ] || return 1
	printf '%s\n' /bin/true > /proc/sys/kernel/poweroff_cmd || return 1
	[ "$(cat /proc/sys/kernel/poweroff_cmd)" = /bin/true ]
}

rtx830_do_upgrade() {
	local dir hash
	# A stage2 failure still reboots under standard upgraded. Never retry writes.
	rtx830_defer_poweroff || exit 1
	rtx830_upgrade_devices && rtx830_upgrade_archive "$1" || {
		rtx830_upgrade_cleanup; v "RTX830 validation failed; no SD write performed"; exit 1
	}
	dir="$RTX830_IMAGE_DIR"
	rtx830_root_stream sha256sum > "$dir/root.sha256" || exit 1
	mkdir "$dir/boot" "$dir/rootfs"
	mount -t vfat -o rw,noatime "$RTX830_BOOTDEV" "$dir/boot" || exit 1
	# Reserve space and verify the two new files before modifying rootfs.
	cp "$dir/kernel" "$dir/boot/zImage.new" &&
		cp "$dir/dtb" "$dir/boot/rtx830.dtb.new" && rtx830_drop_caches &&
		cmp "$dir/kernel" "$dir/boot/zImage.new" &&
		cmp "$dir/dtb" "$dir/boot/rtx830.dtb.new" || exit 1
	v "Writing RTX830 SD rootfs (SPI and partition table unchanged)"
	rtx830_require_block_devices || exit 1
	rtx830_root_stream dd of="$RTX830_ROOTDEV" bs=1M conv=fsync || exit 1
	rtx830_drop_caches || exit 1
	hash="$(dd if="$RTX830_ROOTDEV" bs=1M count=512 2>/dev/null | sha256sum | cut -d ' ' -f 1)"
	[ "$hash" = "$(cut -d ' ' -f 1 "$dir/root.sha256")" ] || {
		v "RTX830 rootfs readback failed; standard reboot follows; SD recreation may be needed"; exit 1
	}
	# Once root is verified, keep its kernel/DTB in step even if saving
	# the configuration later fails. An interrupted pair still needs recovery.
	mv -f "$dir/boot/zImage.new" "$dir/boot/zImage" &&
		mv -f "$dir/boot/rtx830.dtb.new" "$dir/boot/rtx830.dtb" || exit 1
	sync
	umount "$dir/boot" || exit 1
	mount -t ext4 -o rw,noatime "$RTX830_ROOTDEV" "$dir/rootfs" || exit 1
	if [ -n "$UPGRADE_BACKUP" ]; then
		# Never leave a truncated archive for preinit to restore.
		cp "$UPGRADE_BACKUP" "$dir/rootfs/$BACKUP_FILE.new" &&
			cmp "$UPGRADE_BACKUP" "$dir/rootfs/$BACKUP_FILE.new" &&
			mv -f "$dir/rootfs/$BACKUP_FILE.new" "$dir/rootfs/$BACKUP_FILE" || {
			rm -f "$dir/rootfs/$BACKUP_FILE.new"
			v "RTX830 settings backup failed; new kernel/root are installed, saved settings may be needed"
			exit 1
		}
	fi
	sync
	umount "$dir/rootfs" || exit 1
	rtx830_upgrade_cleanup
}
