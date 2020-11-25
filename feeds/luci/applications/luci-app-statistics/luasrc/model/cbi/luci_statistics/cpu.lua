-- Copyright 2008 Freifunk Leipzig / Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

m = Map("luci_statistics",
	translate("CPU Plugin Configuration"),
	translate("The cpu plugin collects basic statistics about the processor usage."))

-- collectd_cpu config section
s = m:section( NamedSection, "collectd_cpu", "luci_statistics" )

-- collectd_cpu.enable
enable = s:option( Flag, "enable", translate("Enable this plugin") )
enable.default = 0

-- collectd_cpu.reportbycpu (ReportByCpu)
reportbycpu = s:option( Flag, "ReportByCpu",
	translate("Report by CPU"),
	translate("By setting this, CPU is not aggregate of all processors on the system"))
reportbycpu.default = 1
reportbycpu:depends( "enable", 1 )

-- collectd_cpu.reportbystate (ReportByState)
reportbystate = s:option( Flag, "ReportByState",
	translate("Report by state"),
	translate("When set to true, reports per-state metric (system, user, idle)"))
reportbystate.default = 1
reportbystate:depends( "enable", 1 )

-- collectd_cpu.valuespercentage (ValuesPercentage)
valuespercentage = s:option( Flag, "ValuesPercentage",
	translate("Report in percent"),
	translate("When set to true, we request percentage values"))
valuespercentage.default = 0
valuespercentage:depends({ enable = 1, ReportByCpu = 1, ReportByState = 1 })

return m
