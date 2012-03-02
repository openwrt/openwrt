#!/bin/sh
# Copyright (C) 2006-2011 OpenWrt.org
# Copyright (C) 2006 Fokus Fraunhofer <carsten.tittel@fokus.fraunhofer.de>


debug () {
	${DEBUG:-:} "$@"
}
mount() {
	busybox mount "$@"
}

# newline
N="
"

_C=0
NO_EXPORT=1
LOAD_STATE=1
LIST_SEP=" "

hotplug_dev() {
	env -i ACTION=$1 INTERFACE=$2 /sbin/hotplug-call net
}

append() {
	local var="$1"
	local value="$2"
	local sep="${3:- }"

	eval "export ${NO_EXPORT:+-n} -- \"$var=\${$var:+\${$var}\${value:+\$sep}}\$value\""
}

list_contains() {
	local var="$1"
	local str="$2"
	local val

	eval "val=\" \${$var} \""
	[ "${val%% $str *}" != "$val" ]
}

list_remove() {
	local var="$1"
	local remove="$2"
	local val

	eval "val=\" \${$var} \""
	val1="${val%% $remove *}"
	[ "$val1" = "$val" ] && return
	val2="${val##* $remove }"
	[ "$val2" = "$val" ] && return
	val="${val1## } ${val2%% }"
	val="${val%% }"
	eval "export ${NO_EXPORT:+-n} -- \"$var=\$val\""
}

config_load() {
	[ -n "$IPKG_INSTROOT" ] && return 0
	uci_load "$@"
}

reset_cb() {
	config_cb() { return 0; }
	option_cb() { return 0; }
	list_cb() { return 0; }
}
reset_cb

package() {
	return 0
}

config () {
	local cfgtype="$1"
	local name="$2"

	export ${NO_EXPORT:+-n} CONFIG_NUM_SECTIONS=$(($CONFIG_NUM_SECTIONS + 1))
	name="${name:-cfg$CONFIG_NUM_SECTIONS}"
	append CONFIG_SECTIONS "$name"
	[ -n "$NO_CALLBACK" ] || config_cb "$cfgtype" "$name"
	export ${NO_EXPORT:+-n} CONFIG_SECTION="$name"
	export ${NO_EXPORT:+-n} "CONFIG_${CONFIG_SECTION}_TYPE=$cfgtype"
}

option () {
	local varname="$1"; shift
	local value="$*"

	export ${NO_EXPORT:+-n} "CONFIG_${CONFIG_SECTION}_${varname}=$value"
	[ -n "$NO_CALLBACK" ] || option_cb "$varname" "$*"
}

list() {
	local varname="$1"; shift
	local value="$*"
	local len

	config_get len "$CONFIG_SECTION" "${varname}_LENGTH" 0
	[ $len = 0 ] && append CONFIG_LIST_STATE "${CONFIG_SECTION}_${varname}"
	len=$(($len + 1))
	config_set "$CONFIG_SECTION" "${varname}_ITEM$len" "$value"
	config_set "$CONFIG_SECTION" "${varname}_LENGTH" "$len"
	append "CONFIG_${CONFIG_SECTION}_${varname}" "$value" "$LIST_SEP"
	list_cb "$varname" "$*"
}

config_rename() {
	local OLD="$1"
	local NEW="$2"
	local oldvar
	local newvar

	[ -n "$OLD" -a -n "$NEW" ] || return
	for oldvar in `set | grep ^CONFIG_${OLD}_ | \
		sed -e 's/\(.*\)=.*$/\1/'` ; do
		newvar="CONFIG_${NEW}_${oldvar##CONFIG_${OLD}_}"
		eval "export ${NO_EXPORT:+-n} \"$newvar=\${$oldvar}\""
		unset "$oldvar"
	done
	export ${NO_EXPORT:+-n} CONFIG_SECTIONS="$(echo " $CONFIG_SECTIONS " | sed -e "s, $OLD , $NEW ,")"

	[ "$CONFIG_SECTION" = "$OLD" ] && export ${NO_EXPORT:+-n} CONFIG_SECTION="$NEW"
}

config_unset() {
	config_set "$1" "$2" ""
}

config_clear() {
	local SECTION="$1"
	local oldvar

	list_remove CONFIG_SECTIONS "$SECTION"
	export ${NO_EXPORT:+-n} CONFIG_SECTIONS="${SECTION:+$CONFIG_SECTIONS}"

	for oldvar in `set | grep ^CONFIG_${SECTION:+${SECTION}_} | \
		sed -e 's/\(.*\)=.*$/\1/'` ; do
		unset $oldvar
	done
}

# config_get <variable> <section> <option> [<default>]
# config_get <section> <option>
config_get() {
	case "$3" in
		"") eval echo "\${CONFIG_${1}_${2}:-\${4}}";;
		*)  eval export ${NO_EXPORT:+-n} -- "${1}=\${CONFIG_${2}_${3}:-\${4}}";;
	esac
}

