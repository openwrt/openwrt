PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='fw_printenv fw_setenv ubicrc32'
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

# The bootloader verifies ubi_k0/ubi_r0 against kernel_sz/kernel0_crc and
# rootfs_sz/rootfs0_crc at every boot and restores a mismatch from slot 1.
# The variables are updated before the volumes are written; the other
# order would pair a kernel restored from slot 1 with the new root
# filesystem if the upgrade stops in between. ubi_r1 is removed to make
# room for rootfs_data. ubicrc32 prints the CRC32 without the final
# inversion.
buffalo_wsr1500ax2s_crc32() {
	local crc

	crc="$(ubicrc32 "$1")" || return 1
	printf '%08x' $(( (crc ^ 0xffffffff) & 0xffffffff ))
}

buffalo_wsr1500ax2s_prepare_slot0() {
	local tar_file="$1"
	local cmd board_dir ubidev ubivol kernel rootfs kernel_crc rootfs_crc

	cmd="$(identify_if_gzip "$tar_file")cat"
	board_dir="$($cmd < "$tar_file" | tar tf - | grep -m 1 '^sysupgrade-.*/$')"
	board_dir="${board_dir%/}"
	kernel="/tmp/wsr-kernel"
	rootfs="/tmp/wsr-rootfs"
	$cmd < "$tar_file" | tar xOf - "$board_dir/kernel" > "$kernel" || return 1
	$cmd < "$tar_file" | tar xOf - "$board_dir/root" > "$rootfs" || return 1
	kernel_crc="$(buffalo_wsr1500ax2s_crc32 "$kernel")" || return 1
	rootfs_crc="$(buffalo_wsr1500ax2s_crc32 "$rootfs")" || return 1

	ubidev="$(nand_find_ubi "$CI_UBIPART")"
	ubivol="$(nand_find_volume "$ubidev" ubi_r1)"
	[ -z "$ubivol" ] || ubirmvol "/dev/$ubidev" -N ubi_r1 || return 1

	printf 'kernel_sz %x\nkernel0_crc %s\nrootfs_sz %x\nrootfs0_crc %s\n' \
		"$(wc -c < "$kernel")" "$kernel_crc" \
		"$(wc -c < "$rootfs")" "$rootfs_crc" > /tmp/wsr-env || return 1
	printf 'sw_commit 0\nsw_tryactive 2\n' >> /tmp/wsr-env
	fw_setenv --script /tmp/wsr-env || return 1
	rm -f "$kernel" "$rootfs" /tmp/wsr-env
}

platform_check_image() {
	local board=$(board_name)

	case "$board" in
	draytek,g2282x)
		draytek_g2282x_set_part_name || return 1
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
	buffalo,wsr-1500ax2s)
		CI_KERNPART="ubi_k0"
		CI_ROOTPART="ubi_r0"
		buffalo_wsr1500ax2s_prepare_slot0 "$1" || return 1
		nand_do_upgrade "$1"
		;;
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
