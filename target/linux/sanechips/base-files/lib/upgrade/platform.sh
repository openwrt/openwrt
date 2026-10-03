REQUIRE_IMAGE_METADATA=1

# The ZTE bootloader (cspboot) only boots slot 0 when the kernel starts with
# a 32-byte ZTE slot marker and the "trailer" block holds matching lengths
# and CRC32s for the kernel and the rootfs.  Otherwise it falls back to the
# stock firmware in slot 1.
#
# Trailer layout (little endian):
#   0x34 kernel length (uImage, without the 32-byte marker)
#   0x3c kernel CRC32
#   0x40 rootfs length
#   0x44 rootfs offset (vendor fixed value, left untouched)
#   0x48 rootfs CRC32
#   0xa4 CRC32 of trailer bytes 0x00..0xa3
#   0xf4 header magic
#   0x1f4 image serial
#
# The bootloader takes the last valid header below slot 1 as slot 0's and
# stops after two headers.  It boots slot 0 first only while slot 0's serial
# is at least slot 1's.

H3600_MARKER="33333333cccccccc88888888dddddddd"
H3600_HDR_MAGIC="333333336666666699999999cccccccc"

# h3600_do_upgrade runs in the sysupgrade stage2 ramfs, which only has the
# tools listed in /lib/upgrade/stage2 (no head, no tr): use dd instead.

h3600_trailer_template() {
	# The first 512 bytes of the factory slot-0 trailer (V9.0.24P5_HOP,
	# ZXHN H1600V9), with the length and CRC fields zeroed.  The rest of
	# the block stays erased.
	printf '\000\000\000\000\000\000\000\000\146\002\000\000\010\000\000\000\126\071\056\060\056\062\064\120\065\137\110\117\120\000\000\000'
	printf '\000\000\000\000\000\000\000\000\000\000\000\000\001\000\000\000\000\000\230\001\000\000\000\000\064\002\000\000\000\000\000\000'
	printf '\000\000\000\000\024\002\066\000\000\000\000\000\000\000\160\000\000\000\000\002\000\000\000\000\000\000\000\001\000\000\160\002'
	printf '\000\000\000\002\000\000\000\000\000\000\000\001\132\130\110\116\040\110\061\066\060\060\126\071\000\000\000\000\000\000\000\000'
	printf '\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\001\000\000\000'
	printf '\001\000\000\000\000\000\000\000\062\060\062\061\061\061\061\060\061\062\062\060\065\060\000\000\000\000\000\000\377\377\377\377'
	printf '\377\377\377\377\000\000\000\010\000\000\001\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000'
	printf '\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\063\063\063\063\146\146\146\146\231\231\231\231'
	printf '\314\314\314\314\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000'
	printf '\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000'
	printf '\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000'
	printf '\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000'
	printf '\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000'
	printf '\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000'
	printf '\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000'
	printf '\000\000\160\000\000\000\160\002\000\000\160\002\000\000\160\004\000\000\000\000\002\000\000\000\000\000\000\000\035\314\131\253'
}

h3600_crc32() {
	# CRC32 of stdin, printed as 4 raw little-endian bytes.  The gzip
	# trailer carries the standard CRC32 of the uncompressed data.
	gzip -c | tail -c 8 | dd bs=4 count=1 2>/dev/null
}

h3600_u32le() {
	local v="$1"

	printf "\\$(printf %03o $((v & 0xff)))\\$(printf %03o $(((v >> 8) & 0xff)))"
	printf "\\$(printf %03o $(((v >> 16) & 0xff)))\\$(printf %03o $(((v >> 24) & 0xff)))"
}

h3600_put() {
	# h3600_put <file> <offset>: copy stdin into <file> at <offset>
	dd of="$1" bs=1 seek="$2" conv=notrunc 2>/dev/null
}

h3600_hex() {
	# h3600_hex <file> <offset> <count>: <count> bytes at <offset> as hex
	dd if="$1" bs=1 skip="$2" count="$3" 2>/dev/null | hexdump -v -e '1/1 "%02x"'
}

h3600_header_serial() {
	# h3600_header_serial <file> <offset>: print the serial of the valid
	# image header at <offset>, nothing if there is none
	local file="$1" off="$2"

	[ "$(h3600_hex "$file" $((off + 0xf4)) 16)" = "$H3600_HDR_MAGIC" ] || return
	[ "$(dd if="$file" bs=1 skip="$off" count=$((0xa4)) 2>/dev/null |
		h3600_crc32 | hexdump -v -e '1/1 "%02x"')" = \
		"$(h3600_hex "$file" $((off + 0xa4)) 4)" ] || return
	dd if="$file" bs=1 skip=$((off + 0x1f4)) count=4 2>/dev/null |
		hexdump -v -e '1/4 "%u"'
}

h3600_gap_headers() {
	# Offsets of the valid image headers in trailer_gap.  The stock
	# firmware puts its slot-0 header there when it rewrites slot 0 with
	# a larger JFFS2.
	local dev size ebs off=0

	dev=/dev/mtd$(find_mtd_index trailer_gap)
	[ -c "$dev" ] || return
	size=$((0x$(grep '"trailer_gap"' /proc/mtd | awk '{print $2}')))
	ebs=$((0x$(grep '"trailer_gap"' /proc/mtd | awk '{print $3}')))
	while [ "$off" -lt "$size" ]; do
		[ -n "$(h3600_header_serial "$dev" "$off")" ] && echo "$off"
		off=$((off + ebs))
	done
}

