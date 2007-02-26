interface[] {
	proto: string, required;
	ipaddr: ip, required[proto=static];
	netmask: ip, required[proto=static];
	gateway: ip;
	dns: ip;
}
