# SPDX-License-Identifier: GPL-2.0-only
# Copyright (c) OpenWrt.org

RAMFS_COPY_BIN='fitblk fit_check_sign'

. /lib/functions.sh
. /lib/upgrade/xb432v_wrap_tpv3.sh

REQUIRE_IMAGE_METADATA=1

# Slot A only. Stock slot B (kernel_slave/rootfs_slave/firmware_slave) is recovery.
XB432V_KERNEL_MTD=$((10 * 1024 * 1024))
XB432V_ROOTFS_MTD=$((0x3bc0000))
XB432V_FIRMWARE_MTD=$((0x045c0000))
XB432V_LOADADDR=0x80088000
XB432V_TP_V3_TAG=512
XB432V_TP_V3_VERSION=03000003

# stage2 ramdisk may not have od/tr on PATH; prefer busybox applets.
xb432v_bb() {
	if command -v busybox >/dev/null 2>&1; then
		busybox "$@"
	else
		"$@"
	fi
}

xb432v_mtd_expect() {
	local label="$1"
	local size="$2"
	local found size_hex

	found=$(grep "\"$label\"" /proc/mtd | head -n1)
	[ -n "$found" ] || {
		v "missing MTD partition $label"
		return 1
	}
	size_hex=$(echo "$found" | awk '{print $2}')
	[ $((0x$size_hex)) -eq "$size" ] || {
		v "MTD $label size 0x$size_hex != expected $(printf '0x%x' "$size")"
		return 1
	}
	return 0
}

xb432v_verify_mtd_layout() {
	xb432v_mtd_expect boot $((0xc0000)) || return 1
	xb432v_mtd_expect art $((0x380000)) || return 1
	xb432v_mtd_expect misc $((0x400000)) || return 1
	xb432v_mtd_expect kernel $XB432V_KERNEL_MTD || return 1
	xb432v_mtd_expect rootfs $XB432V_ROOTFS_MTD || return 1
	xb432v_mtd_expect firmware $XB432V_FIRMWARE_MTD || return 1
	xb432v_mtd_expect kernel_slave $XB432V_KERNEL_MTD || return 1
	xb432v_mtd_expect rootfs_slave $XB432V_ROOTFS_MTD || return 1
	xb432v_mtd_expect firmware_slave $XB432V_FIRMWARE_MTD || return 1
	xb432v_mtd_expect reserve $((0x40000)) || return 1
	return 0
}

xb432v_image_payload_len() {
	local file="$1"
	local len

	len=$(wc -c < "$file")
	# Allow OpenWrt append-metadata JSON trailer (typically a few KiB).
	[ "$len" -le $((XB432V_FIRMWARE_MTD + 65536)) ] || {
		v "image file $len exceeds firmware slot plus metadata"
		return 1
	}
	echo "$len"
}

xb432v_check_sysupgrade_layout() {
	local file="$1"
	local payload_len rootfs_off magic

	payload_len=$(xb432v_image_payload_len "$file")
	[ "$payload_len" -ge $((XB432V_KERNEL_MTD + 64)) ] || {
		v "image too small for kernel+rootfs layout"
		return 1
	}

	magic=$(dd if="$file" bs=4 count=1 2>/dev/null | hexdump -v -n 4 -e '1/1 "%02x"')
	[ "$magic" = "d00dfeed" ] || {
		v "kernel region is not FIT (d00dfeed)"
		return 1
	}

	rootfs_off=$XB432V_KERNEL_MTD
	magic=$(dd if="$file" bs=1 skip="$rootfs_off" count=4 2>/dev/null | hexdump -v -n 4 -e '1/1 "%02x"')
	[ "$magic" = "68737173" ] || {
		v "rootfs region is not squashfs (hsqs)"
		return 1
	}

	[ "$payload_len" -le $((XB432V_FIRMWARE_MTD + 4096)) ] || {
		v "image payload $payload_len exceeds firmware slot"
		return 1
	}

	return 0
}

xb432v_check_metadata() {
	local file="$1"
	local dev

	# Sysupgrade already enforces REQUIRE_IMAGE_METADATA; this is a fail-closed
	# board guard without invoking nand_do_platform_check / UBI helpers.
	if ! command -v fwtool >/dev/null 2>&1; then
		v "fwtool unavailable; relying on sysupgrade metadata gate"
		return 0
	fi

	dev=$(fwtool -i - "$file" 2>/dev/null | jsonfilter -e '@.supported_devices[0]' 2>/dev/null)
	[ -n "$dev" ] || {
		v "missing supported_devices in image metadata"
		return 1
	}
	echo "$dev" | grep -qx "tplink,xb432v" || {
		v "image metadata device $dev != tplink,xb432v"
		return 1
	}
	return 0
}

