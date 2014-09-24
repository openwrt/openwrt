#!/bin/sh

. /lib/functions.sh
. ../netifd-proto.sh
init_proto "$@"

proto_directip_init_config() {
	available=1
	no_device=1
	proto_config_add_string "device:device"
	proto_config_add_string "ifname"
	proto_config_add_string "apn"
	proto_config_add_string "pincode"
	proto_config_add_string "auth"
	proto_config_add_string "username"
	proto_config_add_string "password"
}

proto_directip_setup() {
	local interface="$1"
	local chat

	local device apn pincode ifname auth username password
	json_get_vars device apn pincode ifname auth username password

	[ -e "$device" ] || {
		proto_notify_error "$interface" NO_DEVICE
		proto_set_available "$interface" 0
		return 1
	}

	[ -n "$ifname" ] || {
		proto_notify_error "$interface" NO_IFNAME
		proto_set_available "$interface" 0
		return 1
	}

	cardinfo=$(gcom -d "$device" -s /etc/gcom/getcardinfo.gcom)
	[ -n $(echo "$cardinfo" | grep -q "Sierra Wireless") ] || {
		proto_notify_error "$interface" BAD_DEVICE
		proto_block_restart "$interface"
		return 1
	}

	if [ -n "$pincode" ]; then
		PINCODE="$pincode" gcom -d "$device" -s /etc/gcom/setpin.gcom || {
			proto_notify_error "$interface" PIN_FAILED
			proto_block_restart "$interface"
			return 1
		}
	fi
	# wait for carrier to avoid firmware stability bugs
	gcom -d "$device" -s /etc/gcom/getcarrier.gcom || return 1

	local auth_type=0
	[ -z "$auth" ] && case $auth in
	pap) auth_type=1;;
	chap) auth_type=1;;
	esac

	USE_APN="$apn" USE_USER="$username" USE_PASS="$password" USE_AUTH="$auth_type" \
			gcom -d "$device" -s /etc/gcom/directip.gcom || {
		proto_notify_error "$interface" CONNECT_FAILED
		proto_block_restart "$interface"
		return 1
	}

	logger -p daemon.info -t "directip[$$]" "Connected, starting DHCP"
	proto_init_update "*" 1
	proto_send_update "$interface"

	json_init
	json_add_string name "${interface}_dhcp"
	json_add_string ifname "@$interface"
	json_add_string proto "dhcp"
	ubus call network add_dynamic "$(json_dump)"

	return 0
}

proto_directip_teardown() {
	local interface="$1"

	local device
	json_get_vars device

	gcom -d "$device" -s /etc/gcom/directip-stop.gcom || proto_notify_error "$interface" CONNECT_FAILED

	proto_init_update "*" 0
	proto_send_update "$interface"
}

add_protocol directip
