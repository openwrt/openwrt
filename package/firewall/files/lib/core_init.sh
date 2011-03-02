# Copyright (C) 2009-2010 OpenWrt.org
# Copyright (C) 2008 John Crispin <blogic@openwrt.org>

FW_INITIALIZED=

FW_ZONES=
FW_ZONES4=
FW_ZONES6=
FW_CONNTRACK_ZONES=
FW_NOTRACK_DISABLED=

FW_DEFAULTS_APPLIED=
FW_ADD_CUSTOM_CHAINS=
FW_ACCEPT_REDIRECTS=
FW_ACCEPT_SRC_ROUTE=

FW_DEFAULT_INPUT_POLICY=REJECT
FW_DEFAULT_OUTPUT_POLICY=REJECT
FW_DEFAULT_FORWARD_POLICY=REJECT

FW_DISABLE_IPV4=0
FW_DISABLE_IPV6=0


fw_load_defaults() {
	fw_config_get_section "$1" defaults { \
		string input $FW_DEFAULT_INPUT_POLICY \
		string output $FW_DEFAULT_OUTPUT_POLICY \
		string forward $FW_DEFAULT_FORWARD_POLICY \
		boolean drop_invalid 0 \
		boolean syn_flood 0 \
		boolean synflood_protect 0 \
		string synflood_rate 25 \
		string synflood_burst 50 \
		boolean tcp_syncookies 1 \
		boolean tcp_ecn 0 \
		boolean tcp_westwood 0 \
		boolean tcp_window_scaling 1 \
		boolean accept_redirects 0 \
		boolean accept_source_route 0 \
		boolean custom_chains 1 \
		boolean disable_ipv6 0 \
	} || return
	[ -n "$FW_DEFAULTS_APPLIED" ] && {
		fw_log error "duplicate defaults section detected, skipping"
		return 1
	}
	FW_DEFAULTS_APPLIED=1

	FW_DEFAULT_INPUT_POLICY=$defaults_input
	FW_DEFAULT_OUTPUT_POLICY=$defaults_output
	FW_DEFAULT_FORWARD_POLICY=$defaults_forward

	FW_ADD_CUSTOM_CHAINS=$defaults_custom_chains

	FW_ACCEPT_REDIRECTS=$defaults_accept_redirects
	FW_ACCEPT_SRC_ROUTE=$defaults_accept_source_route

	FW_DISABLE_IPV6=$defaults_disable_ipv6

	fw_callback pre defaults

	# Seems like there are only one sysctl for both IP versions.
	for s in syncookies ecn westwood window_scaling; do
		eval "sysctl -e -w net.ipv4.tcp_${s}=\$defaults_tcp_${s}" >/dev/null
	done
	fw_sysctl_interface all

	[ $defaults_drop_invalid == 1 ] && {
		fw add i f INPUT   DROP { -m state --state INVALID }
		fw add i f OUTPUT  DROP { -m state --state INVALID }
		fw add i f FORWARD DROP { -m state --state INVALID }
		FW_NOTRACK_DISABLED=1
	}

	fw add i f INPUT   ACCEPT { -m state --state RELATED,ESTABLISHED }
	fw add i f OUTPUT  ACCEPT { -m state --state RELATED,ESTABLISHED }
	fw add i f FORWARD ACCEPT { -m state --state RELATED,ESTABLISHED }

	fw add i f INPUT  ACCEPT { -i lo }
	fw add i f OUTPUT ACCEPT { -o lo }

	# Compatibility to old 'syn_flood' parameter
	[ $defaults_syn_flood == 1 ] && \
		defaults_synflood_protect=1

	[ "${defaults_synflood_rate%/*}" == "$defaults_synflood_rate" ] && \
		defaults_synflood_rate="$defaults_synflood_rate/second"

	[ $defaults_synflood_protect == 1 ] && {
		echo "Loading synflood protection"
		fw_callback pre synflood
		fw add i f syn_flood
		fw add i f syn_flood RETURN { \
			-p tcp --syn \
			-m limit --limit "${defaults_synflood_rate}" --limit-burst "${defaults_synflood_burst}" \
		}
		fw add i f syn_flood DROP
		fw add i f INPUT syn_flood { -p tcp --syn }
		fw_callback post synflood
	}

	[ $defaults_custom_chains == 1 ] && {
		echo "Adding custom chains"
		fw add i f input_rule
		fw add i f output_rule
		fw add i f forwarding_rule
		fw add i n prerouting_rule
		fw add i n postrouting_rule

		fw add i f INPUT       input_rule
		fw add i f OUTPUT      output_rule
		fw add i f FORWARD     forwarding_rule
		fw add i n PREROUTING  prerouting_rule
		fw add i n POSTROUTING postrouting_rule
	}

	fw add i f input
	fw add i f output
	fw add i f forward

	fw add i f INPUT   input
	fw add i f OUTPUT  output
	fw add i f FORWARD forward

	fw add i f reject
	fw add i f reject REJECT { --reject-with tcp-reset -p tcp }
	fw add i f reject REJECT { --reject-with port-unreach }

	fw_set_filter_policy

	fw_callback post defaults
}


