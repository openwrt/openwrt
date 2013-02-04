# Copyright (C) 2009-2010 OpenWrt.org
# Copyright (C) 2009 Malte S. Stretz

export FW_4_ERROR=0
export FW_6_ERROR=0
export FW_i_ERROR=0
export FW_e_ERROR=0
export FW_a_ERROR=0

#TODO: remove this
[ "${-#*x}" == "$-" ] && {
	fw() {
		fw__exec "$@"
	}
} || {
	fw() {
		local os=$-
		set +x
		fw__exec "$@"
		local rc=$?
		set -$os
		return $rc
	}
}

fw__exec() { # <action> <family> <table> <chain> <target> <position> { <rules> }
	local cmd fam tab chn tgt pos
	local i
	for i in cmd fam tab chn tgt pos; do
		if [ "$1" -a "$1" != '{' ]; then
			eval "$i='$1'"
			shift
		else
			eval "$i=-"
		fi
	done

	fw__rc() {
		export FW_${fam#G}_ERROR=$1
		return $1
	}

	fw__dualip() {
		fw $cmd 4 $tab $chn $tgt $pos "$@"
		fw $cmd 6 $tab $chn $tgt $pos "$@"
		fw__rc $((FW_4_ERROR | FW_6_ERROR))
	}

	fw__autoip() {
		local ip4 ip6
		shift
		while [ "$1" != '}' ]; do
			case "$1" in
				*:*) ip6=1 ;;
				*.*.*.*) ip4=1 ;;
			esac
			shift
		done
		shift
		if [ "${ip4:-4}" == "${ip6:-6}" ]; then
			echo "fw: can't mix ip4 and ip6" >&2
 			return 1
		fi
		local ver=${ip4:+4}${ip6:+6}
		fam=i
		fw $cmd ${ver:-i} $tab $chn $tgt $pos "$@"
		fw__rc $?
	}

	fw__has() {
		local tab=${1:-$tab}
		if [ $tab == '-' ]; then
			type $app > /dev/null 2> /dev/null
			fw__rc $(($? & 1))
			return
		fi
		[ "$app" != ip6tables ] || [ "$tab" != nat ]
		fw__rc $?
	}

	fw__err() {
		local err
		eval "err=\$FW_${fam}_ERROR"
		fw__rc $err
	}

	local app=
	local pol=
	case "$fam" in
		*4) [ $FW_DISABLE_IPV4 == 0 ] && app=iptables  || return ;;
		*6) [ $FW_DISABLE_IPV6 == 0 ] && app=ip6tables || return ;;
		i) fw__dualip "$@"; return ;;
		I) fw__autoip "$@"; return ;;
		e) app=ebtables ;;
		a) app=arptables ;;
		-) fw $cmd i $tab $chn $tgt $pos "$@"; return ;;
		*) return 254 ;;
	esac
	case "$tab" in
		f) tab=filter ;;
		m) tab=mangle ;;
		n) tab=nat ;;
		r) tab=raw ;;
		-) tab=filter ;;
	esac
	case "$cmd:$chn:$tgt:$pos" in
		add:*:-:*) cmd=new-chain ;;
		add:*:*:-) cmd=append ;;
		add:*:*:$) cmd=append ;;
		add:*:*:*) cmd=insert ;;
		del:-:*:*) cmd=delete-chain; fw flush $fam $tab ;;
		del:*:-:*) cmd=delete-chain; fw flush $fam $tab $chn ;;
		del:*:*:*) cmd=delete ;;
		flush:*) ;;
		policy:*) pol=$tgt; tgt=- ;;
		has:*) fw__has; return ;;
		err:*) fw__err; return ;;
		list:*) cmd="numeric --verbose --$cmd" ;;
		*) return 254 ;;
	esac
	case "$chn" in
		-) chn= ;;
	esac
	case "$tgt" in
		-) tgt= ;;
	esac

	local rule_offset
	case "$pos" in
		^) pos=1 ;;
		$) pos= ;;
		-) pos= ;;
		+) eval "rule_offset=\${FW__RULE_OFS_${app}_${tab}_${chn}:-1}" ;;
	esac

	if ! fw__has - family || ! fw__has $tab ; then
		export FW_${fam}_ERROR=0
		return 0
	fi

	case "$fam" in
		G*) shift; while [ $# -gt 0 ] && [ "$1" != "{" ]; do shift; done ;;
	esac

	if [ $# -gt 0 ]; then
		shift
		if [ $cmd == delete ]; then
			pos=
		fi
	fi

	local cmdline="$app --table ${tab} --${cmd} ${chn} ${pol} ${rule_offset:-${pos}} ${tgt:+--jump "$tgt"}"
	while [ $# -gt 1 ]; do
		# special parameter handling
		case "$1:$2" in
			-p:icmp*|-p:1|-p:58|--protocol:icmp*|--protocol:1|--protocol:58)
				[ "$app" = ip6tables ] && \
					cmdline="$cmdline -p icmpv6" || \
					cmdline="$cmdline -p icmp"
				shift
			;;
			--icmp-type:*|--icmpv6-type:*)
				local icmp_type
				if [ "$app" = ip6tables ] && fw_check_icmptype6 icmp_type "$2"; then
					cmdline="$cmdline $icmp_type"
				elif [ "$app" = iptables ] && fw_check_icmptype4 icmp_type "$2"; then
					cmdline="$cmdline $icmp_type"
				else
					local fam=IPv4; [ "$app" = ip6tables ] && fam=IPv6
					fw_log info "ICMP type '$2' is not valid for $fam address family, skipping rule"
					return 1
				fi
				shift
			;;
			*) cmdline="$cmdline $1" ;;
		esac
		shift
	done

	[ -n "$FW_TRACE" ] && echo $cmdline >&2

	$cmdline

	local rv=$?
	[ $rv -eq 0 ] && [ -n "$rule_offset" ] && \
		export -- "FW__RULE_OFS_${app}_${tab}_${chn}=$(($rule_offset + 1))"
	fw__rc $rv
}

