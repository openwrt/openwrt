#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only

IX2106_FW_SIZE=0x019c0000
IX2106_MAX_IMAGE_SIZE=0x019d0000
IX2106_ROOTFS_OFFSET=0x00820000
IX2106_ROOTFS_SIZE=0x011a0000
IX2106_ERASE_SIZE=0x00020000
IX2106_IMAGE=/tmp/ix2106-sysupgrade.raw
IX2106_STATE=/tmp/ix2106-sysupgrade.state
IX2106_READY=/tmp/ix2106-sysupgrade.ready
IX2106_IMAGE_BASE=$IX2106_IMAGE
IX2106_STATE_BASE=$IX2106_STATE
IX2106_READY_BASE=$IX2106_READY

ix2106_hash_file() {
	local result

	result=$(busybox sha256sum "$1") || return 1
	printf '%s\n' "${result%% *}"
}

ix2106_set_paths() {
	IX2106_IMAGE="$IX2106_IMAGE_BASE.$1"
	IX2106_STATE="$IX2106_STATE_BASE.$1"
	IX2106_READY="$IX2106_READY_BASE.$1"
}

ix2106_write_state() {
	local source_hash="$1" raw_hash
	[ -n "$source_hash" ] || return 1
	raw_hash=$(ix2106_hash_file "$2") || return 1
	printf '%s\n%s\n' "$source_hash" "$raw_hash"
}

ix2106_verify_state() {
	local source="$1"
	local state="$2"
	local source_hash raw_hash expected_source expected_raw

	[ -s "$state" ] || return 1
	{
		read -r expected_source
		read -r expected_raw
	} < "$state"
	[ -n "$expected_source" ] && [ -n "$expected_raw" ] || return 1

	source_hash=$(ix2106_hash_file "$source") || return 1
	raw_hash=$(ix2106_hash_file "$IX2106_IMAGE") || return 1
	[ "$source_hash" = "$expected_source" ] && [ "$raw_hash" = "$expected_raw" ] || return 1
	ix2106_validate_raw_image "$IX2106_IMAGE"
}

ix2106_validate_raw_image() {
	local image="$1"
	local size magic bytes used mark_offset marker
	local b1 b2 b3 b4 b5 b6 b7 b8

	[ -f "$image" ] || return 1
	size=$(wc -c < "$image") || return 1
	[ "$((size))" -eq "$((IX2106_FW_SIZE))" ] || {
		echo "IX2106: stripped firmware size must be 25.75 MiB" >&2
		return 1
	}

	magic=$(hexdump -v -e '4/1 "%02x"' -s "$((IX2106_ROOTFS_OFFSET))" \
		-n 4 "$image") || return 1
	[ "$magic" = "68737173" ] || {
		echo "IX2106: SquashFS magic missing at firmware offset 0x820000" >&2
		return 1
	}

	# SquashFS bytes_used is a little-endian u64 at superblock offset 40.
	bytes=$(hexdump -v -e '8/1 "%u "' \
		-s "$((IX2106_ROOTFS_OFFSET + 40))" -n 8 "$image") || return 1
	set -- $bytes
	[ "$#" -eq 8 ] || return 1
	b1=$1; b2=$2; b3=$3; b4=$4
	b5=$5; b6=$6; b7=$7; b8=$8
	[ "$b5$b6$b7$b8" = "0000" ] || {
		echo "IX2106: SquashFS length exceeds the supported range" >&2
		return 1
	}
	used=$((b1 + (b2 << 8) + (b3 << 16) + (b4 << 24)))
	[ "$used" -gt 0 ] && [ "$used" -le "$((IX2106_ROOTFS_SIZE))" ] || {
		echo "IX2106: invalid SquashFS length" >&2
		return 1
	}

	# The rootfs offset and CFI erase size are both 128 KiB aligned.
	mark_offset=$((IX2106_ROOTFS_OFFSET +
		((used + IX2106_ERASE_SIZE - 1) & ~(IX2106_ERASE_SIZE - 1))))
	[ "$mark_offset" -lt "$((IX2106_FW_SIZE))" ] || return 1
	marker=$(hexdump -v -e '4/1 "%02x"' -s "$mark_offset" -n 4 "$image") || return 1
	[ "$marker" = "deadc0de" ] || {
		echo "IX2106: 128 KiB JFFS2 boundary marker missing" >&2
		return 1
	}

	return 0
}

