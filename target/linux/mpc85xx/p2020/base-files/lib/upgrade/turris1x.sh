# SPDX-License-Identifier: GPL-2.0-or-later

. /lib/functions/turris1x.sh

RAMFS_COPY_BIN="$RAMFS_COPY_BIN df"

# from stdin, as busybox can seek past a truncation in a file
turris1x_parse_tar() {
	local tar_file="$1"
	local list

	list=$(tar tvf - < "$tar_file" 2>/dev/null) || {
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

# libfstools expects the overlay at the first 64 KiB boundary after root
turris1x_overlay_block() {
	echo $(((($1 + 511) / 512 + 127) & ~127))
}

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

# the old zImage is removed first, 64 KiB covers the cluster rounding
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

	# as emmc_upgrade_tar(): old kernel out first, new kernel in last
	mkdir -p "$mnt"
	mount -t vfat -o rw,noatime "/dev/$turris1x_bootdev" "$mnt" || return 1
	turris1x_kernel_fits "$mnt" || {
		umount "$mnt"
		return 1
	}
	rm -f "$mnt/zImage" "$mnt/zImage.new" || {
		umount "$mnt"
		return 1
	}
	sync
	umount "$mnt"

	tar xOf "$tar_file" "$turris1x_dir/root" > "/dev/$turris1x_rootdev" || {
		echo "could not write the root filesystem to /dev/$turris1x_rootdev"
		return 1
	}

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
	local mnt=/tmp/turris1x_boot opt=ro ret

	# do_stage2 ignores errors, so check everything here. Anything but the
	# sysupgrade tar is broken (74), which -F cannot override.
	nand_do_platform_check cznic,turris1x "$1" || return 1
	if turris1x_boots_from_nand; then
		turris1x_parse_tar "$1" || return 74
	else
		turris1x_sdcard_check "$1" || return 74
		# a second mount has to keep the ro/rw state of the first
		grep -q "^/dev/$turris1x_bootdev [^ ]* vfat rw[, ]" /proc/mounts &&
			opt=rw,noatime
		mkdir -p "$mnt"
		mount -t vfat -o "$opt" "/dev/$turris1x_bootdev" "$mnt" || return 74
		turris1x_kernel_fits "$mnt"
		ret=$?
		umount "$mnt"
		[ "$ret" = 0 ] || return 74
	fi
}

turris1x_do_upgrade() {
	if turris1x_boots_from_nand; then
		export CI_UBIPART="nand_ubi"
		# "kernel" on the NOR holds the factory rescue system
		export CI_SKIP_KERNEL_MTD=1
		nand_do_upgrade "$1"
	else
		turris1x_sdcard_do_upgrade "$1"
	fi
}
