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
	proto_config_add_int linksettle
	proto_config_add_int attachwait
	proto_config_add_string mode
	proto_config_add_string pdptype
	proto_config_add_boolean sourcefilter
	proto_config_add_boolean delegate
	proto_config_add_int profile
	proto_config_add_int mtu
	proto_config_add_defaults
}

# Wait for carrier to settle after connecting: modems often drop it again
# briefly while finalising, and the PDP address can change across that flap.
ncm_wait_link() {
	local ifname="$1" settle="$2"
	local stable=0 waited=0 carrier timeout

	# must outlast settle
	timeout=$((settle * 3))
	[ "$timeout" -lt 30 ] && timeout=30

	/sbin/ip link set dev "$ifname" up 2>/dev/null

	while [ "$waited" -lt "$timeout" ]; do
		# unreadable (netdev down, no carrier support): nothing to wait for
		carrier=$(cat "/sys/class/net/$ifname/carrier" 2>/dev/null) || return 0

		[ "$carrier" = "1" ] && stable=$((stable + 1)) || stable=0

		sleep 1
		waited=$((waited + 1))

		# after the sleep, so settle counts seconds
		[ "$stable" -ge "$settle" ] && return 0
	done

	return 1
}

# Ask the modem for one identity string, e.g. CGMI for the manufacturer.
ncm_query_id() {
	local device="$1" script="$2" tag="$3"
	local val

	val=$(gcom -d "$device" -s "/etc/gcom/$script.gcom" | awk -v RS='\r?\n' -v tag="$tag" '
		# echo of any AT command; remember whether it was the one we sent
		$1 ~ /^AT[+*^#$]/ { seen = ($0 ~ ("AT\\+" tag)); next }
		sub(("\\+" tag ": "), "") { print tolower($1); done = 1; exit }
		# unsolicited result codes and bare status lines are not an answer
		($1 ~ /^[+*^]/ || $1 == "OK" || $1 == "ERROR" || $1 == "RDY") { next }
		NF {
			if (seen) { print tolower($1); done = 1; exit }
			if (fallback == "") fallback = tolower($1)
		}
		# no echo (ATE0): fall back to the first line that looked like an answer
		END { if (!done && fallback != "") print fallback }')

	echo "$val"
}

# Is there any "<manufacturer>-<model>" entry for this vendor at all? If not,
# there is nothing to be gained by waiting for the model.
ncm_has_model_entry() {
	local manufacturer="$1"
	local keys key

	# json_get_keys reports the names with anything but a letter or a digit
	# replaced, so "quectel-eg060w" arrives as "quectel_eg060w"
	json_get_keys keys
	for key in $keys; do
		case "$key" in
		"$manufacturer"-*|"$manufacturer"_*) return 0 ;;
		esac
	done

	return 1
}

# Wait for the modem to attach to the packet domain before dialling. A modem
# that never reports an attach state is not held up - but only once the wait
# is over, because a single probe can come back empty just from missing the
# read window of runquery.gcom.
ncm_wait_attach() {
	local device="$1" deadline="$2"
	local reply state answered=0

	while :; do
		reply=$(COMMAND="AT+CGATT?" gcom -d "$device" -s /etc/gcom/runquery.gcom)

		# the modem rejects AT+CGATT?: there is nothing to wait for
		case "$reply" in
		*ERROR*|*"NOT SUPPORT"*) return 0 ;;
		esac

		state=$(echo "$reply" | awk -v RS='\r?\n' \
			'/\+CGATT: [0-9]/ { sub(/.*\+CGATT: /, ""); print $1 + 0; exit }')
		[ -n "$state" ] && answered=1
		[ "$state" = 1 ] && return 0

		[ "$(date +%s)" -lt "$deadline" ] || break
		sleep 1
	done

	# it never said anything about the packet domain: do not hold up the dial
	[ "$answered" = 0 ] && {
		echo "Modem does not report an attach state, dialling anyway"
		return 0
	}

	return 1
}

# Pick the ncm.json entry: "<manufacturer>-<model>" wins over the plain
# manufacturer one. json_is_a() avoids a warning when there is no such entry.
ncm_select_modem() {
	local manufacturer="$1" model="$2"

	[ -n "$model" ] && json_is_a "$manufacturer-$model" object && {
		json_select "$manufacturer-$model"
		return 0
	}

	json_is_a "$manufacturer" object || return 1
	json_select "$manufacturer"
}