ix2106_check_image() {
	local image="$1"
	local image_size source_hash snapshot snapshot_hash raw_tmp state_tmp workdir

	[ -f "$image" ] || return 74
	image_size=$(wc -c < "$image") || return 74
	[ "$((image_size))" -gt 0 ] && [ "$((image_size))" -le "$((IX2106_MAX_IMAGE_SIZE))" ] || {
		echo "IX2106: sysupgrade image exceeds the supported firmware and metadata size" >&2
		return 74
	}
	# Use a per-source namespace so parallel checks cannot mix payloads/state.
	# Extract from a private snapshot and make sure it matches the caller's file.
	source_hash=$(ix2106_hash_file "$image") || return 74
	ix2106_set_paths "$source_hash"
	workdir=$(umask 077; mktemp -d /tmp/ix2106.XXXXXX) || return 74
	snapshot="$workdir/input"
	raw_tmp="$workdir/raw"
	state_tmp="$workdir/state"
	if ! cp "$image" "$snapshot"; then
		rm -rf "$workdir"
		return 74
	fi
	snapshot_hash=$(ix2106_hash_file "$snapshot") || {
		rm -rf "$workdir"
		return 74
	}
	[ "$snapshot_hash" = "$source_hash" ] || {
		rm -rf "$workdir"
		echo "IX2106: image changed while creating validation snapshot" >&2
		return 74
	}
	# fwtool skips signature trailers when no signature output is requested.
	if ! (umask 077; fwtool -q -T -i /dev/null "$snapshot" > "$raw_tmp"); then
		rm -rf "$workdir"
		echo "IX2106: cannot strip required fwtool metadata" >&2
		return 74
	fi

	if ! ix2106_validate_raw_image "$raw_tmp"; then
		rm -rf "$workdir"
		return 74
	fi
	if [ "$(ix2106_hash_file "$image")" != "$source_hash" ] ||
	   [ "$(ix2106_hash_file "$snapshot")" != "$source_hash" ]; then
		rm -rf "$workdir"
		echo "IX2106: image changed during validation" >&2
		return 74
	fi
	if ! (umask 077; ix2106_write_state "$source_hash" "$raw_tmp" > "$state_tmp"); then
		rm -rf "$workdir"
		echo "IX2106: cannot bind the checked image to its raw payload" >&2
		return 74
	fi
	mv -fT "$state_tmp" "$IX2106_STATE" || {
		rm -rf "$workdir"
		return 74
	}
	rm -rf "$workdir"

	return 0
}

ix2106_prepare_upgrade() {
	local source_hash expected_source expected_raw raw_tmp state_tmp workdir
	unset IX2106_READY_HASH
	source_hash=$(ix2106_hash_file "$1") || return 1
	ix2106_set_paths "$source_hash"
	[ -s "$IX2106_STATE" ] || {
		echo "IX2106: checked image changed before upgrade" >&2
		return 1
	}
	{
		read -r expected_source
		read -r expected_raw
	} < "$IX2106_STATE"
	[ "$source_hash" = "$expected_source" ] && [ -n "$expected_raw" ] || {
		echo "IX2106: checked image changed before upgrade" >&2
		return 1
	}
	workdir=$(umask 077; mktemp -d /tmp/ix2106.XXXXXX) || return 1
	raw_tmp="$workdir/raw"
	state_tmp="$workdir/ready"
	# Re-extract only for an actual upgrade, not for check-only runs.
	if ! (umask 077; fwtool -q -T -i /dev/null "$1" > "$raw_tmp") ||
	   ! ix2106_validate_raw_image "$raw_tmp" ||
	   [ "$(ix2106_hash_file "$1")" != "$source_hash" ] ||
	   [ "$(ix2106_hash_file "$raw_tmp")" != "$expected_raw" ]; then
		rm -rf "$workdir"
		echo "IX2106: checked image changed before upgrade" >&2
		return 1
	fi
	if ! (umask 077; cp "$IX2106_STATE" "$state_tmp") ||
	   ! mv -fT "$raw_tmp" "$IX2106_IMAGE" ||
	   ! mv -fT "$state_tmp" "$IX2106_READY"; then
		rm -rf "$workdir"
		return 1
	fi
	rm -rf "$workdir"
	IX2106_READY_HASH=$source_hash
	export IX2106_READY_HASH
	return 0
}