# config_get_bool <variable> <section> <option> [<default>]
config_get_bool() {
	local _tmp
	config_get _tmp "$2" "$3" "$4"
	case "$_tmp" in
		1|on|true|enabled) _tmp=1;;
		0|off|false|disabled) _tmp=0;;
		*) _tmp="$4";;
	esac
	export ${NO_EXPORT:+-n} "$1=$_tmp"
}

config_set() {
	local section="$1"
	local option="$2"
	local value="$3"
	local old_section="$CONFIG_SECTION"

	CONFIG_SECTION="$section"
	option "$option" "$value"
	CONFIG_SECTION="$old_section"
}

config_foreach() {
	local function="$1"
	[ "$#" -ge 1 ] && shift
	local type="$1"
	[ "$#" -ge 1 ] && shift
	local section cfgtype

	[ -z "$CONFIG_SECTIONS" ] && return 0
	for section in ${CONFIG_SECTIONS}; do
		config_get cfgtype "$section" TYPE
		[ -n "$type" -a "x$cfgtype" != "x$type" ] && continue
		eval "$function \"\$section\" \"\$@\""
	done
}

config_list_foreach() {
	[ "$#" -ge 3 ] || return 0
	local section="$1"; shift
	local option="$1"; shift
	local function="$1"; shift
	local val
	local len
	local c=1

	config_get len "${section}" "${option}_LENGTH"
	[ -z "$len" ] && return 0
	while [ $c -le "$len" ]; do
		config_get val "${section}" "${option}_ITEM$c"
		eval "$function \"\$val\" \"$@\""
		c="$(($c + 1))"
	done
}

load_modules() {
	[ -d /etc/modules.d ] && {
		cd /etc/modules.d
		sed 's/^[^#]/insmod &/' $* | ash 2>&- || :
	}
}

include() {
	local file

	for file in $(ls $1/*.sh 2>/dev/null); do
		. $file
	done
}

find_mtd_part() {
	local PART="$(grep "\"$1\"" /proc/mtd | awk -F: '{print $1}')"
	local PREFIX=/dev/mtdblock

	PART="${PART##mtd}"
	[ -d /dev/mtdblock ] && PREFIX=/dev/mtdblock/
	echo "${PART:+$PREFIX$PART}"
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

		export ${NO_EXPORT:+-n} "$1=$tmp"; count=$((count+1))
		shift 2
	done

	if [ $# -gt 0 -a -n "$val" ]; then
		export ${NO_EXPORT:+-n} "$1=$val"; count=$((count+1))
	fi

	return $count
}


jffs2_mark_erase() {
	local part="$(find_mtd_part "$1")"
	[ -z "$part" ] && {
		echo Partition not found.
		return 1
	}
	echo -e "\xde\xad\xc0\xde" | mtd -qq write - "$1"
}

uci_apply_defaults() {
	cd /etc/uci-defaults || return 0
	files="$(ls)"
	[ -z "$files" ] && return 0
	mkdir -p /tmp/.uci
	for file in $files; do
		( . "./$(basename $file)" ) && rm -f "$file"
	done
	uci commit
}

#
# service: simple wrapper around start-stop-daemon
#
# Usage: service ACTION EXEC ARGS...
#
# Action:
#   -C   check if EXEC is alive
#   -S   start EXEC, passing it ARGS as its arguments
#   -K   kill EXEC, sending it a TERM signal if not specified otherwise
#
# Environment variables exposed:
#   SERVICE_DAEMONIZE    run EXEC in background
#   SERVICE_WRITE_PID    create a pid-file and use it for matching
#   SERVICE_MATCH_EXEC   use EXEC command-line for matching (default)
#   SERVICE_MATCH_NAME   use EXEC process name for matching
#   SERVICE_USE_PID      assume EXEC create its own pid-file and use it for matching
#   SERVICE_NAME         process name to use (default to EXEC file part)
#   SERVICE_PID_FILE     pid file to use (default to /var/run/$SERVICE_NAME.pid)
#   SERVICE_SIG          signal to send when using -K
#   SERVICE_SIG_RELOAD   default signal used when reloading
#   SERVICE_SIG_STOP     default signal used when stopping
#   SERVICE_STOP_TIME    time to wait for a process to stop gracefully before killing it
#   SERVICE_UID          user EXEC should be run as
#   SERVICE_GID          group EXEC should be run as
#
#   SERVICE_DEBUG        don't do anything, but show what would be done
#   SERVICE_QUIET        don't print anything
#

SERVICE_QUIET=1
SERVICE_SIG_RELOAD="HUP"
SERVICE_SIG_STOP="TERM"
SERVICE_STOP_TIME=5
SERVICE_MATCH_EXEC=1

service() {
	local ssd
	local exec
	local name
	local start
	ssd="${SERVICE_DEBUG:+echo }start-stop-daemon${SERVICE_QUIET:+ -q}"
	case "$1" in
	  -C)
		ssd="$ssd -K -t"
		;;
	  -S)
		ssd="$ssd -S${SERVICE_DAEMONIZE:+ -b}${SERVICE_WRITE_PID:+ -m}"
		start=1
		;;
	  -K)
		ssd="$ssd -K${SERVICE_SIG:+ -s $SERVICE_SIG}"
		;;
	  *)
		echo "service: unknown ACTION '$1'" 1>&2
		return 1
	esac
	shift
	exec="$1"
	[ -n "$exec" ] || {
		echo "service: missing argument" 1>&2
		return 1
	}
	[ -x "$exec" ] || {
		echo "service: file '$exec' is not executable" 1>&2
		return 1
	}
	name="${SERVICE_NAME:-${exec##*/}}"
	[ -z "$SERVICE_USE_PID$SERVICE_WRITE_PID$SERVICE_PID_FILE" ] \
		|| ssd="$ssd -p ${SERVICE_PID_FILE:-/var/run/$name.pid}"
	[ -z "$SERVICE_MATCH_NAME" ] || ssd="$ssd -n $name"
	ssd="$ssd${SERVICE_UID:+ -c $SERVICE_UID${SERVICE_GID:+:$SERVICE_GID}}"
	[ -z "$SERVICE_MATCH_EXEC$start" ] || ssd="$ssd -x $exec"
	shift
	$ssd${1:+ -- "$@"}
}

