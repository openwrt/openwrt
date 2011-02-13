#!/bin/sh

awk -f /usr/lib/common.awk -f - $* <<EOF
function bitcount(c) {
	c=and(rshift(c, 1),0x55555555)+and(c,0x55555555)
	c=and(rshift(c, 2),0x33333333)+and(c,0x33333333)
	c=and(rshift(c, 4),0x0f0f0f0f)+and(c,0x0f0f0f0f)
	c=and(rshift(c, 8),0x00ff00ff)+and(c,0x00ff00ff)
	c=and(rshift(c,16),0x0000ffff)+and(c,0x0000ffff)
	return c
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

BEGIN {
	ipaddr=ip2int(ARGV[1])
	netmask=ip2int(ARGV[2])
	network=and(ipaddr,netmask)
	broadcast=or(network,compl(netmask))
	
	start=or(network,and(ip2int(ARGV[3]),compl(netmask)))
	limit=network+1
	if (start<limit) start=limit
	
	end=start+ARGV[4]
	limit=or(network,compl(netmask))-1
	if (end>limit) end=limit

	print "IP="int2ip(ipaddr)
	print "NETMASK="int2ip(netmask)
	print "BROADCAST="int2ip(broadcast)
	print "NETWORK="int2ip(network)
	print "PREFIX="32-bitcount(compl(netmask))
	
	# range calculations:
	# ipcalc <ip> <netmask> <start> <num>
	
	if (ARGC > 3) {
		print "START="int2ip(start)
		print "END="int2ip(end)
	}
}
EOF
