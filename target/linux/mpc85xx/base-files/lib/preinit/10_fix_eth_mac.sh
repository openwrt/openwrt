. /lib/functions.sh
. /lib/functions/system.sh

preinit_set_mac_address() {
	case $(board_name) in
	extreme-networks,ws-ap3825i)
		ip link set dev eth0 address $(mtd_get_mac_ascii cfg1 ethaddr)
		ip link set dev eth1 address $(mtd_get_mac_ascii cfg1 eth1addr)
		;;
	esac
}

boot_hook_add preinit_main preinit_set_mac_address
