require("luci.ip")
local uci = require "luci.model.uci".cursor()
local multilock = uci:get("custom", "multiuser", "multi") or "0"
local rootlock = uci:get("custom", "multiuser", "root") or "0"

local m = Map("guestwifi", translate("Guest Wifi Configuration"), translate("Set up a Guest Wifi"))

e = m:section(NamedSection, "guest", "")

m.on_init = function(self)
	--luci.sys.call("/usr/lib/wireguard/keygen.sh " .. arg[1])
end

btn = e:option(Button, "_btn", translate(" "))
btn.inputtitle = translate("Back to Main Page")
btn.inputstyle = "apply"
btn.redirect = luci.dispatcher.build_url(
	"admin", "network", "guestwifi"
)
function btn.write(self, section, value)
	luci.http.redirect( self.redirect )
end

local s = m:section( NamedSection, arg[1], "guestwifi", translate("Instance Name : " .. arg[1]) )

ssid = s:option(Value, "ssid", translate("SSID :")); 
ssid.rmempty = true;
ssid.optional=false;
ssid.default="Guest";

bl = s:option(ListValue, "freq", translate("Frequency :"));
bl:value("0", "2.4Ghz")
bl.rmempty = true;
bl.optional=false;
wifi5g = uci:get("guestwifi", arg[1], "radio5g")
if wifi5g == "1" then
	bl:value("1", "5.0Ghz")
end

el = s:option(ListValue, "encrypted", translate("Encryption :"));
el:value("0", translate("None"))
el:value("1", translate("WPA-PSK (Medium Security)"))
el:value("2", translate("WPA2-PSK (Strong Security)"))
el.default=0

pass = s:option(Value, "password", translate("Password :")); 
pass.rmempty = true;
pass.optional=false;
pass.default="";
pass.datatype="wpakey";
pass.password = true

if (multilock == "0") then
	ql = s:option(ListValue, "qos", translate("Bandwidth Limited :"));
	ql:value("0", "Disabled")
	ql:value("1", "Enabled")
	ql.default=0

	dl = s:option(Value, "dl", translate("Download Speed (Mbit/s) :"));
	dl.optional=false; 
	dl.rmempty = true;
	dl.datatype = "and(uinteger,min(1))"
	dl.default=10

	ul = s:option(Value, "ul", translate("Upload Speed (Mbit/s) :"));
	ul.optional=false; 
	ul.rmempty = true;
	ul.datatype = "and(uinteger,min(1))"
	ul.default=2
end

return m