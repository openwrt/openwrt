#
# Copyright (C) 2014-2016 OpenWrt.org
# Copyright (C) 2016 LEDE-Project.org
#

PART_NAME='firmware'
RAMFS_COPY_BIN='fw_printenv fw_setenv'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'
REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	case "$(board_name)" in
	cznic,turris-omnia|globalscale,espressobin|globalscale,espressobin-emmc|globalscale,espressobin-v7|globalscale,espressobin-v7-emmc|\
	marvell,armada8040-mcbin|solidrun,clearfog-base-a1|solidrun,clearfog-pro-a1)
		platform_check_image_sdcard "$ARGV"
		;;
	*)
		return 0
		;;
	esac
}

platform_do_upgrade_mv1000(){
	local firmware=`fw_printenv firmware | awk -F '=' '{print $2}'`				
	
	case "$firmware" in
	gl-mv1000-emmc)
		platform_do_upgrade_sdcard "$ARGV"
		;;
	gl-mv1000-emmc-gzip)
		platform_do_upgrade_sdcard "$ARGV"
		;;
	*)
		default_do_upgrade "$ARGV"
		;;
	esac
}

platform_do_upgrade() {
	case "$(board_name)" in
	linksys,caiman|linksys,cobra|linksys,mamba|linksys,rango|linksys,shelby|linksys,venom)
		platform_do_upgrade_linksys "$ARGV"
		;;
	cznic,turris-omnia|globalscale,espressobin|globalscale,espressobin-emmc|globalscale,espressobin-v7|globalscale,espressobin-v7-emmc|\
	marvell,armada8040-mcbin|solidrun,clearfog-base-a1|solidrun,clearfog-pro-a1)
		platform_do_upgrade_sdcard "$ARGV"
		;;
	gl-mv1000)
		platform_do_upgrade_mv1000 "$ARGV"
		;;
	*)
		default_do_upgrade "$ARGV"
		;;
	esac
}
platform_copy_config() {
	case "$(board_name)" in
	linksys,caiman|linksys,cobra|linksys,mamba|linksys,rango|linksys,shelby|linksys,venom)
		platform_copy_config_linksys
		;;
	cznic,turris-omnia|globalscale,espressobin|globalscale,espressobin-emmc|globalscale,espressobin-v7|globalscale,espressobin-v7-emmc|\
	marvell,armada8040-mcbin|solidrun,clearfog-base-a1|solidrun,clearfog-pro-a1)
		platform_copy_config_sdcard "$ARGV"
		;;
	gl-mv1000)
		platform_copy_config_sdcard "$ARGV"
		;;
	esac
}
