uint_max=4294967295

assert_uint32() {
    local __n="$1"

    if [ -z "$__n" -o -n "${__n//[0-9]/}" ]; then
	printf "Not a decimal integer (%s)\n" "$__n ">&2
	return 1
    fi

    if [ "$__n" -gt $uint_max ]; then
	printf "Out of range (%s)\n" "$__n" >&2
	return 1
    fi

    if [ "$((__n + 0))" != "$__n" ]; then
	printf "Not normalized notation (%s)\n" "$__n" >&2
	return 1
    fi

    return 0
}

bitcount() {
    local __var="$1" __c="$2"
    assert_uint32 "$__c" || return 1

    __c=$((((__c >> 1) & 0x55555555) + (__c & 0x55555555)))
    __c=$((((__c >> 2) & 0x33333333) + (__c & 0x33333333)))
    __c=$((((__c >> 4) & 0x0f0f0f0f) + (__c & 0x0f0f0f0f)))
    __c=$((((__c >> 8) & 0x00ff00ff) + (__c & 0x00ff00ff)))
    __c=$((((__c >> 16) & 0x0000ffff) + (__c & 0x0000ffff)))

    export -- "$__var=$__c"
}

# tedious but portable with busybox's limited shell
str2ip() {
    local __var="$1" __ip="$2" __n __val=0

    case "$__ip" in
    [0-9].*)
	__n="${__ip:0:1}"
	__ip="${__ip:2}"
	;;
    [1-9][0-9].*)
	__n="${__ip:0:2}"
	__ip="${__ip:3}"
	;;
    1[0-9][0-9].*|2[0-4][0-9].*|25[0-5].*)
	__n="${__ip:0:3}"
	__ip="${__ip:4}"
	;;
    *)
	printf "Not a dotted quad (%s)\n" "$2" >&2
	return 1
	;;
    esac

    __val=$((__n << 24))

    case "$__ip" in
    [0-9].*)
	__n="${__ip:0:1}"
	__ip="${__ip:2}"
	;;
    [1-9][0-9].*)
	__n="${__ip:0:2}"
	__ip="${__ip:3}"
	;;
    1[0-9][0-9].*|2[0-4][0-9].*|25[0-5].*)
	__n="${__ip:0:3}"
	__ip="${__ip:4}"
	;;
    *)
	printf "Not a dotted quad (%s)\n" "$2" >&2
	return 1
	;;
    esac

    __val=$((__val + (__n << 16)))

    case "$__ip" in
    [0-9].*)
	__n="${__ip:0:1}"
	__ip="${__ip:2}"
	;;
    [1-9][0-9].*)
	__n="${__ip:0:2}"
	__ip="${__ip:3}"
	;;
    1[0-9][0-9].*|2[0-4][0-9].*|25[0-5].*)
	__n="${__ip:0:3}"
	__ip="${__ip:4}"
	;;
    *)
	printf "Not a dotted quad (%s)\n" "$2" >&2
	return 1
	;;
    esac

    __val=$((__val + (__n << 8)))

    case "$__ip" in
    [0-9])
	__n="${__ip:0:1}"
	__ip="${__ip:1}"
	;;
    [1-9][0-9])
	__n="${__ip:0:2}"
	__ip="${__ip:2}"
	;;
    1[0-9][0-9]|2[0-4][0-9]|25[0-5])
	__n="${__ip:0:3}"
	__ip="${__ip:3}"
	;;
    *)
	printf "Not a dotted quad (%s)\n" "$2" >&2
	return 1
	;;
    esac

    __val=$((__val + __n))

    if [ -n "$__ip" ]; then
	printf "Not a dotted quad (%s)\n" "$2" >&2
	return 1
    fi

    export -- "$__var=$__val"
    return 0
}

ip2str() {
    local __var="$1" __n="$2"
    assert_uint32 "$__n" || return 1

    export -- "$__var=$((__n >> 24)).$(((__n >> 16) & 255)).$(((__n >> 8) & 255)).$((__n & 255))"
}

