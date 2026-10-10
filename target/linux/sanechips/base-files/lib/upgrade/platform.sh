REQUIRE_IMAGE_METADATA=1

# The ZTE bootloader (cspboot) boots slot 0 (0x700000-0x2700000 on the chip)
# when its kernel starts with a 32-byte ZTE slot marker and a slot header
# below slot 1 holds the kernel's length and CRC32.  With a rootfs length of
# 0 in the header it checks nothing else.  Otherwise it falls back to the
# stock firmware in slot 1.
#
# Slot 0 holds the kernel and the trailer. UBI is above slot 1: cspboot
# mistakes its erase-counter headers for boot headers and stops scanning
# after two headers, so it must find both genuine slot headers first.
#
# Header layout (little endian):
#   0x34 kernel length (uImage, without the 32-byte marker)
#   0x3c kernel CRC32
#   0x40 rootfs length (0: no rootfs to check)
#   0xa4 CRC32 of header bytes 0x00..0xa3
#   0xf4 header magic
#   0x1f4 image serial
#
# The bootloader boots slot 0 first only while its serial is at least
# slot 1's.

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

h3600_partition_serial() {
	# The serial of a valid header in the named physical partition
	local dev size ebs off=0 s

	dev=/dev/mtd$(find_mtd_index "$1")
	[ -c "$dev" ] || return
	size=$(cat /sys/class/mtd/"${dev##*/}"/size)
	ebs=$(cat /sys/class/mtd/"${dev##*/}"/erasesize)
	while [ "$off" -lt "$size" ]; do
		s=$(h3600_header_serial "$dev" "$off")
		[ -n "$s" ] && echo "$s" && return
		off=$((off + ebs))
	done
}

h3600_check_image() {
	local tar_file="$1" board_dir magic ubi klen

	[ "$#" -eq 1 ] || return 1
	[ "$(get_magic_long "$tar_file")" = "73797375" ] || {
		echo "Invalid image: expected an H3600 sysupgrade archive"
		return 1
	}

	ubi=$(find_mtd_index ubi)
	[ -n "$ubi" ] &&
		[ "$(cat /sys/class/mtd/mtd"$ubi"/offset)" = "$((0x4700000))" ] &&
		[ "$(cat /sys/class/mtd/mtd"$ubi"/size)" = "$((0x3900000))" ] || {
		echo "Boot the matching initramfs over TFTP before migrating to UBI"
		return 1
	}

	nand_do_platform_check "zte_zxhn-h3600" "$tar_file" || return 1

	board_dir=$(tar tf "$tar_file" | grep -m 1 '^sysupgrade-.*/$')
	case "$board_dir" in
	sysupgrade-zte_zxhn-h3600/|sysupgrade-zte,zxhn-h3600/) ;;
	*)
		echo "Invalid image: expected the H3600 archive directory"
		return 1
		;;
	esac
	magic=$(tar xf "$tar_file" "${board_dir%/}/kernel" -O | dd bs=16 count=1 2>/dev/null | hexdump -v -e '16/1 "%02x"')
	[ "$magic" = "$H3600_MARKER" ] || {
		echo "Invalid image: kernel lacks the ZTE slot marker"
		return 1
	}
	magic=$(tar xf "$tar_file" "${board_dir%/}/kernel" -O | \
		dd bs=1 skip=32 count=4 2>/dev/null | hexdump -v -e '4/1 "%02x"')
	[ "$magic" = "27051956" ] || {
		echo "Invalid image: expected a uImage after the ZTE slot marker"
		return 1
	}
	klen=$(tar xf "$tar_file" "${board_dir%/}/kernel" -O | wc -c)
	[ "$klen" -gt 96 ] && [ "$klen" -le "$((0x400000))" ] || {
		echo "Invalid image: kernel does not fit its 4 MiB partition"
		return 1
	}
	[ "$(identify_tar "$tar_file" cat "${board_dir%/}/root")" = squashfs ] || {
		echo "Invalid image: expected squashfs for the UBI rootfs volume"
		return 1
	}

	return 0
}

h3600_do_upgrade() {
	local tar_file="$1"
	local board_dir kernel serial stock_serial klen trailer=/tmp/h3600-trailer.bin

	board_dir=$(tar tf "$tar_file" | grep -m 1 '^sysupgrade-.*/$')
	kernel="${board_dir%/}/kernel"

	# Keep the layout guard even when sysupgrade was invoked with -F.
	h3600_check_image "$tar_file" || nand_do_upgrade_failed

	# Both genuine headers must precede UBI in the boot-loader scan.
	# Keep slot 0's serial at least as high as stock's, also on migration.
	stock_serial=$(h3600_partition_serial stock)
	[ -n "$stock_serial" ] || {
		echo "No valid stock-slot header found; refusing to write flash"
		nand_do_upgrade_failed
	}
	serial=$(h3600_partition_serial trailer)
	[ -n "$serial" ] || serial=2
	[ "$serial" -ge "$stock_serial" ] || serial="$stock_serial"

	# Migration runs from the new initramfs. Clear the old rootfs and
	# any misplaced UBI headers before sealing the new slot header.
	# Normal upgrades leave the already-erased gap alone.
	if grep -q '^tmpfs / tmpfs ' /proc/mounts; then
		mtd erase slot0_gap || nand_do_upgrade_failed
	fi

	# Invalidate slot 0 first.  If the upgrade is interrupted, the
	# bootloader then falls back to slot 1 instead of booting a
	# half-written kernel.
	mtd erase trailer || nand_do_upgrade_failed

	# The kernel into its partition, the rootfs into its UBI volume
	nand_do_flash_file "$tar_file" || nand_do_upgrade_failed

	# The erased block supplies the 0xff fill of the header block.
	dd if=/dev/mtd"$(find_mtd_index trailer)" of="$trailer" bs=131072 count=1 2>/dev/null
	[ "$(wc -c < "$trailer")" = 131072 ] || nand_do_upgrade_failed
	h3600_trailer_template | h3600_put "$trailer" 0
	h3600_u32le "$serial" | h3600_put "$trailer" $((0x1f4))
	klen=$(($(tar xf "$tar_file" "$kernel" -O | wc -c) - 32))
	h3600_u32le "$klen" | h3600_put "$trailer" $((0x34))
	tar xf "$tar_file" "$kernel" -O | tail -c +33 | h3600_crc32 | h3600_put "$trailer" $((0x3c))
	h3600_u32le 0 | h3600_put "$trailer" $((0x40))
	dd if="$trailer" bs=$((0xa4)) count=1 2>/dev/null | h3600_crc32 | h3600_put "$trailer" $((0xa4))

	echo "Sealing trailer..."
	mtd write "$trailer" trailer || nand_do_upgrade_failed

	nand_do_upgrade_success
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
