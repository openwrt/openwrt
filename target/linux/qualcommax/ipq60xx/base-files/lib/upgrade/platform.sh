PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='fw_printenv fw_setenv head'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

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

platform_check_image() {
	return 0;
}


yuncore_fap650_env_setup() {
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
owrt_setslot0=setenv bootargs console=ttyMSM0,115200n8 ubi.mtd=rootfs root=mtd:rootfs rootfstype=squashfs rootwait swiotlb=1 && setenv mtdparts mtdparts=nand0:0x3c00000@0(fs)
owrt_setslot1=setenv bootargs console=ttyMSM0,115200n8 ubi.mtd=rootfs_1 root=mtd:rootfs rootfstype=squashfs rootwait swiotlb=1 && setenv mtdparts mtdparts=nand0:0x3c00000@0x3c00000(fs)
owrt_slotswap=setexpr owrt_slotactive 1 - \${owrt_slotactive} && saveenv && echo slot swapped. new active slot: \$owrt_slotactive
owrt_slotselect=setenv mtdids nand0=nand0,nand1=spi0.0; if test \$owrt_slotactive = 0; then run owrt_setslot0; else run owrt_setslot1; fi
owrt_tftprecover=echo trying to recover firmware with tftp... && sleep 10 && dhcp && flash rootfs && flash rootfs_1 && setenv owrt_bootcount 0 && setenv owrt_slotactive 0 && saveenv && reset
owrt_env_ver=7
EOF
	fw_setenv --script /tmp/env_tmp
}

platform_do_upgrade() {
	case "$(board_name)" in
	alfa-network,ap120c-ax)
		CI_UBIPART="rootfs_1"
		alfa_bootconfig_rootfs_rotate "0:BOOTCONFIG" "148"
		nand_do_upgrade "$1"
		;;
	cambiumnetworks,xe3-4)
		fw_setenv bootcount 0
		nand_do_upgrade "$1"
		;;
	edgecore,eap101)
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
	glinet,gl-ax1800|\
	glinet,gl-axt1800|\
	netgear,wax214|\
	netgear,wax610|\
	netgear,wax610y|\
	qihoo,360v6)
		nand_do_upgrade "$1"
		;;
	linksys,mr7350|\
	linksys,mr7500)
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
	tplink,eap610-outdoor|\
	tplink,eap623od-hd-v1|\
	tplink,eap625-outdoor-hd-v1)
		tplink_do_upgrade "$1"
		;;
	yuncore,fap650)
		[ "$(fw_printenv -n owrt_env_ver 2>/dev/null)" != "7" ] && yuncore_fap650_env_setup
		local active="$(fw_printenv -n owrt_slotactive 2>/dev/null)"
		if [ "$active" = "1" ]; then
			CI_UBIPART="rootfs"
		else
			CI_UBIPART="rootfs_1"
		fi
		fw_setenv owrt_bootcount 0
		fw_setenv owrt_slotactive $((1 - active))
		nand_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
