. /lib/functions/system.sh

preinit_set_mac_address() {
	case $(board_name) in
	acer,predator-w6|\
	acer,predator-w6d)
		$(mmc_get_mac_ascii u-boot-env WANMAC)
		$(mmc_get_mac_ascii u-boot-env LANMAC)
		ip link set dev lan1 address "$lan_mac"
		ip link set dev lan2 address "$lan_mac"
		ip link set dev lan3 address "$lan_mac"
		ip link set dev game address "$lan_mac"
		ip link set dev eth1 address "$wan_mac"
		;;
	acer,vero-w6m)
		wan_mac=$(mmc_get_mac_ascii u-boot-env WANMAC)
		lan_mac=$(mmc_get_mac_ascii u-boot-env LANMAC)
		ip link set dev lan1 address "$lan_mac"
		ip link set dev lan2 address "$lan_mac"
		ip link set dev lan3 address "$lan_mac"
		ip link set dev internet address "$wan_mac"
		;;
	asus,tuf-ax4200|\
	asus,tuf-ax4200q|\
	asus,tuf-ax6000)
		CI_UBIPART="UBI_DEV"
		addr=$(mtd_get_mac_binary_ubi "Factory" 0x4)
		ip link set dev eth0 address "$addr"
		ip link set dev eth1 address "$addr"
		;;
	dlink,aquila-pro-ai-m30-a1|\
	dlink,aquila-pro-ai-m60-a1)
		part=$(find_mtd_part "Odm")
		# locate the MAC record header, the MAC follows it
		hex=$(hexdump -ve '1/1 "%.2x"' -n 256 "$part")
		pos=$(echo "$hex" | awk -v RS='304200800600' '{print length($0); exit}')
		[ -n "$pos" ] && [ "$pos" -lt "${#hex}" ] || return
		offset=$((pos / 2 + 6))
		wan_mac=$(mtd_get_mac_binary "Odm" $offset)
		lan_mac=$(macaddr_add $wan_mac 1)
		ip link set dev internet address "$wan_mac"
		ip link set eth0 down
		ip link set dev eth0 address "$lan_mac"
		ip link set eth0 up
		ip link set dev lan1 address "$lan_mac"
		ip link set dev lan2 address "$lan_mac"
		ip link set dev lan3 address "$lan_mac"
		ip link set dev lan4 address "$lan_mac"
		;;
	mercusys,mr90x-v1|\
	tplink,archer-ax80-v1|\
	tplink,archer-ax80-v1-eu|\
	tplink,re6000xd)
		addr=$(get_mac_binary "/tmp/tp_data/default-mac" 0)
		ip link set dev eth1 address "$(macaddr_add $addr 1)"
		;;
	tplink,be450)
		addr=$(get_mac_binary "/tmp/tp_data/default-mac" 0)
		ip link set dev lan1 address "$addr"
		ip link set dev lan2 address "$addr"
		ip link set dev lan3 address "$addr"
		ip link set dev eth1 address "$(macaddr_add $addr 1)"
		ip link set dev eth2 address "$(macaddr_add $addr 2)"
		;;
	tplink,fr365-v1)
		lan_mac=$(strings /dev/mtd11 | grep 'option macaddr' | awk -F"'" '{print $2}')
		wan_mac="$(macaddr_add $lan_mac 1)"
		ip link set dev port2 address "$wan_mac"
		ip link set dev port1 address "$lan_mac"
		ip link set dev port3 address "$lan_mac"
		ip link set dev port4 address "$lan_mac"
		ip link set dev port5 address "$lan_mac"
		ip link set dev port6 address "$lan_mac"
		;;
	*)
		;;
	esac
}

boot_hook_add preinit_main preinit_set_mac_address
