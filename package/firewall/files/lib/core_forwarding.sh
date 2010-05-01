# Copyright (C) 2009-2010 OpenWrt.org

fw_config_get_forwarding() {
	[ "${forwarding_NAME}" != "$1" ] || return
	fw_config_get_section "$1" forwarding { \
		string _name "$1" \
		string name "" \
		string src "" \
		string dest "" \
	} || return
	[ -n "$forwarding_name" ] || forwarding_name=$forwarding__name
}

fw_load_forwarding() {
	fw_config_get_forwarding "$1"

	fw_callback pre forwarding

	local chain=forward
	[ -n "$forwarding_src" ] && {
		chain=zone_${forwarding_src}_forward 
	}

	local target=ACCEPT
	[ -n "$forwarding_dest" ] && {
		target=zone_${forwarding_dest}_ACCEPT
	}

	fw add i f $chain $target ^

	# propagate masq zone flag
	[ -n "$forwarding_src" ] && list_contains CONNTRACK_ZONES $forwarding_src && {
		append CONNTRACK_ZONES $forwarding_dest
	}
	[ -n "$forwarding_dest" ] && list_contains CONNTRACK_ZONES $forwarding_dest && {
		append CONNTRACK_ZONES $forwarding_src
	}

	fw_callback post forwarding
}
