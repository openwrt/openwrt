# Copyright (C) 2011 OpenWrt.org

PART_NAME=firmware

REQUIRE_IMAGE_METADATA=1
platform_check_image() {
	return 0
}

RAMFS_COPY_BIN='fw_printenv fw_setenv nandwrite'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

bsb002_do_upgrade() {
	local target
	if grep -q 'ubi.mtd=root-1' /proc/cmdline; then
		target=0
	else
		target=1
	fi
	CI_KERNPART="kernel-$target"
	CI_UBIPART="root-$target"
	nand_do_flash_file "$1" || {
		echo "BSB002: failed to write slot $target"
		nand_do_upgrade_failed
	}
	fw_setenv bootslot "$target" || {
		echo "BSB002: failed to set bootslot"
		nand_do_upgrade_failed
	}
	nand_do_upgrade_success
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	philips,bsb002)
		bsb002_do_upgrade "$1"
		;;
	glinet,gl-ar300m-nand|\
	glinet,gl-ar300m-nor)
		glinet_nand_nor_do_upgrade "$1"
		;;
	glinet,gl-ar750s-nor|\
	glinet,gl-ar750s-nor-nand|\
	glinet,gl-s200-nor|\
	glinet,gl-s200-nor-nand|\
	glinet,gl-x1200-nor|\
	glinet,gl-x1200-nor-nand)
		nand_nor_do_upgrade "$1"
		;;
	*)
		nand_do_upgrade "$1"
		;;
	esac
}
