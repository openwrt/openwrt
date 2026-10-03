PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='fw_printenv fw_setenv'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

# The TP-Link bootloader gets its flash layout from "bootargs".
# Use this to our advantage, and:
#  1. Disable the second rootfs (usrimg2)
#  2. Extend the first rootfs (usrimg1) to include more space
#  3. Increase the baudrate to the expected 115200
tplink_sg2xxx_fix_mtdparts() {
	local args
	args="bootargs mtdparts=spi_flash:896K(boot),128K(env),6144K(sys)"
	args="$args,20480K(usrimg1@main),0K(usrimg2)"
	args="$args,4096K(usrappfs),1024K(para)\n"
	args="$args baudrate 115200"

	echo -e "$args" | fw_setenv --script -
}

# Board identity is checked before this: REQUIRE_IMAGE_METADATA is set, so
# fwtool refuses an image whose metadata does not list this board unless -F
# is given. This check only rejects an image that is not in the Realtek SDK
# uImage container (magic 0x83800000), which the bootloader requires.
horaco_check_image() {
	local magic
	magic="$(get_magic_long "$1")"

	[ "$magic" = "83800000" ] && return 0

	echo "Invalid image magic $magic; expected 83800000." >&2
	echo "This board takes an OpenWrt image built for it, or a vendor" >&2
	echo "upgrade file. Nothing else will boot." >&2
	return 1
}

platform_check_image() {
	local board=$(board_name)

	case "$board" in
	draytek,g2282x)
		draytek_g2282x_set_part_name || return 1
		;;
	horaco,zx-swtgw2c48ns)
		horaco_check_image "$1"
		return $?
		;;
	esac

	return 0
}

# Active slot is a "bootpartition" variable in the SYSINFO mtd
# partition, not standard U-Boot env. Self-upgrades the active slot
# instead of flipping it.
draytek_g2282x_set_part_name() {
	local bootpart sysinfo
	sysinfo=$(find_mtd_part SYSINFO)
	bootpart=$(sed -n 's/.*bootpartition=\([01]\).*/\1/p' "${sysinfo:-/dev/null}")
	case "$bootpart" in
	0) PART_NAME="Kernel" ;;
	1) PART_NAME="Kernel2" ;;
	*)
		echo "draytek,g2282x: could not read bootpartition from SYSINFO" >&2
		return 1
		;;
	esac
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	draytek,g2282x)
		draytek_g2282x_set_part_name || return 1
		default_do_upgrade "$1"
		;;
	plasmacloud,esx28|\
	plasmacloud,mcx3|\
	plasmacloud,psx8|\
	plasmacloud,psx10|\
	plasmacloud,psx28)
		PART_NAME="inactive"
		platform_do_upgrade_dualboot_plasmacloud "$1"
		;;
	tplink,sg2008p-v1|\
	tplink,sg2008p-v3|\
	tplink,sg2210p-v3)
		tplink_sg2xxx_fix_mtdparts
		default_do_upgrade "$1"
		;;
	linksys,lgs328c|\
	linksys,lgs328mpc-v2|\
	linksys,lgs352c)
		nand_do_upgrade "$1"
		;;
	zyxel,xgs1930-28hp|\
	zyxel,xmg1915-10e|\
	zyxel,xmg1915-10ep|\
	zyxel,xs1930-10|\
	zyxel,xs1930-12f|\
	zyxel,xs1930-12hp)
		PART_NAME="factory"
		default_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
