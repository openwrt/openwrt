stop_interface_pppoe() {
	stop_interface_ppp "$1"
}

setup_interface_pppoe() {
	local iface="$1"
	local config="$2"

	for module in slhc ppp_generic pppox pppoe; do
		/sbin/insmod $module 2>&- >&-
	done

	local mtu
	config_get mtu "$config" mtu 1492

	local ac
	config_get ac "$config" ac

	local service
	config_get service "$config" service

	# NB: the first nic-* argument will be moved to the
	#     end of the argument list by start_pppd()
	start_pppd "$config" \
		"nic-$iface" \
		${ac:+rp_pppoe_ac "$ac"} \
		${service:+rp_pppoe_service "$service"} \
		plugin rp-pppoe.so \
		mtu $mtu mru $mtu
}