fw_get_port_range() {
	local _var=$1
	local _ports=$2
	local _delim=${3:-:}
	if [ "$4" ]; then
		fw_get_port_range $_var "${_ports}-${4}" $_delim
		return
	fi

	local _first=${_ports%-*}
	local _last=${_ports#*-}
	if [ "${_first#!}" != "${_last#!}" ]; then
		export -- "$_var=$_first$_delim${_last#!}"
	else
		export -- "$_var=$_first"
	fi
}

fw_get_family_mode() {
	local _var="$1"
	local _hint="$2"
	local _zone="$3"
	local _mode="$4"

	local _ipv4 _ipv6
	[ "$_zone" != "*" ] && {
		[ -n "$FW_ZONES4$FW_ZONES6" ] && {
			list_contains FW_ZONES4 "$_zone" && _ipv4=1 || _ipv4=0
			list_contains FW_ZONES6 "$_zone" && _ipv6=1 || _ipv6=0
		} || {
			_ipv4=$(uci_get_state firewall core "${_zone}_ipv4" 0)
			_ipv6=$(uci_get_state firewall core "${_zone}_ipv6" 0)
		}
	} || {
		_ipv4=1
		_ipv6=1
	}

	case "$_hint:$_ipv4:$_ipv6" in
		*4:1:*|*:1:0) export -n -- "$_var=G4" ;;
		*6:*:1|*:0:1) export -n -- "$_var=G6" ;;
		*) export -n -- "$_var=$_mode" ;;
	esac
}

fw_get_negation() {
	local _var="$1"
	local _flag="$2"
	local _value="$3"

	[ "${_value#!}" != "$_value" ] && \
		export -n -- "$_var=! $_flag ${_value#!}" || \
		export -n -- "$_var=${_value:+$_flag $_value}"
}

fw_get_subnet4() {
	local _var="$1"
	local _flag="$2"
	local _name="$3"

	local _ipaddr="$(uci_get_state network "${_name#!}" ipaddr)"
	local _netmask="$(uci_get_state network "${_name#!}" netmask)"

	case "$_ipaddr" in
		*.*.*.*)
			[ "${_name#!}" != "$_name" ] && \
				export -n -- "$_var=! $_flag $_ipaddr/${_netmask:-255.255.255.255}" || \
				export -n -- "$_var=$_flag $_ipaddr/${_netmask:-255.255.255.255}"
			return 0
		;;
	esac

	export -n -- "$_var="
	return 1
}

fw_check_icmptype4() {
	local _var="$1"
	local _type="$2"
	case "$_type" in
		![0-9]*) export -n -- "$_var=! --icmp-type ${_type#!}"; return 0 ;;
		[0-9]*)  export -n -- "$_var=--icmp-type $_type";       return 0 ;;
	esac

	[ -z "$FW_ICMP4_TYPES" ] && \
		export FW_ICMP4_TYPES=$(
			iptables -p icmp -h 2>/dev/null | \
			sed -n -e '/^Valid ICMP Types:/ {
				n; :r; s/[()]/ /g; s/[[:space:]]\+/\n/g; p; n; b r
			}' | sort -u
		)

	local _check
	for _check in $FW_ICMP4_TYPES; do
		if [ "$_check" = "${_type#!}" ]; then
			[ "${_type#!}" != "$_type" ] && \
				export -n -- "$_var=! --icmp-type ${_type#!}" || \
				export -n -- "$_var=--icmp-type $_type"
			return 0
		fi
	done

	export -n -- "$_var="
	return 1
}

fw_check_icmptype6() {
	local _var="$1"
	local _type="$2"
	case "$_type" in
		![0-9]*) export -n -- "$_var=! --icmpv6-type ${_type#!}"; return 0 ;;
		[0-9]*)  export -n -- "$_var=--icmpv6-type $_type";       return 0 ;;
	esac

	[ -z "$FW_ICMP6_TYPES" ] && \
	 	export FW_ICMP6_TYPES=$(
	 		ip6tables -p icmpv6 -h 2>/dev/null | \
	 		sed -n -e '/^Valid ICMPv6 Types:/ {
	 			n; :r; s/[()]/ /g; s/[[:space:]]\+/\n/g; p; n; b r
	 		}' | sort -u
	 	)

	local _check
	for _check in $FW_ICMP6_TYPES; do
		if [ "$_check" = "${_type#!}" ]; then
			[ "${_type#!}" != "$_type" ] && \
				export -n -- "$_var=! --icmpv6-type ${_type#!}" || \
				export -n -- "$_var=--icmpv6-type $_type"
			return 0
		fi
	done

	export -n -- "$_var="
	return 1
}
