#!/bin/sh

. /lib/functions.sh
. /lib/functions/network.sh

switch_names=""

warn() {
	echo "$@" >&2
}

clear_port_vlans() {
	local port=$1
	local self=$2
	local vlans=$(bridge vlan show dev "$port" | sed -ne 's#^[^ ]* \+\([0-9]\+\).*$#\1#p')

	local vlan
	for vlan in $vlans; do
		bridge vlan del vid "$vlan" dev "$port" $self
	done
}

lookup_switch() {
	local cfg=$1
	local swname

	config_get swname "$cfg" switch

	# Auto-determine switch if not specified ...
	if [ -z "$swname" ]; then
		case "$switch_names" in
			*\ *)
				warn "VLAN section '$cfg' does not specify a switch but multiple switches present, using first one"
				swname=${switch_names%% *}
			;;
			*)
				swname=${switch_names}
			;;
		esac

	# ... otherwise check if the referenced switch is declared
	else
		case " $switch_names " in
			*" $swname "*) : ;;
			*)
				warn "Switch '$swname' specified by VLAN section '$cfg' does not exist"
				return 1
			;;
		esac
	fi

	export -n "switch=$swname"
}

validate_vid() {
	local vid=$1

	case "$vid" in
		[1-9]|[1-9][0-9]|[1-9][0-9][0-9]|[1-4][0-9][0-9][0-9])
			if [ $vid -gt 4096 ]; then
				return 1
			fi
		;;
		*)
			return 1
		;;
	esac

	return 0
}

setup_switch() {
	local cfg=$1
	local cpu

	# Read configured CPU port from uci ...
	config_get cpu "$cfg" cpu_port

	# ... if unspecified, find first CPU port
	if [ -z "$cpu" ]; then
		local e
		for e in /sys/class/net/*/dsa; do
			if [ -d "$e" ]; then
				cpu=${e%/dsa}
				cpu=${cpu##*/}
				break
			fi
		done
	fi

	# Bail out if we cannot determine the CPU port
	if [ -z "$cpu" ]; then
		warn "Unable to determine CPU port for switch '$cfg'"
		return 1
	fi

	append switch_names "$cfg"

	# Prevent netifd from picking up our switch bridge just yet
	network_defer_device "$cfg"

	# Increase MTU of CPU port to 1508 to accomodate for VID + DSA tag
	ip link set "$cpu" mtu 1508
	ip link set "$cpu" up

	# (Re)create switch bridge device in case it is not yet set up
	local filtering=$(cat "/sys/class/net/$cfg/bridge/vlan_filtering" 2>/dev/null)
	if [ ${filtering:-0} != 1 ]; then
		ip link set "$cfg" down 2>/dev/null
		ip link delete dev "$cfg" 2>/dev/null
		ip link add name "$cfg" type bridge
		echo 1 > "/sys/class/net/$cfg/bridge/vlan_filtering"
	fi

	ip link set "$cfg" up

	# Unbridge DSA ports and flush any VLAN filters on them, they're added back later
	local port
	for port in /sys/class/net/*"/upper_${cfg}"; do
		if [ -e "$port" ]; then
			port=${port%/upper_*}
			port=${port##*/}

			ip link set "$port" nomaster

			# Unbridging the port should already clear VLANs, but be safe
			clear_port_vlans "$port"
		fi
	done

	# Clear any VLANs on the switch bridge, they're added back later
	clear_port_vlans "$cfg" self
}

