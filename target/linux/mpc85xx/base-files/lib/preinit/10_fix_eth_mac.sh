. /lib/functions.sh
. /lib/functions/system.sh

preinit_set_mac_address() {
	case $(board_name) in
	enterasys,ws-ap3710i)
		ip link set dev eth0 address $(mtd_get_mac_ascii cfg1 ethaddr)
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
