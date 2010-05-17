# Copyright (C) 2009-2010 OpenWrt.org

FW_LIBDIR=${FW_LIBDIR:-/lib/firewall}

. $FW_LIBDIR/fw.sh
include /lib/network

fw_start() {
	fw_init

	lock /var/lock/firewall.start

	FW_DEFAULTS_APPLIED=

	fw_is_loaded && {
		echo "firewall already loaded" >&2
		exit 1
	}
	uci_set_state firewall core "" firewall_state

	fw_clear DROP

	fw_callback pre core

	echo "Loading defaults"
	fw_config_once fw_load_defaults defaults

	echo "Loading zones"
	config_foreach fw_load_zone zone

	echo "Loading forwardings"
	config_foreach fw_load_forwarding forwarding

	echo "Loading redirects"
	config_foreach fw_load_redirect redirect

	echo "Loading rules"
	config_foreach fw_load_rule rule

	echo "Loading includes"
	config_foreach fw_load_include include

	[ -n "$FW_NOTRACK_DISABLED" ] && {
		echo "Optimizing conntrack"
		config_foreach fw_load_notrack_zone zone
	}

	echo "Loading interfaces"
	config_foreach fw_configure_interface interface add

	fw_callback post core

	uci_set_state firewall core loaded 1

	lock -u /var/lock/firewall.start
}

fw_stop() {
	fw_init

	fw_callback pre stop

	fw_clear ACCEPT

	fw_callback post stop

	uci_revert_state firewall
	config_clear
	unset FW_INITIALIZED
}

fw_restart() {
	fw_stop
	fw_start
}

fw_reload() {
	fw_restart
}

fw_is_loaded() {
	local bool=$(uci -q -P /var/state get firewall.core.loaded)
	return $((! ${bool:-0}))
}


fw_die() {
	echo "Error:" "$@" >&2
	fw_log error "$@"
	fw_stop
	exit 1
}

fw_log() {
	local level="$1"
	[ -n "$2" ] || {
		shift
		level=notice
	}
	logger -t firewall -p user.$level "$@"
}


fw_init() {
	[ -z "$FW_INITIALIZED" ] || return 0

	. $FW_LIBDIR/config.sh

	scan_interfaces
	fw_config_append firewall

	local hooks="core stop defaults zone notrack synflood"
	local file lib hk pp
	for file in $FW_LIBDIR/core_*.sh; do
		. $file
		hk=$(basename $file .sh)
		hk=${hk#core_}
		append hooks $hk
	done
	for file in $FW_LIBDIR/*.sh; do
		lib=$(basename $file .sh)
		lib=${lib##[0-9][0-9]_}
		case $lib in
			core*|fw|config|uci_firewall) continue ;;
		esac
		. $file
		for hk in $hooks; do
			for pp in pre post; do
				type ${lib}_${pp}_${hk}_cb >/dev/null &&
					append FW_CB_${pp}_${hk} ${lib}
			done
		done
	done

	fw_callback post init

	FW_INITIALIZED=1
	return 0
}
