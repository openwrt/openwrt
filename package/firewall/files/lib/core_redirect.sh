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
		string dest_port "" \
		string proto "tcpudp" \
		string family "" \
		string target "DNAT" \
		string extra "" \
	} || return
	[ -n "$redirect_name" ] || redirect_name=$redirect__name
}

fw_load_redirect() {
	fw_config_get_redirect "$1"

	fw_callback pre redirect

	local fwdchain natchain natopt nataddr natports srcdaddr srcdports
	if [ "$redirect_target" == "DNAT" ]; then
		[ -n "${redirect_src#*}" -a -n "$redirect_dest_ip$redirect_dest_port" ] || {
			fw_log error "DNAT redirect ${redirect_name}: needs src and dest_ip or dest_port, skipping"
			return 0
		}

		fwdchain="zone_${redirect_src}${redirect_dest_ip:+_forward}"

		natopt="--to-destination"
		natchain="zone_${redirect_src}_prerouting"
		nataddr="$redirect_dest_ip"
		fw_get_port_range natports "${redirect_dest_port#!}" "-"

		fw_get_negation srcdaddr '-d' "${redirect_src_dip:+$redirect_src_dip/$redirect_src_dip_prefixlen}"
		fw_get_port_range srcdports "$redirect_src_dport" ":"
		fw_get_negation srcdports '--dport' "$srcdports"

		list_contains FW_CONNTRACK_ZONES $redirect_src || \
			append FW_CONNTRACK_ZONES $redirect_src

	elif [ "$redirect_target" == "SNAT" ]; then
		[ -n "${redirect_dest#*}" -a -n "$redirect_src_dip" ] || {
			fw_log error "SNAT redirect ${redirect_name}: needs dest and src_dip, skipping"
			return 0
		}

		fwdchain="${redirect_src:+zone_${redirect_src}_forward}"

		natopt="--to-source"
		natchain="zone_${redirect_dest}_nat"
		nataddr="$redirect_src_dip"
		fw_get_port_range natports "${redirect_src_dport#!}" "-"

		fw_get_negation srcdaddr '-d' "${redirect_dest_ip:+$redirect_dest_ip/$redirect_dest_ip_prefixlen}"
		fw_get_port_range srcdports "$redirect_dest_port" ":"
		fw_get_negation srcdports '--dport' "$srcdports"

		list_contains FW_CONNTRACK_ZONES $redirect_dest || \
			append FW_CONNTRACK_ZONES $redirect_dest

	else
		fw_log error "redirect ${redirect_name}: target must be either DNAT or SNAT, skipping"
		return 0
	fi

	local mode
	fw_get_family_mode mode ${redirect_family:-x} ${redirect_src:-$redirect_dest} I

	local srcaddr
	fw_get_negation srcaddr '-s' "${redirect_src_ip:+$redirect_src_ip/$redirect_src_ip_prefixlen}"

	local srcports
	fw_get_port_range srcports "$redirect_src_port" ":"
	fw_get_negation srcports '--sport' "$srcports"

	local destaddr
	fw_get_negation destaddr '-d' "${redirect_dest_ip:+$redirect_dest_ip/$redirect_dest_ip_prefixlen}"

	local destports
	fw_get_port_range destports "${redirect_dest_port:-$redirect_src_dport}" ":"
	fw_get_negation destports '--dport' "$destports"

	[ "$redirect_proto" == "tcpudp" ] && redirect_proto="tcp udp"
	for redirect_proto in $redirect_proto; do
		fw_get_negation redirect_proto '-p' "$redirect_proto"
		for redirect_src_mac in ${redirect_src_mac:-""}; do
			fw_get_negation redirect_src_mac '--mac-source' "$redirect_src_mac"
			fw add $mode n $natchain $redirect_target + \
				{ $redirect_src_ip $redirect_dest_ip } { \
				$srcaddr $srcdaddr $redirect_proto \
				$srcports $srcdports \
				${redirect_src_mac:+-m mac $redirect_src_mac} \
				$natopt $nataddr${natports:+:$natports} \
				$redirect_options \
			}

			fw add $mode f ${fwdchain:-forward} ACCEPT + \
				{ $redirect_src_ip $redirect_dest_ip } { \
				$srcaddr ${destaddr:--m conntrack --ctstate DNAT} \
				$redirect_proto \
				$srcports $destports \
				$redirect_src_mac \
				$redirect_extra \
			}
		done
	done

	fw_callback post redirect
}
