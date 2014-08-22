#!/bin/sh

. /lib/functions.sh
. ../netifd-proto.sh
init_proto "$@"

#DBG=-v

proto_mbim_init_config() {
	proto_config_add_string "device:device"
	proto_config_add_string apn
	proto_config_add_string pincode
	proto_config_add_string delay
}

proto_mbim_setup() {
	local interface="$1"
	local tid=2
	local ret

	local device apn pincode delay
	json_get_vars device apn pincode delay

	[ -n "$device" ] || {
		logger -p daemon.err -t "mbim[$$]" "No control device specified"
		proto_notify_error "$interface" NO_DEVICE
		proto_block_restart "$interface"
		return 1
	}
	[ -c "$device" ] || {
		logger -p daemon.err -t "mbim[$$]" "The specified control device does not exist"
		proto_notify_error "$interface" NO_DEVICE
		proto_block_restart "$interface"
		return 1
	}
	[ -n "$apn" ] || {
		logger -p daemon.err -t "mbim[$$]" "No APN specified"
		proto_notify_error "$interface" NO_APN
		proto_block_restart "$interface"
		return 1
	}


	[ -n "$delay" ] && sleep "$delay"

	logger -p daemon.info -t "mbim[$$]" "Reading capabilities"
	umbim $DBG -n -d $device caps || {
		logger -p daemon.err -t "mbim[$$]" "Failed to read modem caps"
		proto_notify_error "$interface" PIN_FAILED
		proto_block_restart "$interface"
		return 1
	}
	tid=$((tid + 1))

	[ "$pincode" ] && {
		logger -p daemon.info -t "mbim[$$]" "Sending pin"
		umbim $DBG -n -t $tid -d $device unlock "$pincode" || {
			logger -p daemon.err -t "mbim[$$]" "Unable to verify PIN"
			proto_notify_error "$interface" PIN_FAILED
			proto_block_restart "$interface"
			return 1
		}
	}
	tid=$((tid + 1))

	logger -p daemon.info -t "mbim[$$]" "Checking pin"
	umbim $DBG -n -t $tid -d $device pinstate || {
		logger -p daemon.err -t "mbim[$$]" "PIN required"
		proto_notify_error "$interface" PIN_FAILED
		proto_block_restart "$interface"
		return 1
	}
	tid=$((tid + 1))

	logger -p daemon.info -t "mbim[$$]" "Checking subscriber"
 	umbim $DBG -n -t $tid -d $device subscriber || {
		logger -p daemon.err -t "mbim[$$]" "Subscriber init failed"
		proto_notify_error "$interface" NO_SUBSCRIBER
		proto_block_restart "$interface"
		return 1
	}
	tid=$((tid + 1))

	logger -p daemon.info -t "mbim[$$]" "Register with network"
  	umbim $DBG -n -t $tid -d $device registration || {
		logger -p daemon.err -t "mbim[$$]" "Subscriber registration failed"
		proto_notify_error "$interface" NO_REGISTRATION
		proto_block_restart "$interface"
		return 1
	}
	tid=$((tid + 1))

	logger -p daemon.info -t "mbim[$$]" "Attach to network"
   	umbim $DBG -n -t $tid -d $device attach || {
		logger -p daemon.err -t "mbim[$$]" "Failed to attach to network"
		proto_notify_error "$interface" ATTACH_FAILED
		proto_block_restart "$interface"
		return 1
	}
	tid=$((tid + 1))
 
	logger -p daemon.info -t "mbim[$$]" "Connect to network"
	while ! umbim $DBG -n -t $tid -d $device connect "$apn"; do
		tid=$((tid + 1))
		sleep 1;
	done
	tid=$((tid + 1))

	uci_set_state network $interface tid "$tid"

	logger -p daemon.info -t "mbim[$$]" "Connected, starting DHCP"
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

proto_mbim_teardown() {
	local interface="$1"

	local device
	json_get_vars device
	local tid=$(uci_get_state network $interface tid)

	logger -p daemon.info -t "mbim[$$]" "Stopping network"
	[ -n "$tid" ] && {
		umbim $DBG -t$tid -d "$device" disconnect
		uci_revert_state network $interface tid
	}

	proto_init_update "*" 0
	proto_send_update "$interface"
}

add_protocol mbim
