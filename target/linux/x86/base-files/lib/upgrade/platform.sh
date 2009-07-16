USE_REFRESH=1

platform_check_image() {
	[ "$ARGC" -gt 1 ] && return 1

	case "$(get_magic_word "$1")" in
		eb48) return 0;;
		*)
			echo "Invalid image type"
			return 1
		;;
	esac
}

platform_do_upgrade() {
	sync
	get_image "$1" > /dev/hda
}

x86_prepare_ext2() {
	# if we're running from ext2, we need to make sure that we have a mtd 
	# partition that points to the active rootfs partition.
	# however this only matters if we actually need to preserve the config files
	[ "$SAVE_CONFIG" -eq 1 ] || return 0
	grep rootfs /proc/mtd >/dev/null || {
		echo /dev/hda2,65536,rootfs > /sys/module/block2mtd/parameters/block2mtd
	}
}
append sysupgrade_pre_upgrade x86_prepare_ext2
