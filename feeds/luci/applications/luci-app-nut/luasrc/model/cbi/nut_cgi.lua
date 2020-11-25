-- Copyright 2015 Daniel Dickinson <openwrt@daniel.thecshore.com>
-- Licensed to the public under the Apache License 2.0.

local m, s, o

m = Map("nut_cgi", translate("Network UPS Tools (CGI)"),
	translate("Network UPS Tools CGI Configuration"))

s = m:section(SimpleSection, translate("NUT CGI Access"))
s.addremove = false
s.anonymous = true

o = s:option(DummyValue, "nut_cgi_go", '<a href="/nut">' .. translate("Go to NUT CGI") .. '</a>')
o.section = "cbi-nut_cgi"

s = m:section(TypedSection, "host", translate("Host"))
s.addremove = true
s.anonymous = true

o = s:option(Value, "upsname", translate("UPS name"), translate("As configured by NUT"))
o.optional = false

o = s:option(Value, "hostname", translate("Hostname or IP address"))
o.optional = false
o.datatype = "host"

o = s:option(Value, "port", translate("Port"))
o.datatype = "port"
o.optional = true
o.placeholder = 3493

o = s:option(Value, "displayname", translate("Display name"))
o.optional = false

s = m:section(TypedSection, "upsset", translate("Control UPS via CGI"))
s.addremove = false
s.anonymous = true
s.optional = false

o = s:option(Flag, "enable", translate("Enable"))
o.optional = false
o.default = false

return m