service_check() {
	service -C "$@"
}

service_signal() {
	SERVICE_SIG="${SERVICE_SIG:-USR1}" service -K "$@"
}

service_start() {
	service -S "$@"
}

service_stop() {
	local try
	SERVICE_SIG="${SERVICE_SIG:-$SERVICE_SIG_STOP}" service -K "$@" || return 1
	while [ $((try++)) -lt $SERVICE_STOP_TIME ]; do
		service -C "$@" || return 0
		sleep 1
	done
	SERVICE_SIG="KILL" service -K "$@"
	sleep 1
	! service -C "$@"
}

service_reload() {
	SERVICE_SIG="${SERVICE_SIG:-$SERVICE_SIG_RELOAD}" service -K "$@"
}

service_kill() {
	cat 1>&2 << __END_OF_WARNING__
#
# WARNING: the 'service_kill' function is now deprecated and might be
# removed soon. Consider using the other new service_* wrappers instead.
#
__END_OF_WARNING__
	local name="${1}"
	local pid="${2:-$(pidof "$name")}"
	local grace="${3:-5}"

	[ -f "$pid" ] && pid="$(head -n1 "$pid" 2>/dev/null)"

	for pid in $pid; do
		[ -d "/proc/$pid" ] || continue
		local try=0
		kill -TERM $pid 2>/dev/null && \
			while grep -qs "$name" "/proc/$pid/cmdline" && [ $((try++)) -lt $grace ]; do sleep 1; done
		kill -KILL $pid 2>/dev/null && \
			while grep -qs "$name" "/proc/$pid/cmdline"; do sleep 1; done
	done
}


group_add() {
	local name="$1"
	local gid="$2"
	local rc
	[ -f "${IPKG_INSTROOT}/etc/group" ] || return 1
	[ -n "$IPKG_INSTROOT" ] || lock /var/lock/group
	echo "${name}:x:${gid}:" >> ${IPKG_INSTROOT}/etc/group
	rc=$?
	[ -n "$IPKG_INSTROOT" ] || lock -u /var/lock/group
	return $rc
}

group_exists() {
	grep -qs "^${1}:" ${IPKG_INSTROOT}/etc/group
}

user_add() {
	local name="${1}"
	local uid="${2}"
	local gid="${3:-$2}"
	local desc="${4:-$1}"
	local home="${5:-/var/run/$1}"
	local shell="${6:-/bin/false}"
	local rc
	[ -f "${IPKG_INSTROOT}/etc/passwd" ] || return 1
	[ -n "$IPKG_INSTROOT" ] || lock /var/lock/passwd
	echo "${name}:x:${uid}:${gid}:${desc}:${home}:${shell}" >> ${IPKG_INSTROOT}/etc/passwd
	rc=$?
	[ -n "$IPKG_INSTROOT" ] || lock -u /var/lock/passwd
	return $rc
}

user_exists() {
	grep -qs "^${1}:" ${IPKG_INSTROOT}/etc/passwd
}


pi_include() {
	if [ -f "/tmp/overlay/$1" ]; then
		. "/tmp/overlay/$1"
	elif [ -f "$1" ]; then
		. "$1"
	elif [ -d "/tmp/overlay/$1" ]; then
		if [ -n "$(ls /tmp/overlay/$1/*.sh 2>/dev/null)" ]; then
			for src_script in /tmp/overlay/$1/*.sh; do
				. "$src_script"
			done
		fi
	elif [ -d "$1" ]; then
		if [ -n "$(ls $1/*.sh 2>/dev/null)" ]; then
			for src_script in $1/*.sh; do
				. "$src_script"
			done
		fi
	else
		echo "WARNING: $1 not found"
		return 1
	fi
	return 0
}

[ -z "$IPKG_INSTROOT" -a -f /lib/config/uci.sh ] && . /lib/config/uci.sh
