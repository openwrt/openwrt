. /lib/functions/system.sh

preinit_set_mac_address() {
	case $(board_name) in
	acer,predator-w6)
		key_path="/var/qcidata/data"
		ip link set dev lan1 address "$(cat $key_path/LANMAC)"
		ip link set dev lan2 address "$(cat $key_path/LANMAC)"
		ip link set dev lan3 address "$(cat $key_path/LANMAC)"
		ip link set dev game address "$(cat $key_path/LANMAC)"
		ip link set dev eth1 address "$(cat $key_path/WANMAC)"
		;;
	asus,tuf-ax4200|\
	asus,tuf-ax6000)
		CI_UBIPART="UBI_DEV"
		addr=$(mtd_get_mac_binary_ubi "Factory" 0x4)
		ip link set dev eth0 address "$addr"
		ip link set dev eth1 address "$addr"
		;;
	mercusys,mr90x-v1)
		addr=$(get_mac_binary "/tmp/tp_data/default-mac" 0)
		ip link set dev eth1 address "$(macaddr_add $addr 1)"
		;;
	*)
		;;
	esac
}

boot_hook_add preinit_main preinit_set_mac_address
