#!/bin/sh

awk -f /usr/lib/common.awk -f - $* <<EOF
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
		print "END="int2ip(end-1)
	}
}
EOF
