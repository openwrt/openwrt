PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1
RAMFS_COPY_BIN='fitblk fit_check_sign'

platform_check_image() {
	local board=$(board_name)

	case "$board" in
		gemtek,w1700k-ubi)
			return 0
			;;
		bell,xg-140g-y003)
			nand_do_platform_check "$board" "$1"
			return $?
			;;
	esac

	echo "Sysupgrade is not yet supported on $board."
	return 1
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
		gemtek,w1700k-ubi)
			fit_do_upgrade "$1"
			;;
		bell,xg-140g-y003)
			CI_KERNPART="kernel"
			CI_UBIPART="ubi"
			nand_do_upgrade "$1"
			;;
		*)
			nand_do_upgrade "$1"
			;;
	esac
}
