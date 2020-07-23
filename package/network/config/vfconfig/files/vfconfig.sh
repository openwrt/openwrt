#!/bin/sh

. /lib/functions.sh
. /lib/functions/network.sh

bridge_names=""

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

add_bridge() {
	local cfg=$1
	local brname

	config_get brname "$cfg" bridge "switch0"

	case " $bridge_names " in
		*" $brname "*) return 1 ;;
	esac

	append bridge_names "$brname"

	export -n "bridge=$brname"
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

setup_bridge() {
	local cfg=$1
	local bridge

	add_bridge "$cfg" || return 0

	# Prevent netifd from picking up our switch bridge just yet
	network_defer_device "$bridge"

	# (Re)create switch bridge device in case it is not yet set up
	local filtering=$(cat "/sys/class/net/$bridge/bridge/vlan_filtering" 2>/dev/null)
	if [ ${filtering:-0} != 1 ]; then
		ip link set "$bridge" down 2>/dev/null
		ip link delete dev "$bridge" 2>/dev/null
		ip link add name "$bridge" type bridge
		echo 1 > "/sys/class/net/$bridge/bridge/vlan_filtering"
	fi

	ip link set "$bridge" up

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
	clear_port_vlans "$bridge" self
}

setup_bridge_vlan() {
	local cfg=$1
	local bridge vlan ports

	config_get bridge "$cfg" bridge "switch0"
	config_get vlan "$cfg" vlan
	config_get ports "$cfg" ports

	validate_vid "$vlan" || {
		warn "VLAN section '$cfg' specifies an invalid VLAN ID '$vlan'"
		return 1
	}

	# Setup ports
	local port tag pvid
	for port in $ports; do
		tag=untagged
		pvid=

		case "$port" in
			*"*")
				pvid=pvid
				port=${port%\*}
			;;
		esac

		case "$port" in
			*:u)
				port=${port%:u}
			;;
			*:t)
				tag=tagged
				port=${port%:t}
			;;
		esac

		# Add the port to the switch bridge and delete the default
		# VLAN 1 if it is not yet joined to the bridge.
		if [ ! -e "/sys/class/net/$port/upper_$bridge" ]; then
			ip link set dev "$port" up
			ip link set dev "$port" master "$bridge"

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
	bridge vlan add dev "$bridge" self vid $vlan tagged
}

apply_config() {
	config_load network
	config_foreach setup_bridge vlan_filter
	config_foreach setup_bridge_vlan vlan_filter

	# Ready switch bridge devices
	local bridge
	for bridge in $bridge_names; do
		network_ready_device "$bridge"
	done
}

show_bridge() {
	local cfg=$1
	local bridge

	add_bridge "$cfg" || return 0

	printf "Bridge: %s\n" "$bridge"
	printf "VLAN/"

	local port ports
	for port in "/sys/class/net/$bridge/lower_"*; do
		[ -e "$port" ] || continue

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

	local vlans=$(bridge vlan show dev "$bridge" | sed -ne 's#^[^ ]* \+\([0-9]\+\).*$#\1#p')
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

show_config() {
	config_load network
	config_foreach show_bridge vlan_filter
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
