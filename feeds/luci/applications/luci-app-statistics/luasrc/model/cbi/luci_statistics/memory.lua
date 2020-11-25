-- Copyright 2011 Manuel Munz <freifunk at somakoma dot de>
-- Licensed to the public under the Apache License 2.0.

m = Map("luci_statistics",
	translate("Memory Plugin Configuration"),
	translate("The memory plugin collects statistics about the memory usage."))

s = m:section( NamedSection, "collectd_memory", "luci_statistics" )

enable = s:option( Flag, "enable", translate("Enable this plugin") )
enable.default = 0

-- collectd_memory.valuesabsolute (ValuesAbsolute)
valuesabsolute = s:option( Flag, "ValuesAbsolute",
	translate("Absolute values"),
	translate("When set to true, we request absolute values"))
valuesabsolute.default = 1
valuesabsolute.optional = false
valuesabsolute:depends( "enable", 1 )

-- collectd_memory.valuespercentage (ValuesPercentage)
valuespercentage = s:option( Flag, "ValuesPercentage",
	translate("Percent values"),
	translate("When set to true, we request percentage values"))
valuespercentage.default = 0
valuespercentage.optional = false
valuespercentage:depends( "enable", 1 )

return m
