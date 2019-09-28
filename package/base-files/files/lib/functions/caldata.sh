# Copyright (C) 2019 OpenWrt.org

. /lib/functions.sh
. /lib/functions/system.sh

caldata_die() {
	echo "caldata: " "$*"
	exit 1
}

caldata_extract() {
	local part=$1
	local offset=$(($2))
	local count=$(($3))
	local mtd

	mtd=$(find_mtd_chardev $part)
	[ -n "$mtd" ] || caldata_die "no mtd device found for partition $part"

	dd if=$mtd of=/lib/firmware/$FIRMWARE iflag=skip_bytes bs=$count skip=$offset count=1 2>/dev/null || \
		caldata_die "failed to extract calibration data from $mtd"
}

caldata_extract_ubi() {
	local part=$1
	local offset=$(($2))
	local count=$(($3))
	local ubidev
	local ubi

	. /lib/upgrade/nand.sh

	ubidev=$(nand_find_ubi $CI_UBIPART)
	ubi=$(nand_find_volume $ubidev $part)
	[ -n "$ubi" ] || caldata_die "no UBI volume found for $part"

	dd if=/dev/$ubi of=/lib/firmware/$FIRMWARE iflag=skip_bytes bs=$count skip=$offset count=1 2>/dev/null || \
		caldata_die "failed to extract calibration data from $ubi"
}

caldata_extract_reverse() {
	local part=$1
	local offset=$2
	local count=$(($3))
	local mtd
	local reversed
	local caldata

	mtd=$(find_mtd_chardev "$part")
	reversed=$(hexdump -v -s $offset -n $count -e '/1 "%02x "' $mtd)

	for byte in $reversed; do
		caldata="\x${byte}${caldata}"
	done

	printf "%b" "$caldata" > /lib/firmware/$FIRMWARE
}

caldata_from_file() {
	local source=$1
	local offset=$(($2))
	local count=$(($3))

	dd if=$source of=/lib/firmware/$FIRMWARE iflag=skip_bytes bs=$count skip=$offset count=1 2>/dev/null || \
		caldata_die "failed to extract calibration data from $source"
}

caldata_valid() {
	local expected="$1"

	magic=$(hexdump -v -n 2 -e '1/1 "%02x"' /lib/firmware/$FIRMWARE)
	[[ "$magic" == "$expected" ]]
	return $?
}

