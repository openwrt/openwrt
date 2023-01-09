#!/bin/sh

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. ../netifd-proto.sh
	init_proto "$@"
}
#DBG=-v

proto_mbim_init_config() {
	available=1
	no_device=1
	proto_config_add_string "device:device"
	proto_config_add_string apn
	proto_config_add_string pincode
	proto_config_add_string delay
	proto_config_add_boolean allow_roaming
	proto_config_add_boolean allow_partner
	proto_config_add_string auth
	proto_config_add_string username
	proto_config_add_string password
	proto_config_add_boolean dhcp
	proto_config_add_boolean dhcpv6
	proto_config_add_string pdptype
	proto_config_add_defaults
}

_proto_mbim_setup() {
	local interface="$1"
	local tid=2
	local ret

	local device apn pincode delay allow_roaming allow_partner dhcp dhcpv6 pdptype $PROTO_DEFAULT_OPTIONS
	json_get_vars device apn pincode delay auth username password allow_roaming allow_partner dhcp dhcpv6 pdptype $PROTO_DEFAULT_OPTIONS

	[ -n "$ctl_device" ] && device=$ctl_device

	[ -n "$device" ] || {
		echo "mbim[$$]" "No control device specified"
		proto_notify_error "$interface" NO_DEVICE
		proto_set_available "$interface" 0
		return 1
	}
	[ -c "$device" ] || {
		echo "mbim[$$]" "The specified control device does not exist"
		proto_notify_error "$interface" NO_DEVICE
		proto_set_available "$interface" 0
		return 1
	}

	devname="$(basename "$device")"
	devpath="$(readlink -f /sys/class/usbmisc/$devname/device/)"
	ifname="$( ls "$devpath"/net )"

	[ -n "$ifname" ] || {
		echo "mbim[$$]" "Failed to find matching interface"
		proto_notify_error "$interface" NO_IFNAME
		proto_set_available "$interface" 0
		return 1
	}

	[ -n "$apn" ] || {
		echo "mbim[$$]" "No APN specified"
		proto_notify_error "$interface" NO_APN
		return 1
	}

	[ -n "$delay" ] && sleep "$delay"

	echo "mbim[$$]" "Reading capabilities"
	umbim $DBG -n -d $device caps || {
		echo "mbim[$$]" "Failed to read modem caps"
		tid=$((tid + 1))
		umbim $DBG -t $tid -d "$device" disconnect
		proto_notify_error "$interface" PIN_FAILED
		return 1
	}
	tid=$((tid + 1))

	[ "$pincode" ] && {
		echo "mbim[$$]" "Sending pin"
		umbim $DBG -n -t $tid -d $device unlock "$pincode" || {
			echo "mbim[$$]" "Unable to verify PIN"
			tid=$((tid + 1))
			umbim $DBG -t $tid -d "$device" disconnect
			proto_notify_error "$interface" PIN_FAILED
			proto_block_restart "$interface"
			return 1
		}
	}
	tid=$((tid + 1))

	echo "mbim[$$]" "Checking pin"
	umbim $DBG -n -t $tid -d $device pinstate
	[ $? -eq 2 ] && {
		echo "mbim[$$]" "PIN required"
		tid=$((tid + 1))
		umbim $DBG -t $tid -d "$device" disconnect
		proto_notify_error "$interface" PIN_FAILED
		proto_block_restart "$interface"
		return 1
	}
	tid=$((tid + 1))

	echo "mbim[$$]" "Checking subscriber"
	umbim $DBG -n -t $tid -d $device subscriber || {
		echo "mbim[$$]" "Subscriber init failed"
		tid=$((tid + 1))
		umbim $DBG -t $tid -d "$device" disconnect
		proto_notify_error "$interface" NO_SUBSCRIBER
		return 1
	}
	tid=$((tid + 1))

	echo "mbim[$$]" "Register with network"
	connected=0
	umbim $DBG -n -t $tid -d $device registration
	reg_status=$?
	case $reg_status in
		0)	echo "mbim[$$]" "Registered in home mode"
			tid=$((tid + 1))
			connected=1;;
		4)	if [ "$allow_roaming" = "1" ]; then
				echo "mbim[$$]" "Registered in roaming mode"
				tid=$((tid + 1))
				connected=1
			fi;;
		5) 	if [ "$allow_partner" = "1" ]; then
				echo "mbim[$$]" "Registered in partner mode"
				tid=$((tid + 1))
				connected=1
			fi;;
	esac
	if [ $connected -ne 1 ]; then
		echo "mbim[$$]" "Subscriber registration failed (code $reg_status)"
		tid=$((tid + 1))
		umbim $DBG -t $tid -d "$device" disconnect
		proto_notify_error "$interface" NO_REGISTRATION
		return 1
	fi

	echo "mbim[$$]" "Attach to network"
	umbim $DBG -n -t $tid -d $device attach || {
		echo "mbim[$$]" "Failed to attach to network"
		tid=$((tid + 1))
		umbim $DBG -t $tid -d "$device" disconnect
		proto_notify_error "$interface" ATTACH_FAILED
		return 1
	}
	tid=$((tid + 1))

	pdptype=$(echo "$pdptype" | awk '{print tolower($0)}')

	local req_pdptype="" # Pass "default" PDP type to umbim if unconfigured
	[ "$pdptype" = "ipv4" -o "$pdptype" = "ipv6" -o "$pdptype" = "ipv4v6" ] && req_pdptype="$pdptype:"

	local connect_state
	echo "mbim[$$]" "Connect to network"
	connect_state=$(umbim $DBG -n -t $tid -d $device connect "$req_pdptype$apn" "$auth" "$username" "$password") || {
		echo "mbim[$$]" "Failed to connect bearer"
		tid=$((tid + 1))
		umbim $DBG -t $tid -d "$device" disconnect
		proto_notify_error "$interface" CONNECT_FAILED
		return 1
	}
	tid=$((tid + 1))

	echo "$connect_state"
	local iptype="$(echo "$connect_state" | grep iptype: | awk '{print $4}')"

	echo "mbim[$$]" "Connected"

	local zone="$(fw3 -q network "$interface" 2>/dev/null)"

	echo "mbim[$$]" "Setting up $ifname"
	eval $(umbim $DBG -n -t $tid -d $device config | sed 's/: /=/g')
	tid=$((tid + 1))

	proto_init_update "$ifname" 1
	proto_send_update "$interface"

	[ "$iptype" != "ipv6" ] && {
		if [ -z "$dhcp" -o "$dhcp" = 0 ]; then
			json_init
			json_add_string name "${interface}_4"
			json_add_string ifname "@$interface"
			json_add_string proto "static"
			json_add_array ipaddr
			json_add_string "" "$ipv4address"
			json_close_array
			json_add_string gateway "$ipv4gateway"
			json_add_array dns
			[ "$peerdns" = 0 ] || json_add_string "" "$ipv4dnsserver"
			json_close_array
			proto_add_dynamic_defaults
			[ -n "$zone" ] && json_add_string zone "$zone"
			json_close_object
			ubus call network add_dynamic "$(json_dump)"
		else
			echo "mbim[$$]" "Starting DHCP on $ifname"
			json_init
			json_add_string name "${interface}_4"
			json_add_string ifname "@$interface"
			json_add_string proto "dhcp"
			proto_add_dynamic_defaults
			[ -n "$zone" ] && json_add_string zone "$zone"
			json_close_object
			ubus call network add_dynamic "$(json_dump)"
		fi
	}

	[ "$iptype" != "ipv4" ] && {
		if [ -z "$dhcpv6" -o "$dhcpv6" = 0 ]; then
			json_init
			json_add_string name "${interface}_6"
			json_add_string ifname "@$interface"
			json_add_string proto "static"
			json_add_array ip6addr
			json_add_string "" "$ipv6address"
			json_close_array
			json_add_string ip6gw "$ipv6gateway"
			json_add_array dns
			[ "$peerdns" = 0 ] || json_add_string "" "$ipv6dnsserver"
			json_close_array
			proto_add_dynamic_defaults
			[ -n "$zone" ] && json_add_string zone "$zone"
			json_close_object
			ubus call network add_dynamic "$(json_dump)"
		else
			echo "mbim[$$]" "Starting DHCPv6 on $ifname"
			json_init
			json_add_string name "${interface}_6"
			json_add_string ifname "@$interface"
			json_add_string proto "dhcpv6"
			json_add_string extendprefix 1
			proto_add_dynamic_defaults
			[ -n "$zone" ] && json_add_string zone "$zone"
			json_close_object
			ubus call network add_dynamic "$(json_dump)"
		fi
	}

	uci_set_state network $interface tid "$tid"
}

proto_mbim_setup() {
	local ret

	_proto_mbim_setup $@
	ret=$?

	[ "$ret" = 0 ] || {
		logger "mbim bringup failed, retry in 15s"
		sleep 15
	}

	return $ret
}

proto_mbim_teardown() {
	local interface="$1"

	local device
	json_get_vars device
	local tid=$(uci_get_state network $interface tid)

	[ -n "$ctl_device" ] && device=$ctl_device

	echo "mbim[$$]" "Stopping network"
	[ -n "$tid" ] && {
		umbim $DBG -t $tid -d "$device" disconnect
		uci_revert_state network $interface tid
	}

	proto_init_update "*" 0
	proto_send_update "$interface"
}

[ -n "$INCLUDE_ONLY" ] || add_protocol mbim
