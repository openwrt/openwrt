scan_ipoa() {
	config_get ifname "$1" ifname
	ipoadev="${ipoadev:-0}"
	config_get unit "$1" unit
	[ -z "$unit" ] && {
		config_set "$1" ifname "atm$ipoadev"
		config_set "$1" unit "$ipoadev"
		ipoadev="$(($ipoadev + 1))"
	}
}

setup_interface_ipoa() {
	local iface="$1"
	local config="$2"
	
	config_get device "$config" device
	config_get vpi "$config" vpi
	vpi=${vpi:-8}
	config_get vci "$config" vci
	vci=${vci:-36}

	config_get encaps "$config" encaps
	case "$encaps" in
		1|vc) ENCAPS="vc-encaps" ;;
		*) ENCAPS="llc-encaps" ;;
	esac

	config_get mtu "$cfg" mtu
	mtu=${mtu:-1500}
	atmarp -c $device
	ifconfig $device $ip netmask $mask mtu $mtu up
	atmarp -s $gw $vpi.$vci null
	route add default gw $gw
}
