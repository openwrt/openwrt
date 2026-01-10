. /lib/functions/bootconfig.sh

PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='dumpimage fw_printenv fw_setenv head seq'
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

remove_oem_ubi_volume() {
	local oem_volume_name="$1"
	local oem_ubivol
	local mtdnum
	local ubidev

	mtdnum=$(find_mtd_index "$CI_UBIPART")
	if [ ! "$mtdnum" ]; then
		return
	fi

	ubidev=$(nand_find_ubi "$CI_UBIPART")
	if [ ! "$ubidev" ]; then
		ubiattach --mtdn="$mtdnum"
		ubidev=$(nand_find_ubi "$CI_UBIPART")
	fi

	if [ "$ubidev" ]; then
		oem_ubivol=$(nand_find_volume "$ubidev" "$oem_volume_name")
		[ "$oem_ubivol" ] && ubirmvol "/dev/$ubidev" --name="$oem_volume_name"
	fi
}

linksys_bootconfig_set_primaryboot() {
	local partname=$1
	local tempfile
	local mtdidx

	mtdidx=$(find_mtd_index "$partname")
	[ ! "$mtdidx" ] && {
		echo "cannot find mtd index for $partname"
		return 1
	}

	# No need to cleanup as files in /tmp will be removed upon reboot
	tempfile=/tmp/mtd"$mtdidx".bin
	dd if=/dev/mtd"$mtdidx" of="$tempfile" bs=1 count=336 2>/dev/null
	[ $? -ne 0 ] || [ ! -f "$tempfile" ]&& {
		echo "failed to create a temp copy of /dev/mtd$mtdidx"
		return 1
	}

	set_bootconfig_primaryboot "$tempfile" "0:HLOS" $2
	[ $? -ne 0 ] && {
		echo "failed to toggle primaryboot on 0:HLOS part"
		return 1
	}
	
	set_bootconfig_primaryboot "$tempfile" "rootfs" $2
	[ $? -ne 0 ] && {
		echo "failed to toggle primaryboot for rootfs part"
		return 1
	}

	mtd write "$tempfile" /dev/mtd"$mtdidx" 2>/dev/null
	[ $? -ne 0 ] && {
		echo "failed to write temp copy back to /dev/mtd$mtdidx"
		return 1
	}
}

linksys_bootconfig_pre_upgrade() {
	local setenv_script="/tmp/fw_env_upgrade"

	CI_UBIPART="rootfs_1"
	boot_part="$(fw_printenv -n boot_part)"
	if [ -n "$UPGRADE_OPT_USE_CURR_PART" ]; then
		CI_UBIPART="rootfs"
	else
		if [ "$boot_part" -eq "1" ]; then
			echo "boot_part 2" >> $setenv_script
			linksys_bootconfig_set_primaryboot "0:bootconfig" 1
			linksys_bootconfig_set_primaryboot "0:bootconfig1" 1
		else
			echo "boot_part 1" >> $setenv_script
			linksys_bootconfig_set_primaryboot "0:bootconfig" 0
			linksys_bootconfig_set_primaryboot "0:bootconfig1" 0
		fi
	fi

	boot_part_ready="$(fw_printenv -n boot_part_ready)"
	if [ "$boot_part_ready" -ne "3" ]; then
		echo "boot_part_ready 3" >> $setenv_script
	fi

	auto_recovery="$(fw_printenv -n auto_recovery)"
	if [ "$auto_recovery" != "yes" ]; then
		echo "auto_recovery yes" >> $setenv_script
	fi

	if [ -f "$setenv_script" ]; then
		fw_setenv -s $setenv_script || {
			echo "failed to update U-Boot environment"
			return 1
		}
	fi
}

linksys_mx_pre_upgrade() {
	local setenv_script="/tmp/fw_env_upgrade"

	CI_UBIPART="rootfs"
	boot_part="$(fw_printenv -n boot_part)"
	if [ -n "$UPGRADE_OPT_USE_CURR_PART" ]; then
		if [ "$boot_part" -eq "2" ]; then
			CI_KERNPART="alt_kernel"
			CI_UBIPART="alt_rootfs"
		fi
	else
		if [ "$boot_part" -eq "1" ]; then
			echo "boot_part 2" >> $setenv_script
			CI_KERNPART="alt_kernel"
			CI_UBIPART="alt_rootfs"
		else
			echo "boot_part 1" >> $setenv_script
		fi
	fi

	boot_part_ready="$(fw_printenv -n boot_part_ready)"
	if [ "$boot_part_ready" -ne "3" ]; then
		echo "boot_part_ready 3" >> $setenv_script
	fi

	auto_recovery="$(fw_printenv -n auto_recovery)"
	if [ "$auto_recovery" != "yes" ]; then
		echo "auto_recovery yes" >> $setenv_script
	fi

	if [ -f "$setenv_script" ]; then
		fw_setenv -s $setenv_script || {
			echo "failed to update U-Boot environment"
			return 1
		}
	fi
}

platform_check_image() {
	return 0;
}

platform_pre_upgrade() {
	case "$(board_name)" in
	xiaomi,ax6000)
		xiaomi_initramfs_prepare
		;;
	esac
}

platform_do_upgrade() {
	case "$(board_name)" in
	cmcc,pz-l8|\
	elecom,wrc-x3000gs2|\
	iodata,wn-dax3000gr)
		local delay

		delay=$(fw_printenv bootdelay)
		[ -z "$delay" ] || [ "$delay" -eq "0" ] && \
			fw_setenv bootdelay 3

		elecom_upgrade_prepare

		remove_oem_ubi_volume bt_fw
		remove_oem_ubi_volume ubi_rootfs
		remove_oem_ubi_volume wifi_fw
		nand_do_upgrade "$1"
		;;
	glinet,gl-b3000)
		glinet_do_upgrade "$1"
		;;
	linksys,mr5500|\
	linksys,mx2000|\
	linksys,mx5500|\
	linksys,spnmx56)
		linksys_mx_pre_upgrade "$1"
		remove_oem_ubi_volume squashfs
		nand_do_upgrade "$1"
		;;
	linksys,mx6200)
		linksys_bootconfig_pre_upgrade "$1"
		remove_oem_ubi_volume ubi_rootfs
		nand_do_upgrade "$1"
		;;
	xiaomi,ax6000)
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
	yuncore,ax830|\
	yuncore,ax850|\
	zyxel,scr50axe)
		CI_UBIPART="rootfs"
		remove_oem_ubi_volume ubi_rootfs
		remove_oem_ubi_volume bt_fw
		remove_oem_ubi_volume wifi_fw
		nand_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
