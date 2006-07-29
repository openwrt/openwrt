#!/bin/sh
# Copyright (C) 2006 OpenWrt.org
# Copyright (C) 2006 Fokus Fraunhofer <carsten.tittel@fokus.fraunhofer.de>

alias debug=${DEBUG:-:}

# newline
N="
"

_C=0

# valid interface?
if_valid () (
  ifconfig "$1" >&- 2>&- ||
  [ "${1%%[0-9]}" = "br" ] ||
  { debug "# missing interface '$1' ignored"; false; }
)

hotplug_dev() {
	env -i ACTION=$1 INTERFACE=$2 /sbin/hotplug net
}

append() {
	local var="$1"
	local value="$2"
	local sep="${3:- }"
	eval "export ${var}=\"\${${var}:+\${${var}}${value:+$sep}}$value\""
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
    local type="$1"
    local name="$2"
    _C=$(($_C + 1))
    name="${name:-cfg${_C}}"
    config_cb "$type" "$name"
    export CONFIG_SECTION="$name"
    export CONFIG_${CONFIG_SECTION}_TYPE="$type"
}

option () {
	local varname="$1"; shift
	export CONFIG_${CONFIG_SECTION}_${varname}="$*"
	option_cb "$varname" "$*"
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
		"") eval "echo \${CONFIG_${1}_${2}}";;
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
	for file in $(ls /lib/$1/*.sh 2>/dev/null); do
		. $file
	done
}
