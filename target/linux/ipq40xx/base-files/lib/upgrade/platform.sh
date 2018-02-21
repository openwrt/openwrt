PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='fw_printenv fw_setenv'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

platform_check_image() {
	return 0;
}

platform_do_upgrade() {
	case "$(board_name)" in
	openmesh,a42)
		PART_NAME="inactive"
		platform_do_upgrade_openmesh "$ARGV"
		;;
	*)
		default_do_upgrade "$ARGV"
		;;
	esac
}

blink_led() {
	. /etc/diag.sh; set_state upgrade
}

append sysupgrade_pre_upgrade blink_led
