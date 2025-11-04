# Copyright (C) 2026 OpenWrt.org

REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	return 0
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
		tplink,ex530v-v1 | \
		tplink,xx230v-v1 | \
		tplink,xx530v-v1)
			# Write firmware to the "firmware" mtd because have header on start of image
			PART_NAME=firmware
			default_do_upgrade "$1"
			;;
		*)
			nand_do_upgrade "$1"
			;;
	esac
}