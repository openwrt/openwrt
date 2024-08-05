PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='fw_printenv fw_setenv head'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

ax880_env_setup() {
	local ubifile=$(board_name)
	local active=$(fw_printenv -n owrt_slotactive)
	[ -z "$active" ] && active=$(hexdump -s 0x94 -n 4 -e '4 "%d"' /dev/mtd$(find_mtd_index 0:bootconfig))
	cat > /tmp/env_tmp << EOF
owrt_slotactive=${active}
owrt_bootcount=0
bootfile=${ubifile}.ubi
owrt_bootcountcheck=if test \$owrt_bootcount > 4; then run owrt_tftprecover; fi; if test \$owrt_bootcount = 3; then run owrt_slotswap; else echo bootcountcheck successfull; fi
owrt_bootinc=if test \$owrt_bootcount < 5; then echo save env part; setexpr owrt_bootcount \${owrt_bootcount} + 1 && saveenv; else echo save env skipped; fi; echo current bootcount: \$owrt_bootcount
bootcmd=run owrt_bootinc && run owrt_bootcountcheck && run owrt_slotselect && run owrt_bootlinux
owrt_bootlinux=echo booting linux... && ubi part fs && ubi read 0x44000000 kernel && bootm; reset
owrt_setslot0=setenv bootargs console=ttyMSM0,115200n8 ubi.mtd=rootfs rootwait && setenv mtdparts mtdparts=nand0:0x3400000@0(fs)
owrt_setslot1=setenv bootargs console=ttyMSM0,115200n8 ubi.mtd=rootfs_1 rootwait && setenv mtdparts mtdparts=nand0:0x3400000@0x3c00000(fs)
owrt_slotswap=setexpr owrt_slotactive 1 - \${owrt_slotactive} && saveenv && echo slot swapped. new active slot: \$owrt_slotactive
owrt_slotselect=setenv mtdids nand0=nand0,nand1=spi0.0; if test \$owrt_slotactive = 0; then run owrt_setslot0; else run owrt_setslot1; fi
owrt_tftprecover=echo trying to recover firmware with tftp... && sleep 10 && dhcp && flash rootfs && flash rootfs_1 && setenv owrt_bootcount 0 && setenv owrt_slotactive 0 && saveenv && reset
owrt_env_ver=7
EOF
	fw_setenv --script /tmp/env_tmp
}

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

asus_initial_setup() {
	# Remove existing linux and jffs2 volumes
	[ "$(rootfs_type)" = "tmpfs" ] || return 0

	ubirmvol /dev/ubi0 -N linux
	ubirmvol /dev/ubi0 -N jffs2
}

platform_check_image() {
	return 0;
}

platform_pre_upgrade() {
	case "$(board_name)" in
	asus,rt-ax89x)
		asus_initial_setup
		;;
	redmi,ax6|\
	xiaomi,ax3600|\
	xiaomi,ax9000)
		xiaomi_initramfs_prepare
		;;
	esac
}

platform_do_upgrade() {
	case "$(board_name)" in
	arcadyan,aw1000|\
	cmcc,rm2-6|\
	compex,wpq873|\
	dynalink,dl-wrx36|\
	edimax,cax1800|\
	netgear,rax120v2|\
	netgear,sxr80|\
	netgear,sxs80|\
	netgear,wax218|\
	netgear,wax620|\
	netgear,wax630)
		nand_do_upgrade "$1"
		;;
	asus,rt-ax89x)
		CI_UBIPART="UBI_DEV"
		CI_KERNPART="linux"
		CI_ROOTPART="jffs2"
		nand_do_upgrade "$1"
		;;
	buffalo,wxr-5950ax12)
		CI_KERN_UBIPART="rootfs"
		CI_ROOT_UBIPART="user_property"
		buffalo_upgrade_prepare
		nand_do_flash_file "$1" || nand_do_upgrade_failed
		nand_do_restore_config || nand_do_upgrade_failed
		buffalo_upgrade_optvol
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
	linksys,mx4200v1|\
	linksys,mx4200v2|\
	linksys,mx5300|\
	linksys,mx8500)
		boot_part="$(fw_printenv -n boot_part)"
		if [ "$boot_part" -eq "1" ]; then
			fw_setenv boot_part 2
			CI_KERNPART="alt_kernel"
			CI_UBIPART="alt_rootfs"
		else
			fw_setenv boot_part 1
			CI_UBIPART="rootfs"
		fi
		fw_setenv boot_part_ready 3
		fw_setenv auto_recovery yes
		nand_do_upgrade "$1"
		;;
	prpl,haze|\
	qnap,301w|\
	spectrum,sax1v1k)
		kernelname="0:HLOS"
		rootfsname="rootfs"
		mmc_do_upgrade "$1"
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
	yuncore,ax880)
		#create env vars if needed
		[ "$(fw_printenv -n owrt_env_ver 2>/dev/null)" != "7" ] && ax880_env_setup
		active="$(fw_printenv -n owrt_slotactive 2>/dev/null)"
		if [ "$active" = "1" ]; then
			CI_UBIPART="rootfs"
		else
			CI_UBIPART="rootfs_1"
		fi
		# reset bootcount
		fw_setenv owrt_bootcount 0
		#switch active fw slot
		fw_setenv owrt_slotactive $((1 - active))
		nand_do_upgrade "$1"
		;;
	zbtlink,zbt-z800ax)
		local mtdnum="$(find_mtd_index 0:bootconfig)"
		local alt_mtdnum="$(find_mtd_index 0:bootconfig1)"
		part_num="$(hexdump -e '1/1 "%01x|"' -n 1 -s 168 -C /dev/mtd$mtdnum | cut -f 1 -d "|" | head -n1)"
		# vendor firmware may swap the rootfs partition location, u-boot append: ubi.mtd=rootfs
		# since we use fixed-partitions, need to force boot from the first rootfs partition
		if [ "$part_num" -eq "1" ]; then
			mtd erase /dev/mtd$mtdnum
			mtd erase /dev/mtd$alt_mtdnum
		fi
		nand_do_upgrade "$1"
		;;
	zte,mf269)
		CI_KERN_UBIPART="ubi_kernel"
		CI_ROOT_UBIPART="rootfs"
		nand_do_upgrade "$1"
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
	*)
		default_do_upgrade "$1"
		;;
	esac
}
