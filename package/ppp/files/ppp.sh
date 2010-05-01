scan_ppp() {
	config_get ifname "$1" ifname
	pppdev="${pppdev:-0}"
	config_get devunit "$1" unit
	{
	        unit=
	        pppif=
	        if [ ! -d /tmp/.ppp-counter ]; then
	       	     mkdir -p /tmp/.ppp-counter
	        fi
	        local maxunit
	        maxunit="$(cat /tmp/.ppp-counter/max-unit 2>/dev/null)" 
	        if [ -z "$maxunit" ]; then
	            maxunit=-1
	        fi
	        local i
	        i=0
	        while [ $i -le $maxunit ]; do
	             local unitdev
	             unitdev="$(cat /tmp/.ppp-counter/ppp${i} 2>/dev/null)"
	             if [ "$unitdev" = "$1" ]; then
	                  unit="$i"
	                  pppif="ppp${i}"
	                  break
	             fi
	             i="$(($i + 1))"
	        done 
	        if [ -z "$unit" ] || [ -z "$pppif" ]; then
	            maxunit="$(($maxunit + 1))"
	            if [ -n "$devunit" ]; then
	             	unit="$devunit"
		    elif [ "${ifname%%[0-9]*}" = ppp ]; then
			 unit="${ifname##ppp}"
	            else
	                 unit="$maxunit"
	            fi 
         	    [ "$maxunit" -lt "$unit" ] && maxunit="$unit"
		    pppif="ppp${unit}"
		    echo "$1" >/tmp/.ppp-counter/$pppif 2>/dev/null
		    echo "$maxunit" >/tmp/.ppp-counter/max-unit 2>/dev/null
	        fi
		config_set "$1" ifname "ppp$unit"
		config_set "$1" unit "$unit"
	}
}

start_pppd() {
	local cfg="$1"; shift
	local ifname

	# make sure the network state references the correct ifname
	scan_ppp "$cfg"
	config_get ifname "$cfg" ifname
	set_interface_ifname "$cfg" "$ifname"

	# make sure only one pppd process is started
	lock "/var/lock/ppp-${cfg}"
	local pid="$(head -n1 /var/run/ppp-${cfg}.pid 2>/dev/null)"
	[ -d "/proc/$pid" ] && grep pppd "/proc/$pid/cmdline" 2>/dev/null >/dev/null && {
		lock -u "/var/lock/ppp-${cfg}"
		return 0
	}

	# Workaround: sometimes hotplug2 doesn't deliver the hotplug event for creating
	# /dev/ppp fast enough to be used here
	[ -e /dev/ppp ] || mknod /dev/ppp c 108 0

	local device
	config_get device "$cfg" device

	local unit
	config_get unit "$cfg" unit

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
	[ "$defaultroute" -eq 1 ] && defaultroute="defaultroute replacedefaultroute" || defaultroute=""

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
		unit "$unit" \
		linkname "$cfg" \
		ipparam "$cfg" \
		${connect:+connect "$connect"} \
		${disconnect:+disconnect "$disconnect"} \
		${ipv6} \
		${pppd_options}

	lock -u "/var/lock/ppp-${cfg}"
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

