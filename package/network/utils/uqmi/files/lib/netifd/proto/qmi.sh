#!/bin/sh

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

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
	proto_config_add_boolean ipv6
	proto_config_add_boolean dhcp
	proto_config_add_defaults
}

proto_qmi_setup() {
	local interface="$1"

	local device apn auth username password pincode delay modes ipv6 dhcp $PROTO_DEFAULT_OPTIONS
	local cid_4 pdh_4 cid_6 pdh_6 ipv4
	local ip subnet gateway dns1 dns2 ip_6 ip_prefix_length gateway_6 dns1_6 dns2_6
	json_get_vars device apn auth username password pincode delay modes ipv6 dhcp $PROTO_DEFAULT_OPTIONS

	ipv4=1

	[ "$ipv6" = 1 ] || ipv6=""

	[ "$metric" = "" ] && metric="0"

	[ -n "$ctl_device" ] && device=$ctl_device

	[ -n "$device" ] || {
		echo "No control device specified"
		proto_notify_error "$interface" NO_DEVICE
		proto_set_available "$interface" 0
		return 1
	}
	[ -c "$device" ] || {
		echo "The specified control device does not exist"
		proto_notify_error "$interface" NO_DEVICE
		proto_set_available "$interface" 0
		return 1
	}

	devname="$(basename "$device")"
	devpath="$(readlink -f /sys/class/usbmisc/$devname/device/)"
	ifname="$( ls "$devpath"/net )"
	[ -n "$ifname" ] || {
		echo "The interface could not be found."
		proto_notify_error "$interface" NO_IFACE
		proto_set_available "$interface" 0
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
		return 1
	}

	# try to clear previous autoconnect state
	# do not reuse previous wds client id to prevent hangs caused by stale data
	uqmi -s -d "$device" \
		--stop-network 0xffffffff \
		--autoconnect > /dev/null

	uqmi -s -d "$device" --set-data-format 802.3
	uqmi -s -d "$device" --wda-set-data-format 802.3

	echo "Waiting for network registration"
	while uqmi -s -d "$device" --get-serving-system | grep '"searching"' > /dev/null; do
		sleep 5;
	done

	[ -n "$modes" ] && uqmi -s -d "$device" --set-network-modes "$modes"

	echo "Starting network $apn"

	cid_4=`uqmi -s -d "$device" --get-client-id wds`
	[ $? -ne 0 ] && {
		echo "Unable to obtain client ID"
		proto_notify_error "$interface" NO_CID
		return 1
	}

	pdh_4=`uqmi -s -d "$device" --set-client-id wds,"$cid_4" \
		--start-network "$apn" \
		${auth:+--auth-type $auth} \
		${username:+--username $username} \
		${password:+--password $password} \
		--ip-family ipv4 \
		--autoconnect`
	[ $? -ne 0 ] && {
		echo "Unable to connect IPv4"
		uqmi -s -d "$device" --set-client-id wds,"$cid_4" --release-client-id wds
		ipv4=""
	}

	[ -n "$ipv6" ] && {
		cid_6=`uqmi -s -d "$device" --get-client-id wds`
		if [ $? = 0 ]; then
			pdh_6=`uqmi -s -d "$device" --set-client-id wds,"$cid_6" \
				--start-network "$apn" \
				${auth:+--auth-type $auth} \
				${username:+--username $username} \
				${password:+--password $password} \
				--ip-family ipv6 --autoconnect`
			[ $? -ne 0 ] && {
				echo "Unable to connect IPv6"
				uqmi -s -d "$device" --set-client-id wds,"$cid_6" --release-client-id wds
				ipv6=""
			}
		else
			echo "Unable to connect IPv6"
			ipv6=""
		fi
	}

	[ -z "$ipv4" -a -z "$ipv6" ] && {
		echo "Unable to connect"
		proto_notify_error "$interface" CALL_FAILED
		return 1
	}

	echo "Setting up $ifname"
	proto_init_update "$ifname" 1
	proto_add_data
	[ -n "$ipv4" ] && {
		json_add_string "cid_4" "$cid_4"
		json_add_string "pdh_4" "$pdh_4"
	}
	[ -n "$ipv6" ] && {
		json_add_string "cid_6" "$cid_6"
		json_add_string "pdh_6" "$pdh_6"
	}

	[ -n "$ipv6" ] && {
		if [ -z "$dhcp" -o "$dhcp" = 0 ]; then
			json_load "$(uqmi -s -d $device --set-client-id wds,$cid_6 --get-current-settings)"
			json_select ipv6
			json_get_var ip_6 ip
			json_get_var gateway_6 gateway
			json_get_var dns1_6 dns1
			json_get_var dns2_6 dns2
			json_get_var ip_prefix_length ip-prefix-length

			# RFC 7278: Extend an IPv6 /64 Prefix to LAN
			proto_add_ipv6_address "$ip_6" "128"
			proto_add_ipv6_prefix "${ip_6}/${ip_prefix_length}"
			proto_add_ipv6_route "$gateway_6" "128"
			[ "$defaultroute" = 0 ] || proto_add_ipv6_route "::0" 0 "$gateway_6" "" "" "${ip_6}/${ip_prefix_length}"
			[ "$peerdns" = 0 ] || {
				proto_add_dns_server "$dns1_6"
				proto_add_dns_server "$dns2_6"
			}
			proto_add_data
			json_add_string "cid_6" "$cid_6"
			json_add_string "pdh_6" "$pdh_6"
		else
			json_init
			json_add_string name "${interface}_6"
			json_add_string ifname "@$interface"
			json_add_string proto "dhcpv6"
			proto_add_dynamic_defaults
			# RFC 7278: Extend an IPv6 /64 Prefix to LAN
			json_add_string extendprefix 1
			json_close_object
			ubus call network add_dynamic "$(json_dump)"
		fi
	}

	proto_close_data
	proto_send_update "$interface"

	[ -n "$ipv4" ] && {
		json_init
		json_add_string name "${interface}_4"
		json_add_string ifname "@$interface"
		json_add_string proto "dhcp"
		proto_add_dynamic_defaults
		json_close_object
		ubus call network add_dynamic "$(json_dump)"
	}
}

qmi_wds_stop() {
	local cid="$1"
	local pdh="$2"

	[ -n "$cid" ] || return

	# disable previous autoconnect state using the global handle
	uqmi -s -d "$device" --set-client-id wds,"$cid" --stop-network "0xffffffff"

	[ -n "$pdh" ] && uqmi -s -d "$device" --set-client-id wds,"$cid" --stop-network "$pdh"
	uqmi -s -d "$device" --set-client-id wds,"$cid" --release-client-id wds
}

proto_qmi_teardown() {
	local interface="$1"

	local device cid_4 pdh_4 cid_6 pdh_6
	json_get_vars device

	[ -n "$ctl_device" ] && device=$ctl_device

	echo "Stopping network"

	json_load "$(ubus call network.interface.$interface status)"
	json_select data
	json_get_vars cid_4 pdh_4 cid_6 pdh_6

	qmi_wds_stop "$cid_4" "$pdh_4"
	qmi_wds_stop "$cid_6" "$pdh_6"

	proto_init_update "*" 0
	proto_send_update "$interface"
}

[ -n "$INCLUDE_ONLY" ] || {
	add_protocol qmi
}
