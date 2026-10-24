#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
#
# Upgrade for the UniFi Cloud Gateway Fiber.
#
# OpenWrt is installed next to the stock firmware (dual boot, see
# ucgfiber-dualboot.sh): its kernel is the file uImage.owrt in the ext4
# "kernel" partition, its system the file rootfs.owrt in the ext4 "root"
# partition, its settings live in the "rootfs_data" partition. U-Boot boots
# whichever kernel is named uImage. The sysupgrade tar members are installed
# as those files through ucgfiber-dualboot.sh; stock's files are never
# written. A raw write to either partition would destroy the file system
# and both systems with it.

UCGFIBER_MNT="${UCGFIBER_MNT:-/tmp}"

ucgfiber_gz() { # <image>
	[ "$(dd if="$1" bs=2 count=1 2>/dev/null | hexdump -v -e '2/1 "%02x"')" = 1f8b ] && echo z
}

ucgfiber_member() { # <image> <member>   writes the member to stdout
	tar x$(ucgfiber_gz "$1")f "$1" "$2" -O 2>/dev/null
}

ucgfiber_board_dir() { # <image>
	local dir

	dir="$(tar t$(ucgfiber_gz "$1")f "$1" 2>/dev/null |
		grep -m 1 '^sysupgrade-.*/$')" || return 1
	echo "${dir%/}"
}

ucgfiber_check_image() { # <image>
	local img="$1" dir magic

	dir="$(ucgfiber_board_dir "$img")" || {
		echo "ucgfiber: no sysupgrade-* directory in image" >&2
		return 1
	}
	[ "$dir" = sysupgrade-ubnt_ucgfiber ] || {
		echo "ucgfiber: image is for $dir" >&2
		return 1
	}

	# kernel: flattened device tree (FIT); root: squashfs
	magic="$(ucgfiber_member "$img" "$dir/kernel" |
		hexdump -v -n 4 -e '4/1 "%02x"')"
	[ "$magic" = d00dfeed ] || {
		echo "ucgfiber: kernel is not a FIT image" >&2
		return 1
	}
	magic="$(ucgfiber_member "$img" "$dir/root" |
		hexdump -v -n 4 -e '4/1 "%02x"')"
	[ "$magic" = 68737173 ] || {
		echo "ucgfiber: root is not a squashfs image" >&2
		return 1
	}
	db_find_part kernel >/dev/null && db_find_part root >/dev/null || {
		echo "ucgfiber: kernel/root partitions not found" >&2
		return 1
	}
	db_find_part rootfs_data >/dev/null ||
		echo "ucgfiber: no rootfs_data partition; settings cannot be kept" >&2
	return 0
}

# mount an ext4 partition read-write. The running system's root partition
# stays mounted (lazily detached) during stage 2; mounting it again gives
# the same file system, which works because /init mounts it read-write.
ucgfiber_mount() { # <dev> <dir>
	local n=0

	mkdir -p "$2" || return 1
	while ! mount -t ext4 -o rw,noatime "$1" "$2" 2>/dev/null; do
		n=$((n + 1))
		[ "$n" -lt 5 ] || {
			mount -t ext4 -o rw,noatime "$1" "$2"
			return
		}
		sleep 1
	done
}

ucgfiber_do_upgrade() { # <image>
	local img="$1" dir kdev rdev ddev kmnt rmnt kimg rimg rc=1

	kdev="$(db_find_part kernel)" && rdev="$(db_find_part root)" || {
		echo "ucgfiber: kernel/root partitions not found" >&2
		return 1
	}
	dir="$(ucgfiber_board_dir "$img")" || return 1
	kmnt="$UCGFIBER_MNT/ucgfiber-kernel"
	rmnt="$UCGFIBER_MNT/ucgfiber-root"
	kimg="$UCGFIBER_MNT/ucgfiber-new-kernel"
	rimg="$UCGFIBER_MNT/ucgfiber-new-root"

	# the members as files: their hashes become the stored ones
	if ucgfiber_member "$img" "$dir/kernel" >"$kimg" &&
	   ucgfiber_member "$img" "$dir/root" >"$rimg" &&
	   ucgfiber_mount "$kdev" "$kmnt" && ucgfiber_mount "$rdev" "$rmnt" &&
	   db_install_owrt "$kmnt" "$rmnt" "$kimg" "$rimg"; then
		rc=0
	else
		echo "ucgfiber: upgrade failed" >&2
	fi
	sync
	umount "$kmnt" 2>/dev/null
	umount "$rmnt" 2>/dev/null
	rm -f "$kimg" "$rimg"

	# without a config backup, start with fresh settings (as the generic
	# eMMC upgrade does): fstools formats rootfs_data on the next boot
	if [ "$rc" = 0 ] && [ -z "$UPGRADE_BACKUP" ] && ddev="$(db_find_part rootfs_data)"; then
		dd if=/dev/zero of="$ddev" bs=512 count=8 2>/dev/null
		sync
	fi
	return $rc
}

# the config backup goes to the start of rootfs_data; fstools unpacks it
# and formats the partition on the next boot (as the generic eMMC upgrade)
ucgfiber_copy_config() {
	local ddev

	ddev="$(db_find_part rootfs_data)" || {
		echo "ucgfiber: no rootfs_data partition; settings not kept" >&2
		return 0
	}
	dd if="$UPGRADE_BACKUP" of="$ddev" bs=512 2>/dev/null
	sync
}
