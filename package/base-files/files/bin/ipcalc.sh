#!/bin/sh

. /lib/functions/ipv4.sh

PROG="$(basename "$0")"

# wrapper to convert an integer to an address, unless we're using
# decimal output format.
# hook for library function
_ip2str() {
    local var="$1" n="$2"
    assert_uint32 "$n" || exit 1

    if [ "$decimal" -ne 0 ]; then
	export -- "$var=$n"
    elif [ "$hexadecimal" -ne 0 ]; then
	export -- "$var=$(printf "%x" "$n")"
    else
        ip2str "$@"
    fi
}

usage() {
    echo "Usage: $PROG [ -d | -x ] address/prefix [ start limit ]" >&2
    exit 1
}

decimal=0
hexadecimal=0
if [ "$1" = "-d" ]; then
    decimal=1
    shift
elif [ "$1" = "-x" ]; then
    hexadecimal=1
    shift
fi

if [ $# -eq 0 ]; then
    usage
fi

case "$1" in
*/*.*)
    # data is n.n.n.n/m.m.m.m format, like on a Cisco router
    str2ip ipaddr "${1%/*}" || exit 1
    str2ip netmask "${1#*/}" || exit 1
    shift
    ;;
*/*)
    # more modern prefix notation of n.n.n.n/p
    str2ip ipaddr "${1%/*}" || exit 1
    prefix="${1#*/}"
    assert_uint32 "$prefix" || exit 1
    if [ "$prefix" -gt 32 ]; then
	printf "Prefix out of range (%s)\n" "$prefix" >&2
	exit 1
    fi
    prefix2netmask netmask "$prefix" || exit 1
    shift
    ;;
*)
    # address and netmask as two separate arguments
    str2ip ipaddr "$1" || exit 1
    str2ip netmask "$2" || exit 1
    shift 2
    ;;
esac

# we either have no arguments left, or we have a range start and length
if [ $# -ne 0 ] && [ $# -ne 2 ]; then
    usage
fi

if ! bitcount prefix "$netmask"; then
    printf "Invalid netmask (%s)\n" "$netmask" >&2
    exit 1
fi

# complement of the netmask, i.e. the hostmask
hostmask=$((netmask ^ 0xffffffff))
network=$((ipaddr & netmask))
broadcast=$((network | hostmask))
count=$((hostmask + 1))

_ip2str IP "$ipaddr"
_ip2str NETMASK "$netmask"
_ip2str NETWORK "$network"

echo "IP=$IP"
echo "NETMASK=$NETMASK"
# don't include this-network or broadcast addresses
if [ "$prefix" -le 30 ]; then
    _ip2str BROADCAST "$broadcast"
    echo "BROADCAST=$BROADCAST"
fi
echo "NETWORK=$NETWORK"
echo "PREFIX=$prefix"
echo "COUNT=$count"

# if there's no range, we're done
[ $# -eq 0 ] && exit 0

if [ "$prefix" -le 30 ]; then
    lower=$((network + 1))
else
    lower="$network"
fi

start="$1"
assert_uint32 "$start" || exit 1
start=$((network | (start & hostmask)))
[ "$start" -lt "$lower" ] && start="$lower"
[ "$start" -eq "$ipaddr" ] && start=$((start + 1))

if [ "$prefix" -le 30 ]; then
    upper=$(((network | hostmask) - 1))
else
    upper="$network"
fi

range="$2"
assert_uint32 "$range" || exit 1
end=$((start + range - 1))
[ "$end" -gt "$upper" ] && end="$upper"
[ "$end" -eq "$ipaddr" ] && end=$((end - 1))

if [ "$start" -gt "$end" ]; then
    echo "network ($NETWORK/$prefix) too small" >&2
    exit 1
fi

_ip2str START "$start"
_ip2str END "$end"

if [ "$start" -le "$ipaddr" ] && [ "$ipaddr" -le "$end" ]; then
    echo "error: address $IP inside range $START..$END" >&2
    exit 1
fi

echo "START=$START"
echo "END=$END"

exit 0
