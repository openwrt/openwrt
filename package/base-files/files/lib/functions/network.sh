. /usr/share/libubox/jshn.sh

__network_switch_inactive()
{
	local __tmp

	if [ "$1" = 0 ] || [ "$1" = false ]; then
		return 1
	fi

	json_get_type __tmp "inactive"

	if [ "$__tmp" = object ]; then
		json_select "inactive"
	fi
}

__network_set_cache()
{
	__NETWORK_CACHE="${__NETWORK_CACHE:+$__NETWORK_CACHE }__NETWORK_CV_$1"
	eval "__NETWORK_CV_$1='\$$2'"
}

__network_get_cache()
{
	eval "[ -n \"\${__NETWORK_CV_$1+x}\" ] && export -- \"$2=\$__NETWORK_CV_$1\""
}

__network_ipaddr()
{
	local __var="$1"
	local __iface="$2"
	local __family="$3"
	local __prefix="$4"
	local __key="ipaddr_${2}_${3}_${4}"

	__network_get_cache "$__key" "$__var" && return 0

	local __tmp="$(ubus call network.interface."$__iface" status 2>/dev/null)"

	json_load "${__tmp:-{}}"
	json_get_type __tmp "ipv${__family}_address"

	if [ "$__tmp" = array ]; then

		json_select "ipv${__family}_address"
		json_get_type __tmp 1

		if [ "$__tmp" = object ]; then

			json_select 1
			json_get_var $__var address

			[ $__prefix -gt 0 ] && {
				json_get_var __tmp mask
				eval "export -- \"$__var=\${$__var}/$__tmp\""
			}

			__network_set_cache "$__key" "$__var"
			return 0
		fi
	fi

	return 1
}

# determine IPv4 address of given logical interface
# 1: destination variable
# 2: interface
network_get_ipaddr()  { __network_ipaddr "$1" "$2" 4 0; }

# determine IPv6 address of given logical interface
# 1: destination variable
# 2: interface
network_get_ipaddr6() { __network_ipaddr "$1" "$2" 6 0; }

# determine IPv4 subnet of given logical interface
# 1: destination variable
# 2: interface
network_get_subnet()  { __network_ipaddr "$1" "$2" 4 1; }

# determine IPv6 subnet of given logical interface
# 1: destination variable
# 2: interface
network_get_subnet6() { __network_ipaddr "$1" "$2" 6 1; }


__network_gateway()
{
	local __var="$1"
	local __iface="$2"
	local __family="$3"
	local __key="gateway_${2}_${3}"

	__network_get_cache "$__key" "$__var" && return 0

	local __tmp="$(ubus call network.interface."$__iface" status 2>/dev/null)"

	json_load "${__tmp:-{}}"

	for __tmp in 0 1; do

		if json_get_type __tmp route && [ "$__tmp" = array ]; then

			json_select route

			local __idx=1
			while json_get_type __tmp "$__idx" && [ "$__tmp" = object ]; do

				json_select "$((__idx++))"
				json_get_var __tmp target

				case "${__family}/${__tmp}" in
					4/0.0.0.0|6/::)
						json_get_var "$__var" nexthop
						__network_set_cache "$__key" "$__var"
						return $?
					;;
				esac

				json_select ".."

			done

			json_select ".."

		fi

		__network_switch_inactive "$4" || break

	done

	return 1
}

# determine IPv4 gateway of given logical interface
# 1: destination variable
# 2: interface
# 3: consider inactive gateway if "true" (optional)
network_get_gateway()  { __network_gateway "$1" "$2" 4 "${3:-0}"; }

# determine  IPv6 gateway of given logical interface
# 1: destination variable
# 2: interface
# 3: consider inactive gateway if "true" (optional)
network_get_gateway6() { __network_gateway "$1" "$2" 6 "${3:-0}"; }


__network_dns() {
	local __var="$1"
	local __iface="$2"
	local __field="$3"
	local __key="dns_${2}_${3}"

	__network_get_cache "$__key" "$__var" && return 0

	local __tmp="$(ubus call network.interface."$__iface" status 2>/dev/null)"
	local __dns=""

	json_load "${__tmp:-{}}"

	for __tmp in 0 1; do

		if json_get_type __tmp "$__field" && [ "$__tmp" = array ]; then

			json_select "$__field"

			local __idx=1
			while json_get_type __tmp "$__idx" && [ "$__tmp" = string ]; do

				json_get_var __tmp "$((__idx++))"
				__dns="${__dns:+$__dns }$__tmp"

			done

			json_select ".."
		fi

		__network_switch_inactive "$4" || break

	done

	if [ -n "$__dns" ]; then
		eval "export -- \"$__var=$__dns\""
		__network_set_cache "$__key" "$__var"
	fi
}

# determine the DNS servers of the given logical interface
# 1: destination variable
# 2: interface
# 3: consider inactive servers if "true" (optional)
network_get_dnsserver() { __network_dns "$1" "$2" dns_server "${3:-0}"; }

# determine the domains of the given logical interface
# 1: destination variable
# 2: interface
# 3: consider inactive servers if "true" (optional)
network_get_dnssearch() { __network_dns "$1" "$2" dns_search "${3:-0}"; }


__network_wan() {
	local __var="$1"
	local __family="$2"
	local __key="wan_${2}"
	local __iface

	__network_get_cache "$__key" "$__var" && return 0

	for __iface in $(ubus list | sed -ne 's/^network\.interface\.//p'); do
		if __network_gateway "$__var" "$__iface" "$__family"; then
			eval "export -- \"$__var=$__iface\""
			__network_set_cache "$__key" "$__var"
			return 0
		fi
	done

	eval "export -- \"$__var=\""
	return 1
}

# find the logical interface which holds the current IPv4 default route
# 1: destination variable
network_find_wan()  { __network_wan "$1" 4; }

# find the logical interface which holds the current IPv6 default route
# 1: destination variable
network_find_wan6() { __network_wan "$1" 6; }


__network_device()
{
	local __var="$1"
	local __iface="$2"
	local __field="$3"
	local __key="device_${2}_${3}"

	__network_get_cache "$__key" "$__var" && return 0

	local __tmp="$(ubus call network.interface."$__iface" status 2>/dev/null)"
	[ -n "$__tmp" ] || return 1

	json_load "$__tmp"
	json_get_var "$__var" "$__field" && __network_set_cache "$__key" "$__var"
}

# test whether the given logical interface is running
# 1: interface
network_is_up()
{
	local __up
	__network_device __up "$1" up && [ $__up -eq 1 ]
}

# determine the layer 3 linux network device of the given logical interface
# 1: destination variable
# 2: interface
network_get_device()  { __network_device "$1" "$2" l3_device; }

# determine the layer 2 linux network device of the given logical interface
# 1: destination variable
# 2: interface
network_get_physdev() { __network_device "$1" "$2" device;    }


__network_defer()
{
	local __device="$1"
	local __defer="$2"

	json_init
	json_add_string name "$__device"
	json_add_boolean defer "$__defer"

	ubus call network.device set_state "$(json_dump)" 2>/dev/null
}

# defer netifd actions on the given linux network device
# 1: device name
network_defer_device() { __network_defer "$1" 1; }

# continue netifd actions on the given linux network device
# 1: device name
network_ready_device() { __network_defer "$1" 0; }

# flush the internal value cache to force re-reading values from ubus
network_flush_cache()
{
	local __tmp
	for __tmp in $__NETWORK_CACHE __NETWORK_CACHE; do
		unset "$__tmp"
	done
}
