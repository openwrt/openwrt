#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only

IX2106_FACTORY_SIZE=0x00020000
IX2106_FACTORY_DATA_SIZE=0x003e0000
IX2106_FW_SIZE=0x019c0000
IX2106_SYSUPGRADE_METADATA_MAX=0x00010000
IX2106_SYSUPGRADE_MAX_SIZE=$((IX2106_FW_SIZE + IX2106_SYSUPGRADE_METADATA_MAX))
IX2106_BOOT_RESERVED_SIZE=0x00240000
IX2106_ERASE_SIZE=0x00020000
IX2106_FW_OFFSET=0x00400000
IX2106_FW_END=$((IX2106_FW_OFFSET + IX2106_FW_SIZE))
IX2106_ROOTFS_OFFSET=0x00820000
IX2106_ROOTFS_SIZE=$((IX2106_FW_SIZE - IX2106_ROOTFS_OFFSET))

ix2106_read_le32() {
	local file="$1" offset="$2" bytes b1 b2 b3 b4
	bytes=$(hexdump -v -e '4/1 "%u "' -s "$offset" -n 4 "$file") || return 1
	set -- $bytes
	[ "$#" -eq 4 ] || return 1
	b1=$1 b2=$2 b3=$3 b4=$4
	printf '%s\n' "$((b1 + (b2 << 8) + (b3 << 16) + (b4 << 24)))"
}

# OpenWrt's validator treats 74 as a broken image, even with --force.
ix2106_validate_raw_image() {
	local image="$1"
	local size magic used high mark_offset marker

	[ -f "$image" ] || return 1
	size=$(wc -c < "$image") || return 1
	[ "$((size))" -eq "$((IX2106_FW_SIZE))" ] || {
		echo "IX2106: stripped firmware size does not match the firmware partition" >&2
		return 74
	}

	magic=$(hexdump -v -e '4/1 "%02x"' -s "$((IX2106_ROOTFS_OFFSET))" \
		-n 4 "$image") || return 1
	[ "$magic" = "68737173" ] || {
		echo "IX2106: SquashFS magic missing at the rootfs offset" >&2
		return 74
	}

	# SquashFS bytes_used is a little-endian u64 at superblock offset 40.
	used=$(ix2106_read_le32 "$image" "$((IX2106_ROOTFS_OFFSET + 40))") || return 1
	high=$(ix2106_read_le32 "$image" "$((IX2106_ROOTFS_OFFSET + 44))") || return 1
	[ "$high" -eq 0 ] || {
		echo "IX2106: SquashFS length exceeds the supported range" >&2
		return 74
	}
	[ "$used" -gt 0 ] && [ "$used" -le "$((IX2106_ROOTFS_SIZE))" ] || {
		echo "IX2106: invalid SquashFS length" >&2
		return 74
	}

	# The rootfs offset and CFI erase size are both 128 KiB aligned.
	mark_offset=$((IX2106_ROOTFS_OFFSET +
		((used + IX2106_ERASE_SIZE - 1) & ~(IX2106_ERASE_SIZE - 1))))
	[ "$mark_offset" -lt "$((IX2106_FW_SIZE))" ] || return 74
	marker=$(hexdump -v -e '4/1 "%02x"' -s "$mark_offset" -n 4 "$image") || return 1
	[ "$marker" = "deadc0de" ] || {
		echo "IX2106: JFFS2 marker missing at the erase block boundary" >&2
		return 74
	}

	return 0
}

ix2106_extract_raw() {
	local image="$1" raw="$2" size status

	[ -f "$image" ] || {
		echo "IX2106: image is not a regular file" >&2
		return 1
	}
	busybox head -c "$((IX2106_SYSUPGRADE_MAX_SIZE + 1))" "$image" > "$raw" || return 1
	size=$(wc -c < "$raw") || return 1
	if [ "$size" -eq 0 ] || [ "$size" -gt "$IX2106_SYSUPGRADE_MAX_SIZE" ]; then
		echo "IX2106: image size is outside the supported bounds" >&2
		return 74
	fi

	# Strip trailers from the private copy in place to avoid a second RAM copy.
	# fwtool reports invalid trailers and some I/O failures with status 1.
	fwtool -t -i /dev/null "$raw" || {
		status=$?
		[ "$status" -eq 1 ] && return 74
		return 1
	}
	ix2106_validate_raw_image "$raw"
}

