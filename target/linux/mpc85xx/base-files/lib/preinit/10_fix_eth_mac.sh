. /lib/functions.sh
. /lib/functions/system.sh

preinit_set_mac_address() {
	case $(board_name) in
	adtran,bsap-1930)
		base_mac=$(mtd_get_mac_binary SENA0 0x18)
		ip link set dev eth0 address $base_mac
		ip link set dev eth1 address $(macaddr_add "$base_mac" 0x1f)
		;;
	enterasys,ws-ap3715i|\
	extreme-networks,ws-ap3825i)
		ip link set dev eth0 address $(mtd_get_mac_ascii cfg1 ethaddr)
		ip link set dev eth1 address $(mtd_get_mac_ascii cfg1 eth1addr)
		;;
	watchguard,firebox-t10)
		ip link set dev eth0 address "$(mtd_get_mac_text "device_id" 0x1830)"
		ip link set dev eth1 address "$(mtd_get_mac_text "device_id" 0x1844)"
		ip link set dev eth2 address "$(mtd_get_mac_text "device_id" 0x1858)"
		;;
	esac
}

boot_hook_add preinit_main preinit_set_mac_address
