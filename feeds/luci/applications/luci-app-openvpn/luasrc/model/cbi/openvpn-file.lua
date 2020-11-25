-- Licensed to the public under the Apache License 2.0.

local ip        = require("luci.ip")
local fs        = require("nixio.fs")
local util      = require("luci.util")
local uci       = require("luci.model.uci").cursor()
local cfg_file  = uci:get("openvpn", arg[1], "config")
local auth_file = cfg_file:match("(.+)%..+").. ".auth"

local m = Map("openvpn")

local p = m:section( SimpleSection )
p.template = "openvpn/pageswitch"
p.mode     = "file"
p.instance = arg[1]

if not cfg_file or not fs.access(cfg_file) then
	local f = SimpleForm("error", nil, translatef("The OVPN config file (%s) could not be found, please check your configuration.", cfg_file or "n/a"))
	f:append(Template("openvpn/ovpn_css"))
	f.reset = false
	f.submit = false
	return m, f
end

if fs.stat(cfg_file).size >= 102400 then
	f = SimpleForm("error", nil,
		translatef("The size of the OVPN config file (%s) is too large for online editing in LuCI (&ge; 100 KB). ", cfg_file)
		.. translate("Please edit this file directly in a terminal session."))
	f:append(Template("openvpn/ovpn_css"))
	f.reset = false
	f.submit = false
	return m, f
end

f = SimpleForm("cfg", nil)
f:append(Template("openvpn/ovpn_css"))
f.submit = translate("Save")
f.reset = false

s = f:section(SimpleSection, nil, translatef("Section to modify the OVPN config file (%s)", cfg_file))
file = s:option(TextValue, "data1")
file.datatype = "string"
file.rows = 20

function file.cfgvalue()
	return fs.readfile(cfg_file) or ""
end

function file.write(self, section, data1)
	return fs.writefile(cfg_file, "\n" .. util.trim(data1:gsub("\r\n", "\n")) .. "\n")
end

function file.remove(self, section, value)
	return fs.writefile(cfg_file, "")
end

function s.handle(self, state, data1)
	return true
end

s = f:section(SimpleSection, nil, translatef("Section to add an optional 'auth-user-pass' file with your credentials (%s)", auth_file))
file = s:option(TextValue, "data2")
file.datatype = "string"
file.rows = 5

function file.cfgvalue()
	return fs.readfile(auth_file) or ""
end

function file.write(self, section, data2)
	return fs.writefile(auth_file, util.trim(data2:gsub("\r\n", "\n")) .. "\n")
end

function file.remove(self, section, value)
	return fs.writefile(auth_file, "")
end

function s.handle(self, state, data2)
	return true
end

return m, f
