# Copyright (C) 2009-2010 OpenWrt.org

fw_configure_interface() {
	local iface=$1
	local action=$2
	local ifname=$3
	local aliasnet=$4

	[ "$action" == "add" ] && {
		local status=$(uci_get_state network "$iface" up 0)
		[ "$status" == 1 ] || [ -n "$aliasnet" ] || return 0
	}

	[ -n "$ifname" ] || {
		ifname=$(uci_get_state network "$iface" ifname)
		ifname="${ifname%%:*}"
		[ -z "$ifname" ] && return 0
	}

	[ "$ifname" == "lo" ] && return 0

	fw_callback pre interface

	fw__do_rules() {
		local action=$1
		local zone=$2
		local chain=zone_${zone}
		local ifname=$3
		local subnet=$4
		local masq_src=$5
		local masq_dest=$6

		local inet onet
		local mode=$(fw_get_family_mode x $zone i)

		case "$mode/$subnet" in
			# Zone supports v6 only or dual, need v6
			G6/*:*|i/*:*)
				inet="-s $subnet -d ::/0"
				onet="-s ::/0 -d $subnet"
				mode=6
			;;

			# Zone supports v4 only or dual, need v4
			G4/*.*.*.*|i/*.*.*.*)
				inet="-s $subnet -d 0.0.0.0/0"
				onet="-s 0.0.0.0/0 -d $subnet"
				mode=4
			;;

			# Need v6 while zone is v4
			*/*:*) fw_log info "zone $zone does not support IPv6 address family, skipping"; return ;;

			# Need v4 while zone is v6
			*/*.*) fw_log info "zone $zone does not support IPv4 address family, skipping"; return ;;
		esac

		fw $action $mode f ${chain}_ACCEPT ACCEPT $ { -o "$ifname" $onet }
		fw $action $mode f ${chain}_ACCEPT ACCEPT $ { -i "$ifname" $inet }
		fw $action $mode f ${chain}_DROP   DROP   $ { -o "$ifname" $onet }
		fw $action $mode f ${chain}_DROP   DROP   $ { -i "$ifname" $inet }
		fw $action $mode f ${chain}_REJECT reject $ { -o "$ifname" $onet }
		fw $action $mode f ${chain}_REJECT reject $ { -i "$ifname" $inet }

		# NB: if MASQUERADING for IPv6 becomes available we'll need a family check here
		local msrc mdst
		for msrc in ${masq_src:-0.0.0.0/0}; do
			[ "${msrc#!}" != "$msrc" ] && msrc="! -s ${msrc#!}" || msrc="-s $msrc"
			for mdst in ${subnet:-${masq_dest:-0.0.0.0/0}}; do
				[ "${mdst#!}" != "$mdst" ] && mdst="! -d ${mdst#!}" || mdst="-d $mdst"
				fw $action $mode n ${chain}_nat MASQUERADE $ { -o "$ifname" $msrc $mdst }
			done
		done

		fw $action $mode f ${chain}_MSSFIX TCPMSS  $ { -o "$ifname" -p tcp --tcp-flags SYN,RST SYN --clamp-mss-to-pmtu $onet }

		fw $action $mode f input   ${chain}         $ { -i "$ifname" $inet }
		fw $action $mode f forward ${chain}_forward $ { -i "$ifname" $inet }
		fw $action $mode n PREROUTING ${chain}_prerouting $ { -i "$ifname" $inet }
		fw $action $mode r PREROUTING ${chain}_notrack    $ { -i "$ifname" $inet }
	}

	local old_zones old_ifname old_subnets old_masq_src old_masq_dest
	config_get old_zones core "${iface}_zone"
	[ -n "$old_zones" ] && {
		config_get old_ifname core "${iface}_ifname"
		config_get old_subnets core "${iface}_subnets"
		config_get old_masq_src core "${iface}_masq_src"
		config_get old_masq_dest core "${iface}_masq_dest"

		local z
		for z in $old_zones; do
			local n
			for n in ${old_subnets:-""}; do
				fw_log info "removing $iface ($old_ifname${n:+ alias $n}) from zone $z"
				fw__do_rules del $z $old_ifname $n "$old_masq_src" "$old_masq_dest"
			done

			[ -n "$old_subnets" ] || ACTION=remove ZONE="$z" INTERFACE="$iface" DEVICE="$ifname" /sbin/hotplug-call firewall
		done

		local old_aliases
		config_get old_aliases core "${iface}_aliases"

		local a
		for a in $old_aliases; do
			fw_configure_interface "$a" del "$old_ifname"
		done

		uci_revert_state firewall core "${iface}_zone"
		uci_revert_state firewall core "${iface}_ifname"
		uci_revert_state firewall core "${iface}_subnets"
		uci_revert_state firewall core "${iface}_aliases"
		uci_revert_state firewall core "${iface}_masq_src"
		uci_revert_state firewall core "${iface}_masq_dest"
	}

	[ "$action" == del ] && return

	[ -z "$aliasnet" ] && {
		local aliases
		config_get aliases "$iface" aliases

		local a
		for a in $aliases; do
			local ipaddr netmask ip6addr
			config_get ipaddr "$a" ipaddr
			config_get netmask "$a" netmask
			config_get ip6addr "$a" ip6addr

			[ -n "$ipaddr" ] && fw_configure_interface "$a" add "" "$ipaddr${netmask:+/$netmask}"
			[ -n "$ip6addr" ] && fw_configure_interface "$a" add "" "$ip6addr"
		done

		fw_sysctl_interface $ifname
		fw_callback post interface

		uci_set_state firewall core "${iface}_aliases" "$aliases"
	} || {
		local subnets=
		config_get subnets core "${iface}_subnets"
		append subnets "$aliasnet"

		config_set core "${iface}_subnets" "$subnets"
		uci_set_state firewall core "${iface}_subnets" "$subnets"
	}

	local new_zones=
	local new_masq_src=
	local new_masq_dest=
	load_zone() {
		fw_config_get_zone "$1"
		list_contains zone_network "$iface" || return

		fw_log info "adding $iface ($ifname${aliasnet:+ alias $aliasnet}) to zone $zone_name"
		fw__do_rules add ${zone_name} "$ifname" "$aliasnet" "$zone_masq_src" "$zone_masq_dest"
		append new_zones $zone_name
		append new_masq_src "$zone_masq_src"
		append new_masq_dest "$zone_masq_dest"

		[ -n "$aliasnet" ] || ACTION=add ZONE="$zone_name" INTERFACE="$iface" DEVICE="$ifname" /sbin/hotplug-call firewall
	}
	config_foreach load_zone zone

	uci_set_state firewall core "${iface}_zone" "$new_zones"
	uci_set_state firewall core "${iface}_ifname" "$ifname"
	uci_set_state firewall core "${iface}_masq_src" "$new_masq_src"
	uci_set_state firewall core "${iface}_masq_dest" "$new_masq_dest"
}

fw_sysctl_interface() {
	local ifname=$1
	{
		sysctl -w net.ipv4.conf.${ifname}.accept_redirects=$FW_ACCEPT_REDIRECTS
		sysctl -w net.ipv6.conf.${ifname}.accept_redirects=$FW_ACCEPT_REDIRECTS
		sysctl -w net.ipv4.conf.${ifname}.accept_source_route=$FW_ACCEPT_SRC_ROUTE
		sysctl -w net.ipv6.conf.${ifname}.accept_source_route=$FW_ACCEPT_SRC_ROUTE
	} >/dev/null 2>/dev/null
}

