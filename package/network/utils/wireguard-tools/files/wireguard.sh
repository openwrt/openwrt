#!/bin/sh
# Copyright 2016-2017 Dan Luedtke <mail@danrl.com>
# Licensed to the public under the Apache License 2.0.
# shellcheck disable=SC2317

WG=/usr/bin/wg
if [ ! -x $WG ]; then
	logger -t "wireguard" "error: missing wireguard-tools (${WG})"
	exit 0
fi

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

proto_wireguard_init_config() {
	renew_handler=1
	peer_detect=1

	proto_config_add_string "private_key"
	proto_config_add_int "listen_port"
	proto_config_add_int "mtu"
	proto_config_add_string "fwmark"
	proto_config_add_string "addresses"

	available=1
	no_proto_task=1
}

ensure_key_is_generated() {
	local private_key
	private_key="$(uci get network."$1".private_key)"

	if [ "$private_key" = "generate" ] || [ -z "$private_key" ]; then
		private_key="$("${WG}" genkey)"
		uci -q set network."$1".private_key="$private_key" && \
			uci -q commit network
	fi
}

proto_wireguard_setup() {
	local config="$1"

	local private_key listen_port mtu fwmark addresses ip6prefix nohostroute tunlink
	ensure_key_is_generated "${config}"

	config_load network
	config_get private_key "${config}" "private_key"
	config_get listen_port "${config}" "listen_port"
	config_get addresses "${config}" "addresses"
	config_get mtu "${config}" "mtu"
	config_get fwmark "${config}" "fwmark"
	config_get ip6prefix "${config}" "ip6prefix"
	config_get nohostroute "${config}" "nohostroute"
	config_get tunlink "${config}" "tunlink"

	# Add the link only if it didn't already exist
	ip -br link show "${config}" >/dev/null 2>&1 || ip link add dev "${config}" type wireguard

	[ -n "${mtu}" ] && ip link set mtu "${mtu}" dev "${config}"

	proto_init_update "${config}" 1

	# Build WireGuard configuration entirely in memory
	local wg_config="[Interface]\n"
	wg_config="${wg_config}PrivateKey=${private_key}\n"
	[ -n "${listen_port}" ]	&& wg_config="${wg_config}ListenPort=${listen_port}\n"
	[ -n "${fwmark}" ]		&& wg_config="${wg_config}FwMark=${fwmark}\n"

	# Collect peer configs into wg_config as well
	local peer_config
	peer_config=""
	proto_wireguard_setup_peer_collect() {
		local section="$1"
		local peer_block

		config_get_bool route_allowed_ips "$section" "route_allowed_ips" 0
		config_get_bool disabled "$section" "disabled" 0
		[ "$disabled" = 1 ] && return;
		config_get peer_key "$section" "public_key"
		config_get peer_eph "$section" "endpoint_host"
		config_get peer_port "$section" "endpoint_port" "51820"
		config_get peer_a_ips "$section" "allowed_ips"
		config_get peer_p_ka "$section" "persistent_keepalive"
		config_get peer_psk "$section" "peer_psk"


		[ "${peer_eph##*:}" != "$peer_eph" ] && peer_eph="[$peer_eph]"
		peer_port=${peer_port:-51820}

		peer_block="\n[Peer]\n"
		[ -n "${peer_key}" ]	&& peer_block="${peer_block}PublicKey=${peer_key}\n"
		[ -n "${peer_psk}" ]	&& peer_block="${peer_block}PresharedKey=${peer_psk}\n"
		[ -n "${peer_eph}" ]	&& peer_block="${peer_block}Endpoint=${peer_eph}${peer_port:+:$peer_port}\n"
		[ -n "${peer_a_ips}" ]	&& peer_block="${peer_block}AllowedIPs=${peer_a_ips/ /, }\n"
		[ -n "${peer_p_ka}" ]	&& peer_block="${peer_block}PersistentKeepalive=${peer_p_ka}\n"

		[ -n "$peer_key" ] && peer_config="$peer_config$peer_block\n"
		if [ $route_allowed_ips -ne 0 ]; then
			for allowed_ip in $peer_a_ips; do
				case "${allowed_ip}" in
					*:*/*) proto_add_ipv6_route "${allowed_ip%%/*}" "${allowed_ip##*/}" ;;
					*.*/*) proto_add_ipv4_route "${allowed_ip%%/*}" "${allowed_ip##*/}" ;;
					*:*) proto_add_ipv6_route "${allowed_ip%%/*}" "128" ;;
					*.*) proto_add_ipv4_route "${allowed_ip%%/*}" "32" ;;
				esac
			done
		fi

	}

	config_foreach proto_wireguard_setup_peer_collect "wireguard_${config}"

	# Combine interface + peer config into one variable
	wg_config="${wg_config}${peer_config}"

	# Apply configuration directly using wg syncconf via stdin
	printf "%b" "$wg_config" | ${WG} syncconf "${config}" /dev/stdin
	local WG_RETURN=$?

	if [ ${WG_RETURN} -ne 0 ]; then
		echo "Could not sync WireGuard configuration"
		sleep 5
		proto_setup_failed "${config}"
		exit 1
	fi

	# Assign addresses
	for address in ${addresses}; do
		case "${address}" in
			*:*/*) proto_add_ipv6_address "${address%%/*}" "${address##*/}" ;;
			*.*/*) proto_add_ipv4_address "${address%%/*}" "${address##*/}" ;;
			*:*)   proto_add_ipv6_address "${address%%/*}" "128" ;;
			*.*)   proto_add_ipv4_address "${address%%/*}" "32" ;;
		esac
	done

	for prefix in ${ip6prefix}; do
		proto_add_ipv6_prefix "$prefix"
	done

	# Endpoint dependency tracking
	if [ "${nohostroute}" != "1" ]; then
		wg show "${config}" endpoints | \
		sed -E 's/\[?([0-9.:a-f]+)\]?:([0-9]+)/\1 \2/' | \
		while IFS=$'\t ' read -r key address port; do
			[ -n "${port}" ] || continue
			proto_add_host_dependency "${config}" "${address}" "${tunlink}"
		done
	fi

	proto_send_update "${config}"
}

proto_wireguard_renew() {
	local interface="$1"
	proto_wireguard_setup "$interface"
}

proto_wireguard_teardown() {
	local config="$1"
	ip link del dev "${config}" >/dev/null 2>&1
}

[ -n "$INCLUDE_ONLY" ] || {
	add_protocol wireguard
}
