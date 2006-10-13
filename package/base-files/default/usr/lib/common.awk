# Copyright (C) 2006 OpenWrt.org

function portstr(type, str) {
	gsub(/-/, ":", str)
	if (insmod_mport != 1) {
		print "insmod ipt_multiport >&- 2>&-"
		insmod_mport = 1
	}
	if (type == "src") return " -m multiport --sports " str
	else return " -m multiport --dports " str
}

function str2ipt(str) {
	str2data(str)
	_cmd = ""
	if (_l["src"] != "") _cmd = _cmd " -s " _l["src"]
	if (_l["dest"] != "") _cmd = _cmd " -d " _l["dest"]
	if (_l["proto"] != "") {
		_cmd = _cmd " -p " _l["proto"]
	}
	# scripts need to check for proto="" and emit two rules in that case
	if ((_l["proto"] == "") || (_l["proto"] == "tcp") || (_l["proto"] == "udp")) {
		if (_l["sport"] != "") _cmd = _cmd portstr("src", _l["sport"])
		if (_l["dport"] != "") _cmd = _cmd portstr("dest", _l["dport"])
	}
	if (_l["layer7"] != "") {
		if (insmod_l7 != 1) {
			print "insmod ipt_layer7 >&- 2>&-"
			insmod_l7 = 1
		}
		_cmd = _cmd " -m layer7 --l7proto " _l["layer7"]
	}
	return _cmd
}

function str2data(str) {
	delete _l
	_n = split(str, _o, "[\t ]")
	for (_i = 1; _i <= _n; _i++) {
		_n2 = split(_o[_i], _c, "=")
		if (_n2 == 2) _l[_c[1]] = _c[2]
	}
}

function bitcount(c) {
	c=and(rshift(c, 1),0x55555555)+and(c,0x55555555)
	c=and(rshift(c, 2),0x33333333)+and(c,0x33333333)
	c=and(rshift(c, 4),0x0f0f0f0f)+and(c,0x0f0f0f0f)
	c=and(rshift(c, 8),0x00ff00ff)+and(c,0x00ff00ff)
	c=and(rshift(c,16),0x0000ffff)+and(c,0x0000ffff)
	return c
}

function validate_netmask(nm) {
	return and(-nm,compl(nm))
}

function ip2int(ip) {
	for (ret=0,n=split(ip,a,"\."),x=1;x<=n;x++) ret=or(lshift(ret,8),a[x]) 
	return ret
}

function int2ip(ip,ret,x) {
	ret=and(ip,255)
	ip=rshift(ip,8)
	for(;x<3;ret=and(ip,255)"."ret,ip=rshift(ip,8),x++);
	return ret
}
