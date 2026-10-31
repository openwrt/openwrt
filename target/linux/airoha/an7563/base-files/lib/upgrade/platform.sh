# SPDX-License-Identifier: GPL-2.0-only

REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	local board=$(board_name)

	[ "$#" -gt 1 ] && return 1

	case "$board" in
	tplink,xx530v-v2)
		return 0
		;;
	esac

	return 1
}

# The sysupgrade tar carries the tagged FIT as "kernel" and the ubinized
# rootfs as "root".  nand_upgrade_tar writes the first into the "kernel"
# mtd partition (0x620000, the address the stock U-Boot hardcodes) and the
# second into the "rootfs"/"rootfs_data" volumes of the "ubi" partition.
# The bootloader and oem partitions are read-only, so they cannot be hit.
platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	tplink,xx530v-v2)
		nand_do_upgrade "$1"
		;;
	esac
}
