scan_pppoa() {
	scan_ppp "$@"
}

setup_interface_pppoa() {
	local iface="$1"
	local config="$2"
	
	config_get device "$config" device
	config_get vpi "$config" vpi
	config_get vci "$config" vci

	for module in slhc ppp_generic pppoatm; do
		/sbin/insmod $module 2>&- >&-
	done
	
	config_get encaps "$config" encaps
	case "$encaps" in
		1|vc) ENCAPS="vc-encaps" ;;
		*) ENCAPS="llc-encaps" ;;
	esac

	config_get mtu "$config" mtu
	mtu=${mtu:-1500}
	start_pppd "$config" \
		plugin pppoatm.so ${vpi:-8}.${vci:-35} ${ENCAPS} \
		mtu $mtu mru $mtu
}
