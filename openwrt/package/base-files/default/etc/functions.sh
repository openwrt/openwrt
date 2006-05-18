#!/bin/sh
alias debug=${DEBUG:-:}

# valid interface?
if_valid () (
  ifconfig "$1" >&- 2>&- ||
  [ "${1%%[0-9]}" = "br" ] ||
  { debug "# missing interface '$1' ignored"; false; }
)

hotplug_dev() {
	env -i ACTION=$1 INTERFACE=$2 /sbin/hotplug net
}

config_cb() {
	return 0
}
option_cb() {
	return 0
}

config () {
	config_cb "$@"
	_C=$((${_C:-0} + 1))
	export CONFIG_SECTION="${2:-cfg${_C}}"
	export CONFIG_${CONFIG_SECTION}_TYPE="$1"
}

option () {
	local varname="$1" ; shift
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
	local CD=""
	if [ \! -e "$1" -a -e "/etc/config/$1" ]; then
		cd /etc/config && local CD=1
	fi
	[ -e "$1" ] && . $1
	${CD:+cd - >/dev/null}
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
