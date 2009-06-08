module("trigger.base", package.seeall)
require("uci.trigger")

uci.trigger.add {
	{
		id = "dnsmasq_restart",
		title = "Restart dnsmasq",
		package = "dhcp",
		action = uci.trigger.service_restart("dnsmasq"),
	},
	{
		id = "dropbear_restart",
		title = "Restart dropbear",
		package = "dropbear",
		action = uci.trigger.service_restart("dropbear"),
	},
	{
		id = "fstab_restart",
		title = "Remount filesystems",
		package = "fstab",
		action = uci.trigger.service_restart("fstab"),
	},
	{
		id = "firewall_restart",
		title = "Reload firewall rules",
		package = "firewall",
		action = uci.trigger.service_restart("firewall"),
	},
	{
		id = "httpd_restart",
		title = "Restart the http server",
		package = "httpd",
		action = uci.trigger.service_restart("httpd")
	},
	{
		id = "led_restart",
		title = "Reload LED settings",
		package = "system",
		section = "led",
		action = uci.trigger.service_restart("led")
	},
	{
		id = "network_restart",
		title = "Restart networking and wireless",
		package = "network",
		action = uci.trigger.service_restart("network")
	},
	{
		id = "qos_restart",
		title = "Reload Quality of Service rules",
		package = "qos",
		action = uci.trigger.service_restart("qos"),
	},
	{
		id = "wireless_restart",
		title = "Restart all wireless interfaces",
		package = "wireless",
		section = { "wifi-device", "wifi-iface" },
		action = uci.trigger.system_command("wifi"),
		belongs_to = "network_restart"
	},
}