ix2106_mtd_partition_check() {
	local name="$1"
	local expected_offset="$2"
	local expected_size="$3"
	local expected_writeable="$4"
	local line device index proc_size proc_erase sys_size sys_erase
	local flags type reg_bytes reg_offset reg_size
	local b1 b2 b3 b4 b5 b6 b7 b8

	line=$(grep "\"$name\"" /proc/mtd) || return 1
	set -- $line
	[ "$#" -eq 4 ] || return 1
	device=${1%:}
	index=${device#mtd}
	proc_size=$((0x$2))
	proc_erase=$((0x$3))
	[ -n "$index" ] || return 1

	line="/sys/class/mtd/mtd$index"
	sys_size=$(cat "$line/size") || return 1
	sys_erase=$(cat "$line/erasesize") || return 1
	flags=$(cat "$line/flags") || return 1
	type=$(cat "$line/type") || return 1
	if [ -n "$expected_size" ]; then
		[ "$((sys_size))" -eq "$((expected_size))" ] || return 1
		[ "$proc_size" -eq "$((expected_size))" ] || return 1
	fi
	[ "$((sys_erase))" -eq "$((IX2106_ERASE_SIZE))" ] || return 1
	[ "$proc_erase" -eq "$((IX2106_ERASE_SIZE))" ] || return 1
	[ "$type" = "nor" ] || return 1
	[ "$((flags & 0x400))" -eq "$expected_writeable" ] || return 1

	if [ -n "$expected_offset" ]; then
		reg_bytes=$(hexdump -v -e '8/1 "%u "' -n 8 "$line/of_node/reg") || return 1
		set -- $reg_bytes
		[ "$#" -eq 8 ] || return 1
		b1=$1; b2=$2; b3=$3; b4=$4
		b5=$5; b6=$6; b7=$7; b8=$8
		reg_offset=$(((b1 << 24) + (b2 << 16) + (b3 << 8) + b4))
		reg_size=$(((b5 << 24) + (b6 << 16) + (b7 << 8) + b8))
		[ "$reg_offset" -eq "$((expected_offset))" ] || return 1
		[ "$reg_size" -eq "$((expected_size))" ] || return 1
	fi

	return 0
}

ix2106_check_mtd_layout() {
	local proc_mtd="${1:-/proc/mtd}"
	local proc_mounts="${2:-/proc/mounts}"

	# firmware parent is the only writable updater target. Validate physical
	# offsets, sizes, NOR type and erase size against the active OF partitions.
	ix2106_mtd_partition_check firmware 0x00400000 0x019c0000 1024 || return 1
	ix2106_mtd_partition_check factory 0x00000000 0x00020000 0 || return 1
	ix2106_mtd_partition_check factory-data 0x00020000 0x003e0000 0 || return 1
	ix2106_mtd_partition_check boot-reserved 0x01dc0000 0x00240000 0 || return 1
	ix2106_mtd_partition_check kernel 0x00000000 0x00800000 0 || return 1
	ix2106_mtd_partition_check rootfs-header 0x00800000 0x00020000 0 || return 1
	ix2106_mtd_partition_check rootfs 0x00820000 0x011a0000 1024 || return 1
	ix2106_check_rootfs_data "$proc_mtd" "$proc_mounts" || return 1
	return 0
}

ix2106_check_rootfs_data() {
	local proc_mtd="$1"
	local proc_mounts="$2"

	if grep -q '"rootfs_data"' "$proc_mtd"; then
		ix2106_mtd_partition_check rootfs_data "" "" 1024 || return 1
	else
		# A TFTP recovery initramfs has not split the flash rootfs into
		# rootfs_data yet. Permit that one missing partition only while the
		# active root is RAM-backed; malformed or absent NOR is still rejected.
		awk '$2 == "/" && $3 ~ /^(tmpfs|ramfs|rootfs)$/ { found = 1 }
			END { exit !found }' "$proc_mounts" || {
			echo "IX2106: rootfs_data is missing outside a RAM recovery environment" >&2
			return 1
		}
	fi
	return 0
}

ix2106_do_upgrade() {
	local source_hash
	[ -n "${IX2106_READY_HASH:-}" ] || {
		echo "IX2106: checked image identity is missing in stage2" >&2
		exit 1
	}
	source_hash=$(ix2106_hash_file "$1") || {
		echo "IX2106: cannot identify upgrade image" >&2
		exit 1
	}
	[ "$source_hash" = "$IX2106_READY_HASH" ] || {
		echo "IX2106: upgrade image changed after preparation" >&2
		exit 1
	}
	ix2106_set_paths "$source_hash"
	# sysupgrade stage2 has pivoted to RAMFS; verify the original/raw image
	# hashes again before touching flash.
	ix2106_verify_state "$1" "$IX2106_READY" || {
		echo "IX2106: upgrade image binding is missing or changed" >&2
		exit 1
	}
	ix2106_check_mtd_layout || {
		echo "IX2106: active MTD layout differs from the protected NOR map" >&2
		exit 1
	}

	if [ -n "$UPGRADE_BACKUP" ]; then
		[ -s "$UPGRADE_BACKUP" ] || {
			echo "IX2106: requested configuration backup is missing" >&2
			exit 1
		}
	fi

	# Skip marker scanning through the fixed kernel and header. mtd -j inserts
	# the config archive at the marker after the new SquashFS and skips the same
	# number of input bytes, so the remainder of the fixed-size image is written.
	if [ -n "$UPGRADE_BACKUP" ]; then
		mtd -s "$IX2106_ROOTFS_OFFSET" -j "$UPGRADE_BACKUP" \
			write "$IX2106_IMAGE" firmware || exit 1
	else
		mtd -s "$IX2106_ROOTFS_OFFSET" write "$IX2106_IMAGE" firmware || exit 1
	fi
}
