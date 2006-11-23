#!/bin/sh
# Copyright (C) 2006 OpenWrt.org
# Copyright (C) 2006 Fokus Fraunhofer <carsten.tittel@fokus.fraunhofer.de>

alias debug=${DEBUG:-:}

# newline
N="
"

_C=0

hotplug_dev() {
	env -i ACTION=$1 INTERFACE=$2 /sbin/hotplug net
}

append() {
	local var="$1"
	local value="$2"
	local sep="${3:- }"
	
	eval "export -n -- \"$var=\${$var:+\${$var}\${value:+\$sep}}\$value\""
}

reset_cb() {
	config_cb() { return 0; }
	option_cb() { return 0; }
}
reset_cb

config () {
	local cfgtype="$1"
	local name="$2"
    
	_C=$((_C + 1))
	name="${name:-cfg${_C}}"
	config_cb "$cfgtype" "$name"
	CONFIG_SECTION="$name"
	export -n "CONFIG_${CONFIG_SECTION}_TYPE=$cfgtype"
}

option () {
	local varname="$1"; shift
	local value="$*"
	
	export -n "CONFIG_${CONFIG_SECTION}_${varname}=$value"
	option_cb "$varname" "$*"
}

config_rename() {
	local OLD="$1"
	local NEW="$2"
	local oldvar
	local newvar
	
	[ "$OLD" -a "$NEW" ] || return
	for oldvar in `set | grep ^CONFIG_${OLD}_ | \
		sed -e 's/\(.*\)=.*$/\1/'` ; do
		newvar="CONFIG_${NEW}_${oldvar##CONFIG_${OLD}_}"
		eval "export -n \"$newvar=\${$oldvar}\""
		unset "$oldvar"
	done
	
	[ "$CONFIG_SECTION" = "$OLD" ] && CONFIG_SECTION="$NEW"
}

config_unset() {
	config_set "$1" "$2" ""
}

config_clear() {
	local SECTION="$1"
	local oldvar
	
	for oldvar in `set | grep ^CONFIG_${SECTION}_ | \
		sed -e 's/\(.*\)=.*$/\1/'` ; do 
		unset $oldvar 
	done
}

config_load() {
	local file="/etc/config/$1"
	_C=0
	CONFIG_SECTION=
	
	[ -e "$file" ] && {
		. $file
	} || return 1
	
	${CONFIG_SECTION:+config_cb}
}

config_get() {
	case "$3" in
		"") eval "echo \"\${CONFIG_${1}_${2}}\"";;
		*)  eval "export -n -- \"$1=\${CONFIG_${2}_${3}}\"";;
	esac
}

config_set() {
	local section="$1"
	local option="$2"
	local value="$3"
	export -n "CONFIG_${section}_${option}=$value"
}

load_modules() {
	cd /etc/modules.d
	sed 's/^[^#]/insmod &/' $* | ash 2>&- || :
}

include() {
	local file
	
	for file in $(ls $1/*.sh 2>/dev/null); do
		. $file
	done
}

find_mtd_part() {
	local PART="$(grep "\"$1\"" /proc/mtd | awk -F: '{print $1}')"
	
	PART="${PART##mtd}"
	echo "${PART:+/dev/mtdblock/$PART}"
}

strtok() { # <string> { <variable> [<separator>] ... }
	local tmp
	local val="$1"
	local count=0

	shift

	while [ $# -gt 1 ]; do
		tmp="${val%%$2*}"

		[ "$tmp" = "$val" ] && break

		val="${val#$tmp$2}"

		export -n "$1=$tmp"; count=$((count+1))
		shift 2
	done

	if [ $# -gt 0 -a "$val" ]; then
		export -n "$1=$val"; count=$((count+1))
	fi

	return $count
}
