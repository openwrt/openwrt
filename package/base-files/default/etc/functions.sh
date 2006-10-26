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
	eval "export ${var}=\"\${${var}:+\${${var}}${value:+$sep}}\$value\""
}

reset_cb() {
	config_cb() {
		return 0
	}
	option_cb() {
		return 0
	}
}
reset_cb

config () {
    local cfgtype="$1"
    local name="$2"
    _C=$(($_C + 1))
    name="${name:-cfg${_C}}"
    config_cb "$cfgtype" "$name"
    export CONFIG_SECTION="$name"
    export CONFIG_${CONFIG_SECTION}_TYPE="$cfgtype"
}

option () {
	local varname="$1"; shift
	export CONFIG_${CONFIG_SECTION}_${varname}="$*"
	option_cb "$varname" "$*"
}

config_rename() {
	local OLD="$1"
	local NEW="$2"
	local oldsetting
	local newvar
	
	[ -z "$OLD" -o -z "$NEW" ] && return
	for oldsetting in `set | grep ^CONFIG_${OLD}_ | \
		sed -e 's/\(.*\)=.*$/\1/'` ; do
		newvar="CONFIG_${NEW}_${oldsetting##CONFIG_${OLD}_}"
		eval "${newvar}=\${$oldsetting}"
		unset "$oldsetting"
	done
	[ "$CONFIG_SECTION" = "$OLD" ] && CONFIG_SECTION="$NEW"
}

config_unset() {
	config_set "$1" "$2" ""
}

config_clear() {
	[ -z "$CONFIG_SECTION" ] && return
	for oldsetting in `set | grep ^CONFIG_${CONFIG_SECTION}_ | \
		sed -e 's/\(.*\)=.*$/\1/'` ; do 
		unset $oldsetting 
	done
	unset CONFIG_SECTION
}

config_load() {
	local DIR="./"
	_C=0
	[ \! -e "$1" -a -e "/etc/config/$1" ] && {
		DIR="/etc/config/"
	}
	[ -e "$DIR$1" ] && {
		CONFIG_FILENAME="$DIR$1"
		. ${CONFIG_FILENAME}
	} || return 1
	${CD:+cd -} >/dev/null
	${CONFIG_SECTION:+config_cb}
}

config_get() {
	case "$3" in
		"") eval "echo \"\${CONFIG_${1}_${2}}\"";;
		*) eval "$1=\"\${CONFIG_${2}_${3}}\"";;
	esac
}

config_set() {
	export CONFIG_${1}_${2}="${3}"
}

load_modules() {
	sed 's/^[^#]/insmod &/' $* | ash 2>&- || :
}

include() {
	for file in $(ls $1/*.sh 2>/dev/null); do
		. $file
	done
}

find_mtd_part() {
	local PART="$(grep "\"$1\"" /proc/mtd | awk -F: '{print $1}')"
	PART="${PART##mtd}"
	echo "${PART:+/dev/mtdblock/$PART}"
}

