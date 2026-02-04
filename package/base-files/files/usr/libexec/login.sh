#!/bin/sh

[ -t 0 ] && {
	tty_dev=$(readlink /proc/self/fd/0)
	case "$tty_dev" in
		/dev/console|/dev/tty[0-9]*)
			export TERM=${TERM:-linux}
			;;
		/dev/*)
			export TERM=vt102
			;;
	esac
}

[ "$(uci -q get system.@system[0].ttylogin)" = 1 ] || exec /bin/login -f root

exec /bin/login
