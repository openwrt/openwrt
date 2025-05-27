#!/bin/sh

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

proto_ncm_init_config() {
	no_device=1
	available=1
	proto_config_add_string "device:device"
	proto_config_add_string ifname
	proto_config_add_string apn
	proto_config_add_string auth
	proto_config_add_string username
	proto_config_add_string password
	proto_config_add_string pincode
	proto_config_add_string delay
	proto_config_add_string mode
	proto_config_add_string pdptype
	proto_config_add_boolean sourcefilter
	proto_config_add_boolean delegate
	proto_config_add_int profile
	proto_config_add_defaults
}

proto_ncm_setup() {
	local interface="$1"

	local manufacturer initialize setmode connect finalize devname devpath ifpath

	local device ifname  apn auth username password pincode delay mode pdptype profile $PROTO_DEFAULT_OPTIONS
	json_get_vars device ifname apn auth username password pincode delay mode pdptype sourcefilter delegate profile $PROTO_DEFAULT_OPTIONS

	local context_type

	[ "$metric" = "" ] && metric="0"

	[ -n "$profile" ] || profile=1

	pdptype=$(echo "$pdptype" | awk '{print toupper($0)}')
	[ "$pdptype" = "IP" -o "$pdptype" = "IPV6" -o "$pdptype" = "IPV4V6" ] || pdptype="IP"

	[ "$pdptype" = "IPV4V6" ] && context_type=3
	[ -z "$context_type" -a "$pdptype" = "IPV6" ] && context_type=2
	[ -n "$context_type" ] || context_type=1

	[ -n "$ctl_device" ] && device=$ctl_device

	[ -n "$device" ] || {
		echo "No control device specified"
		proto_notify_error "$interface" NO_DEVICE
		proto_set_available "$interface" 0
		return 1
	}

	device="$(readlink -f $device)"
	[ -e "$device" ] || {
		echo "Control device not valid"
		proto_set_available "$interface" 0
		return 1
	}

	[ -z "$ifname" ] && {
		devname="$(basename "$device")"
		case "$devname" in
		'ttyACM'*)
			devpath="$(readlink -f /sys/class/tty/$devname/device)"
			ifpath="$devpath/../*/net"
			;;
		'tty'*)
			devpath="$(readlink -f /sys/class/tty/$devname/device)"
			ifpath="$devpath/../../*/net"
			;;
		*)
			devpath="$(readlink -f /sys/class/usbmisc/$devname/device/)"
			ifpath="$devpath/net"
			;;
		esac
		ifname="$(ls $(ls -1 -d $ifpath | head -n 1))"
	}

	[ -n "$ifname" ] || {
		echo "The interface could not be found."
		proto_notify_error "$interface" NO_IFACE
		proto_set_available "$interface" 0
		return 1
	}

	start=$(date +%s)
	while true; do
		manufacturer=$(gcom -d "$device" -s /etc/gcom/getcardinfo.gcom | awk 'NF && $0 !~ /AT\+CGMI/ { sub(/\+CGMI: /,""); print tolower($1); exit; }')
		[ "$manufacturer" = "error" ] && {
			manufacturer=""
		}
		[ -n "$manufacturer" ] && {
			break
		}
		[ -z "$delay" ] && {
			break
		}
		sleep 1
		elapsed=$(($(date +%s) - start))
		[ "$elapsed" -gt "$delay" ] && {
			break
		}
	done
	[ -z "$manufacturer" ] && {
		echo "Failed to get modem information"
		proto_notify_error "$interface" GETINFO_FAILED
		return 1
	}

	json_load "$(cat /etc/gcom/ncm.json)"
	json_select "$manufacturer"
	[ $? -ne 0 ] && {
		echo "Unsupported modem"
		proto_notify_error "$interface" UNSUPPORTED_MODEM
		proto_set_available "$interface" 0
		return 1
	}

	json_get_values initialize initialize
	for i in $initialize; do
		eval COMMAND="$i" gcom -d "$device" -s /etc/gcom/runcommand.gcom || {
			echo "Failed to initialize modem"
			proto_notify_error "$interface" INITIALIZE_FAILED
			return 1
		}
	done

	[ -n "$pincode" ] && {
		PINCODE="$pincode" gcom -d "$device" -s /etc/gcom/setpin.gcom || {
			echo "Unable to verify PIN"
			proto_notify_error "$interface" PIN_FAILED
			proto_block_restart "$interface"
			return 1
		}
	}

	json_get_values configure configure
	echo "Configuring modem"
	for i in $configure; do
		eval COMMAND="$i" gcom -d "$device" -s /etc/gcom/runcommand.gcom || {
			echo "Failed to configure modem"
			proto_notify_error "$interface" CONFIGURE_FAILED
			return 1
		}
	done

	[ -n "$mode" ] && {
		json_select modes
		json_get_var setmode "$mode"
		[ -n "$setmode" ] && {
			echo "Setting mode"
			eval COMMAND="$setmode" gcom -d "$device" -s /etc/gcom/runcommand.gcom || {
				echo "Failed to set operating mode"
				proto_notify_error "$interface" SETMODE_FAILED
				return 1
			}
		}
		json_select ..
	}

	echo "Starting network $interface"
	json_get_vars connect
	[ -n "$connect" ] && {
		echo "Connecting modem"
		eval COMMAND="$connect" gcom -d "$device" -s /etc/gcom/runcommand.gcom || {
			echo "Failed to connect"
			proto_notify_error "$interface" CONNECT_FAILED
			return 1
		}
	}

	json_get_vars finalize

	echo "Setting up $ifname"
	proto_init_update "$ifname" 1
	proto_add_data
	json_add_string "manufacturer" "$manufacturer"
	proto_close_data
	proto_send_update "$interface"

	local zone="$(fw3 -q network "$interface" 2>/dev/null)"

	[ "$pdptype" = "IP" -o "$pdptype" = "IPV4V6" ] && {
		json_init
		json_add_string name "${interface}_4"
		json_add_string ifname "@$interface"
		json_add_string proto "dhcp"
		proto_add_dynamic_defaults
		[ -n "$zone" ] && {
			json_add_string zone "$zone"
		}
		json_close_object
		ubus call network add_dynamic "$(json_dump)"
	}

	[ "$pdptype" = "IPV6" -o "$pdptype" = "IPV4V6" ] && {
		json_init
		json_add_string name "${interface}_6"
		json_add_string ifname "@$interface"
		json_add_string proto "dhcpv6"
		json_add_string extendprefix 1
		[ "$delegate" = "0" ] && json_add_boolean delegate "0"
		[ "$sourcefilter" = "0" ] && json_add_boolean sourcefilter "0"
		proto_add_dynamic_defaults
		[ -n "$zone" ] && {
			json_add_string zone "$zone"
		}
		json_close_object
		ubus call network add_dynamic "$(json_dump)"
	}

	[ -n "$finalize" ] && {
		eval COMMAND="$finalize" gcom -d "$device" -s /etc/gcom/runcommand.gcom || {
			echo "Failed to configure modem"
			proto_notify_error "$interface" FINALIZE_FAILED
			return 1
		}
	}
}

