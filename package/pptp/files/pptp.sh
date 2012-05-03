find_route() {
	ip route get $1 | sed -e 's/ /\n/g' | \
            sed -ne '1p;/via/{N;p};/dev/{N;p};/src/{N;p};/mtu/{N;p}'
}

scan_pptp() {
	config_set "$1" device "pptp-$1"
}

stop_interface_pptp() {
	stop_interface_ppp "$1"
	for ip in $(uci_get_state network "$1" serv_addrs); do
		ip route del "$ip" 2>/dev/null
	done
}

coldplug_interface_pptp() {
	setup_interface_pptp "pptp-$1" "$1"
}

setup_interface_pptp() {
	local config="$2"
	local ifname

	local device
	config_get device "$config" device

	local server
	config_get server "$config" server

	local buffering
	config_get_bool buffering "$config" buffering 1
	[ "$buffering" == 0 ] && buffering="--nobuffer" || buffering=

	for module in slhc ppp_generic ppp_async ip_gre; do
		/sbin/insmod $module 2>&- >&-
	done
	sleep 1

	local serv_addrs=""
	for ip in $(resolveip -t 3 "${server}"); do
		append serv_addrs "$ip"
		ip route replace $(find_route $ip)
	done
	uci_toggle_state network "$config" serv_addrs "$serv_addrs"

	# fix up the netmask
	config_get netmask "$config" netmask
	[ -z "$netmask" -o -z "$device" ] || ifconfig $device netmask $netmask

	config_get mtu "$config" mtu
	mtu=${mtu:-1452}
	start_pppd "$config" \
		pty "/usr/sbin/pptp $server --loglevel 0 --nolaunchpppd $buffering" \
		file /etc/ppp/options.pptp \
		mtu $mtu mru $mtu
}
