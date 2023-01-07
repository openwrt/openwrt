

local sys   = require "luci.sys"
local zones = require "luci.sys.zoneinfo"
local fs    = require "nixio.fs"
local conf  = require "luci.config"

m = Map("iframe", translate("Splash Screen Configuration"),translate("Change the configuration of the Splash and Login screen."))
m:chain("luci")
	
s = m:section(TypedSection, "iframe", translate("Status Page Configuration"))
s.anonymous = true
s.addremove = false

c1 = s:option(ListValue, "splashpage", translate("Enable Network Status Page Before Login :"));
c1:value("0", translate("Disabled"))
c1:value("1", translate("Enabled"))
c1.default=0

a1 = s:option(Value, "splashtitle", translate("Network Status Title :")); 
a1.optional=false;
a1.default = translate("ROOter Status")
a1:depends("splashpage", "1")

dc1 = s:option(ListValue, "dual", translate("Enable Modem 2 Status :"));
dc1:value("0", translate("Disabled"))
dc1:value("1", translate("Enabled"))
dc1.default=0
dc1:depends("splashpage", "1")

cc1 = s:option(ListValue, "speed", translate("Enable OpenSpeedTest :"));
cc1:value("0", translate("Disabled"))
cc1:value("1", translate("Enabled"))
cc1.default=0
cc1:depends("splashpage", "1")

ec1 = s:option(ListValue, "band", translate("Enable Bandwidth Summary :"));
ec1:value("0", translate("Disabled"))
ec1:value("1", translate("Enabled"))
ec1.default=0
ec1:depends("splashpage", "1")

	
return m