h3600_root_stream() {
	# The squashfs, padded to the next erase block, followed by the JFFS2 end
	# marker.  mtd -j puts the config backup at that marker instead of into
	# the squashfs's last block, and without a backup fstools formats
	# rootfs_data from it on first boot.
	local tar_file="$1" root="$2" flen="$3" ebs pad

	ebs=$((0x$(grep '"rootfs"' /proc/mtd | awk '{print $3}')))
	pad=$(( (ebs - flen % ebs) % ebs ))
	tar xf "$tar_file" "$root" -O
	[ "$pad" -gt 0 ] && dd if=/dev/zero bs="$pad" count=1 2>/dev/null
	printf '\336\255\300\336'
}

h3600_tar_member() {
	local tar_file="$1" name="$2"
	local board_dir

	board_dir=$(tar tf "$tar_file" | grep -m 1 '^sysupgrade-.*/$')
	echo "${board_dir%/}/$name"
}

h3600_check_image() {
	local tar_file="$1"
	local kernel root magic

	kernel=$(h3600_tar_member "$tar_file" kernel)
	root=$(h3600_tar_member "$tar_file" root)

	tar tf "$tar_file" | grep -qx "$kernel" || {
		echo "Invalid image: no kernel"
		return 1
	}
	tar tf "$tar_file" | grep -qx "$root" || {
		echo "Invalid image: no rootfs"
		return 1
	}

	magic=$(tar xf "$tar_file" "$kernel" -O | head -c 16 | hexdump -v -e '16/1 "%02x"')
	[ "$magic" = "$H3600_MARKER" ] || {
		echo "Invalid image: kernel lacks the ZTE slot marker"
		return 1
	}

	return 0
}

h3600_do_upgrade() {
	local tar_file="$1"
	local kernel root klen flen serial s off gap trailer=/tmp/h3600-trailer.bin

	kernel=$(h3600_tar_member "$tar_file" kernel)
	root=$(h3600_tar_member "$tar_file" root)

	# Keep the image serial of the headers on flash.  The stock firmware
	# gives both slots the same serial when it rewrites slot 0; a lower
	# one in our trailer would make the bootloader start slot 1 first.
	serial=$(h3600_header_serial /dev/mtd"$(find_mtd_index trailer)" 0)
	gap=$(h3600_gap_headers)
	for off in $gap; do
		s=$(h3600_header_serial /dev/mtd"$(find_mtd_index trailer_gap)" "$off")
		[ "$s" -gt "${serial:-0}" ] && serial=$s
	done
	[ -n "$serial" ] || serial=2

	# Invalidate slot 0 first.  If the upgrade is interrupted, the
	# bootloader then falls back to slot 1 instead of booting a
	# half-written kernel.
	mtd erase trailer || return 1

	# A stock header after the trailer would replace ours as slot 0.
	for off in $gap; do
		echo "Clearing stale header at trailer_gap+$off..."
		dd if=/dev/zero bs=131072 count=1 2>/dev/null |
			mtd -p "$off" write - trailer_gap || return 1
	done

	# Build the trailer from the factory layout; only the length and CRC
	# fields change.  Do not start from the block on flash: after the stock
	# firmware has rewritten slot 0 it holds stock JFFS2 data there, and a
	# trailer patched into that is not recognised, so the bootloader keeps
	# booting the stock firmware.  The erased block supplies the 0xff fill.
	dd if=/dev/mtd"$(find_mtd_index trailer)" of="$trailer" bs=131072 count=1 2>/dev/null
	[ "$(wc -c < "$trailer")" = 131072 ] || {
		echo "Cannot read trailer"
		return 1
	}
	h3600_trailer_template | h3600_put "$trailer" 0
	h3600_u32le "$serial" | h3600_put "$trailer" $((0x1f4))

	echo "Flashing kernel..."
	tar xf "$tar_file" "$kernel" -O | mtd write - kernel || return 1

	# rootfs_data lives in the same partition, right after the squashfs,
	# and its offset moves with the squashfs size.  Erase the whole
	# partition so no JFFS2 nodes of the old overlay survive: fstools then
	# formats a fresh overlay on first boot, or mounts the one mtd -j
	# appends with the config backup.
	echo "Erasing rootfs..."
	mtd erase rootfs || return 1

	klen=$(($(tar xf "$tar_file" "$kernel" -O | wc -c) - 32))
	flen=$(tar xf "$tar_file" "$root" -O | wc -c)

	echo "Flashing rootfs..."
	if [ -n "$UPGRADE_BACKUP" ]; then
		h3600_root_stream "$tar_file" "$root" "$flen" |
			mtd -j "$UPGRADE_BACKUP" write - rootfs || return 1
	else
		h3600_root_stream "$tar_file" "$root" "$flen" |
			mtd write - rootfs || return 1
	fi

	h3600_u32le "$klen" | h3600_put "$trailer" $((0x34))
	tar xf "$tar_file" "$kernel" -O | tail -c +33 | h3600_crc32 | h3600_put "$trailer" $((0x3c))
	h3600_u32le "$flen" | h3600_put "$trailer" $((0x40))
	tar xf "$tar_file" "$root" -O | h3600_crc32 | h3600_put "$trailer" $((0x48))
	dd if="$trailer" bs=$((0xa4)) count=1 2>/dev/null | h3600_crc32 | h3600_put "$trailer" $((0xa4))

	echo "Sealing trailer..."
	mtd write "$trailer" trailer || return 1

	return 0
}

platform_check_image() {
	local board=$(board_name)

	case "$board" in
	zte,zxhn-h3600)
		h3600_check_image "$1"
		return $?
		;;
	esac

	return 0
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	zte,zxhn-h3600)
		h3600_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
