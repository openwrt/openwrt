-- Licensed to the public under the Apache License 2.0.

m = Map("luci_statistics",
	translate("CPU Frequency Plugin Configuration"),
	translate("This plugin collects statistics about the processor frequency scaling."))

-- collectd_cpufreq config section
s = m:section( NamedSection, "collectd_cpufreq", "luci_statistics" )

-- collectd_cpufreq.enable
enable = s:option( Flag, "enable", translate("Enable this plugin") )
enable.default = 0

-- collectd_cpufreq.extraitems
extraitems = s:option( Flag, "ExtraItems", translate("Extra items"), translate("More details about frequency usage and transitions"))
extraitems.default = "0"
extraitems.optional = true
extraitems:depends( "enable", 1 )

return m
