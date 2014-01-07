x86_get_rootfs() {
	local rootfsdev
	local rootfstype
	
	rootfstype="$(awk 'BEGIN { RS=" "; FS="="; } ($1 == "rootfstype") { print $2 }' < /proc/cmdline)"
	case "$rootfstype" in
		squashfs|jffs2)
			rootfsdev="$(awk 'BEGIN { RS=" "; FS="="; } ($1 == "block2mtd.block2mtd") { print substr($2,1,index($2, ",")-1) }' < /proc/cmdline)";;
		ext4)
			rootfsdev="$(awk 'BEGIN { RS=" "; FS="="; } ($1 == "root") { print $2 }' < /proc/cmdline)";;
	esac
		
	echo "$rootfstype:$rootfsdev"
}

platform_check_image() {
	[ "$ARGC" -gt 1 ] && return 1

	case "$(get_magic_word "$1")" in
		eb48|eb63) return 0;;
		*)
			echo "Invalid image type"
			return 1
		;;
	esac
}

platform_copy_config() {
	local rootfs="$(x86_get_rootfs)"
	local rootfsdev="${rootfs##*:}"
	
	mount -t ext4 -o rw,noatime "${rootfsdev%[0-9]}1" /mnt
	cp -af "$CONF_TAR" /mnt/
	umount /mnt
}

platform_do_upgrade() {
	local rootfs="$(x86_get_rootfs)"
	local rootfsdev="${rootfs##*:}"

	sync
	[ -b ${rootfsdev%[0-9]} ] && get_image "$@" | dd of=${rootfsdev%[0-9]} bs=4096 conv=fsync
	sleep 1
}
