. /lib/functions.sh

preinit_set_mac_address() {
	case $(board_name) in
	asus,map-ac2200)
		base_mac=$(mtd_get_mac_binary_ubi Factory 0x1006)
		ip link set dev eth0 address $(macaddr_add "$base_mac" 1)
		ip link set dev eth1 address $(macaddr_add "$base_mac" 3)
		;;
	asus,rt-ac42u)
		base_mac=$(mtd_get_mac_binary_ubi Factory 0x1006)
		ip link set dev eth0 address $base_mac
		ip link set dev lan1 address $base_mac
		ip link set dev lan2 address $base_mac
		ip link set dev lan3 address $base_mac
		ip link set dev lan4 address $base_mac
		ip link set dev wan address $(mtd_get_mac_binary_ubi Factory 0x9006)
		;;
	engenius,eap2200)
		base_mac=$(cat /sys/class/net/eth0/address)
		ip link set dev eth1 address $(macaddr_add "$base_mac" 1)
		;;
	extreme-networks,ws-ap3915i)
		ip link set dev eth0 address $(mtd_get_mac_ascii CFG1 ethaddr)
		;;
	linksys,ea8300|\
	linksys,mr8300)
		base_mac=$(mtd_get_mac_ascii devinfo hw_mac_addr)
		ip link set dev lan1 address $(macaddr_add "$base_mac" 1)
		ip link set dev eth0 address $(macaddr_setbit "$base_mac" 7)
		;;
	mikrotik,wap-ac)
		base_mac=$(cat /sys/firmware/mikrotik/hard_config/mac_base)
		ip link set dev sw-eth1 address "$base_mac"
		ip link set dev sw-eth2 address $(macaddr_add "$base_mac" 1)
		;;
	zyxel,nbg6617)
		base_mac=$(cat /sys/class/net/eth0/address)
		ip link set dev eth0 address $(macaddr_add "$base_mac" 2)
		ip link set dev eth1 address $(macaddr_add "$base_mac" 3)
		;;
	esac
}

boot_hook_add preinit_main preinit_set_mac_address
