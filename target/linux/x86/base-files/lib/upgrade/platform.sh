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
	local ROOTFS
	sync
	grep -q -e "jffs2" -e "squashfs" /proc/cmdline \
		&& ROOTFS="$(awk 'BEGIN { RS=" "; FS="="; } ($1 == "block2mtd.block2mtd") { print substr($2,1,index($2, ",")-1) }' < /proc/cmdline)" \
		|| ROOTFS="$(awk 'BEGIN { RS=" "; FS="="; } ($1 == "root") { print $2 }' < /proc/cmdline)"
	[ -b ${ROOTFS%[0-9]} ] && get_image "$1" > ${ROOTFS%[0-9]}
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
