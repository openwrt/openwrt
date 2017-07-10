. /lib/ipq806x.sh

PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='/usr/sbin/fw_printenv /usr/sbin/fw_setenv'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

platform_check_image() {
	return 0;
}

platform_do_upgrade() {
	local board=$(ipq806x_board_name)

	case "$board" in
	ap148 |\
	ap-dk04.1-c1 |\
	d7800 |\
	nbg6817 |\
	r7500 |\
	r7500v2 |\
	r7800)
		nand_do_upgrade "$ARGV"
		;;
	c2600)
		PART_NAME="os-image:rootfs"
		MTD_CONFIG_ARGS="-s 0x200000"
		default_do_upgrade "$ARGV"
		;;
	ea8500)
		platform_do_upgrade_linksys "$ARGV"
		;;
	vr2600v)
		PART_NAME="kernel:rootfs"
		MTD_CONFIG_ARGS="-s 0x200000"
		default_do_upgrade "$ARGV"
		;;
	*)
		default_do_upgrade "$ARGV"
		;;
	esac
}

platform_nand_pre_upgrade() {
	local board=$(ipq806x_board_name)

	case "$board" in
	nbg6817)
		zyxel_do_upgrade "$1"
		;;
	esac
}

blink_led() {
	. /etc/diag.sh; set_state upgrade
}

append sysupgrade_pre_upgrade blink_led
