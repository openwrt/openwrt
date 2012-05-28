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


network_get_device()
{
	local __var="$1"
	local __iface="$2"

	local __tmp="$(ubus call network.interface."$__iface" status 2>/dev/null)"
	[ -n "$__tmp" ] || return 1

	json_load "$__tmp"
	json_get_var "$__var" device

	return 0
}
