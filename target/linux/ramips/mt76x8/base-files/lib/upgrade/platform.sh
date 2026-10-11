#
# Copyright (C) 2010 OpenWrt.org
#

PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='fw_printenv fw_setenv sha256sum dumpimage fwtool jsonfilter cmp'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

# The stock loader uses two firmware slots. The virtual OpenWrt firmware MTD
# concatenates only A, spare and tail. Storage and B are writable for an
# explicitly guarded layout transition, but stock upgrades must preserve them.
lt22m_stock_check_device() {
	local expected number part bytes offset writable flags digest sysfs

	[ "$(board_name)" = 'tuoshi,lt22m' ] || return 74
	[ -r /proc/mtd ] || return 74
	for expected in \
		'0:u-boot:00030000:0:1' '1:u-boot-env:00010000:196608:1' \
		'2:factory:00010000:262144:0' '3:fwconcat0:00770000:327680:1' \
		'4:stock-storage:00040000:8126464:1' '5:fwconcat1:00050000:8388608:1' \
		'6:stock-firmware2:00770000:8716288:1' '7:fwconcat2:00040000:16515072:1'; do
		number="${expected%%:*}"
		part="${expected#*:}"; part="${part%%:*}"
		bytes="${expected#*:*:}"; bytes="${bytes%%:*}"
		offset="${expected#*:*:*:}"; offset="${offset%%:*}"
		writable="${expected##*:}"
		awk -v idx="mtd$number:" -v name="\"$part\"" -v size="$bytes" \
			'$1 == idx && $2 == size && $3 == "00010000" && $4 == name { found=1 }
			 END { exit !found }' /proc/mtd || return 74
		sysfs="/sys/class/mtd/mtd$number"
		[ "$(sed -n '1p' "$sysfs/offset" 2>/dev/null)" = "$offset" ] || return 74
		[ "$(sed -n '1p' "$sysfs/size" 2>/dev/null)" = "$((0x$bytes))" ] || return 74
		[ "$(sed -n '1p' "$sysfs/erasesize" 2>/dev/null)" = 65536 ] || return 74
		flags="$(sed -n '1p' "$sysfs/flags" 2>/dev/null)" || return 74
		case "$flags" in 0x[0-9a-fA-F]*) ;; *) return 74 ;; esac
		if [ "$writable" = 1 ]; then
			[ "$((flags & 0x400))" -ne 0 ] || return 74
		else
			[ "$((flags & 0x400))" -eq 0 ] || return 74
		fi
	done
	awk '$1 == "mtd8:" && $2 == "00800000" && $3 == "00010000" &&
		$4 == "\"firmware\"" { found=1 }
		END { exit !found }' /proc/mtd || return 74
	digest="$(sha256sum /dev/mtd0 2>/dev/null)" || return 74
	[ "${digest%% *}" = \
		'45eb1fbce7dbd5ede73054e5d1870c5e131f1cd8f0b641844889afae2421542b' ] || return 74
	[ "$(fw_printenv -n Image1Stable 2>/dev/null)" = 1 ] || return 74
	[ "$(fw_printenv -n Image1Try 2>/dev/null)" = 0 ] || return 74
}

lt22m_image_hex() {
	hexdump -v -s "$2" -n "$3" -e '4/1 "%02x"' "$1" 2>/dev/null
}

lt22m_image_le32() {
	hexdump -v -s "$2" -n 4 -e '1/4 "%u"' "$1" 2>/dev/null
}

