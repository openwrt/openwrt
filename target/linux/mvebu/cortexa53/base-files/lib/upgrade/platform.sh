#
# Copyright (C) 2014-2016 OpenWrt.org
# Copyright (C) 2016 LEDE-Project.org
#

RAMFS_COPY_BIN='fw_printenv fw_setenv mkfs.f2fs fdisk'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'
REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	case "$(board_name)" in
	glinet,gl-mv1000|\
	globalscale,espressobin|\
	globalscale,espressobin-emmc|\
	globalscale,espressobin-ultra|\
	globalscale,espressobin-v7|\
	globalscale,espressobin-v7-emmc)
		legacy_sdcard_check_image "$1"
		;;
	*)
		return 0
		;;
	esac
}

platform_do_upgrade() {
	case "$(board_name)" in
	glinet,gl-mv1000|\
	globalscale,espressobin|\
	globalscale,espressobin-emmc|\
	globalscale,espressobin-ultra|\
	globalscale,espressobin-v7|\
	globalscale,espressobin-v7-emmc)
		legacy_sdcard_do_upgrade "$1"
		;;
	methode,udpu|\
	methode,edpu)
		[ "$(rootfs_type)" = "tmpfs" ] && {
			local firmware_active="$(fw_printenv -n bootactive)"
			case "$firmware_active" in
			1)
				CI_KERNPART="kernel_2"
				CI_ROOTPART="rootfs_2"
				fw_setenv bootactive 2
				;;
			2)
				CI_KERNPART="kernel_1"
				CI_ROOTPART="rootfs_1"
				fw_setenv bootactive 1
				;;
			esac
		}

		local root="$(cmdline_get_var root)"
		case "$root" in
		/dev/mmcblk*p2)
			CI_KERNPART="kernel_2"
			CI_ROOTPART="rootfs_2"
			fw_setenv bootactive 2
			;;
		/dev/mmcblk*p4)
			CI_KERNPART="kernel_1"
			CI_ROOTPART="rootfs_1"
			fw_setenv bootactive 1
			;;
		esac

		emmc_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
platform_copy_config() {
	case "$(board_name)" in
	glinet,gl-mv1000|\
	globalscale,espressobin|\
	globalscale,espressobin-emmc|\
	globalscale,espressobin-ultra|\
	globalscale,espressobin-v7|\
	globalscale,espressobin-v7-emmc)
		legacy_sdcard_copy_config
		;;
	methode,udpu|\
	methode,edpu)
		emmc_copy_config
		;;
	esac
}
