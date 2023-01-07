

local sys   = require "luci.sys"
local zones = require "luci.sys.zoneinfo"
local fs    = require "nixio.fs"
local conf  = require "luci.config"

m = Map("system", "Change GUI Theme",
	translate("Change the Theme of the GUI"))
m:chain("luci")
	
s = m:section(TypedSection, "system", "Available Themes")
s.anonymous = true
s.addremove = false

o = s:option(ListValue, "_mediaurlbase", translate("Design"))
for k, v in pairs(conf.themes) do
	if k:sub(1, 1) ~= "." then
		if string.match(k, "Bootstrap") then
		else
			o:value(v, k)
		end
	end
end

function o.cfgvalue(...)
	return m.uci:get("luci", "main", "mediaurlbase")
end

function o.write(self, section, value)
	m.uci:set("luci", "main", "mediaurlbase", value)
end	
	
return m