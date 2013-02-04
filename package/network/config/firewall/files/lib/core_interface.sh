# Copyright (C) 2009-2013 OpenWrt.org

fw__uci_state_add() {
	local var="$1"
	local item="$2"

	local val="$(uci_get_state firewall core $var)"
	local e1; for e1 in $item; do
		local e2; for e2 in $val; do
			[ "$e1" = "$e2" ] && e1=""
		done
		val="${val:+$val${e1:+ }}$e1"
	done

	uci_toggle_state firewall core $var "$val"
}

fw__uci_state_del() {
	local var="$1"
	local item="$2"

	local rest=""
	local val="$(uci_get_state firewall core $var)"
	local e1; for e1 in $val; do
		local e2; for e2 in $item; do
			[ "$e1" = "$e2" ] && e1=""
		done
		rest="${rest:+$rest${e1:+ }}$e1"
	done

	uci_toggle_state firewall core $var "$rest"
}

fw_do_interface_rules() {
    local action=$1
    local zone=$2
    local chain=zone_${zone}
    local ifname=$3
    local subnet=$4
    local extra_src="$5"
    local extra_dest="$6"

    local idev odev inet onet mode
    fw_get_family_mode mode x $zone i

    fw_get_negation idev '-i' "$ifname"
    fw_get_negation odev '-o' "$ifname"

    case "$mode/$subnet" in
        # Zone supports v6 only or dual, need v6
        G6/*:*|i/*:*)
            fw_get_negation inet '-s' "$subnet"
            fw_get_negation onet '-d' "$subnet"
            mode=6
        ;;

        # Zone supports v4 only or dual, need v4
        G4/*.*.*.*|i/*.*.*.*)
            fw_get_negation inet '-s' "$subnet"
            fw_get_negation onet '-d' "$subnet"
            mode=4
        ;;

        # Need v6 while zone is v4
        */*:*) fw_log info "zone $zone does not support IPv6 address family, skipping"; return ;;

        # Need v4 while zone is v6
        */*.*) fw_log info "zone $zone does not support IPv4 address family, skipping"; return ;;

        # Strip prefix
        *) mode="${mode#G}" ;;
    esac

    lock /var/run/firewall-interface.lock

    fw $action $mode f ${chain}_dest_ACCEPT ACCEPT $ { $odev $onet $extra_dest }
    fw $action $mode f ${chain}_src_ACCEPT  ACCEPT $ { $idev $inet $extra_src  }
    fw $action $mode f ${chain}_dest_DROP   DROP   $ { $odev $onet $extra_dest }
    fw $action $mode f ${chain}_src_DROP    DROP   $ { $idev $inet $extra_src  }
    fw $action $mode f ${chain}_dest_REJECT reject $ { $odev $onet $extra_dest }
    fw $action $mode f ${chain}_src_REJECT  reject $ { $idev $inet $extra_src  }

    [ "$(uci_get_state firewall core "${zone}_tcpmss")" == 1 ] && \
        fw $action $mode m ${chain}_MSSFIX TCPMSS $ \
            { $odev -p tcp --tcp-flags SYN,RST SYN --clamp-mss-to-pmtu $onet $extra_dest }

    fw $action $mode f delegate_input   ${chain}_input   $ { $idev $inet $extra_src  }
    fw $action $mode f delegate_forward ${chain}_forward $ { $idev $inet $extra_src  }
    fw $action $mode f delegate_output  ${chain}_output  $ { $odev $onet $extra_dest }

    fw $action $mode n PREROUTING ${chain}_prerouting $ { $idev $inet $extra_src  }
    fw $action $mode r PREROUTING ${chain}_notrack    $ { $idev $inet $extra_src  }
    fw $action $mode n POSTROUTING ${chain}_nat       $ { $odev $onet $extra_dest }

    # Flush conntrack table
    echo f >/proc/net/nf_conntrack 2>/dev/null

    lock -u /var/run/firewall-interface.lock
}

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

	local old_zones old_ifname old_subnets
	config_get old_zones core "${iface}_zone"
	[ -n "$old_zones" ] && {
		config_get old_ifname core "${iface}_ifname"
		config_get old_subnets core "${iface}_subnets"

		local z
		for z in $old_zones; do
			local n
			for n in ${old_subnets:-""}; do
				fw_log info "removing $iface ($old_ifname${n:+ alias $n}) from zone $z"
				fw_do_interface_rules del $z $old_ifname $n
			done

			[ -n "$old_subnets" ] || {
				fw__uci_state_del "${z}_networks" "$iface"
				env -i ACTION=remove ZONE="$z" INTERFACE="$iface" DEVICE="$ifname" /sbin/hotplug-call firewall
			}
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

		uci_toggle_state firewall core "${iface}_aliases" "$aliases"
	} || {
		local subnets=
		config_get subnets core "${iface}_subnets"
		append subnets "$aliasnet"

		config_set core "${iface}_subnets" "$subnets"
		uci_toggle_state firewall core "${iface}_subnets" "$subnets"
	}

	local new_zones=
	load_zone() {
		fw_config_get_zone "$1"
		list_contains zone_network "$iface" || return

		fw_log info "adding $iface ($ifname${aliasnet:+ alias $aliasnet}) to zone $zone_name"
		fw_do_interface_rules add ${zone_name} "$ifname" "$aliasnet"
		append new_zones $zone_name

		[ -n "$aliasnet" ] || {
			fw__uci_state_add "${zone_name}_networks" "${zone_network}"
			env -i ACTION=add ZONE="$zone_name" INTERFACE="$iface" DEVICE="$ifname" /sbin/hotplug-call firewall
		}
	}
	config_foreach load_zone zone

	uci_toggle_state firewall core "${iface}_zone" "$new_zones"
	uci_toggle_state firewall core "${iface}_ifname" "$ifname"
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