proto_ncm_teardown() {
	local interface="$1"

	local manufacturer disconnect

	local device profile
	json_get_vars device profile

	[ -n "$ctl_device" ] && device=$ctl_device

	[ -n "$device" ] || {
		echo "No control device specified"
		proto_notify_error "$interface" NO_DEVICE
		proto_set_available "$interface" 0
		return 1
	}

	device="$(readlink -f $device)"
	[ -e "$device" ] || {
		echo "Control device not valid"
		proto_set_available "$interface" 0
		return 1
	}

	[ -n "$profile" ] || profile=1

	echo "Stopping network $interface"

	json_load "$(ubus call network.interface.$interface status)"
	json_select data
	json_get_vars manufacturer
	[ $? -ne 0 -o -z "$manufacturer" ] && {
		# Fallback to direct detect, for proper handle device replug.
		manufacturer=$(gcom -d "$device" -s /etc/gcom/getcardinfo.gcom | awk 'NF && $0 !~ /AT\+CGMI/ { sub(/\+CGMI: /,""); print tolower($1); exit; }')
		[ $? -ne 0 -o -z "$manufacturer" ] && {
			echo "Failed to get modem information"
			proto_notify_error "$interface" GETINFO_FAILED
			return 1
		}
		json_add_string "manufacturer" "$manufacturer"
	}

	json_load "$(cat /etc/gcom/ncm.json)"
	json_select "$manufacturer" || {
		echo "Unsupported modem"
		proto_notify_error "$interface" UNSUPPORTED_MODEM
		return 1
	}

	json_get_vars disconnect
	[ -n "$disconnect" ] && {
		eval COMMAND="$disconnect" gcom -d "$device" -s /etc/gcom/runcommand.gcom || {
			echo "Failed to disconnect"
			proto_notify_error "$interface" DISCONNECT_FAILED
			return 1
		}
	}

	proto_init_update "*" 0
	proto_send_update "$interface"
}
[ -n "$INCLUDE_ONLY" ] || {
	add_protocol ncm
}