# Verify the OEM kernel CRC and the external SquashFS superblock and bounds.
# The uImage CRC does not cover the complete SquashFS; these are structural
# checks, not a signature or checksum of all rootfs bytes. Repeat in stage 2.
lt22m_stock_check_image() {
	local image="$1" size kernel_hex kernel_bytes marker_offset marker
	local squash_inodes squash_block squash_used squash_high metadata supported board

	lt22m_stock_check_device || return 74
	[ -f "$image" ] && [ -r "$image" ] || return 74
	size="$(wc -c < "$image")" || return 74
	[ "$size" -gt 262144 ] && [ "$size" -le $((7104 * 1024)) ] || return 74
	[ "$(get_magic_long "$image")" = 27151967 ] || return 74
	[ "$(lt22m_image_hex "$image" 28 4)" = 05050203 ] || return 74
	[ "$(lt22m_image_hex "$image" 16 8)" = 8000000080000000 ] || return 74
	kernel_hex="$(lt22m_image_hex "$image" 12 4)" || return 74
	case "$kernel_hex" in ????????) ;; *) return 74 ;; esac
	kernel_bytes=$((0x$kernel_hex + 64))
	[ "$kernel_bytes" -gt 64 ] && [ "$kernel_bytes" -le $((0x770000)) ] || return 74
	[ "$((kernel_bytes + 96))" -le "$size" ] || return 74
	dumpimage -M 0x27151967 -l "$image" >/dev/null 2>&1 || return 74
	[ "$(lt22m_image_hex "$image" "$kernel_bytes" 4)" = 68737173 ] || return 74
	squash_inodes="$(lt22m_image_le32 "$image" "$((kernel_bytes + 4))")" || return 74
	squash_block="$(lt22m_image_le32 "$image" "$((kernel_bytes + 12))")" || return 74
	squash_used="$(lt22m_image_le32 "$image" "$((kernel_bytes + 40))")" || return 74
	squash_high="$(lt22m_image_le32 "$image" "$((kernel_bytes + 44))")" || return 74
	[ "$squash_inodes" -gt 0 ] && [ "$squash_high" -eq 0 ] || return 74
	[ "$squash_block" -ge 4096 ] && [ "$squash_block" -le 1048576 ] &&
		[ "$((squash_block & (squash_block - 1)))" -eq 0 ] || return 74
	[ "$(lt22m_image_hex "$image" "$((kernel_bytes + 28))" 4)" = 04000000 ] || return 74

	# mtd -j substitutes the aligned JFFS2 marker block with saved settings.
	marker_offset=$((size / 65536 * 65536))
	[ "$((marker_offset - kernel_bytes))" -ge 96 ] &&
		[ "$squash_used" -ge 96 ] &&
		[ "$squash_used" -le "$((marker_offset - kernel_bytes))" ] || return 74
	[ "$((size - marker_offset))" -ge 100 ] &&
		[ "$((size - marker_offset))" -le 4096 ] || return 74
	[ "$((marker_offset + 262144))" -le $((0x770000)) ] || return 74
	marker="$(lt22m_image_hex "$image" "$marker_offset" 4)" || return 74
	[ "$marker" = deadc0de ] || return 74
	metadata="$(fwtool -q -i - "$image" 2>/dev/null)" || return 74
	supported="$(printf '%s' "$metadata" | \
		jsonfilter -e '@.supported_devices[0]' 2>/dev/null)" || return 74
	board="$(printf '%s' "$metadata" | \
		jsonfilter -e '@.version.board' 2>/dev/null)" || return 74
	[ "$supported" = 'tuoshi,lt22m' ] && [ "$board" = tuoshi_lt22m ] || return 74
	if [ -n "$UPGRADE_BACKUP" ]; then
		[ -f "$UPGRADE_BACKUP" ] && [ -r "$UPGRADE_BACKUP" ] || return 74
		[ "$(wc -c < "$UPGRADE_BACKUP")" -le 262144 ] || return 74
		tar -tzf "$UPGRADE_BACKUP" >/dev/null 2>&1 || return 74
	fi
}

platform_check_image() {
	case "$(board_name)" in
		tuoshi,lt22m)
			lt22m_stock_check_image "$1" || {
				echo 'LT22M: unsafe stock loader, layout or image' >&2
				return 74
			}
			;;
	esac
	return 0
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	tuoshi,lt22m)
		# do_stage2 reboots after a normal return, so an unsafe image must
		# terminate the RAMFS stage before any firmware write.
		lt22m_stock_check_image "$1" || exit 1
		local protected_before protected_after size marker_offset
		protected_before="$(sha256sum /dev/mtd0 /dev/mtd1 /dev/mtd2 \
			/dev/mtd4 /dev/mtd6)" || exit 1
		default_do_upgrade "$1"
		# mtd -j replaces the aligned marker block with saved settings.
		# Compare the immutable kernel and complete SquashFS before that block.
		size="$(wc -c < "$1")" || exit 1
		marker_offset=$((size / 65536 * 65536))
		[ "$marker_offset" -gt 0 ] &&
			cmp -s -n "$marker_offset" "$1" /dev/mtd3 || {
				echo 'LT22M: slot A readback mismatch; refusing reboot' >&2
				exit 1
			}
		protected_after="$(sha256sum /dev/mtd0 /dev/mtd1 /dev/mtd2 \
			/dev/mtd4 /dev/mtd6)" || exit 1
		[ "$protected_before" = "$protected_after" ] || exit 1
		;;
	alfa-network,awusfree1)
		[ "$(fw_printenv -n dual_image 2>/dev/null)" = "1" ] &&\
		[ -n "$(find_mtd_part backup)" ] && {
			PART_NAME=backup
			if [ "$(fw_printenv -n bootactive 2>/dev/null)" = "1" ]; then
				fw_setenv bootactive 2 || exit 1
			else
				fw_setenv bootactive 1 || exit 1
			fi
		}
		default_do_upgrade "$1"
		;;
	tplink,archer-c20-v5|\
	tplink,archer-c50-v4|\
	tplink,archer-c50-v6)
		MTD_ARGS="-t romfile"
		default_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
