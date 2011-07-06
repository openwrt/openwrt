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
		string limit "" \
		string limit_burst "" \
		string extra "" \
	} || return
	[ -n "$rule_name" ] || rule_name=$rule__name
}

fw_load_rule() {
	fw_config_get_rule "$1"

	[ "$rule_target" != "NOTRACK" ] || [ -n "$rule_src" ] || [ "$rule_src" != "*" ] || {
		fw_log error "NOTRACK rule ${rule_name}: needs src, skipping"
		return 0
	}

	fw_callback pre rule

	local table=f
	local chain=input
	local target="${rule_target:-REJECT}"
	if [ "$target" == "NOTRACK" ]; then
		table=r
		chain="zone_${rule_src}_notrack"
	else
		if [ -n "$rule_src" ]; then
			if [ "$rule_src" != "*" ]; then
				chain="zone_${rule_src}${rule_dest:+_forward}"
			else
				chain="${rule_dest:+forward}"
				chain="${chain:-input}"
			fi
		fi

		if [ -n "$rule_dest" ]; then
			if [ "$rule_dest" != "*" ]; then
				target="zone_${rule_dest}_${target}"
			elif [ "$target" = REJECT ]; then
				target=reject
			fi
		fi
	fi

	local mode
	fw_get_family_mode mode ${rule_family:-x} $rule_src I

	local src_spec dest_spec
	fw_get_negation src_spec '-s' "${rule_src_ip:+$rule_src_ip/$rule_src_ip_prefixlen}"
	fw_get_negation dest_spec '-d' "${rule_dest_ip:+$rule_dest_ip/$rule_dest_ip_prefixlen}"

	local rule_src_port_copy
	local rule_dest_port_copy

	[ "$rule_proto" == "tcpudp" ] && rule_proto="tcp udp"
	rule_src_port_copy="$rule_src_port"
	rule_dest_port_copy="$rule_dest_port"
	for rule_proto in $rule_proto; do
		rule_src_port="$rule_src_port_copy"
		rule_dest_port="$rule_dest_port_copy"
		fw_get_negation rule_proto '-p' "$rule_proto"
		for rule_src_port in ${rule_src_port:-""}; do
			fw_get_port_range rule_src_port $rule_src_port
			fw_get_negation rule_src_port '--sport' "$rule_src_port"
			for rule_dest_port in ${rule_dest_port:-""}; do
				fw_get_port_range rule_dest_port $rule_dest_port
				fw_get_negation rule_dest_port '--dport' "$rule_dest_port"
				for rule_src_mac in ${rule_src_mac:-""}; do
					fw_get_negation rule_src_mac '--mac-source' "$rule_src_mac"
					for rule_icmp_type in ${rule_icmp_type:-""}; do
						[ "$rule_proto" = "-p icmp" ] || rule_icmp_type=""
						fw add $mode $table $chain $target + \
							{ $rule_src_ip $rule_dest_ip } { \
							$src_spec $dest_spec $rule_proto \
							$rule_src_port $rule_dest_port \
							${rule_src_mac:+-m mac $rule_src_mac} \
							${rule_icmp_type:+--icmp-type $rule_icmp_type} \
							${rule_limit:+-m limit --limit $rule_limit \
								${rule_limit_burst:+--limit-burst $rule_limit_burst}} \
							$rule_extra \
						}
					done
				done
			done
		done
	done

	fw_callback post rule
}
