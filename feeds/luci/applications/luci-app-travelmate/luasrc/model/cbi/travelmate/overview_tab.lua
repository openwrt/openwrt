-- Copyright 2017-2019 Dirk Brenken (dev@brenken.org)
-- This is free software, licensed under the Apache License, Version 2.0

local fs       = require("nixio.fs")
local uci      = require("luci.model.uci").cursor()
local util     = require("luci.util")
local nw       = require("luci.model.network").init()
local fw       = require("luci.model.firewall").init()
local dump     = util.ubus("network.interface", "dump", {})
local trmiface = uci:get("travelmate", "global", "trm_iface") or "trm_wwan"
local uplink   = uci:get("network", trmiface) or ""

m = Map("travelmate", translate("Travelmate"),
	translate("Configuration of the travelmate package to to enable travel router functionality. ")
	.. translatef("For further information "
	.. "<a href=\"%s\" target=\"_blank\">"
	.. "see online documentation</a>", "https://github.com/openwrt/packages/blob/master/net/travelmate/files/README.md"))
m:chain("network")
m:chain("firewall")

-- Interface Wizard

if uplink == "" then
	ds = m:section(NamedSection, "global", "travelmate", translate("Interface Wizard"))
	o = ds:option(Value, "trm_iface", translate("Create Uplink interface"),
		translate("Create a new wireless wan uplink interface, configure it to use dhcp and ")
		.. translate("add it to the wan zone of the firewall. ")
		.. translate("This step has only to be done once."))
	o.datatype = "and(uciname,rangelength(3,15))"
	o.default = trmiface
	o.rmempty = false

	function o.validate(self, value)
		if value then
			local nwnet = nw:get_network(value)
			local zone  = fw:get_zone("wan")
			local fwnet = fw:get_zone_by_network(value)
			if not nwnet then
				nwnet = nw:add_network(value, { proto = "dhcp" })
			end
			if zone and not fwnet then
				fwnet = zone:add_network(value)
			end
		end
		return value
	end
	return m
end

-- Main travelmate options

s = m:section(NamedSection, "global", "travelmate")

o1 = s:option(Flag, "trm_enabled", translate("Enable Travelmate"))
o1.default = o1.disabled
o1.rmempty = false

o2 = s:option(Flag, "trm_captive", translate("Captive Portal Detection"),
	translate("Check the internet availability, log captive portal redirections and keep the uplink connection 'alive'."))
o2.default = o2.enabled
o2.rmempty = false

o3 = s:option(Flag, "trm_netcheck", translate("Net Error Check"),
	translate("Treat missing internet availability as an error."))
o3:depends("trm_captive", 1)
o3.default = o3.disabled
o3.rmempty = false

o4 = s:option(Flag, "trm_proactive", translate("ProActive Uplink Switch"),
	translate("Proactively scan and switch to a higher prioritized uplink, despite of an already existing connection."))
o4.default = o4.enabled
o4.rmempty = false

o5 = s:option(Flag, "trm_autoadd", translate("Add Open Uplinks"),
	translate("Automatically add open uplinks like hotel captive portals to your wireless config."))
o5.default = o5.disabled
o5.rmempty = false

o6 = s:option(ListValue, "trm_iface", translate("Uplink / Trigger interface"),
	translate("Name of the used uplink interface."))
if dump then
	local i, v
	for i, v in ipairs(dump.interface) do
		if v.interface ~= "loopback" and v.interface ~= "lan" then
			local device = v.l3_device or v.device or "-"
			o6:value(v.interface, v.interface.. " (" ..device.. ")")
		end
	end
end
o6.default = trmiface
o6.rmempty = false

-- Runtime information

ds = s:option(DummyValue, "_dummy")
ds.template = "travelmate/runtime"

-- Extra options

e = m:section(NamedSection, "global", "travelmate", translate("Extra Options"),
translate("Options for further tweaking in case the defaults are not suitable for you."))

e1 = e:option(Flag, "trm_debug", translate("Enable Verbose Debug Logging"))
e1.default = e1.disabled
e1.rmempty = false

e2 = e:option(Value, "trm_radio", translate("Radio Selection / Order"),
	translate("Restrict travelmate to a single radio (e.g. 'radio1') or change the overall scanning order (e.g. 'radio1 radio2 radio0')."))
e2.rmempty = true

e3 = e:option(Value, "trm_listexpiry", translate("List Auto Expiry"),
	translate("Automatically resets the 'Faulty Stations' list after n minutes. Default is '0' which means no expiry."))
e3.datatype = "range(0,300)"
e3.default = 0
e3.rmempty = false

e4 = e:option(Value, "trm_triggerdelay", translate("Trigger Delay"),
	translate("Additional trigger delay in seconds before travelmate processing begins."))
e4.datatype = "range(1,60)"
e4.default = 2
e4.rmempty = false

e5 = e:option(Value, "trm_maxretry", translate("Connection Limit"),
	translate("Retry limit to connect to an uplink."))
e5.default = 5
e5.datatype = "range(1,10)"
e5.rmempty = false

e6 = e:option(Value, "trm_minquality", translate("Signal Quality Threshold"),
	translate("Minimum signal quality threshold as percent for conditional uplink (dis-) connections."))
e6.default = 35
e6.datatype = "range(20,80)"
e6.rmempty = false

e7 = e:option(Value, "trm_maxwait", translate("Interface Timeout"),
	translate("How long should travelmate wait for a successful wlan uplink connection."))
e7.default = 30
e7.datatype = "range(20,40)"
e7.rmempty = false

e8 = e:option(Value, "trm_timeout", translate("Overall Timeout"),
	translate("Overall retry timeout in seconds."))
e8.default = 60
e8.datatype = "range(30,300)"
e8.rmempty = false

e10 = e:option(Value, "trm_scanbuffer", translate("Scan Buffer Size"),
  translate("Buffer size in bytes to prepare nearby scan results."))
e10.default = 1024
e10.datatype = "range(512,4096)"
e10.optional = true

return m
