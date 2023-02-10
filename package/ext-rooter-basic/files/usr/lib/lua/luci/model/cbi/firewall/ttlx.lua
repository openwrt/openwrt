local utl = require "luci.util"
local uci  = require "luci.model.uci".cursor()

m = Map("ttl", translate("Firewall - Custom TTL Settings"),
	translate("Enable and use a custom TTL value with modems"))

m.on_after_save = function(self)
	--luci.sys.call("/usr/lib/custom/ttlx.sh &")
end

gw = m:section(TypedSection, translate("ttl"), translate("Settings"))
gw.anonymous = true

en = gw:option(Flag, "enabled", translate("Enabled :"), translate("Enable the use of custom TTL value")); 
en.default="0"
en.rmempty = false;
en.optional=false;

val = gw:option(ListValue, "value", translate("TTL Value :"), translate("Custom TTL value to be used on modems")); 
val.default="65"
val:depends("enabled", "1")
val:value("63", "63")
val:value("64", "64")
val:value("65", "65")
val:value("66", "66")
val:value("67", "67")
val:value("117", "117")
val:value("0", "TTL-INC 1")

return m