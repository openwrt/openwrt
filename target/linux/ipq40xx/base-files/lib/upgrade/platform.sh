PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='fw_printenv fw_setenv'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

platform_check_image() {
	case "$(board_name)" in
	asus,rt-ac58u)
		CI_UBIPART="UBI_DEV"
		local ubidev=$(nand_find_ubi $CI_UBIPART)
		local asus_root=$(nand_find_volume $ubidev jffs2)

		[ -n "$asus_root" ] || return 0

		cat << EOF
jffs2 partition is still present.
There's probably no space left
to install the filesystem.

You need to delete the jffs2 partition first:
# ubirmvol /dev/ubi0 --name=jffs2

Once this is done. Retry.
EOF
		return 1
		;;
	esac
	return 0;
}

askey_do_upgrade() {
	local tar_file="$1"

	local board_dir=$(tar tf $tar_file | grep -m 1 '^sysupgrade-.*/$')
	board_dir=${board_dir%/}

	tar Oxf $tar_file ${board_dir}/root | mtd write - rootfs

	nand_do_upgrade "$1"
}

zyxel_do_upgrade() {
	local tar_file="$1"

	local board_dir=$(tar tf $tar_file | grep -m 1 '^sysupgrade-.*/$')
	board_dir=${board_dir%/}

	tar Oxf $tar_file ${board_dir}/kernel | mtd write - kernel

	if [ -n "$UPGRADE_BACKUP" ]; then
		tar Oxf $tar_file ${board_dir}/root | mtd -j "$UPGRADE_BACKUP" write - rootfs
	else
		tar Oxf $tar_file ${board_dir}/root | mtd write - rootfs
	fi
}

platform_do_upgrade() {
	case "$(board_name)" in
	8dev,jalapeno |\
	aruba,ap-303 |\
	aruba,ap-303h |\
	aruba,ap-365 |\
	avm,fritzbox-7530 |\
	avm,fritzrepeater-1200 |\
	avm,fritzrepeater-3000 |\
	buffalo,wtr-m2133hp |\
	cilab,meshpoint-one |\
	edgecore,ecw5211 |\
	edgecore,oap100 |\
	engenius,eap2200 |\
	glinet,gl-ap1300 |\
	luma,wrtq-329acn |\
	mobipromo,cm520-79f |\
	qxwlan,e2600ac-c2)
		nand_do_upgrade "$1"
		;;
	alfa-network,ap120c-ac)
		part="$(awk -F 'ubi.mtd=' '{printf $2}' /proc/cmdline | sed -e 's/ .*$//')"
		if [ "$part" = "rootfs1" ]; then
			fw_setenv active 2 || exit 1
			CI_UBIPART="rootfs2"
		else
			fw_setenv active 1 || exit 1
			CI_UBIPART="rootfs1"
		fi
		nand_do_upgrade "$1"
		;;
	asus,map-ac2200)
		CI_KERNPART="linux"
		nand_do_upgrade "$1"
		;;
	asus,rt-ac58u)
		CI_UBIPART="UBI_DEV"
		CI_KERNPART="linux"
		nand_do_upgrade "$1"
		;;
	cellc,rtl30vw)
		CI_UBIPART="ubifs"
		askey_do_upgrade "$1"
		;;
	compex,wpj419)
		nand_do_upgrade "$1"
		;;
	linksys,ea6350v3 |\
	linksys,ea8300 |\
	linksys,mr8300)
		platform_do_upgrade_linksys "$1"
		;;
	meraki,mr33)
		CI_KERNPART="part.safe"
		nand_do_upgrade "$1"
		;;
	mikrotik,hap-ac2|\
	mikrotik,sxtsq-5-ac)
		[ "$(rootfs_type)" = "tmpfs" ] && mtd erase firmware
		default_do_upgrade "$1"
		;;
	openmesh,a42 |\
	openmesh,a62 |\
	plasmacloud,pa1200 |\
	plasmacloud,pa2200)
		PART_NAME="inactive"
		platform_do_upgrade_dualboot_datachk "$1"
		;;
	zyxel,nbg6617)
		zyxel_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
