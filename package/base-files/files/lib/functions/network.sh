. /usr/share/libubox/jshn.sh

__network_ipaddr()
{
	local __var="$1"
	local __iface="$2"
	local __family="$3"
	local __prefix="${4:-0}"

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

			return 0
		fi
	fi

	return 1
}

network_get_ipaddr()  { __network_ipaddr "$1" "$2" 4 0; }
network_get_ipaddr6() { __network_ipaddr "$1" "$2" 6 0; }

network_get_subnet()  { __network_ipaddr "$1" "$2" 4 1; }
network_get_subnet6() { __network_ipaddr "$1" "$2" 6 1; }


__network_gateway()
{
	local __var="$1"
	local __iface="$2"
	local __family="$3"

	local __tmp="$(ubus call network.interface."$__iface" status 2>/dev/null)"
	local __idx=1

	json_load "${__tmp:-{}}"

	if json_get_type __tmp route && [ "$__tmp" = array ]; then

		json_select route

		while json_get_type __tmp "$__idx" && [ "$__tmp" = object ]; do

			json_select "$((__idx++))"
			json_get_var __tmp target

			case "${__family}/${__tmp}" in
				4/0.0.0.0|6/::)
					json_get_var "$__var" nexthop
					return $?
				;;
			esac

			json_select ".."

		done
	fi

	return 1
}

network_get_gateway()  { __network_gateway "$1" "$2" 4; }
network_get_gateway6() { __network_gateway "$1" "$2" 6; }


__network_dns() {
	local __var="$1"
	local __iface="$2"
	local __field="$3"

	local __tmp="$(ubus call network.interface."$__iface" status 2>/dev/null)"
	local __dns=""
	local __idx=1

	json_load "${__tmp:-{}}"

	if json_get_type __tmp "$__field" && [ "$__tmp" = array ]; then

		json_select "$__field"

		while json_get_type __tmp "$__idx" && [ "$__tmp" = string ]; do

			json_get_var __tmp "$((__idx++))"
			__dns="${__dns:+$__dns }$__tmp"

		done
	fi

	eval "export -- \"$__var=$__dns\""
	[ -n "$__dns" ]
}

network_get_dnsserver() { __network_dns "$1" "$2" dns_server; }
network_get_dnssearch() { __network_dns "$1" "$2" dns_search; }


__network_wan() {
	local __var="$1"
	local __family="$2"
	local __iface

	for __iface in $(ubus list | sed -ne 's/^network\.interface\.//p'); do
		if __network_gateway "$__var" "$__iface" "$__family"; then
			eval "export -- \"$__var=$__iface\""
			return 0
		fi
	done

	eval "export -- \"$__var=\""
	return 1
}

network_find_wan()  { __network_wan "$1" 4; }
network_find_wan6() { __network_wan "$1" 6; }


__network_device()
{
	local __var="$1"
	local __iface="$2"
	local __field="$3"

	local __tmp="$(ubus call network.interface."$__iface" status 2>/dev/null)"
	[ -n "$__tmp" ] || return 1

	json_load "$__tmp"
	json_get_var "$__var" "$__field"
}

network_is_up()
{
	local __up
	__network_device __up "$1" up && [ $__up -eq 1 ]
}

network_get_device()  { __network_device "$1" "$2" l3_device; }
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

network_defer_device() { __network_defer "$1" 1; }
network_ready_device() { __network_defer "$1" 0; }
