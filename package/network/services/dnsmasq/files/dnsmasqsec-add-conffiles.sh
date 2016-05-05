add_dnsmasqsec_conffiles()
{
	local filelist="$1"

	# do NOT include timestamp in a backup, only system upgrade
	# dnsmasq restart ensures file timestamp is up to date
	if [ -z $NEED_IMAGE ]; then
		if [ $(ubus call service list '{"name":"dnsmasq"}' | jsonfilter -e '@.*.instances.instance1.running') = "true" ]; then
			/etc/init.d/dnsmasq restart
			sleep 1
			echo "/etc/dnsmasq.time" >>$filelist
		fi
	fi
}

sysupgrade_init_conffiles="$sysupgrade_init_conffiles add_dnsmasqsec_conffiles"