setup_switch_vlan() {
	local cfg=$1
	local switch vlan ports

	config_get switch "$cfg" switch
	config_get vlan "$cfg" vlan
	config_get ports "$cfg" ports

	lookup_switch "$cfg" || return 1
	validate_vid "$vlan" || {
		warn "VLAN section '$cfg' specifies an invalid VLAN ID '$vlan'"
		return 1
	}

	# Setup ports
	local port tag pvid
	for port in $ports; do
		tag=${port#*.}
		port=${port%.*}
		pvid=

		if [ "$tag" != "$port" ] && [ "$tag" = t ]; then
			tag=tagged
		else
			tag=untagged
		fi

		# Add the port to the switch bridge and delete the default
		# VLAN 1 if it is not yet joined to the switch.
		if [ ! -e "/sys/class/net/$port/upper_$switch" ]; then
			ip link set dev "$port" up
			ip link set dev "$port" master "$switch"

			# Get rid of default VLAN 1
			bridge vlan del vid 1 dev "$port"
		fi

		# Promote the first untagged VLAN of this port to the PVID
		if [ "$tag" = untagged ] && ! bridge vlan show dev "$port" | grep -qi pvid; then
			pvid=pvid
		fi

		# Add VLAN filter entry for port
		bridge vlan add dev "$port" vid $vlan $pvid $tag
	done

	# Make the switch bridge itself handle the VLAN as well
	bridge vlan add dev "$switch" self vid $vlan tagged
}

setup_switch_port() {
	local cfg=$1
	local switch port pvid tag

	config_get port "$cfg" port
	config_get pvid "$cfg" pvid

	lookup_switch "$cfg" || return 1
	validate_vid "$pvid" || {
		warn "Port section '$cfg' specifies an invalid PVID '$pvid'"
		return 1
	}

	# Disallow setting the PVID of the switch bridge itself
	[ "$port" != "$switch" ] || {
		warn "Port section '$cfg' must not change PVID of the switch bridge"
		return 1
	}

	# Determine existing VLAN config
	local vlanspec=$(bridge vlan show dev "$port" vid "$pvid" 2>/dev/null | sed -ne2p)
	echo "$vlanspec" | grep -qi untagged && tag=untagged || tag=tagged

	bridge vlan add vid "$pvid" dev "$port" pvid $tag
}

apply_config() {
	config_load network
	config_foreach setup_switch dsa

	# If no switch is explicitely declared, synthesize switch0
	if [ -z "$switch_names" ] && ! setup_switch switch0; then
		warn "No DSA switches found"
		return 1
	fi

	config_foreach setup_switch_vlan dsa_vlan
	config_foreach setup_switch_port dsa_port

	# Ready switch bridge devices
	local switch
	for switch in $switch_names; do
		network_ready_device "$switch"
	done
}

show_switch() {
	local switch=$1

	printf "Switch: %s\n" "$switch"
	printf "VLAN/"

	local port ports
	for port in "/sys/class/net/$switch/lower_"*; do
		port=${port##*/lower_}


		printf " | %-5s" "$port"
		append ports "$port"
	done

	printf " |\nLink:"

	for port in $ports; do
		local carrier=$(cat "/sys/class/net/$port/carrier")
		local duplex=$(cat "/sys/class/net/$port/duplex")
		local speed=$(cat "/sys/class/net/$port/speed")

		if [ ${carrier:-0} -eq 0 ]; then
			printf " | %-5s" "down"
		else
			[ "$duplex" = "full" ] && duplex=F || duplex=H
			printf " | %4d%s" "$speed" "$duplex"
		fi
	done

	local vlans=$(bridge vlan show dev "$switch" | sed -ne 's#^[^ ]* \+\([0-9]\+\).*$#\1#p')
	local vlan
	for vlan in $vlans; do
		printf " |\n%4d " "$vlan"

		for port in $ports; do
			local pvid="" utag="" word
			for word in $(bridge vlan show dev "$port" vid "$vlan"); do
				case "$word" in
					PVID) pvid="*" ;;
					"$vlan") utag="t" ;;
					Untagged) utag="u" ;;
				esac
			done

			printf " |  %-2s  " "$utag$pvid"
		done
	done

	printf " |\n\n"
}

add_switch() {
	append switch_names "$1"
}

show_config() {
	config_load network
	config_foreach add_switch dsa

	local switch
	for switch in ${switch_names:-switch0}; do
		show_switch "$switch"
	done
}


case "$1" in
	show) show_config ;;
	apply) apply_config ;;
	*)
		echo "Usage: ${0##*/} show"
		echo "       ${0##*/} apply"
		exit 1
	;;
esac
