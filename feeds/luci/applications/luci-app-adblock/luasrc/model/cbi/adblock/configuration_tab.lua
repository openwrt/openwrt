-- Copyright 2017-2018 Dirk Brenken (dev@brenken.org)
-- This is free software, licensed under the Apache License, Version 2.0

local fs    = require("nixio.fs")
local util  = require("luci.util")
local input = "/etc/config/adblock"

if not fs.access(input) then
	m = SimpleForm("error", nil, translate("Input file not found, please check your configuration."))
	m.reset = false
	m.submit = false
	return m
end

if fs.stat(input).size >= 102400 then
	m = SimpleForm("error", nil,
		translate("The file size is too large for online editing in LuCI (&ge; 100 KB). ")
		.. translate("Please edit this file directly in a terminal session."))
	m.reset = false
	m.submit = false
	return m
end

m = SimpleForm("input", nil)
m:append(Template("adblock/adblock_css"))
m.submit = translate("Save")
m.reset = false

s = m:section(SimpleSection, nil,
	translate("This form allows you to modify the content of the main adblock configuration file (/etc/config/adblock)."))

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