proto_ncm_setup() {
	local interface="$1"

	local attachwait_default connect context_type devname devpath finalize ifpath initialize linkatfinalize manufacturer model setmode

	local delegate ip4table ip6table mtu sourcefilter $PROTO_DEFAULT_OPTIONS
	json_get_vars delegate ip4table ip6table mtu sourcefilter $PROTO_DEFAULT_OPTIONS

	local apn attachwait auth delay device ifname linksettle mode password pdptype pincode profile username
	json_get_vars apn attachwait auth delay device ifname linksettle mode password pdptype pincode profile username

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

	# a modem that was just powered up takes a few seconds before it answers,
	# so keep asking until it identifies itself
	local deadline=$(($(date +%s) + ${delay:-20}))
	while true; do
		manufacturer=$(ncm_query_id "$device" getcardinfo CGMI)
		model=$(ncm_query_id "$device" getmodel CGMM)
		# drop the region/SKU suffix: EG060W-EAAA -> eg060w
		model=${model%%-*}

		json_load "$(cat /etc/gcom/ncm.json)"
		if [ -n "$manufacturer" ]; then
			if [ -n "$model" ] || ! ncm_has_model_entry "$manufacturer"; then
				ncm_select_modem "$manufacturer" "$model" && break
			fi
		fi

		[ "$(date +%s)" -lt "$deadline" ] && {
			sleep 1
			continue
		}

		[ -n "$manufacturer" ] || {
			echo "Failed to get modem information"
			proto_notify_error "$interface" GETINFO_FAILED
			return 1
		}

		# out of time: a modem that never reports a model still gets its
		# manufacturer entry
		ncm_select_modem "$manufacturer" "$model" && break

		echo "Unsupported modem (manufacturer '$manufacturer', model '$model')"
		proto_notify_error "$interface" UNSUPPORTED_MODEM
		proto_set_available "$interface" 0
		return 1
	done

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

	# ncm.json sets the default for profiles that must not be gated
	json_get_var attachwait_default attachwait

	[ -n "$attachwait" ] || attachwait="${attachwait_default:-10}"
	[ "$attachwait" -gt 0 ] && {
		ncm_wait_attach "$device" $(($(date +%s) + attachwait)) || {
			echo "Modem did not attach to the network"
			proto_notify_error "$interface" NETWORK_REGISTRATION_FAILED
			return 1
		}
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

	json_get_vars finalize linkatfinalize

	# nothing to wait for yet when the profile only enters data state at
	# finalize, further down
	[ -n "$linksettle" ] || linksettle=3
	[ "$linksettle" -gt 0 ] && [ -z "$linkatfinalize" ] && {
		ncm_wait_link "$ifname" "$linksettle" || \
			echo "Link did not settle, continuing anyway"
	}

	echo "Setting up $ifname"
	proto_init_update "$ifname" 1
	proto_add_data
	json_add_string "manufacturer" "$manufacturer"
	json_add_string "model" "$model"
	proto_close_data
	proto_send_update "$interface"

	local zone="$(fw3 -q network "$interface" 2>/dev/null)"

	[ "$pdptype" = "IP" -o "$pdptype" = "IPV4V6" ] && {
		json_init
		json_add_string name "${interface}_4"
		json_add_string ifname "@$interface"
		json_add_string proto "dhcp"
		proto_add_dynamic_defaults
		[ -n "$zone" ] && json_add_string zone "$zone"
		[ -n "$ip4table" ] && json_add_string ip4table "$ip4table"

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
		[ -n "$zone" ] && json_add_string zone "$zone"
		[ -n "$ip6table" ] && json_add_string ip6table "$ip6table"

		json_close_object
		ubus call network add_dynamic "$(json_dump)"
	}

	[ -n "$mtu" -a "$mtu" != 0 ] && {
		echo "Setting MTU of $ifname to $mtu"
		/sbin/ip link set dev $ifname mtu $mtu
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

	local manufacturer model disconnect

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
	json_get_vars manufacturer model
	[ $? -ne 0 -o -z "$manufacturer" ] && {
		# Fallback to direct detect, for proper handle device replug.
		manufacturer=$(ncm_query_id "$device" getcardinfo CGMI)
		[ $? -ne 0 -o -z "$manufacturer" ] && {
			echo "Failed to get modem information"
			proto_notify_error "$interface" GETINFO_FAILED
			return 1
		}
		# model too, or we fall back to the vendor entry on this path
		model=$(ncm_query_id "$device" getmodel CGMM)
		model=${model%%-*}

		json_add_string "manufacturer" "$manufacturer"
		json_add_string "model" "$model"
	}

	json_load "$(cat /etc/gcom/ncm.json)"
	ncm_select_modem "$manufacturer" "$model" || {
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
