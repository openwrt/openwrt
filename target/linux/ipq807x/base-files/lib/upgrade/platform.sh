PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='fw_printenv fw_setenv head'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

xiaomi_initramfs_prepare() {
	# Wipe UBI if running initramfs
	[ "$(rootfs_type)" = "tmpfs" ] || return 0

	local rootfs_mtdnum="$( find_mtd_index rootfs )"
	if [ ! "$rootfs_mtdnum" ]; then
		echo "unable to find mtd partition rootfs"
		return 1
	fi

	local kern_mtdnum="$( find_mtd_index ubi_kernel )"
	if [ ! "$kern_mtdnum" ]; then
		echo "unable to find mtd partition ubi_kernel"
		return 1
	fi

	ubidetach -m "$rootfs_mtdnum"
	ubiformat /dev/mtd$rootfs_mtdnum -y

	ubidetach -m "$kern_mtdnum"
	ubiformat /dev/mtd$kern_mtdnum -y
}

platform_check_image() {
	return 0;
}

platform_pre_upgrade() {
	case "$(board_name)" in
	redmi,ax6|\
	xiaomi,ax3600|\
	xiaomi,ax9000)
		xiaomi_initramfs_prepare
		;;
	esac
}

platform_do_upgrade() {
	case "$(board_name)" in
	buffalo,wxr-5950ax12)
		CI_KERN_UBIPART="rootfs"
		CI_ROOT_UBIPART="user_property"
		buffalo_upgrade_prepare
		nand_do_flash_file "$1" || nand_do_upgrade_failed
		nand_do_restore_config || nand_do_upgrade_failed
		buffalo_upgrade_optvol
		;;
	dynalink,dl-wrx36)
		nand_do_upgrade "$1"
		;;
	edgecore,eap102)
		active="$(fw_printenv -n active)"
		if [ "$active" -eq "1" ]; then
			CI_UBIPART="rootfs2"
		else
			CI_UBIPART="rootfs1"
		fi
		# force altbootcmd which handles partition change in u-boot
		fw_setenv bootcount 3
		fw_setenv upgrade_available 1
		nand_do_upgrade "$1"
		;;
	edimax,cax1800|\
	netgear,wax218)
		nand_do_upgrade "$1"
		;;
	prpl,haze|\
	qnap,301w)
		kernelname="0:HLOS"
		rootfsname="rootfs"
		mmc_do_upgrade "$1"
		;;
	zyxel,nbg7815)
		local config_mtdnum="$(find_mtd_index 0:bootconfig)"
		[ -z "$config_mtdnum" ] && reboot
		part_num="$(hexdump -e '1/1 "%01x|"' -n 1 -s 168 -C /dev/mtd$config_mtdnum | cut -f 1 -d "|" | head -n1)"
		if [ "$part_num" -eq "0" ]; then
			kernelname="0:HLOS"
			rootfsname="rootfs"
			mmc_do_upgrade "$1"
		else
			kernelname="0:HLOS_1"
			rootfsname="rootfs_1"
			mmc_do_upgrade "$1"
		fi
		;;
	redmi,ax6|\
	xiaomi,ax3600|\
	xiaomi,ax9000)
		# Make sure that UART is enabled
		fw_setenv boot_wait on
		fw_setenv uart_en 1

		# Enforce single partition.
		fw_setenv flag_boot_rootfs 0
		fw_setenv flag_last_success 0
		fw_setenv flag_boot_success 1
		fw_setenv flag_try_sys1_failed 8
		fw_setenv flag_try_sys2_failed 8

		# Kernel and rootfs are placed in 2 different UBI
		CI_KERN_UBIPART="ubi_kernel"
		CI_ROOT_UBIPART="rootfs"
		nand_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
