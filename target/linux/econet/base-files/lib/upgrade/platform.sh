platform_check_image() {
	local board=$(board_name)

	case "$board" in
	zte,zxhn-h267a)
		# Bespoke path, not nand_do_upgrade(): the bootloader CRC-checks
		# the rootfs slot every boot, so the kernel member must already
		# carry a valid tclinux header (device-entry.mk's
		# h267a-sysupgrade-tar) rather than being written raw.
		tar tf "$1" | grep -q '^sysupgrade-.*/kernel$' || return 1
		tar tf "$1" | grep -q '^sysupgrade-.*/root$' || return 1
		return 0
		;;
	chinamobile,gs3101|\
	dasan,h660gm-a-airtel|\
	dasan,h660gm-a-generic|\
	jiofiber,jcow407|\
	jiofiber,jcow414)
		return 0
		;;
	esac

	return 1
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	zte,zxhn-h267a)
		local tar_file="$1"
		local board_dir
		board_dir="$(tar tf "$tar_file" | grep -m1 '^sysupgrade-.*/$')"
		board_dir="${board_dir%/}"

		# rootfs first, kernel second: an interrupted write just drops
		# the bootloader to its recovery page, not a bad boot.
		tar xf "$tar_file" "$board_dir/root" -O | mtd write - rootfs_ro
		tar xf "$tar_file" "$board_dir/kernel" -O | mtd write - kernel1
		;;
	chinamobile,gs3101|\
	dasan,h660gm-a-airtel|\
	dasan,h660gm-a-generic|\
	jiofiber,jcow407|\
	jiofiber,jcow414)
		CI_KERNPART="tclinux_kernel"
		nand_do_upgrade "$1"
		;;
	esac
}

platform_copy_config() {
	case "$(board_name)" in
	zte,zxhn-h267a)
		# CI_UBIPART's default ("ubi") matches this board's partition
		# label, so no override is needed.
		nand_restore_config "$UPGRADE_BACKUP"
		;;
	esac
}
