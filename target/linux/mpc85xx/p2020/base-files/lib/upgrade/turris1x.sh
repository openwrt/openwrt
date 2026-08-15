# SPDX-License-Identifier: GPL-2.0-or-later

. /lib/functions/turris1x.sh

# stage 2 runs from a ramfs that has only the commands it copies there
RAMFS_COPY_BIN="$RAMFS_COPY_BIN df"

# Check that the sysupgrade tar is complete and has a kernel and a root
# filesystem, and set turris1x_dir, turris1x_kernel_size and
# turris1x_root_size. A truncated tar fails the listing.
turris1x_parse_tar() {
	local tar_file="$1"
	local list

	list=$(tar tvf "$tar_file" 2>/dev/null) || {
		echo "$tar_file is not a complete tar archive"
		return 1
	}
	turris1x_dir=$(echo "$list" | awk '$NF ~ /^sysupgrade-.*\/$/ { print $NF; exit }')
	turris1x_dir=${turris1x_dir%/}
	turris1x_kernel_size=$(echo "$list" | awk -v f="$turris1x_dir/kernel" '$NF == f { print $3 }')
	turris1x_root_size=$(echo "$list" | awk -v f="$turris1x_dir/root" '$NF == f { print $3 }')
	[ -n "$turris1x_dir" ] && [ "${turris1x_kernel_size:-0}" -gt 0 ] &&
		[ "${turris1x_root_size:-0}" -gt 0 ] || {
		echo "$tar_file has no kernel and root filesystem for this board"
		return 1
	}
}

# libfstools looks for the overlay, or for a config backup to restore, at
# the first 64 KiB boundary after the root filesystem.
turris1x_overlay_block() {
	echo $(((($1 + 511) / 512 + 127) & ~127))
}

# The root filesystem has to fit the partition together with the config
# backup after it, or the 4 KiB that clear the old overlay.
turris1x_sdcard_check() {
	local tar_file="$1"
	local end

	turris1x_parse_tar "$tar_file" || return 1
	export_bootdevice && export_partdevice turris1x_bootdev 1 &&
		export_partdevice turris1x_rootdev 2 || {
		echo "Unable to determine upgrade device"
		return 1
	}

	end=$(($(turris1x_overlay_block "$turris1x_root_size") * 512 + 4096))
	[ -n "$UPGRADE_BACKUP" ] && [ -f "$UPGRADE_BACKUP" ] &&
		end=$((end - 4096 + $(wc -c < "$UPGRADE_BACKUP")))
	[ "$end" -le $(($(cat "/sys/class/block/$turris1x_rootdev/size") * 512)) ] || {
		echo "the root filesystem does not fit /dev/$turris1x_rootdev"
		return 1
	}
}

# The new kernel has to fit the FAT mounted on $1 next to the other files
# there, 64 KiB added for the cluster rounding. The old zImage is removed
# first.
turris1x_kernel_fits() {
	local free old=0

	free=$(df -k "$1" | awk 'NR == 2 { print $4 * 1024 }')
	[ -n "$free" ] || {
		echo "could not determine the free space on /dev/$turris1x_bootdev"
		return 1
	}
	[ -f "$1/zImage" ] && old=$(wc -c < "$1/zImage")
	[ $((free + old)) -ge $((turris1x_kernel_size + 65536)) ] || {
		echo "the kernel does not fit /dev/$turris1x_bootdev"
		return 1
	}
}

turris1x_sdcard_do_upgrade() {
	local tar_file="$1"
	local mnt=/tmp/turris1x_boot ret

	turris1x_sdcard_check "$tar_file" || return 1
	tar xOf "$tar_file" "$turris1x_dir/kernel" > /tmp/turris1x_kernel || {
		echo "could not unpack the kernel from $tar_file"
		return 1
	}

	# Remove the old kernel first and put the new one in place last, as
	# emmc_upgrade_tar() does, so that an interrupted upgrade leaves a
	# card that does not boot rather than a new kernel on the old root
	# filesystem. Check first that the new kernel will fit.
	mkdir -p "$mnt"
	mount -t vfat -o rw,noatime "/dev/$turris1x_bootdev" "$mnt" || return 1
	turris1x_kernel_fits "$mnt" || {
		umount "$mnt"
		return 1
	}
	rm -f "$mnt/zImage" "$mnt/zImage.new"
	sync
	umount "$mnt"

	tar xOf "$tar_file" "$turris1x_dir/root" > "/dev/$turris1x_rootdev" || {
		echo "could not write the root filesystem to /dev/$turris1x_rootdev"
		return 1
	}

	# libfstools restores a config backup that it finds at the overlay
	# offset, as on emmc_copy_config() boards. Without a backup, clear the
	# old overlay.
	if [ -n "$UPGRADE_BACKUP" ] && [ -f "$UPGRADE_BACKUP" ]; then
		dd if="$UPGRADE_BACKUP" of="/dev/$turris1x_rootdev" bs=512 \
			seek="$(turris1x_overlay_block "$turris1x_root_size")" 2>/dev/null ||
			echo "could not write the config backup to /dev/$turris1x_rootdev"
	else
		dd if=/dev/zero of="/dev/$turris1x_rootdev" bs=512 count=8 \
			seek="$(turris1x_overlay_block "$turris1x_root_size")" 2>/dev/null ||
			echo "could not clear the old overlay on /dev/$turris1x_rootdev"
	fi
	sync

	# Copy under a temporary name, so that a half written file is never
	# booted as zImage.
	mount -t vfat -o rw,noatime "/dev/$turris1x_bootdev" "$mnt" || return 1
	cp /tmp/turris1x_kernel "$mnt/zImage.new" && sync &&
		mv "$mnt/zImage.new" "$mnt/zImage"
	ret=$?
	[ "$ret" = 0 ] || rm -f "$mnt/zImage.new"
	sync
	umount "$mnt"
	[ "$ret" = 0 ] || {
		echo "could not write the kernel to /dev/$turris1x_bootdev"
		return 1
	}
}

turris1x_check_image() {
	local mnt=/tmp/turris1x_boot ret

	# Fail here rather than after the switch to ramfs, where do_stage2
	# ignores the result and reboots regardless. REQUIRE_IMAGE_METADATA
	# already rejects images of other boards. nand_do_platform_check also
	# accepts a raw FIT, UBI or UBIFS image, which neither upgrade path
	# writes, so require the sysupgrade tar and report anything else as
	# broken (74), which -F cannot override.
	nand_do_platform_check cznic,turris1x "$1" || return 1
	if turris1x_boots_from_nand; then
		turris1x_parse_tar "$1" || return 74
	else
		turris1x_sdcard_check "$1" || return 74
		mkdir -p "$mnt"
		mount -t vfat -o ro "/dev/$turris1x_bootdev" "$mnt" || return 74
		turris1x_kernel_fits "$mnt"
		ret=$?
		umount "$mnt"
		[ "$ret" = 0 ] || return 74
	fi
}

turris1x_do_upgrade() {
	if turris1x_boots_from_nand; then
		export CI_UBIPART="nand_ubi"
		# the NOR has an MTD partition called "kernel" as well, and that
		# one holds the factory rescue system
		export CI_SKIP_KERNEL_MTD=1
		nand_do_upgrade "$1"
	else
		turris1x_sdcard_do_upgrade "$1"
	fi
}
