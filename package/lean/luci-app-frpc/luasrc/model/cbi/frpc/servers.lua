-- Copyright 2019 Xingwang Liao <kuoruan@gmail.com>
-- Licensed to the public under the MIT License.

local dsp = require "luci.dispatcher"

local m, s, o

m = Map("frpc", "%s - %s" % { translate("Frpc"), translate("Frps Servers") })

s = m:section(TypedSection, "server")
s.anonymous = true
s.addremove = true
s.sortable = true
s.template = "cbi/tblsection"
s.extedit = dsp.build_url("admin/services/frpc/servers/%s")
function s.create(...)
	local sid = TypedSection.create(...)
	if sid then
		m.uci:save("frpc")
		luci.http.redirect(s.extedit % sid)
		return
	end
end

o = s:option(DummyValue, "alias", translate("Alias"))
o.cfgvalue = function (...)
	return Value.cfgvalue(...) or translate("None")
end

o = s:option(DummyValue, "server_addr", translate("Server Addr"))
o.cfgvalue = function (...)
	return Value.cfgvalue(...) or "?"
end

o = s:option(DummyValue, "server_port", translate("Server Port"))
o.cfgvalue = function (...)
	return Value.cfgvalue(...) or "?"
end

o = s:option(DummyValue, "tcp_mux", translate("TCP Mux"))
o.cfgvalue = function (...)
	local v = Value.cfgvalue(...)
	return v == "true" and translate("True") or translate("False")
end

return m
