# Copyright (C) 2009-2010 OpenWrt.org

fw_config_get_rule() {
	[ "${rule_NAME}" != "$1" ] || return
	fw_config_get_section "$1" rule { \
		string _name "$1" \
		string name "" \
		string src "" \
		ipaddr src_ip "" \
		string src_mac "" \
		string src_port "" \
		string dest "" \
		ipaddr dest_ip "" \
		string dest_port "" \
		string icmp_type "" \
		string proto "tcpudp" \
		string target "" \
		string family "" \
	} || return
	[ -n "$rule_name" ] || rule_name=$rule__name
	[ "$rule_proto" == "icmp" ] || rule_icmp_type=
}

fw_load_rule() {
	fw_config_get_rule "$1"

	[ "$rule_target" != "NOTRACK" ] || [ -n "$rule_src" ] || {
		fw_log error "NOTRACK rule ${rule_name}: needs src, skipping"
		return 0
	}

	fw_callback pre rule

	fw_get_port_range rule_src_port $rule_src_port
	fw_get_port_range rule_dest_port $rule_dest_port

	local table=f
	local chain=input
	local target="${rule_target:-REJECT}"
	if [ "$target" == "NOTRACK" ]; then
		table=r
		chain="zone_${rule_src}_notrack"
	else
		[ -n "$rule_src" ] && chain="zone_${rule_src}${rule_dest:+_forward}"
		[ -n "$rule_dest" ] && target="zone_${rule_dest}_${target}"
	fi

	local mode
	fw_get_family_mode mode ${rule_family:-x} $rule_src I

	local src_spec dest_spec
	fw_get_negation src_spec '-s' "${rule_src_ip:+$rule_src_ip/$rule_src_ip_prefixlen}"
	fw_get_negation dest_spec '-d' "${rule_dest_ip:+$rule_dest_ip/$rule_dest_ip_prefixlen}"

	[ "$rule_proto" == "tcpudp" ] && rule_proto="tcp udp"
	for rule_proto in $rule_proto; do
		local rule_pos
		eval 'rule_pos=$((++FW__RULE_COUNT_'${mode#G}'_'$chain'))'

		fw add $mode $table $chain $target $rule_pos { $rule_src_ip $rule_dest_ip } { \
			$src_spec $dest_spec \
			${rule_proto:+-p $rule_proto} \
			${rule_src_port:+--sport $rule_src_port} \
			${rule_src_mac:+-m mac --mac-source $rule_src_mac} \
			${rule_dest_port:+--dport $rule_dest_port} \
			${rule_icmp_type:+--icmp-type $rule_icmp_type} \
		}
	done

	fw_callback post rule
}
