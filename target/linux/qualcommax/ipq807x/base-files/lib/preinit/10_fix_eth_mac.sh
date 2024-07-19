. /lib/functions/system.sh

preinit_set_mac_address() {
	case $(board_name) in
	tplink,x80-5g)
		addr=$(get_mac_binary "/tmp/factory_data/default-mac" 0)
		ip link set dev lan address "$(macaddr_add $addr 1)"
		;;
	*)
		;;
	esac
}

boot_hook_add preinit_main preinit_set_mac_address
