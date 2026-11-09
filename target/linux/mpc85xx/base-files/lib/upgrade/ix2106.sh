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
IX2106_STATE_BASE=${IX2106_STATE_BASE:-/tmp/ix2106-sysupgrade.state}

ix2106_hash_file() {
	local result

	result=$(busybox sha256sum "$1") || return 1
	printf '%s\n' "${result%% *}"
}

ix2106_state_path() {
	printf '%s.%s\n' "$IX2106_STATE_BASE" "$1"
}

ix2106_snapshot_image() {
	local image="$1" workdir="$2" image_size snapshot_size
	local snapshot="$workdir/input"

	[ -f "$image" ] || return 1
	image_size=$(wc -c < "$image") || return 1
	[ "$((image_size))" -gt 0 ] &&
		[ "$((image_size))" -le "$((IX2106_SYSUPGRADE_MAX_SIZE))" ] || return 1
	head -c "$((IX2106_SYSUPGRADE_MAX_SIZE + 1))" "$image" > "$snapshot" || return 1
	snapshot_size=$(wc -c < "$snapshot") || return 1
	[ "$((snapshot_size))" -gt 0 ] &&
		[ "$((snapshot_size))" -le "$((IX2106_SYSUPGRADE_MAX_SIZE))" ] || return 1
	ix2106_hash_file "$snapshot"
}

ix2106_read_le32() {
	local file="$1" offset="$2" bytes b1 b2 b3 b4
	bytes=$(hexdump -v -e '4/1 "%u "' -s "$offset" -n 4 "$file") || return 1
	set -- $bytes
	[ "$#" -eq 4 ] || return 1
	b1=$1 b2=$2 b3=$3 b4=$4
	printf '%s\n' "$((b1 + (b2 << 8) + (b3 << 16) + (b4 << 24)))"
}

ix2106_read_be32() {
	local file="$1" offset="$2" bytes b1 b2 b3 b4
	bytes=$(hexdump -v -e '4/1 "%u "' -s "$offset" -n 4 "$file") || return 1
	set -- $bytes
	[ "$#" -eq 4 ] || return 1
	b1=$1 b2=$2 b3=$3 b4=$4
	printf '%s\n' "$(((b1 << 24) + (b2 << 16) + (b3 << 8) + b4))"
}

ix2106_validate_raw_image() {
	local image="$1"
	local size magic used high mark_offset marker

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
	used=$(ix2106_read_le32 "$image" "$((IX2106_ROOTFS_OFFSET + 40))") || return 1
	high=$(ix2106_read_le32 "$image" "$((IX2106_ROOTFS_OFFSET + 44))") || return 1
	[ "$high" -eq 0 ] || {
		echo "IX2106: SquashFS length exceeds the supported range" >&2
		return 1
	}
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
	local image="$1" snapshot_hash image_hash state_tmp workdir raw_tmp

	workdir=$(umask 077; mktemp -d /tmp/ix2106.XXXXXX) || return 74
	snapshot_hash=$(ix2106_snapshot_image "$image" "$workdir") || {
		rm -rf "$workdir"
		return 74
	}
	raw_tmp="$workdir/raw"
	if ! (umask 077; fwtool -q -T -i /dev/null "$workdir/input" > "$raw_tmp") ||
	   ! ix2106_validate_raw_image "$raw_tmp"; then
		rm -rf "$workdir"
		return 74
	fi
	image_hash=$(ix2106_hash_file "$image") || {
		rm -rf "$workdir"
		return 74
	}
	if [ "$image_hash" != "$snapshot_hash" ]; then
		rm -rf "$workdir"
		echo "IX2106: image changed during validation" >&2
		return 74
	fi
	state_tmp="$workdir/state"
	if ! (umask 077; printf '%s\n' "$snapshot_hash" > "$state_tmp") ||
	   ! mv -fT "$state_tmp" "$(ix2106_state_path "$snapshot_hash")"; then
		rm -rf "$workdir"
		echo "IX2106: cannot record the checked input" >&2
		return 74
	fi
	rm -rf "$workdir"
	return 0
}

ix2106_prepare_upgrade() {
	local image="$1" source_hash checked_hash workdir receipt prepared_hash
	unset IX2106_PREPARED_IMAGE IX2106_PREPARED_IMAGE_HASH
	workdir=$(umask 077; mktemp -d /tmp/ix2106.XXXXXX) || return 1
	source_hash=$(ix2106_snapshot_image "$image" "$workdir") || {
		rm -rf "$workdir"
		echo "IX2106: checked image changed before upgrade" >&2
		return 1
	}
	receipt=$(ix2106_state_path "$source_hash")
	if ! IFS= read -r checked_hash < "$receipt" ||
	   [ "$source_hash" != "$checked_hash" ] ||
	   ! (umask 077; fwtool -q -T -i /dev/null "$workdir/input" > "$workdir/raw") ||
	   ! ix2106_validate_raw_image "$workdir/raw"; then
		rm -rf "$workdir"
		echo "IX2106: checked image receipt or firmware payload is invalid" >&2
		return 1
	fi
	prepared_hash=$(ix2106_hash_file "$workdir/raw") || {
		rm -rf "$workdir"
		return 1
	}
	rm -f "$workdir/input"
	IX2106_PREPARED_IMAGE="$workdir/raw"
	IX2106_PREPARED_IMAGE_HASH=$prepared_hash
	export IX2106_PREPARED_IMAGE IX2106_PREPARED_IMAGE_HASH
	return 0
}

