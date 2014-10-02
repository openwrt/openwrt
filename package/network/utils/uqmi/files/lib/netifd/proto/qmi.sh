#!/bin/sh

. /lib/functions.sh
. ../netifd-proto.sh
init_proto "$@"

proto_qmi_init_config() {
	available=1
	no_device=1
	proto_config_add_string "device:device"
	proto_config_add_string apn
	proto_config_add_string auth
	proto_config_add_string username
	proto_config_add_string password
	proto_config_add_string pincode
	proto_config_add_string delay
	proto_config_add_string modes
}

proto_qmi_setup() {
	local interface="$1"

	local device apn auth username password pincode delay modes cid pdh
	json_get_vars device apn auth username password pincode delay modes

	[ -n "$device" ] || {
		echo "No control device specified"
		proto_notify_error "$interface" NO_DEVICE
		proto_block_restart "$interface"
		return 1
	}
	[ -c "$device" ] || {
		echo "The specified control device does not exist"
		proto_notify_error "$interface" NO_DEVICE
		proto_block_restart "$interface"
		return 1
	}

	devname="$(basename "$device")"
	devpath="$(readlink -f /sys/class/usbmisc/$devname/device/)"
	ifname="$( ls "$devpath"/net )"
	[ -n "$ifname" ] || {
		echo "The interface could not be found."
		proto_notify_error "$interface" NO_IFACE
		proto_block_restart "$interface"
		return 1
	}

	[ -n "$delay" ] && sleep "$delay"

	while uqmi -s -d "$device" --get-pin-status | grep '"UIM uninitialized"' > /dev/null; do
		sleep 1;
	done

	[ -n "$pincode" ] && {
		uqmi -s -d "$device" --verify-pin1 "$pincode" || {
			echo "Unable to verify PIN"
			proto_notify_error "$interface" PIN_FAILED
			proto_block_restart "$interface"
			return 1
		}
	}

	[ -n "$apn" ] || {
		echo "No APN specified"
		proto_notify_error "$interface" NO_APN
		proto_block_restart "$interface"
		return 1
	}

	echo "Waiting for network registration"
	while uqmi -s -d "$device" --get-serving-system | grep '"searching"' > /dev/null; do
		sleep 5;
	done

	[ -n "$modes" ] && uqmi -s -d "$device" --set-network-modes "$modes"

	echo "Starting network $apn"
	cid=`uqmi -s -d "$device" --get-client-id wds`
	[ $? -ne 0 ] && {
		echo "Unable to obtain client ID"
		proto_notify_error "$interface" NO_CID
		proto_block_restart "$interface"
		return 1
	}
	uci_set_state network $interface cid "$cid"

	pdh=`uqmi -s -d "$device" --set-client-id wds,"$cid" --start-network "$apn" \
	${auth:+--auth-type $auth} \
	${username:+--username $username} \
	${password:+--password $password}`
	[ $? -ne 0 ] && {
		echo "Unable to connect, check APN and authentication"
		proto_notify_error "$interface" NO_PDH
		proto_block_restart "$interface"
		return 1
	}
	uci_set_state network $interface pdh "$pdh"

	if ! uqmi -s -d "$device" --get-data-status | grep '"connected"' > /dev/null; then
		echo "Connection lost"
		proto_notify_error "$interface" NOT_CONNECTED
		proto_block_restart "$interface"
		return 1
	fi

	echo "Connected, starting DHCP"
	proto_init_update "$ifname" 1
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

proto_qmi_teardown() {
	local interface="$1"

	local device
	json_get_vars device
	local cid=$(uci_get_state network $interface cid)
	local pdh=$(uci_get_state network $interface pdh)

	echo "Stopping network"
	[ -n "$cid" ] && {
		[ -n "$pdh" ] && {
			uqmi -s -d "$device" --set-client-id wds,"$cid" --stop-network "$pdh"
			uci_revert_state network $interface pdh
		}
		uqmi -s -d "$device" --set-client-id wds,"$cid" --release-client-id wds
		uci_revert_state network $interface cid
	}

	proto_init_update "*" 0
	proto_send_update "$interface"
}

add_protocol qmi

