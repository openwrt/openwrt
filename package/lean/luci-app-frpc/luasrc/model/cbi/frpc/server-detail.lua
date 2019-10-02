-- Copyright 2019 Xingwang Liao <kuoruan@gmail.com>
-- Licensed to the public under the MIT License.

local dsp = require "luci.dispatcher"

local m, s, o

local sid = arg[1]

m = Map("frpc", "%s - %s" % { translate("Frpc"), translate("Edit Frps Server") })
m.redirect = dsp.build_url("admin/services/frpc/servers")

if m.uci:get("frpc", sid) ~= "server" then
	luci.http.redirect(m.redirect)
	return
end

s = m:section(NamedSection, sid, "server")
s.anonymous = true
s.addremove = false

o = s:option(Value, "alias", translate("Alias"))

o = s:option(Value, "server_addr", translate("Server addr"))
o.datatype = "host"
o.rmempty = false

o = s:option(Value, "server_port", translate("Server port"))
o.datatype = "port"
o.rmempty = false

o = s:option(Value, "token", translate("Token"))
o.password = true

o = s:option(Flag, "tcp_mux", translate("TCP mux"))
o.enabled = "true"
o.disabled = "false"
o.defalut = o.enabled
o.rmempty = false

return m
