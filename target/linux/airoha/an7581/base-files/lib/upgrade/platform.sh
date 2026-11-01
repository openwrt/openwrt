RAMFS_COPY_BIN='fitblk fit_check_sign tr'

REQUIRE_IMAGE_METADATA=1

gemtek_factory_valid() {
	local vol="$1"
	local mac

	[ "$(hexdump -v -n 2 -e '2/1 "%02x"' "$vol")" = "9079" ] || return 1

	mac="$(hexdump -v -s $((0x5000)) -n 6 -e '6/1 "%02x"' "$vol")"
	case "$mac" in
	000000000000|ffffffffffff|?[13579bdf]*)
		return 1
		;;
	esac
}

gemtek_mac_bytes() {
	printf "$(echo "$1" | sed 's/^/\\x/;s/:/\\x/g')"
}

# The factory volume (MT7996 EEPROM and the MAC addresses) is built from the
# vendor partition, which the install leaves untouched. Rebuild it when it is
# missing or empty, as after the chainloader formatted the ubi partition.
gemtek_prepare_factory() {
	local prefix="$1"
	local ubidev vol mtd dsd ebs off eeprom wan lan fan serial mac
	local blob="/tmp/factory.bin"

	ubidev="$(nand_find_ubi ubi)" || return 0
	vol="$(nand_find_volume "$ubidev" factory)"
	[ -n "$vol" ] && gemtek_factory_valid "/dev/$vol" && return 0

	mtd="$(find_mtd_index vendor)"
	[ -n "$mtd" ] || return 0
	mtd="/dev/mtd$mtd"

	dsd="$(dd if="$mtd" bs=4096 skip=$((0x400000 / 4096)) count=1 2>/dev/null | tr -c ' -~' '\n')"
	wan="$(echo "$dsd" | sed -n '/^wan_mac=/{s///p;q}')"
	lan="$(echo "$dsd" | sed -n '/^lan_mac=/{s///p;q}')"
	fan="$(echo "$dsd" | sed -n '/^fan_id=/{s///p;q}')"
	serial="$(echo "$dsd" | sed -n '/^serial_number=/{s///p;q}')"

	case "$serial" in
	$prefix*) ;;
	*)
		v "factory: serial number '$serial' does not match, not rebuilding"
		return 0
		;;
	esac

	for mac in "$wan" "$lan"; do
		echo "$mac" | grep -qE '^[0-9A-Fa-f]{2}(:[0-9A-Fa-f]{2}){5}$' || {
			v "factory: invalid MAC '$mac' in the vendor partition, not rebuilding"
			return 0
		}
	done

	# The EEPROM is in the first good block from 0x405000.
	ebs="$(cat /sys/class/mtd/${mtd#/dev/}/erasesize)"
	for off in 0 1 2 3; do
		off=$((0x405000 + off * ebs))
		[ "$(hexdump -v -s "$off" -n 2 -e '2/1 "%02x"' "$mtd")" = "9079" ] && {
			eeprom="$off"
			break
		}
	done
	[ -n "$eeprom" ] || {
		v "factory: no EEPROM in the vendor partition, not rebuilding"
		return 0
	}

	dd if=/dev/zero of="$blob" bs=4096 count=16 2>/dev/null
	dd if="$mtd" bs=512 skip=$((eeprom / 512)) count=15 2>/dev/null | \
		dd of="$blob" conv=notrunc 2>/dev/null
	gemtek_mac_bytes "$wan" | dd of="$blob" bs=4096 seek=5 conv=notrunc 2>/dev/null
	gemtek_mac_bytes "$lan" | dd of="$blob" bs=4096 seek=6 conv=notrunc 2>/dev/null
	printf '%s' "$fan" | dd of="$blob" bs=4096 seek=7 conv=notrunc 2>/dev/null
	printf '%s' "$serial" | dd of="$blob" bs=4096 seek=8 conv=notrunc 2>/dev/null

	if [ -z "$vol" ]; then
		ubimkvol "/dev/$ubidev" -N factory -t static -s $((0x10000)) || return 0
		vol="$(nand_find_volume "$ubidev" factory)"
	fi
	ubiupdatevol "/dev/$vol" "$blob" && \
		v "factory: rebuilt from the vendor partition (WAN $wan, LAN $lan)"
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
	nokia,xg-040g-md)
		nand_do_platform_check "$board" "$1"
		return $?
		;;
	gemtek,xr1710g-ubi|\
	nokia,xg-040g-md-ubi|\
	quantum,q1000k-ubi)
		fit_check_image "$1"
		return $?
		;;
	esac

	return 0
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
		gemtek,xr1710g-ubi)
			gemtek_prepare_factory XR1710G
			fit_do_upgrade "$1"
			;;
		nokia,xg-040g-md-ubi|\
		quantum,q1000k-ubi)
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
	*)
		;;
	esac
}
