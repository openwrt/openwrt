. /lib/functions.sh
. /lib/functions/system.sh

preinit_set_mac_address() {
	case $(board_name) in
	lantiq,easy80920-nand|\
	lantiq,easy80920-nor|\
	zyxel,p-2812hnu-f1|\
	zyxel,p-2812hnu-f3)
		lan_mac=$(mtd_get_mac_ascii u-boot-env ethaddr)
		[ -n "$lan_mac" ] || return

		# Set eth0 (DSA master) MAC
		ip link set dev eth0 address "$lan_mac"

		# Set wan port MAC (lan_mac + 1)
		wan_mac=$(macaddr_add "$lan_mac" 1)
		[ -n "$wan_mac" ] && ip link set dev wan address "$wan_mac"
		;;
	esac
}

boot_hook_add preinit_main preinit_set_mac_address
