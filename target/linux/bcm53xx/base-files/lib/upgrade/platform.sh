PART_NAME=firmware

# $(1): file to read magic from
# $(2): offset in bytes
get_magic_long_at() {
	dd if="$1" skip=$2 bs=1 count=4 2>/dev/null | hexdump -v -e '1/1 "%02x"'
}

platform_machine() {
	cat /proc/device-tree/compatible | tr '\0' '\t' | cut -f 1
}

platform_flash_type() {
	# On NAND devices "rootfs" is UBI volume, so won't be find in /proc/mtd
	grep -q "\"rootfs\"" /proc/mtd && {
		echo "serial"
		return
	}

	echo "nand"
}

platform_expected_image() {
	local machine=$(platform_machine)

	case "$machine" in
		"netgear,r6250v1")	echo "chk U12H245T00_NETGEAR"; return;;
		"netgear,r6300v2")	echo "chk U12H240T00_NETGEAR"; return;;
		"netgear,r8000")	echo "chk U12H315T00_NETGEAR"; return;;
	esac
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

	local file_type=$(platform_identify "$1")
	local magic
	local error=0

	case "$file_type" in
		"chk")
			local header_len=$((0x$(get_magic_long_at "$1" 4)))
			local board_id_len=$(($header_len - 40))
			local board_id=$(dd if="$1" skip=40 bs=1 count=$board_id_len 2>/dev/null | hexdump -v -e '1/1 "%c"')
			local dev_board_id=$(platform_expected_image)
			echo "Found CHK image with device board_id $board_id"

			[ -n "$dev_board_id" -a "chk $board_id" != "$dev_board_id" ] && {
				echo "Firmware board_id doesn't match device board_id ($dev_board_id)"
				error=1
			}

			if ! otrx check "$1" -o "$header_len"; then
				echo "No valid TRX firmware in the CHK image"
				error=1
			fi
		;;
		"cybertan")
			local pattern=$(dd if="$1" bs=1 count=4 2>/dev/null | hexdump -v -e '1/1 "%c"')
			local dev_pattern=$(platform_expected_image)
			echo "Found CyberTAN image with device pattern: $pattern"

			[ -n "$dev_pattern" -a "cybertan $pattern" != "$dev_pattern" ] && {
				echo "Firmware pattern doesn't match device pattern ($dev_pattern)"
				error=1
			}

			if ! otrx check "$1" -o 32; then
				echo "No valid TRX firmware in the CyberTAN image"
				error=1
			fi
		;;
		"trx")
			if ! otrx check "$1"; then
				echo "Invalid (corrupted?) TRX firmware"
				error=1
			fi
		;;
		*)
			echo "Invalid image type. Please use only .trx files"
			error=1
		;;
	esac

	return $error
}

platform_pre_upgrade() {
	local file_type=$(platform_identify "$1")
	local dir="/tmp/sysupgrade-bcm53xx"
	local trx="$1"
	local offset

	[ "$(platform_flash_type)" != "nand" ] && return

	# Find trx offset
	case "$file_type" in
		"chk")		offset=$((0x$(get_magic_long_at "$1" 4)));;
		"cybertan")	offset=32;;
	esac

	# Extract partitions from trx
	rm -fR $dir
	mkdir -p $dir
	otrx extract "$trx" \
		${offset:+-o $offset} \
		-1 $dir/kernel \
		-2 $dir/root
	[ $? -ne 0 ] && {
		echo "Failed to extract TRX partitions."
		return
	}

	# Firmwares without UBI image should be flashed "normally"
	local root_type=$(identify $dir/root)
	[ "$root_type" != "ubi" ] && {
		echo "Provided firmware doesn't use UBI for rootfs."
		return
	}

	# Prepare TRX file with just a kernel that will replace current one
	local linux_length=$(grep "\"linux\"" /proc/mtd | sed "s/mtd[0-9]*:[ \t]*\([^ \t]*\).*/\1/")
	[ -z "$linux_length" ] && {
		echo "Unable to find \"linux\" partition size"
		exit 1
	}
	linux_length=$((0x$linux_length))
	local kernel_length=$(wc -c $dir/kernel | cut -d ' ' -f 1)
	[ $kernel_length -gt $linux_length ] && {
		echo "New kernel doesn't fit \"linux\" partition."
		return
	}
	rm -f /tmp/null.bin
	rm -f /tmp/kernel.trx
	touch /tmp/null.bin
	otrx create /tmp/kernel.trx \
		-f $dir/kernel -b $(($linux_length + 28)) \
		-f /tmp/null.bin
	[ $? -ne 0 ] && {
		echo "Failed to create simple TRX with new kernel."
		return
	}

	# Prepare UBI image (drop unwanted extra blocks)
	local ubi_length=0
	while [ "$(dd if=$dir/root skip=$ubi_length bs=1 count=4 2>/dev/null)" = "UBI#" ]; do
		ubi_length=$(($ubi_length + 131072))
	done
	dd if=$dir/root of=/tmp/root.ubi bs=131072 count=$((ubi_length / 131072)) 2>/dev/null
	[ $? -ne 0 ] && {
		echo "Failed to prepare new UBI image."
		return
	}

	# Flash
	mtd write /tmp/kernel.trx firmware
	nand_do_upgrade /tmp/root.ubi
}

platform_trx_from_chk_cmd() {
	local header_len=$((0x$(get_magic_long_at "$1" 4)))

	echo -n dd bs=$header_len skip=1
}

platform_trx_from_cybertan_cmd() {
	echo -n dd bs=32 skip=1
}

platform_do_upgrade() {
	local file_type=$(platform_identify "$1")
	local trx="$1"
	local cmd=

	[ "$(platform_flash_type)" == "nand" ] && {
		echo "Writing whole image to NAND flash. All erase counters will be lost."
	}

	case "$file_type" in
		"chk")		cmd=$(platform_trx_from_chk_cmd "$trx");;
		"cybertan")	cmd=$(platform_trx_from_cybertan_cmd "$trx");;
	esac

	default_do_upgrade "$trx" "$cmd"
}
