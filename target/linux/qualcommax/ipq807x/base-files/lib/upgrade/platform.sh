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

tplink_get_boot_part() {
	local cur_boot_part
	local args

	# Try to find rootfs from kernel arguments
	read -r args < /proc/cmdline
	for arg in $args; do
		local ubi_mtd_arg=${arg#ubi.mtd=}
		case "$ubi_mtd_arg" in
		rootfs|rootfs_1)
			echo "$ubi_mtd_arg"
			return
		;;
		esac
	done

	# Fallback to u-boot env (e.g. when running initramfs)
	cur_boot_part="$(/usr/sbin/fw_printenv -n tp_boot_idx)"
	case $cur_boot_part in
	1)
		echo rootfs_1
		;;
	0|*)
		echo rootfs
		;;
	esac
}

tplink_do_upgrade() {
	local new_boot_part

	case $(tplink_get_boot_part) in
	rootfs)
		CI_UBIPART="rootfs_1"
		new_boot_part=1
	;;
	rootfs_1)
		CI_UBIPART="rootfs"
		new_boot_part=0
	;;
	esac

	fw_setenv -s - <<-EOF
		tp_boot_idx $new_boot_part
	EOF

	remove_oem_ubi_volume ubi_rootfs
	nand_do_upgrade "$1"
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
	aliyun,ap8220)
		active="$(fw_printenv -n active)"
		if [ "$active" -eq "1" ]; then
			CI_UBIPART="rootfs1"
		else
			CI_UBIPART="rootfs2"
		fi
		nand_do_upgrade "$1"
		;;
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
	linksys,homewrk)
		CI_UBIPART="rootfs"
		remove_oem_ubi_volume ubi_rootfs
		nand_do_upgrade "$1"
		;;
	linksys,mx4200v1|\
	linksys,mx4200v2|\
	linksys,mx4300)
		linksys_mx_pre_upgrade "$1"
		remove_oem_ubi_volume squashfs
		nand_do_upgrade "$1"
		;;
	linksys,mx5300|\
	linksys,mx8500)
		linksys_mx_pre_upgrade "$1"
		remove_oem_ubi_volume ubifs
		nand_do_upgrade "$1"
		;;
	prpl,haze|\
	qnap,301w)
		CI_KERNPART="0:HLOS"
		CI_ROOTPART="rootfs"
		emmc_do_upgrade "$1"
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
	spectrum,sax1v1k)
		CI_KERNPART="0:HLOS"
		CI_ROOTPART="rootfs"
		CI_DATAPART="rootfs_data"
		emmc_do_upgrade "$1"
		;;
	tplink,eap620hd-v1|\
	tplink,eap660hd-v1)
		tplink_do_upgrade "$1"
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
			CI_KERNPART="0:HLOS"
			CI_ROOTPART="rootfs"
		else
			CI_KERNPART="0:HLOS_1"
			CI_ROOTPART="rootfs_1"
		fi
		emmc_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}

platform_copy_config() {
	case "$(board_name)" in
	prpl,haze|\
	qnap,301w|\
	spectrum,sax1v1k|\
	zyxel,nbg7815)
		emmc_copy_config
		;;
	esac
}
