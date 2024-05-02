PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='fw_printenv fw_setenv head'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

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
	netgear,wax214)
		nand_do_upgrade "$1"
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
