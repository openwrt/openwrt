PART_NAME=firmware

platform_check_image() {
	[ "$#" -gt 1 ] && return 1

	case "$(get_magic_word "$1")" in
		# .trx files
		4844) return 0;;
		*)
			echo "Invalid image type. Please use only .trx files"
			return 1
		;;
	esac
}

# use default for platform_do_upgrade()
