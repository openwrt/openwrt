stop_interface_ppp() {
	local cfg="$1"

	local proto
	config_get proto "$cfg" proto

	local ifname
	config_get ifname "$cfg" ifname

	local link="${proto:-ppp}-$ifname"
	[ -f "/var/run/ppp-${link}.pid" ] && {
		local pid="$(head -n1 /var/run/ppp-${link}.pid 2>/dev/null)"
		grep -qs pppd "/proc/$pid/cmdline" && kill -TERM $pid
	}
}

start_pppd() {
	local cfg="$1"; shift

	local proto
	config_get proto "$cfg" proto

	# unique link identifier
	local link="${proto:-ppp}-$cfg"

	# make sure only one pppd process is started
	lock "/var/lock/ppp-${link}"
	local pid="$(head -n1 /var/run/ppp-${link}.pid 2>/dev/null)"
	[ -d "/proc/$pid" ] && grep pppd "/proc/$pid/cmdline" 2>/dev/null >/dev/null && {
		lock -u "/var/lock/ppp-${link}"
		return 0
	}

	# Workaround: sometimes hotplug2 doesn't deliver the hotplug event for creating
	# /dev/ppp fast enough to be used here
	[ -e /dev/ppp ] || mknod /dev/ppp c 108 0

	local device
	config_get device "$cfg" device

	local username
	config_get username "$cfg" username

	local password
	config_get password "$cfg" password

	local keepalive
	config_get keepalive "$cfg" keepalive

	local connect
	config_get connect "$cfg" connect

	local disconnect
	config_get disconnect "$cfg" disconnect

	local pppd_options
	config_get pppd_options "$cfg" pppd_options

	local defaultroute
	config_get_bool defaultroute "$cfg" defaultroute 1
	[ "$defaultroute" -eq 1 ] && \
		defaultroute="defaultroute replacedefaultroute" || defaultroute=""

	local interval="${keepalive##*[, ]}"
	[ "$interval" != "$keepalive" ] || interval=5

	local dns
	config_get dns "$config" dns

	local has_dns=0
	local peer_default=1
	[ -n "$dns" ] && {
		has_dns=1
		peer_default=0
	}

	local peerdns
	config_get_bool peerdns "$cfg" peerdns $peer_default

	echo -n "" > /tmp/resolv.conf.auto

	[ "$peerdns" -eq 1 ] && {
		peerdns="usepeerdns"
	} || {
		peerdns=""
		for dns in $dns; do
			echo "nameserver $dns" >> /tmp/resolv.conf.auto
		done
	}

	local demand
	config_get_bool demand "$cfg" demand 0

	local demandargs
	[ "$demand" -eq 1 ] && {
		demandargs="precompiled-active-filter /etc/ppp/filter demand idle"
		[ "$has_dns" -eq 0 ] && \
			echo "nameserver 1.1.1.1" > /tmp/resolv.conf.auto
	} || {
		demandargs="persist"
	}

	local ipv6
	config_get_bool ipv6 "$cfg" ipv6 0
	[ "$ipv6" -eq 1 ] && ipv6="+ipv6" || ipv6=""

	/usr/sbin/pppd "$@" \
		${keepalive:+lcp-echo-interval $interval lcp-echo-failure ${keepalive%%[, ]*}} \
		$demandargs \
		$peerdns \
		$defaultroute \
		${username:+user "$username" password "$password"} \
		linkname "$link" \
		ipparam "$cfg" \
		ifname "$link" \
		${connect:+connect "$connect"} \
		${disconnect:+disconnect "$disconnect"} \
		${ipv6} \
		${pppd_options}

	lock -u "/var/lock/ppp-${link}"
}

setup_interface_ppp() {
	local iface="$1"
	local config="$2"

	local device
	config_get device "$config" device

	local mtu
	config_get mtu "$config" mtu

	mtu=${mtu:-1492}
	start_pppd "$config" \
		mtu $mtu mru $mtu \
		"$device"
}

