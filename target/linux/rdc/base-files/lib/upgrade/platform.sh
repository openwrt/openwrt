PART_NAME=firmware

platform_check_image() {
	[ "$(get_magic_word $(find_mtd_part firmware))" -eq "$(get_magic_word "$1")" ] && return 0

	echo "Invalid image type. Please use an appropriate .bin file"
	return 1
}

# use default for platform_do_upgrade()
