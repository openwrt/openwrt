scan_pppoa() {
	scan_ppp "$@"
}

setup_interface_pppoa() {
	local iface="$1"
	local config="$2"
	
	config_get device "$config" device

	for module in slhc ppp_generic pppoatm; do
		/sbin/insmod $module 2>&- >&-
	done

	config_get mtu "$cfg" mtu
	mtu=${mtu:-1492}
	start_pppd "$config" \
		plugin pppoatm.so ${atm_vpi:-8}.${atm_vci:-35} \
		mtu $mtu mru $mtu
}
