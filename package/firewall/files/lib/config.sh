# Copyright (C) 2009-2010 OpenWrt.org
# Copyright (C) 2009 Malte S. Stretz <http://msquadrat.de>
#
# This is a temporary file, I hope to have some of this stuff merged into
# /lib/functions.sh (without the fw_ prefix of course) one day.

fw_config_append() { # <package>
	CONFIG_APPEND=1 config_load "$@"
	unset CONFIG_APPEND
}

fw_config_once() { # <function> <type>
	local func=$1
	local type=$2
	shift 2

	local config=cfg00nil
	fw_config__once() {
		config=$1
	}
	config_foreach fw_config__once "$type"

	$func $config "$@"
}

fw_config_get_section() { # <config> <prefix> <type> <name> <default> ...
	local config=$1
	local prefix=$2
	shift 2

	[ -n "$config" ] || return 1
	[ -n "$prefix" ] && {
		prefix="${prefix}_"
		export ${NO_EXPORT:+-n} -- "${prefix}NAME"="${config}"
		config_get "${prefix}TYPE" "$config" TYPE
	}
	
	[ "$1" == '{' ] && shift
	while [ $# -ge 3 ]; do
		local type=$1
		local name=$2
		local dflt=$3
		shift 3
		# TODO: Move handling of defaults to /lib/functions.sh
		# and get replace the case block with the following 
		# two lines:
		# type=${type#string}
		# config_get${type:+_${type}} "${prefix}${name}" "$config" "$name" "$dflt" || return
		case "$type" in
			string)
				local tmp
				config_get tmp "$config" "$name" || return
				[ -z "$tmp" ] && tmp=$dflt
				export ${NO_EXPORT:+-n} -- "${prefix}${name}=${tmp}"
				continue
			;;
			boolean)
				type=bool
			;;
		esac;
		
		local cmd=${prefix}config_get_${type}
		type $cmd > /dev/null || {
			cmd=config_get_${type} 
		}
		type $cmd > /dev/null || {
			echo "config type $type (for $name) not supported" >&2
			return 1
		}
		$cmd "${prefix}${name}" "$config" "$name" "$dflt" || return
	done
}

config_get_ipaddr() {
	local varn=$1
	local conf=$2
	local name=$3
	local dflt=$4

	local addr
	config_get addr "$conf" "$name" || return
	[ -n "$addr" ] || addr=$dflt

	local mask=${addr#*/}
	[ "$mask" != "$addr" ] || mask=
	addr=${addr%/*}

	local vers=
	case "$addr" in
		*:*) vers=6; mask="${mask:-128}" ;;
		*.*) vers=4; mask="${mask:-32}" ;;
	esac
	
	export ${NO_EXPORT:+-n} -- "${varn}=${addr}"
	export ${NO_EXPORT:+-n} -- "${varn}_prefixlen=${mask}"
	export ${NO_EXPORT:+-n} -- "${varn}_version=${vers}"
}
