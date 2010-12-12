PART_NAME=linux

platform_check_image() {
	[ "$ARGC" -gt 1 ] && return 1

	case "$(get_magic_word "$1")" in
		# .trx files
		2705) return 0;;
		*)
			echo "Invalid image type"
			return 1
		;;
	esac
}

# use default for platform_do_upgrade()

disable_watchdog() {
	killall watchdog
	( ps | grep -v 'grep' | grep '/dev/watchdog' ) && {
		echo 'Could not disable watchdog'
		return 1
	}
}
append sysupgrade_pre_upgrade disable_watchdog
