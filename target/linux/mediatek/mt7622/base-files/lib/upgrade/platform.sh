REQUIRE_IMAGE_METADATA=1

platform_do_upgrade() {
	local board=$(board_name)
	local file_type=$(identify $1)

	case "$board" in
	bananapi,bpi-r64)
		local rootdev="$(cmdline_get_var root)"
		rootdev="${rootdev##*/}"
		rootdev="${rootdev%p[0-9]*}"
		case "$rootdev" in
		mmc*)
			CI_ROOTDEV="$rootdev"
			CI_KERNPART="production"
			emmc_do_upgrade "$1"
			;;
		*)
			CI_KERNPART="fit"
			nand_do_upgrade "$1"
			;;
		esac
		;;
	buffalo,wsr-2533dhp2)
		local magic="$(get_magic_long "$1")"

		# use "mtd write" if the magic is "DHP2 (0x44485032)"
		# or "DHP3 (0x44485033)"
		if [ "$magic" = "44485032" -o "$magic" = "44485033" ]; then
			buffalo_upgrade_ubinized "$1"
		else
			CI_KERNPART="firmware"
			nand_do_upgrade "$1"
		fi
		;;
	elecom,wrc-x3200gst3|\
	iptime,ax8004m-ubi)
		nand_do_upgrade_iptime_ubi "$1"
		;;
	iptime,ax8004m)
		if grep -q boot_from=firmware1 /proc/cmdline; then
			PART_NAME=firmware1
		elif grep -q boot_from=firmware2 /proc/cmdline; then
			PART_NAME=firmware2
		fi
		default_do_upgrade "$1"
		;;
	mediatek,mt7622-rfb1-ubi|\
	netgear,wax206|\
	totolink,a8000ru|\
	xiaomi,redmi-router-ax6s)
		nand_do_upgrade "$1"
		;;
	linksys,e8450-ubi)
		CI_KERNPART="fit"
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
	buffalo,wsr-2533dhp2)
		buffalo_check_image "$board" "$magic" "$1" || return 1
		;;
	elecom,wrc-x3200gst3|\
	iptime,ax8004m-ubi|\
	mediatek,mt7622-rfb1-ubi|\
	netgear,wax206|\
	totolink,a8000ru|\
	xiaomi,redmi-router-ax6s)
		nand_do_platform_check "$board" "$1"
		return $?
		;;
	*)
		[ "$magic" != "d00dfeed" ] && {
			echo "Invalid image type."
			return 1
		}
		return 0
		;;
	esac

	return 0
}

platform_copy_config() {
	case "$(board_name)" in
	bananapi,bpi-r64)
		export_bootdevice
		export_partdevice rootdev 0
		if echo $rootdev | grep -q mmc; then
			emmc_copy_config
		fi
		;;
	esac
}

nand_do_upgrade_iptime_ubi() {
	local file="$1"
	local setenv_script="/tmp/fw_env_upgrade"

	echo '_init_env env set _init_env true; mtdparts default; env set mtdparts ${mtdparts},0x7400000@0x0200000(ubi); env set boot_from ubi; env save' >> $setenv_script
	echo 'bootstock if test "$boot_from" = "firmware1" || test "$boot_from" = "firmware2"; then efm boot; fi' >> $setenv_script
	echo 'bootubi if test "$boot_from" = "ubi"; then ubi part ubi && ubi read $loadaddr kernel && bootm $loadaddr#config-1; fi' >> $setenv_script
	echo 'bootmenu_11 c. Boot efm firmware or OpenWrt via Flash.=run boot11' >> $setenv_script
	echo 'boot11 rst_event; run _init_env ; run bootstock ; run bootubi ; efm tftpup ; reset' >> $setenv_script

	sync
	nand_do_flash_file "$file" && fw_setenv -s $setenv_script && nand_do_upgrade_success
	nand_do_upgrade_failed
}
