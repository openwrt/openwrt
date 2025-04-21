REQUIRE_IMAGE_METADATA=1
RAMFS_COPY_BIN='fitblk fit_check_sign'

platform_do_upgrade() {
	local board=$(board_name)
	local file_type=$(identify $1)

	case "$board" in
	bananapi,bpi-r64|\
	linksys,e8450-ubi|\
	ubnt,unifi-6-lr-v1-ubootmod|\
	ubnt,unifi-6-lr-v2-ubootmod|\
	ubnt,unifi-6-lr-v3-ubootmod|\
	xiaomi,redmi-router-ax6s)
		fit_do_upgrade "$1"
		;;
	buffalo,wsr-2533dhp2|\
	buffalo,wsr-3200ax4s)
		buffalo_do_upgrade "$1"
		;;
	dlink,eagle-pro-ai-m32-a1|\
	dlink,eagle-pro-ai-r32-a1|\
	elecom,wrc-x3200gst3|\
	mediatek,mt7622-rfb1-ubi|\
	netgear,wax206|\
	totolink,a8000ru)
		nand_do_upgrade "$1"
		;;
	linksys,e8450)
		if grep -q mtdparts=slave /proc/cmdline; then
			PART_NAME=firmware2
		else
			PART_NAME=firmware1
		fi
		default_do_upgrade "$1"
		;;
	smartrg,sdg-841-t6)
		CI_KERNPART="boot"
		CI_ROOTPART="res1"
		emmc_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}

PART_NAME=firmware

platform_check_image() {
	local board=$(board_name)
	local magic="$(get_magic_long "$1")"

	[ "$#" -gt 1 ] && return 1

	case "$board" in
	buffalo,wsr-2533dhp2|\
	buffalo,wsr-3200ax4s)
		buffalo_check_image "$board" "$magic" "$1" || return 1
		;;
	dlink,eagle-pro-ai-m32-a1|\
	dlink,eagle-pro-ai-r32-a1|\
	elecom,wrc-x3200gst3|\
	mediatek,mt7622-rfb1-ubi|\
	netgear,wax206|\
	smartrg,sdg-841-t6|\
	totolink,a8000ru)
		nand_do_platform_check "$board" "$1"
		return $?
		;;
	*)
		fit_check_image "$1"
		return $?
		;;
	esac

	return 0
}

platform_copy_config() {
	case "$(board_name)" in
	bananapi,bpi-r64)
		if [ "$CI_METHOD" = "emmc" ]; then
			emmc_copy_config
		fi
		;;
	smartrg,sdg-841-t6)
		emmc_copy_config
		;;
	esac
}
