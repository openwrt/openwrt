platform_export_bootpart() {
	local cmdline uuid disk

	if read cmdline < /proc/cmdline; then
		case "$cmdline" in
			*block2mtd=*)
				disk="${cmdline##*block2mtd=}"
				disk="${disk%%,*}"
			;;
			*root=*)
				disk="${cmdline##*root=}"
				disk="${disk%% *}"
			;;
		esac

		case "$disk" in
			PARTUUID=[a-f0-9][a-f0-9][a-f0-9][a-f0-9][a-f0-9][a-f0-9][a-f0-9][a-f0-9]-02)
				uuid="${disk#PARTUUID=}"
				uuid="${uuid%-02}"
				for disk in /dev/[hsv]d[a-z]; do
					set -- $(dd if=$disk bs=1 skip=440 count=4 2>/dev/null | hexdump -v -e '4/1 "%02x "')
					if [ "$4$3$2$1" = "$uuid" ]; then
						export BOOTPART="${disk}1"
						return 0
					fi
				done
			;;
			/dev/*)
				export BOOTPART="${disk%[0-9]}1"
				return 0
			;;
		esac
	fi

	return 1
}

platform_check_image() {
	[ "$#" -gt 1 ] && return 1

	case "$(get_magic_word "$1")" in
		eb48|eb63) return 0;;
		*)
			echo "Invalid image type"
			return 1
		;;
	esac
}

platform_copy_config() {
	if [ -b "$BOOTPART" ]; then
		mount -t ext4 -o rw,noatime "$BOOTPART" /mnt
		cp -af "$CONF_TAR" /mnt/
		umount /mnt
	fi
}

platform_do_upgrade() {
	platform_export_bootpart

	if [ -b "${BOOTPART%[0-9]}" ]; then
		sync
		get_image "$@" | dd of="${BOOTPART%[0-9]}" bs=4096 conv=fsync
		sleep 1
	fi
}
