PART_NAME=linux

platform_check_image() {
	[ "$ARGC" -gt 1 ] && return 1

	case "$(get_magic_word "$1")" in
		# rdc .bin files for ar360w3g
		474d) return 0;;
		*)
			echo "Invalid image type. Please use an appropriate .bin file"
			return 1
		;;
	esac
}

# use default for platform_do_upgrade()
