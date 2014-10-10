#!/bin/sh

. /lib/functions.sh
. ../netifd-proto.sh
init_proto "$@"

proto_ncm_init_config() {
	no_device=1
	available=1
	proto_config_add_string "device:device"
	proto_config_add_string apn
	proto_config_add_string auth
	proto_config_add_string username
	proto_config_add_string password
	proto_config_add_string pincode
	proto_config_add_string delay
	proto_config_add_string mode
}

proto_ncm_setup() {
	local interface="$1"

	local manufacturer initialize setmode connect

	local device apn auth username password pincode delay mode
	json_get_vars device apn auth username password pincode delay mode

	[ -n "$device" ] || {
		echo "ncm[$$]" "No control device specified"
		proto_notify_error "$interface" NO_DEVICE
		proto_set_available "$interface" 0
		return 1
	}
	[ -e "$device" ] || {
		echo "ncm[$$]" "Control device not valid"
		proto_set_available "$interface" 0
		return 1
	}
	[ -n "$apn" ] || {
		echo "ncm[$$]" "No APN specified"
		proto_notify_error "$interface" NO_APN
		proto_set_available "$interface" 0
		return 1
	}

	[ -n "$delay" ] && sleep "$delay"

	manufacturer=`gcom -d "$device" -s /etc/gcom/getcardinfo.gcom | awk '/Manufacturer/ { print $2 }'`
	[ $? -ne 0 ] && {
		echo "ncm[$$]" "Failed to get modem information"
		proto_notify_error "$interface" GETINFO_FAILED
		proto_set_available "$interface" 0
		return 1
	}

	json_load "$(cat /etc/gcom/ncm.json)"
	json_select "$manufacturer"
	[ $? -ne 0 ] && {
		echo "ncm[$$]" "Unsupported modem"
		proto_notify_error "$interface" UNSUPPORTED_MODEM
		proto_set_available "$interface" 0
		return 1
	}
	json_get_values initialize initialize
	for i in $initialize; do
		eval COMMAND="$i" gcom -d "$device" -s /etc/gcom/runcommand.gcom || {
			echo "ncm[$$]" "Failed to initialize modem"
			proto_notify_error "$interface" INITIALIZE_FAILED
			proto_set_available "$interface" 0
			return 1
		}
	done

	[ -n "$pincode" ] && {
		PINCODE="$pincode" gcom -d "$device" -s /etc/gcom/setpin.gcom || {
			echo "ncm[$$]" "Unable to verify PIN"
			proto_notify_error "$interface" PIN_FAILED
			proto_block_restart "$interface"
			return 1
		}
	}
	[ -n "$mode" ] && {
		json_select modes
		json_get_var setmode "$mode"
		COMMAND="$setmode" gcom -d "$device" -s /etc/gcom/runcommand.gcom || {
			echo "ncm[$$]" "Failed to set operating mode"
			proto_notify_error "$interface" SETMODE_FAILED
			proto_set_available "$interface" 0
			return 1
		}
		json_select ..
	}

	json_get_var connect connect
	eval COMMAND="$connect" gcom -d "$device" -s /etc/gcom/runcommand.gcom || {
		echo "ncm[$$]" "Failed to connect"
		proto_notify_error "$interface" CONNECT_FAILED
		proto_set_available "$interface" 0
		return 1
	}

	echo "ncm[$$]" "Connected, starting DHCP"
	proto_init_update "*" 1
	proto_send_update "$interface"

	json_init
	json_add_string name "${interface}_dhcp"
	json_add_string ifname "@$interface"
	json_add_string proto "dhcp"
	json_close_object
	ubus call network add_dynamic "$(json_dump)"

	json_init
	json_add_string name "${interface}_dhcpv6"
	json_add_string ifname "@$interface"
	json_add_string proto "dhcpv6"
	json_close_object
	ubus call network add_dynamic "$(json_dump)"
}

proto_ncm_teardown() {
	local interface="$1"

	proto_init_update "*" 0
	proto_send_update "$interface"

	local manufacturer disconnect

	local device 
	json_get_vars device

	echo "ncm[$$]" "Stopping network"

	manufacturer=`gcom -d "$device" -s /etc/gcom/getcardinfo.gcom | awk '/Manufacturer/ { print $2 }'`
	[ $? -ne 0 ] && {
		echo "ncm[$$]" "Failed to get modem information"
		proto_notify_error "$interface" GETINFO_FAILED
		return 1
	}

	json_load "$(cat /etc/gcom/ncm.json)"
	json_select "$manufacturer" || {
		echo "ncm[$$]" "Unsupported modem"
		proto_notify_error "$interface" UNSUPPORTED_MODEM
		return 1
	}

	json_get_var disconnect disconnect
	COMMAND="$disconnect" gcom -d "$device" -s /etc/gcom/runcommand.gcom || {
		echo "ncm[$$]" "Failed to disconnect"
		proto_notify_error "$interface" DISCONNECT_FAILED
		return 1
	}
}

add_protocol ncm
