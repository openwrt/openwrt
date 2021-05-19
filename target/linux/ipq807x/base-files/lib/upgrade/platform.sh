PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='fw_printenv fw_setenv'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

platform_check_image() {
	return 0;
}

platform_do_upgrade() {
	case "$(board_name)" in
	xiaomi,ax3600)
		part_num="$(fw_printenv -n flag_boot_rootfs)"
		if [ "$part_num" -eq "0" ]; then
			CI_UBIPART="rootfs_1"
		else
			CI_UBIPART="rootfs"
		fi

		fw_setenv flag_ota_reboot 1
		nand_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