xb432v_squashfs_used() {
	local file="$1"
	local used hex off mult hbyte

	hex=$(xb432v_bb dd if="$file" bs=1 skip=$((XB432V_KERNEL_MTD + 40)) count=8 2>/dev/null | \
		xb432v_bb hexdump -v -e '8/1 "%02x"')
	[ -n "$hex" ] || return 1
	used=0
	mult=1
	off=0
	while [ "$off" -lt 16 ]; do
		hbyte=${hex:$off:2}
		used=$((used + 0x$hbyte * mult))
		mult=$((mult * 256))
		off=$((off + 2))
	done
	[ "$used" -gt 0 ] || return 1
	[ "$used" -le "$XB432V_ROOTFS_MTD" ] || {
		v "squashfs claims $used bytes but rootfs MTD is $XB432V_ROOTFS_MTD"
		return 1
	}
	echo "$used"
}

xb432v_erase_overlay_tail() {
	local squashfs_len="$1"
	local tail_len offset

	tail_len=$((XB432V_ROOTFS_MTD - squashfs_len))
	offset="$squashfs_len"
	[ "$tail_len" -gt 0 ] || return 0

	v "Wiping jffs2 overlay tail ($tail_len bytes from rootfs offset $offset)"
	# Erased NAND flash is 0xFF; uci-defaults re-run after clean upgrade (-n).
	xb432v_bb dd if=/dev/zero bs=65536 count=$(( (tail_len + 65535) / 65536 )) 2>/dev/null | \
		xb432v_bb tr '\000' '\377' | \
		xb432v_bb head -c "$tail_len" 2>/dev/null | \
		mtd write -p "$offset" - rootfs || return 1
	return 0
}

xb432v_do_upgrade() {
	local file="$1"
	local rootfs_len

	rootfs_len=$(xb432v_squashfs_used "$file") || return 1

	v "Writing kernel (TP v3 tag + FIT) to mtd:kernel"
	xb432v_build_tpv3_kernel "$file" | mtd write - kernel || return 1

	v "Writing rootfs (${rootfs_len} bytes squashfs) to mtd:rootfs"
	# Rootfs may exceed one kernel-sized block; read exactly squashfs_used from offset.
	xb432v_bb dd if="$file" bs=1 skip="$XB432V_KERNEL_MTD" count="$rootfs_len" 2>/dev/null | \
		mtd write - rootfs || return 1

	if [ "${SAVE_CONFIG:-1}" = "0" ]; then
		xb432v_erase_overlay_tail "$rootfs_len" || return 1
	else
		v "Preserving jffs2 overlay tail after squashfs (config-preserving upgrade)"
	fi

	xb432v_bb sync
	return 0
}

xb432v_check_tpv3() {
	local file="$1"
	local tagver

	tagver=$(dd if="$file" bs=4 count=1 2>/dev/null | hexdump -v -n 4 -e '1/1 "%02x"')
	[ "$tagver" = "$XB432V_TP_V3_VERSION" ] || {
		v "missing TP-Link v3 tag (03000003)"
		return 1
	}
	return 0
}

nokia_initial_setup()
{
	[ "$(rootfs_type)" = "tmpfs" ] || return 0

	fw_setenv bootcmd "flash read 0xc0000 0x800000 0x85000000; bootm 0x85000000"
}

platform_check_image() {
	local board=$(board_name)

	[ "$#" -gt 1 ] && return 1

	case "$board" in
	tplink,xb432v)
		xb432v_verify_mtd_layout || return 1
		xb432v_check_sysupgrade_layout "$1" || return 1
		xb432v_check_metadata "$1" || return 1
		return 0
		;;
	nokia,xg-040g-md)
		nand_do_platform_check "$board" "$1"
		return $?
		;;
	nokia,xg-040g-md-ubi)
		fit_check_image "$1"
		return $?
		;;
	esac

	return 0
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	tplink,xb432v)
		# Raw MTD slot A only: kernel + rootfs. Never slave/recovery partitions.
		xb432v_do_upgrade "$1"
		;;
	gemtek,w1700k-ubi|\
	nokia,xg-040g-md-ubi)
		fit_do_upgrade "$1"
		;;
	*)
		nand_do_upgrade "$1"
		;;
	esac
}

platform_pre_upgrade() {
	local board=$(board_name)

	case "$board" in
	nokia,xg-040g-md)
		nokia_initial_setup
		;;
	tplink,xb432v)
		# Slot A kernel+rootfs only. JFFS2 overlay lives in the rootfs MTD tail
		# after squashfs_used bytes. Default sysupgrade keeps that tail; -n / -F
		# with SAVE_CONFIG=0 erases it so uci-defaults run again on first boot.
		# Never touch firmware_slave (recovery B).
		;;
	*)
		;;
	esac
}
