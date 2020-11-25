-- Copyright 2017-2018 Dirk Brenken (dev@brenken.org)
-- This is free software, licensed under the Apache License, Version 2.0

local fs    = require("nixio.fs")
local util  = require("luci.util")
local input = "/etc/config/firewall"

if not fs.access(input) then
	m = SimpleForm("error", nil, translate("Input file not found, please check your configuration."))
	return m
end

m = SimpleForm("input", nil)
m:append(Template("travelmate/travelmate_css"))
m.submit = translate("Save")
m.reset = false

s = m:section(SimpleSection, nil,
	translate("This form allows you to modify the content of the main firewall configuration file (/etc/config/firewall)."))

f = s:option(TextValue, "data")
f.rows = 20
f.rmempty = true

function f.cfgvalue()
	return fs.readfile(input) or ""
end

function f.write(self, section, data)
	return fs.writefile(input, "\n" .. util.trim(data:gsub("\r\n", "\n")) .. "\n")
end

function f.remove(self, section, value)
	return fs.writefile(input, "")
end

function s.handle(self, state, data)
	return true
end

return m
