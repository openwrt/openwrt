#
# Copyright (C) 2011 OpenWrt.org
#

PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='fw_printenv fw_setenv'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

platform_check_image() {
	local board=$(board_name)

	case "$board" in
	jjplus,ja76pf2|\
	ubnt,routerstation|\
	ubnt,routerstation-pro)
		platform_check_image_redboot_fis "$1"
		;;
	*)
		return 0
		;;
	esac
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	adtran,bsap1800-v2|\
	adtran,bsap1840)
		platform_do_upgrade_redboot_fis "$1" vmlinux_2
		;;
	allnet,all-wap02860ac|\
	araknis,an-300-ap-i-n|\
	araknis,an-500-ap-i-ac|\
	araknis,an-700-ap-i-ac|\
	engenius,eap1200h|\
	engenius,eap1750h|\
	engenius,eap300-v2|\
	engenius,eap600|\
	engenius,ecb600|\
	engenius,ens202ext-v1|\
	engenius,enstationac-v1|\
	engenius,ews660ap|\
	watchguard,ap100|\
	watchguard,ap200|\
	watchguard,ap300)
		ENV_SCRIPT="/tmp/fw_env"
		IMAGE_LIST="tar tzf $1"
		IMAGE_CMD="tar xzOf $1"
		KERNEL_PART="loader"
		ROOTFS_PART="fwconcat0"
		KERNEL_FILE="uImage-lzma.bin"
		ROOTFS_FILE="root.squashfs"
		platform_do_upgrade_failsafe_datachk "$1"
		;;
	fortinet,fap-220-b|\
	fortinet,fap-221-b)
		SKIP_HASH="1"
		ENV_SCRIPT="/dev/null"
		IMAGE_LIST="tar tzf $1"
		IMAGE_CMD="tar xzOf $1"
		KERNEL_PART="loader"
		ROOTFS_PART="fwconcat0"
		KERNEL_FILE="uImage-lzma.bin"
		ROOTFS_FILE="root.squashfs"
		platform_do_upgrade_failsafe_datachk "$1"
		;;
	jjplus,ja76pf2)
		platform_do_upgrade_redboot_fis "$1" linux
		;;
	openmesh,a40|\
	openmesh,a60|\
	openmesh,mr600-v1|\
	openmesh,mr600-v2|\
	openmesh,mr900-v1|\
	openmesh,mr900-v2|\
	openmesh,mr1750-v1|\
	openmesh,mr1750-v2|\
	openmesh,om2p-v1|\
	openmesh,om2p-v2|\
	openmesh,om2p-v4|\
	openmesh,om2p-hs-v1|\
	openmesh,om2p-hs-v2|\
	openmesh,om2p-hs-v3|\
	openmesh,om2p-hs-v4|\
	openmesh,om2p-lc|\
	openmesh,om5p|\
	openmesh,om5p-ac-v1|\
	openmesh,om5p-ac-v2|\
	openmesh,om5p-an)
		PART_NAME="inactive"
		platform_do_upgrade_openmesh "$1"
		;;
	plasmacloud,pa300|\
	plasmacloud,pa300e)
		PART_NAME="inactive"
		platform_do_upgrade_dualboot_datachk "$1"
		;;
	ubnt,routerstation|\
	ubnt,routerstation-pro)
		platform_do_upgrade_redboot_fis "$1" kernel
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