fw_config_get_zone() {
	[ "${zone_NAME}" != "$1" ] || return
	fw_config_get_section "$1" zone { \
		string name "$1" \
		string network "" \
		string input "$FW_DEFAULT_INPUT_POLICY" \
		string output "$FW_DEFAULT_OUTPUT_POLICY" \
		string forward "$FW_DEFAULT_FORWARD_POLICY" \
		boolean masq 0 \
		string masq_src "" \
		string masq_dest "" \
		boolean conntrack 0 \
		boolean mtu_fix 0 \
		boolean custom_chains "$FW_ADD_CUSTOM_CHAINS" \
		boolean log 0 \
		string log_limit 10 \
		string family "" \
	} || return
	[ -n "$zone_name" ] || zone_name=$zone_NAME
	[ -n "$zone_network" ] || zone_network=$zone_name
}

fw_load_zone() {
	fw_config_get_zone "$1"

	list_contains FW_ZONES $zone_name && {
		fw_log error "zone ${zone_name}: duplicated zone, skipping"
		return 0
	}
	append FW_ZONES $zone_name

	fw_callback pre zone

	[ $zone_conntrack = 1 -o $zone_masq = 1 ] && \
		append FW_CONNTRACK_ZONES "$zone_name"

	local mode
	case "$zone_family" in
		*4)
			mode=4
			append FW_ZONES4 $zone_name
			uci_set_state firewall core ${zone_name}_ipv4 1
		;;
		*6)
			mode=6
			append FW_ZONES6 $zone_name
			uci_set_state firewall core ${zone_name}_ipv6 1
		;;
		*)
			mode=i
			append FW_ZONES4 $zone_name
			append FW_ZONES6 $zone_name
			uci_set_state firewall core ${zone_name}_ipv4 1
			uci_set_state firewall core ${zone_name}_ipv6 1
		;;
	esac

	local chain=zone_${zone_name}

	fw add $mode f ${chain}_ACCEPT
	fw add $mode f ${chain}_DROP
	fw add $mode f ${chain}_REJECT
	fw add $mode f ${chain}_MSSFIX

	# TODO: Rename to ${chain}_input
	fw add $mode f ${chain}
	fw add $mode f ${chain} ${chain}_${zone_input} $

	fw add $mode f ${chain}_forward
	fw add $mode f ${chain}_forward ${chain}_${zone_forward} $

	# TODO: add ${chain}_output
	fw add $mode f output ${chain}_${zone_output} $

	# TODO: Rename to ${chain}_MASQUERADE
	fw add $mode n ${chain}_nat
	fw add $mode n ${chain}_prerouting

	fw add $mode r ${chain}_notrack

	[ $zone_mtu_fix == 1 ] && \
		fw add $mode f FORWARD ${chain}_MSSFIX ^

	[ $zone_custom_chains == 1 ] && {
		[ $FW_ADD_CUSTOM_CHAINS == 1 ] || \
			fw_die "zone ${zone_name}: custom_chains globally disabled"

		fw add $mode f input_${zone_name}
		fw add $mode f ${chain} input_${zone_name} ^

		fw add $mode f forwarding_${zone_name}
		fw add $mode f ${chain}_forward forwarding_${zone_name} ^

		fw add $mode n prerouting_${zone_name}
		fw add $mode n ${chain}_prerouting prerouting_${zone_name} ^
	}

	[ "$zone_log" == 1 ] && {
		[ "${zone_log_limit%/*}" == "$zone_log_limit" ] && \
			zone_log_limit="$zone_log_limit/minute"

		local t
		for t in REJECT DROP MSSFIX; do
			fw add $mode f ${chain}_${t} LOG ^ \
				{ -m limit --limit $zone_log_limit --log-prefix "$t($zone_name): "  }
		done
	}

	# NB: if MASQUERADING for IPv6 becomes available we'll need a family check here
	if [ "$zone_masq" == 1 ]; then
		local msrc mdst
		for msrc in ${zone_masq_src:-0.0.0.0/0}; do
			fw_get_negation msrc '-s' "$msrc"
			for mdst in ${zone_masq_dest:-0.0.0.0/0}; do
				fw_get_negation mdst '-d' "$mdst"
				fw add $mode n ${chain}_nat MASQUERADE $ { $msrc $mdst }
			done
		done
	fi

	fw_callback post zone
}

fw_load_notrack_zone() {
	fw_config_get_zone "$1"
	list_contains FW_CONNTRACK_ZONES "${zone_name}" && return

	fw_callback pre notrack

	fw add i r zone_${zone_name}_notrack NOTRACK $

	fw_callback post notrack
}


fw_load_include() {
	local name="$1"

	local path
	config_get path ${name} path

	[ -e $path ] && ( . $path )
}


fw_clear() {
	local policy=$1

	fw_set_filter_policy $policy

	local tab
	for tab in f n r; do
		fw del i $tab
	done
}

fw_set_filter_policy() {
	local policy=$1

	local chn tgt
	for chn in INPUT OUTPUT FORWARD; do
		eval "tgt=\${policy:-\${FW_DEFAULT_${chn}_POLICY}}"
		[ $tgt == "REJECT" ] && tgt=reject
		[ $tgt == "ACCEPT" -o $tgt == "DROP" ] || {
			fw add i f $chn $tgt $
			tgt=DROP
		}
		fw policy i f $chn $tgt
	done
}


fw_callback() {
	local pp=$1
	local hk=$2

	local libs lib
	eval "libs=\$FW_CB_${pp}_${hk}"
	[ -n "$libs" ] || return
	for lib in $libs; do
		${lib}_${pp}_${hk}_cb
	done
}
