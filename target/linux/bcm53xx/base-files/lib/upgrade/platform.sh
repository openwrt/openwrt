PART_NAME=firmware

# $(1): file to read magic from
# $(2): offset in bytes
get_magic_long_at() {
	dd if="$1" skip=$2 bs=1 count=4 2>/dev/null | hexdump -v -e '1/1 "%02x"'
}

platform_flash_type() {
	# On NAND devices "rootfs" is UBI volume, so won't be find in /proc/mtd
	grep -q "\"rootfs\"" /proc/mtd && {
		echo "serial"
		return
	}

	echo "nand"
}

platform_identify() {
	local magic

	magic=$(get_magic_long "$1")
	case "$magic" in
		"48445230")
			echo "trx"
			return
			;;
		"2a23245e")
			echo "chk"
			return
			;;
	esac

	magic=$(get_magic_long_at "$1" 14)
	[ "$magic" = "55324e44" ] && {
		echo "cybertan"
		return
	}

	echo "unknown"
}

platform_check_image() {
	[ "$#" -gt 1 ] && return 1

	[ "$(platform_flash_type)" = "nand" ] && {
		echo "Firmware upgrade on NAND devices is not implemented."
		return 1
	}

	local file_type=$(platform_identify "$1")

	case "$file_type" in
		"chk")
			local header_len=$((0x$(get_magic_long_at "$1" 4)))
			local board_id_len=$(($header_len - 40))
			local board_id=$(dd if="$1" skip=40 bs=1 count=$board_id_len 2>/dev/null | hexdump -v -e '1/1 "%c"')
			echo "Found CHK image with device board_id $board_id"
			echo "Flashing CHK images in unsupported. Please use only .trx files."
			return 1
		;;
		"cybertan")
			local magic=$(dd if="$1" bs=1 count=4 2>/dev/null | hexdump -v -e '1/1 "%c"')
			echo "Found CyberTAN image with device magic: $magic"
			echo "Flashing CyberTAN images in unsupported. Please use only .trx files."
			return 1
		;;
		"trx")
			return 0
		;;
		*)
			echo "Invalid image type. Please use only .trx files"
			return 1
		;;
	esac
}

# Extract TRX and use stadard upgrade method
platform_do_upgrade_chk() {
	local header_len=$((0x$(get_magic_long_at "$1" 4)))
	local trx="/tmp/$1.trx"

	dd if="$1" of="$trx" bs=$header_len skip=1
	shift
	platform_do_upgrade_trx "$trx" "$@"
}

# Extract TRX and use stadard upgrade method
platform_do_upgrade_cybertan() {
	local trx="/tmp/$1.trx"

	dd if="$1" of="$trx" bs=32 skip=1
	shift
	platform_do_upgrade_trx "$trx" "$@"
}

platform_do_upgrade_trx() {
	local flash_type=$(platform_flash_type)

	case "$flash_type" in
		"serial")
			default_do_upgrade "$@"
		;;
		"nand")
			echo "Firmware upgrade on NAND devices is REALLY unsupported."
		;;
	esac
}

platform_do_upgrade() {
	local file_type=$(platform_identify "$1")

	case "$file_type" in
		"chk")		platform_do_upgrade_chk "$ARGV";;
		"cybertan")	platform_do_upgrade_cybertan "$ARGV";;
		*)		platform_do_upgrade_trx "$ARGV";;
	esac
}
