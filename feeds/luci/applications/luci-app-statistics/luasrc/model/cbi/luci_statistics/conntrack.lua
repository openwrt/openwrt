-- Copyright 2011 Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

m = Map("luci_statistics",
	translate("Conntrack Plugin Configuration"),
	translate("The conntrack plugin collects statistics about the number of tracked connections."))

s = m:section( NamedSection, "collectd_conntrack", "luci_statistics" )

enable = s:option( Flag, "enable", translate("Enable this plugin") )
enable.default = 0

return m
