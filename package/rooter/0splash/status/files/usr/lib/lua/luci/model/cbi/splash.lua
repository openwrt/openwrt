local fs  = require "nixio.fs"
local sys = require "luci.sys"
local uci = require "luci.model.uci".cursor()
require("luci.util")
require("luci.model.ipkg")

local m = Map("splash", translate("Splash Screen"), translate("Enable/disable Splash Screen"))

m.on_after_save = function(self)
	luci.sys.call("/usr/lib/splash/splash.sh &")
end

gwx = m:section(TypedSection, "settings", translate("Management"))
gwx.anonymous = true


bl = gwx:option(ListValue, "enabled", "Enable Splash Screen :");
bl:value("0", "Disabled")
bl:value("1", "Enabled")
bl.default=0

return m