. /lib/functions.sh

set_interface_names() {
	case $(board_name) in
	mikrotik,routerboard-960pgs)
		ip link set dev eth1 name sfp
		;;
	esac
}

boot_hook_add preinit_main set_interface_names
