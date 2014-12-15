PART_NAME=firmware

brcm47xx_identify() {
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

	echo "unknown"
}

platform_check_image() {
	[ "$#" -gt 1 ] && return 1

	local file_type=$(brcm47xx_identify "$1")

	case "$file_type" in
		"trx") return 0;;
		*)
			echo "Invalid image type. Please use only .trx files"
			return 1
		;;
	esac
}

# use default for platform_do_upgrade()
