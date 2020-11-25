-- Copyright 2013 Thomas Endt <tmo26@gmx.de>
-- Licensed to the public under the Apache License 2.0.

m = Map("luci_statistics",
        translate("Uptime Plugin Configuration"),
        translate("The uptime plugin collects statistics about the uptime of the system."))

s = m:section( NamedSection, "collectd_uptime", "luci_statistics" )

enable = s:option( Flag, "enable", translate("Enable this plugin") )
enable.default = 0

return m

