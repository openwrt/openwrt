#
# Copyright (C) 2010 OpenWrt.org
#

PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='fw_printenv fw_setenv'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

platform_check_image() {
	local board=$(board_name)
	local magic="$(get_magic_long "$1")"

	[ "$#" -gt 1 ] && return 1

	case "$board" in
	buffalo,wsr-2533dhpl2|\
	buffalo,wsr-2533dhpls)
		buffalo_check_image "$board" "$magic" "$1" || return 1
		;;
	esac

	return 0
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	alfa-network,quad-e4g)
		[ "$(fw_printenv -n dual_image 2>/dev/null)" = "1" ] &&\
		[ -n "$(find_mtd_part backup)" ] && {
			PART_NAME=backup
			if [ "$(fw_printenv -n bootactive 2>/dev/null)" = "1" ]; then
				fw_setenv bootactive 2 || exit 1
			else
				fw_setenv bootactive 1 || exit 1
			fi
		}
		;;
	ampedwireless,ally-00x19k|\
	ampedwireless,ally-r1900k)
		if [ "$(fw_printenv --lock / -n bootImage 2>/dev/null)" != "0" ]; then
			fw_setenv --lock / bootImage 0 || exit 1
		fi
		;;
	iptime,ax2004m)
		if [ "$(fw_printenv -n boot_from 2>/dev/null)" != "firmware1" ]; then
			fw_setenv boot_from firmware1 || exit 1
		fi
		;;
	mikrotik,ltap-2hnd|\
	mikrotik,routerboard-750gr3|\
	mikrotik,routerboard-760igs|\
	mikrotik,routerboard-m11g|\
	mikrotik,routerboard-m33g)
		[ "$(rootfs_type)" = "tmpfs" ] && mtd erase firmware
		;;
	asus,rt-ac65p|\
	asus,rt-ac85p)
		echo "Backing up firmware"
		dd if=/dev/mtd4 bs=1024 count=4096  > /tmp/backup_firmware.bin
		dd if=/dev/mtd5 bs=1024 count=52224 >> /tmp/backup_firmware.bin
		mtd -e firmware2 write /tmp/backup_firmware.bin firmware2
		;;
	esac

	case "$board" in
	ampedwireless,ally-00x19k|\
	ampedwireless,ally-r1900k|\
	arcadyan,we420223-99|\
	asus,rt-ac65p|\
	asus,rt-ac85p|\
	asus,rt-ax53u|\
	asus,rt-ax54|\
	asus,4g-ax56|\
	beeline,smartbox-flash|\
	beeline,smartbox-giga|\
	beeline,smartbox-pro|\
	beeline,smartbox-turbo|\
	beeline,smartbox-turbo-plus|\
	belkin,rt1800|\
	dlink,covr-x1860-a1|\
	dlink,dap-x1860-a1|\
	dlink,dir-1960-a1|\
        dlink,dir-2055-a1|\
	dlink,dir-2150-a1|\
	dlink,dir-2150-r1|\
	dlink,dir-2640-a1|\
	dlink,dir-2660-a1|\
	dlink,dir-3040-a1|\
	dlink,dir-3060-a1|\
	dlink,dir-853-a3|\
	elecom,wmc-x1800gst|\
	elecom,wsc-x1800gs|\
	etisalat,s3|\
	h3c,tx1800-plus|\
	h3c,tx1801-plus|\
	h3c,tx1806|\
	haier,har-20s2u1|\
	hiwifi,hc5962|\
	gemtek,wvrtm-127acn|\
	gemtek,wvrtm-130acn|\
	iptime,a3004t|\
	iptime,ax2004m|\
	iptime,t5004|\
	jcg,q20|\
	keenetic,kn-3510|\
	linksys,e5600|\
	linksys,e7350|\
	linksys,ea6350-v4|\
	linksys,ea7300-v1|\
	linksys,ea7300-v2|\
	linksys,ea7500-v2|\
	linksys,ea8100-v1|\
	linksys,ea8100-v2|\
	mts,wg430223|\
	netgear,eax12|\
	netgear,r6220|\
	netgear,r6260|\
	netgear,r6350|\
	netgear,r6700-v2|\
	netgear,r6800|\
	netgear,r6850|\
	netgear,r6900-v2|\
	netgear,r7200|\
	netgear,r7450|\
	netgear,wac104|\
	netgear,wac124|\
	netgear,wax202|\
	netgear,wax214v2|\
	netis,n6|\
	netis,wf2881|\
	raisecom,msg1500-x-00|\
	rostelecom,rt-fe-1a|\
	rostelecom,rt-sf-1|\
	sercomm,na502|\
	sercomm,na502s|\
	sim,simax1800t|\
	tplink,ec330-g5u-v1|\
	wifire,s1500-nbn|\
	xiaomi,mi-router-3g|\
	xiaomi,mi-router-3-pro|\
	xiaomi,mi-router-4|\
	xiaomi,mi-router-ac2100|\
	xiaomi,mi-router-cr6606|\
	xiaomi,mi-router-cr6608|\
	xiaomi,mi-router-cr6609|\
	xiaomi,redmi-router-ac2100|\
	z-router,zr-2660|\
	zyxel,nwa50ax|\
	zyxel,nwa55axe)
		nand_do_upgrade "$1"
		;;
	buffalo,wsr-2533dhpl2|\
	buffalo,wsr-2533dhpls)
		buffalo_do_upgrade "$1"
		;;
	dna,valokuitu-plus-ex400|\
	genexis,pulse-ex400)
		inteno_do_upgrade "$1"
		;;
	elecom,wrc-x1800gs)
		[ "$(fw_printenv -n bootmenu_delay)" != "0" ] || \
			fw_setenv bootmenu_delay 3
		iodata_mstc_set_flag "bootnum" "persist" "0x4" "1,2" "1"
		nand_do_upgrade "$1"
		;;
	iodata,wn-ax1167gr2|\
	iodata,wn-ax2033gr|\
	iodata,wn-dx1167r|\
	iodata,wn-dx2033gr)
		iodata_mstc_set_flag "debugflag" "factory" "0xfe75" "0,1" "1"
		iodata_mstc_set_flag "bootnum" "persist" "0x4" "1,2" "1"
		nand_do_upgrade "$1"
		;;
	iodata,wn-deax1800gr)
		iodata_mstc_set_flag "bootnum" "working" "0x4" "0,1" "0"
		nand_do_upgrade "$1"
		;;
	iodata,wn-dx1200gr)
		iodata_mstc_set_flag "debugflag" "factory" "0x1fe75" "0,1" "1"
		iodata_mstc_set_flag "bootnum" "persist" "0x4" "1,2" "1"
		nand_do_upgrade "$1"
		;;
	tplink,er605-v2)
		echo "Upgrading tplink,er605-v2"
		CI_UBIPART="firmware"
		CI_KERNPART="kernel"
		nand_do_upgrade "$1"
		;;
	ubnt,edgerouter-x|\
	ubnt,edgerouter-x-sfp)
		platform_upgrade_ubnt_erx "$1"
		;;
	zyxel,lte3301-plus|\
	zyxel,lte5398-m904|\
	zyxel,lte7490-m904|\
	zyxel,nr7101)
		fw_setenv CheckBypass 0
		fw_setenv Image1Stable 0
		[ "$(fw_printenv -n BootingFlag)" = "0" ] || fw_setenv BootingFlag 0
		CI_KERNPART="Kernel"
		nand_do_upgrade "$1"
		;;
	zyxel,wap6805)
		local kernel2_mtd="$(find_mtd_part Kernel2)"
		[ "$(hexdump -n 4 -e '"%x"' $kernel2_mtd)" = "56190527" ] &&\
		[ "$(hexdump -n 4 -s 104 -e '"%x"' $kernel2_mtd)" != "0" ] &&\
		dd bs=4 count=1 seek=26 conv=notrunc if=/dev/zero of=$kernel2_mtd 2>/dev/null &&\
		echo "Kernel2 sequence number was reset to 0"
		CI_KERNPART="Kernel"
		nand_do_upgrade "$1"
		;;
	zyxel,wsm20)
		zyxel_mstc_upgrade_prepare
		nand_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