ix2106_mtd_partition_check() {
	local name="$1"
	local expected_offset="$2"
	local expected_size="$3"
	local expected_writeable="$4"
	local line device index proc_size proc_erase sys_size sys_erase
	local flags type reg_offset reg_size sys_offset

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
	sys_offset=$(cat "$line/offset") || return 1
	flags=$(cat "$line/flags") || return 1
	type=$(cat "$line/type") || return 1
	if [ -n "$expected_size" ]; then
		[ "$((sys_size))" -eq "$((expected_size))" ] || return 1
		[ "$proc_size" -eq "$((expected_size))" ] || return 1
	fi
	if [ -n "$expected_offset" ]; then
		# MTD part offsets and DT reg values are relative to the same parent.
		[ "$((sys_offset))" -eq "$((expected_offset))" ] || return 1
	fi
	[ "$((sys_erase))" -eq "$((IX2106_ERASE_SIZE))" ] || return 1
	[ "$proc_erase" -eq "$((IX2106_ERASE_SIZE))" ] || return 1
	[ "$type" = "nor" ] || return 1
	[ "$((flags & 0x400))" -eq "$expected_writeable" ] || return 1

	if [ -n "$expected_offset" ]; then
		reg_offset=$(ix2106_read_be32 "$line/of_node/reg" 0) || return 1
		reg_size=$(ix2106_read_be32 "$line/of_node/reg" 4) || return 1
		[ "$reg_offset" -eq "$((expected_offset))" ] || return 1
		[ "$reg_size" -eq "$((expected_size))" ] || return 1
	fi

	return 0
}

ix2106_check_mtd_layout() {
	# Validate the fixed NOR layout against the active MTD and OF partitions.
	ix2106_mtd_partition_check firmware "$IX2106_FW_OFFSET" "$IX2106_FW_SIZE" 1024 || return 1
	ix2106_mtd_partition_check factory 0 "$IX2106_FACTORY_SIZE" 0 || return 1
	ix2106_mtd_partition_check factory-data "$IX2106_FACTORY_SIZE" "$IX2106_FACTORY_DATA_SIZE" 0 || return 1
	ix2106_mtd_partition_check boot-reserved "$IX2106_FW_END" "$IX2106_BOOT_RESERVED_SIZE" 0 || return 1
	ix2106_mtd_partition_check kernel 0x00000000 0x00800000 0 || return 1
	ix2106_mtd_partition_check rootfs-header 0x00800000 0x00020000 0 || return 1
	ix2106_mtd_partition_check rootfs "$IX2106_ROOTFS_OFFSET" "$IX2106_ROOTFS_SIZE" 1024 || return 1
	ix2106_check_rootfs_data || return 1
	return 0
}

ix2106_check_rootfs_data() {
	local proc_mtd="${1:-/proc/mtd}"
	local proc_mounts="${2:-/proc/mounts}"

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
	local actual_hash
	[ -n "${IX2106_PREPARED_IMAGE:-}" ] &&
		[ -n "${IX2106_PREPARED_IMAGE_HASH:-}" ] || {
		echo "IX2106: checked image identity is missing in stage2" >&2
		exit 1
	}
	case "$IX2106_PREPARED_IMAGE" in
		/tmp/ix2106.*/raw) ;;
		*)
			echo "IX2106: prepared firmware path is invalid" >&2
			exit 1
			;;
	esac
	[ -f "$IX2106_PREPARED_IMAGE" ] && [ ! -L "$IX2106_PREPARED_IMAGE" ] || {
		echo "IX2106: prepared firmware payload is unavailable" >&2
		exit 1
	}
	actual_hash=$(ix2106_hash_file "$IX2106_PREPARED_IMAGE") || {
		echo "IX2106: cannot verify prepared firmware payload" >&2
		exit 1
	}
	[ "$actual_hash" = "$IX2106_PREPARED_IMAGE_HASH" ] || {
		echo "IX2106: prepared firmware payload changed" >&2
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
			write "$IX2106_PREPARED_IMAGE" firmware || exit 1
	else
		mtd -s "$IX2106_ROOTFS_OFFSET" write "$IX2106_PREPARED_IMAGE" firmware || exit 1
	fi
	rm -f "$IX2106_PREPARED_IMAGE"
	rmdir "${IX2106_PREPARED_IMAGE%/*}" 2>/dev/null || true
}
