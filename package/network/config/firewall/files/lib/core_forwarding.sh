# Copyright (C) 2009-2010 OpenWrt.org

fw_config_get_forwarding() {
	[ "${forwarding_NAME}" != "$1" ] || return
	fw_config_get_section "$1" forwarding { \
		string _name "$1" \
		string name "" \
		string src "" \
		string dest "" \
		string family "" \
	} || return
	[ -n "$forwarding_name" ] || forwarding_name=$forwarding__name
}

fw_load_forwarding() {
	fw_config_get_forwarding "$1"

	fw_callback pre forwarding

	local chain=delegate_forward
	[ -n "$forwarding_src" ] && {
		chain=zone_${forwarding_src}_forward 
	}

	local target=ACCEPT
	[ -n "$forwarding_dest" ] && {
		target=zone_${forwarding_dest}_dest_ACCEPT
	}

	local mode
	fw_get_family_mode mode ${forwarding_family:-x} ${forwarding_dest:-${forwarding_src:--}} i

	fw add $mode f $chain $target ^

	# propagate masq zone flag
	[ -n "$forwarding_src" ] && list_contains FW_CONNTRACK_ZONES $forwarding_src && {
		append FW_CONNTRACK_ZONES $forwarding_dest
	}
	[ -n "$forwarding_dest" ] && list_contains FW_CONNTRACK_ZONES $forwarding_dest && {
		append FW_CONNTRACK_ZONES $forwarding_src
	}

	fw_callback post forwarding
}
