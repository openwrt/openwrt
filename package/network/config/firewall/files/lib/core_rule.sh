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
	local chain=delegate_output
	local target="${rule_target:-REJECT}"
	if [ "$target" == "NOTRACK" ]; then
		table=r
		chain="zone_${rule_src}_notrack"
	else
		if [ -n "$rule_src" ]; then
			if [ "$rule_src" != "*" ]; then
				if [ -n "$rule_dest" ]; then
					chain="zone_${rule_src}_forward"
				else
					chain="zone_${rule_src}_input"
				fi
			else
				chain="${rule_dest:+delegate_forward}"
				chain="${chain:-delegate_input}"
			fi
		fi

		if [ -n "$rule_dest" ]; then
			if [ "$rule_dest" != "*" ]; then
				target="zone_${rule_dest}_dest_${target}"
				if [ -z "$rule_src" ]; then
					chain="zone_${rule_dest}_output"
				fi
			elif [ "$target" = REJECT ]; then
				target=reject
			fi
		fi
	fi

	local mode
	fw_get_family_mode mode ${rule_family:-x} "$rule_src" I

	local src_spec dest_spec
	fw_get_negation src_spec '-s' "${rule_src_ip:+$rule_src_ip/$rule_src_ip_prefixlen}"
	fw_get_negation dest_spec '-d' "${rule_dest_ip:+$rule_dest_ip/$rule_dest_ip_prefixlen}"

	[ "$rule_proto" == "tcpudp" ] && rule_proto="tcp udp"
	local pr; for pr in $rule_proto; do
		local sports dports itypes
		case "$pr" in
			icmp|icmpv6|1|58)
				sports=""; dports=""
				itypes="$rule_icmp_type"
			;;
			*)
				sports="$rule_src_port"
				dports="$rule_dest_port"
				itypes=""
			;;
		esac
	
		fw_get_negation pr '-p' "$pr"
		local sp; for sp in ${sports:-""}; do
			fw_get_port_range sp $sp
			fw_get_negation sp '--sport' "$sp"
			local dp; for dp in ${dports:-""}; do
				fw_get_port_range dp $dp
				fw_get_negation dp '--dport' "$dp"
				local sm; for sm in ${rule_src_mac:-""}; do
					fw_get_negation sm '--mac-source' "$sm"
					local it; for it in ${itypes:-""}; do
						fw_get_negation it '--icmp-type' "$it"
						fw add $mode $table $chain $target + \
							{ $rule_src_ip $rule_dest_ip } { \
							$src_spec $dest_spec \
							$pr $sp $dp $it \
							${sm:+-m mac $sm} \
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