ix2106_check_image() (
	umask 077
	local workdir
	workdir=$(busybox mktemp -d /tmp/ix2106.XXXXXX) || return 1
	trap 'rm -rf "$workdir"' EXIT
	ix2106_extract_raw "$1" "$workdir/raw"
)

ix2106_mtd_partition_check() {
	local name="$1" expected_offset="$2" expected_size="$3" expected_flags="$4"
	local device sys size erase offset flags type

	# mtd resolves its target by name in /proc/mtd. Require a unique match.
	device=$(awk -v name="$name" '
		$1 ~ /^mtd[0-9]+:$/ && $4 == "\"" name "\"" {
			dev = $1
			sub(/:$/, "", dev)
			count++
		}
		END { if (count != 1) exit 1; print dev }
	' /proc/mtd) || return 1

	sys="/sys/class/mtd/$device"
	size=$(cat "$sys/size") || return 1
	erase=$(cat "$sys/erasesize") || return 1
	offset=$(cat "$sys/offset") || return 1
	flags=$(cat "$sys/flags") || return 1
	type=$(cat "$sys/type") || return 1

	# Linux reports partition offsets relative to the parent MTD.
	[ "$size" -eq "$((expected_size))" ] &&
		[ "$offset" -eq "$((expected_offset))" ] &&
		[ "$erase" -eq "$((IX2106_ERASE_SIZE))" ] &&
		[ "$type" = "nor" ] &&
		[ "$((flags & 0x400))" -eq "$((expected_flags))" ]
}

ix2106_check_mtd_layout() {
	# MTD_WRITEABLE is 0x400. rootfs_data is a dynamic split, not a write target.
	ix2106_mtd_partition_check firmware "$IX2106_FW_OFFSET" "$IX2106_FW_SIZE" 0x400 &&
		ix2106_mtd_partition_check factory 0 "$IX2106_FACTORY_SIZE" 0 &&
		ix2106_mtd_partition_check factory-data "$IX2106_FACTORY_SIZE" "$IX2106_FACTORY_DATA_SIZE" 0 &&
		ix2106_mtd_partition_check boot-reserved "$IX2106_FW_END" "$IX2106_BOOT_RESERVED_SIZE" 0 &&
		ix2106_mtd_partition_check kernel 0 0x800000 0 &&
		ix2106_mtd_partition_check rootfs-header 0x800000 0x20000 0 &&
		ix2106_mtd_partition_check rootfs "$IX2106_ROOTFS_OFFSET" "$IX2106_ROOTFS_SIZE" 0x400
}

ix2106_do_upgrade() (
	umask 077
	local workdir raw
	workdir=$(busybox mktemp -d /tmp/ix2106.XXXXXX) || return 1
	trap 'rm -rf "$workdir"' EXIT
	raw="$workdir/raw"

	# The caller has already applied the metadata and force policy.
	ix2106_extract_raw "$1" "$raw" || return 1
	ix2106_check_mtd_layout || {
		echo "IX2106: active MTD layout differs from the protected NOR map" >&2
		return 1
	}

	# Skip marker scanning through the fixed kernel and header. mtd -j inserts
	# the config archive at the marker after the new SquashFS and skips the same
	# number of input bytes, so the remainder of the fixed-size image is written.
	if [ -n "${UPGRADE_BACKUP:-}" ]; then
		[ -s "$UPGRADE_BACKUP" ] || {
			echo "IX2106: requested configuration backup is missing" >&2
			return 1
		}
		mtd -s "$IX2106_ROOTFS_OFFSET" -j "$UPGRADE_BACKUP" write "$raw" firmware
	else
		mtd -s "$IX2106_ROOTFS_OFFSET" write "$raw" firmware
	fi
)
