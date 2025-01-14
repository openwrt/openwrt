REQUIRE_IMAGE_METADATA=1
RAMFS_COPY_BIN='fitblk'

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	bananapi,bpi-rv2-nand)
		fit_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}

PART_NAME=firmware

platform_check_image() {
	local board=$(board_name)
	local magic="$(get_magic_long "$1")"

	[ "$#" -gt 1 ] && return 1

	case "$board" in
	*)
		[ "$magic" != "d00dfeed" ] && {
			echo "Invalid image type."
			return 1
		}
		return 0
		;;
	esac

	return 0
}
