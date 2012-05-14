#!/bin/sh

. /etc/functions.sh
. ../netifd-proto.sh
init_proto "$@"

INCLUDE_ONLY=1
. ./ppp.sh

proto_pptp_init_config() {
	ppp_generic_init_config
	proto_config_add_string "server"
	proto_config_add_boolean "buffering"
	available=1
	no_device=1
}

proto_pptp_setup() {
	local config="$1"
	local iface="$2"
	local load

	json_get_vars server buffering

	serv_addr=
	for ip in $(resolveip -t 5 "$server"); do
		proto_add_host_dependency "$config" "$server"
		serv_addr=1
	done
	[ -n "$serv_addr" ] || {
		echo "Could not resolve server address"
		sleep 5
		proto_setup_failed "$config"
		exit 1
	}

	[ "${buffering:-1}" == 0 ] && buffering="--nobuffer" || buffering=

	for module in slhc ppp_generic ppp_async ip_gre; do
		grep -q "$module" /proc/modules && continue
		/sbin/insmod $module 2>&- >&-
		load=1
	done
	[ "$load" = "1" ] && sleep 1

	ppp_generic_setup "$config" \
		pty "/usr/sbin/pptp $server --loglevel 0 --nolaunchpppd $buffering" \
		file /etc/ppp/options.pptp
}

proto_pptp_teardown() {
	ppp_generic_teardown "$@"
}

add_protocol pptp
