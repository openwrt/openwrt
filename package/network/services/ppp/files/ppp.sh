#!/bin/sh

[ -x /usr/sbin/pppd ] || exit 0

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

ppp_generic_init_config() {
	proto_config_add_string "username"
	proto_config_add_string "password"
	proto_config_add_string "keepalive"
	proto_config_add_int "demand"
	proto_config_add_string "pppd_options"
	proto_config_add_string "connect"
	proto_config_add_string "disconnect"
	proto_config_add_boolean "ipv6"
	proto_config_add_boolean "authfail"
	proto_config_add_int "mtu"
}

ppp_generic_setup() {
	local config="$1"; shift

	json_get_vars ipv6 demand keepalive username password pppd_options
	[ "$ipv6" = 1 ] || ipv6=""
	if [ "${demand:-0}" -gt 0 ]; then
		demand="precompiled-active-filter /etc/ppp/filter demand idle $demand"
	else
		demand="persist"
	fi

	[ -n "$mtu" ] || json_get_var mtu mtu

	local interval="${keepalive##*[, ]}"
	[ "$interval" != "$keepalive" ] || interval=5
	[ -n "$connect" ] || json_get_var connect connect
	[ -n "$disconnect" ] || json_get_var disconnect disconnect

	proto_run_command "$config" /usr/sbin/pppd \
		nodetach ipparam "$config" \
		ifname "${proto:-ppp}-$config" \
		${keepalive:+lcp-echo-interval $interval lcp-echo-failure ${keepalive%%[, ]*}} \
		${ipv6:++ipv6} \
		nodefaultroute \
		usepeerdns \
		$demand maxfail 1 \
		${username:+user "$username" password "$password"} \
		${connect:+connect "$connect"} \
		${disconnect:+disconnect "$disconnect"} \
		ip-up-script /lib/netifd/ppp-up \
		ipv6-up-script /lib/netifd/ppp-up \
		ip-down-script /lib/netifd/ppp-down \
		ipv6-down-script /lib/netifd/ppp-down \
		${mtu:+mtu $mtu mru $mtu} \
		$pppd_options "$@"
}

ppp_generic_teardown() {
	local interface="$1"

	case "$ERROR" in
		11|19)
			proto_notify_error "$interface" AUTH_FAILED
			json_get_var authfail authfail
			if [ "${authfail:-0}" -gt 0 ]; then
				proto_block_restart "$interface"
			fi
		;;
		2)
			proto_notify_error "$interface" INVALID_OPTIONS
			proto_block_restart "$interface"
		;;
	esac
	proto_kill_command "$interface"
}

# PPP on serial device

proto_ppp_init_config() {
	proto_config_add_string "device"
	ppp_generic_init_config
	no_device=1
	available=1
}

proto_ppp_setup() {
	local config="$1"

	json_get_var device device
	ppp_generic_setup "$config" "$device"
}

proto_ppp_teardown() {
	ppp_generic_teardown "$@"
}

proto_pppoe_init_config() {
	ppp_generic_init_config
	proto_config_add_string "ac"
	proto_config_add_string "service"
}

proto_pppoe_setup() {
	local config="$1"
	local iface="$2"

	for module in slhc ppp_generic pppox pppoe; do
		/sbin/insmod $module 2>&- >&-
	done

	json_get_var mtu mtu
	mtu="${mtu:-1492}"

	json_get_var ac ac
	json_get_var service service

	ppp_generic_setup "$config" \
		plugin rp-pppoe.so \
		${ac:+rp_pppoe_ac "$ac"} \
		${service:+rp_pppoe_service "$service"} \
		"nic-$iface"
}

proto_pppoe_teardown() {
	ppp_generic_teardown "$@"
}

proto_pppoa_init_config() {
	ppp_generic_init_config
	proto_config_add_int "atmdev"
	proto_config_add_int "vci"
	proto_config_add_int "vpi"
	proto_config_add_string "encaps"
	no_device=1
	available=1
}

proto_pppoa_setup() {
	local config="$1"
	local iface="$2"

	for module in slhc ppp_generic pppox pppoatm; do
		/sbin/insmod $module 2>&- >&-
	done

	json_get_vars atmdev vci vpi encaps

	case "$encaps" in
		1|vc) encaps="vc-encaps" ;;
		*) encaps="llc-encaps" ;;
	esac

	ppp_generic_setup "$config" \
		plugin pppoatm.so \
		${atmdev:+$atmdev.}${vpi:-8}.${vci:-35} \
		${encaps}
}

proto_pppoa_teardown() {
	ppp_generic_teardown "$@"
}

proto_pptp_init_config() {
	ppp_generic_init_config
	proto_config_add_string "server"
	available=1
	no_device=1
}

proto_pptp_setup() {
	local config="$1"
	local iface="$2"

	local ip serv_addr server
	json_get_var server server && {
		for ip in $(resolveip -t 5 "$server"); do
			( proto_add_host_dependency "$config" "$ip" )
			serv_addr=1
		done
	}
	[ -n "$serv_addr" ] || {
		echo "Could not resolve server address"
		sleep 5
		proto_setup_failed "$config"
		exit 1
	}

	local load
	for module in slhc ppp_generic ppp_async ppp_mppe ip_gre gre pptp; do
		grep -q "$module" /proc/modules && continue
		/sbin/insmod $module 2>&- >&-
		load=1
	done
	[ "$load" = "1" ] && sleep 1

	ppp_generic_setup "$config" \
		plugin pptp.so \
		pptp_server $server \
		file /etc/ppp/options.pptp
}

proto_pptp_teardown() {
	ppp_generic_teardown "$@"
}

[ -n "$INCLUDE_ONLY" ] || {
	add_protocol ppp
	[ -f /usr/lib/pppd/*/rp-pppoe.so ] && add_protocol pppoe
	[ -f /usr/lib/pppd/*/pppoatm.so ] && add_protocol pppoa
	[ -f /usr/lib/pppd/*/pptp.so ] && add_protocol pptp
}

