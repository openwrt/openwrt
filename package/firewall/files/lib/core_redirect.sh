# Copyright (C) 2009-2010 OpenWrt.org

fw_config_get_redirect() {
	[ "${redirect_NAME}" != "$1" ] || return
	fw_config_get_section "$1" redirect { \
		string _name "$1" \
		string name "" \
		string src "" \
		ipaddr src_ip "" \
		ipaddr src_dip "" \
		string src_mac "" \
		string src_port "" \
		string src_dport "" \
		string dest "" \
		ipaddr dest_ip "" \
		string dest_mac "" \
		string dest_port "" \
		string proto "tcpudp" \
		string family "" \
		string target "DNAT" \
	} || return
	[ -n "$redirect_name" ] || redirect_name=$redirect__name
}

fw_load_redirect() {
	fw_config_get_redirect "$1"

	fw_callback pre redirect

	[ -n "$redirect_src" -a -n "$redirect_dest_ip$redirect_dest_port" ] || {
		fw_die "redirect ${redirect_name}: needs src and dest_ip or dest_port"
	}

	local chain destopt destaddr
	if [ "$redirect_target" == "DNAT" ]; then
		chain="zone_${redirect_src}_prerouting"
		destopt="--to-destination"
		destaddr="$redirect_dest_ip"
	elif [ "$redirect_target" == "SNAT" ]; then
		chain="zone_${redirect_src}_nat"
		destopt="--to-source"
		destaddr="$redirect_src_dip"
	else
		fw_die "redirect ${redirect_name}: target must be either DNAT or SNAT"
	fi

	list_contains FW_CONNTRACK_ZONES $redirect_src || \
		append FW_CONNTRACK_ZONES $redirect_src

	local mode=$(fw_get_family_mode ${redirect_family:-x} $redirect_src I)

	local nat_dest_port=$redirect_dest_port
	redirect_dest_port=$(fw_get_port_range $redirect_dest_port)
	redirect_src_port=$(fw_get_port_range $redirect_src_port)
	redirect_src_dport=$(fw_get_port_range $redirect_src_dport)
	local fwd_dest_port=${redirect_dest_port:-$redirect_src_dport}

	[ "$redirect_proto" == "tcpudp" ] && redirect_proto="tcp udp"
	for redirect_proto in $redirect_proto; do
		fw add $mode n $chain $redirect_target $ { $redirect_src_ip $redirect_dest_ip } { \
			${redirect_proto:+-p $redirect_proto} \
			${redirect_src_ip:+-s $redirect_src_ip/$redirect_src_ip_prefixlen} \
			${redirect_src_dip:+-d $redirect_src_dip/$redirect_src_dip_prefixlen} \
			${redirect_src_port:+--sport $redirect_src_port} \
			${redirect_src_dport:+--dport $redirect_src_dport} \
			${redirect_src_mac:+-m mac --mac-source $redirect_src_mac} \
			$destopt ${redirect_dest_ip}${redirect_dest_port:+:$nat_dest_port} \
		}

		[ -n "$destaddr" ] && \
		fw add $mode f zone_${redirect_src}_forward ACCEPT ^ { $redirect_src_ip $redirect_dest_ip } { \
			-d $destaddr \
			${redirect_proto:+-p $redirect_proto} \
			${redirect_src_ip:+-s $redirect_src_ip/$redirect_src_ip_prefixlen} \
			${redirect_src_port:+--sport $redirect_src_port} \
			${fwd_dest_port:+--dport $fwd_dest_port} \
			${redirect_src_mac:+-m mac --mac-source $redirect_src_mac} \
		}
	done

	fw_callback post redirect
}
