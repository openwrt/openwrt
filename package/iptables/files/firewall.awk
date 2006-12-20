# Copyright (C) 2006 OpenWrt.org

BEGIN {
	FS=":"
}

($1 == "accept") || ($1 == "drop") || ($1 == "forward") {
	delete _opt
	str2data($2)
	if ((_l["proto"] == "") && (_l["sport"] _l["dport"] != "")) {
		_opt[0] = " -p tcp"
		_opt[1] = " -p udp"
	} else {
		_opt[0] = ""
	}
}

($1 == "accept") {
	target = " -j ACCEPT"
	for (o in _opt) {
		print "iptables -t nat -A prerouting_wan" _opt[o] str2ipt($2) target
		print "iptables        -A input_wan     " _opt[o] str2ipt($2) target
		print ""
	}
}

($1 == "drop") {
	for (o in _opt) {
		print "iptables -t nat -A prerouting_wan" _opt[o] str2ipt($2) " -j DROP"
		print ""
	}
}

($1 == "forward") {
	target = " -j DNAT --to " $3
	fwopts = ""
	if ($4 != "") {
		if ((_l["proto"] == "tcp") || (_l["proto"] == "udp") || (_l["proto"] == "")) {
			if (_l["proto"] != "") fwopts = " -p " _l["proto"]
			fwopts = fwopts " --dport " $4
			target = target ":" $4
		}
		else fwopts = ""
	}
	for (o in _opt) {
		print "iptables -t nat -A prerouting_wan" _opt[o] str2ipt($2) target
		print "iptables        -A forwarding_wan   " _opt[o] " -d " $3 fwopts " -j ACCEPT"
		print ""
	}
}
