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
	dlink,aquila-pro-ai-m60-a1)
		# multi-pack units (M60-2/M60-3) carry a '-' (0x2d) at offset 0x4d
		# and store MAC at offset 0x83
		odm=$(find_mtd_part "Odm")
		if [ "$(hexdump -n 1 -v -s 0x4d -e '1 "%02x"' "$odm")" = "2d" ]; then
			wan_mac=$(get_mac_binary "$odm" 0x83)
		else
			wan_mac=$(get_mac_binary "$odm" 0x81)
		fi
		ip link set dev lan1 address "$(macaddr_add "$wan_mac" 1)"
		ip link set dev lan2 address "$(macaddr_add "$wan_mac" 1)"
		ip link set dev lan3 address "$(macaddr_add "$wan_mac" 1)"
		ip link set dev lan4 address "$(macaddr_add "$wan_mac" 1)"
		ip link set dev internet address "$wan_mac"
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
