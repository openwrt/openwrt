#
# Copyright (C) 2011 OpenWrt.org
#

. /lib/functions/system.sh
. /lib/realtek.sh

PART_NAME=firmware

mtd_get_offset() {
	cat /sys/class/mtd/$1/offset | awk '{printf ("%08x",$1)}'
}

find_partition_mtd() {
	local first dev size erasesize name
	while read dev size erasesize name; do
		name=${name#'"'}; name=${name%'"'}
		if [ x"$name" = x"$1" ]; then
			echo ${dev%':'}
			return 0
		fi
	done < /proc/mtd

	return 1
}

get_magic_str() {
	(get_image "$@" | dd bs=4 count=1) 2>/dev/null
}

get_burn_addr() {
	(get_image "$@" | dd bs=4 count=1 skip=2 | hexdump -v -n 4 -e '1/1 "%02x"') 2>/dev/null
}

platform_check_image() {
	local partition_offset
	local signature
	local burn_addr

	[ "$#" -gt 1 ] && return 1

	partition_offset=$(mtd_get_offset $(find_partition_mtd $PART_NAME) 2>/dev/null)
	if [ -z "$partition_offset" ]; then
		echo "Unable to fetch offset of parition $PART_NAME"
		return 1
	fi

	signature=$(get_magic_str "$1")
	burn_addr=$(get_burn_addr "$1")

	case "$signature" in
	cs6b|\
	cs6c|\
	csys|\
	cr6b|\
	cr6c|\
	csro)
		;;
	*)
		echo "Invalid image. Signature $signature not recognized."
		return 1;
		;;
	esac

	if [ "$partition_offset" != "$burn_addr" ]; then
		echo "Invalid image. Burn address $burn_addr does not equal to the offset of partition $PART_NAME ($partition_offset)"
		return 1;
	fi

	return 0;
}

platform_do_upgrade() {
	default_do_upgrade "$ARGV"
}
