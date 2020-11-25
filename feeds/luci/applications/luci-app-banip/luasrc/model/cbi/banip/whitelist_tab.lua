-- Copyright 2018 Dirk Brenken (dev@brenken.org)
-- This is free software, licensed under the Apache License, Version 2.0

local fs    = require("nixio.fs")
local util  = require("luci.util")
local uci   = require("luci.model.uci").cursor()
local input = uci:get("banip", "whitelist", "ban_src") or uci:get("banip", "whitelist", "ban_src_6") or "/etc/banip/adblock.whitelist"

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

m = SimpleForm("edit", nil)
m:append(Template("banip/banip_css"))
m.submit = translate("Save")
m.reset = false

s = m:section(SimpleSection, nil,
	translatef("This form allows you to modify the content of the banIP whitelist (%s).<br />", input)
	.. translate("Please add only one IPv4 or IPv6 address per line. IP ranges in CIDR notation and comments introduced with '#' are allowed."))

f = s:option(TextValue, "data")
f.datatype = "string"
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
