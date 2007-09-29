platform_check_image() {
	[ "$ARGC" -gt 1 ] && return 1

	case "$(get_magic_word "$1")" in
		# .trx files
		4844) return 0;;
		# .bin files
		5735) return 0;;
		*)
			echo "Invalid image type"
			return 1
		;;
	esac
}

platform_do_upgrade() {
	get_image "$1" > $(find_mtd_part "linux")
	sync
}

brcm_prepare_mtd() {
	[ "$SAVE_CONFIG" -eq 1 ] && return 0
}
append sysupgrade_pre_upgrade brcm_prepare_mtd
