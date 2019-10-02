-- Copyright 2019 Xingwang Liao <kuoruan@gmail.com>
-- Licensed to the public under the MIT License.

local dsp = require "luci.dispatcher"

local m, s, o

m = Map("frpc", "%s - %s" % { translate("Frpc"), translate("Proxy Rules") })

s = m:section(TypedSection, "rule")
s.anonymous = true
s.addremove = true
s.sortable = true
s.template = "cbi/tblsection"
s.extedit = dsp.build_url("admin/services/frpc/rules/%s")
function s.create(...)
	local sid = TypedSection.create(...)
	if sid then
		m.uci:save("frpc")
		luci.http.redirect(s.extedit % sid)
		return
	end
end

o = s:option(Flag, "disabled", translate("Disabled"))

o = s:option(DummyValue, "name", translate("Name"))
o.cfgvalue = function (...)
	return Value.cfgvalue(...) or "?"
end

o = s:option(DummyValue, "type", translate("Type"))
o.cfgvalue = function (...)
	local v = Value.cfgvalue(...)
	return v and v:upper() or "?"
end

o = s:option(DummyValue, "local_ip", translate("Local IP"))
o.cfgvalue = function (...)
	return Value.cfgvalue(...) or "?"
end

o = s:option(DummyValue, "local_port", translate("Local Port"))
o.cfgvalue = function (...)
	return Value.cfgvalue(...) or "?"
end

o = s:option(DummyValue, "remote_port", translate("Remote Port"))
o.cfgvalue = function (...)
	return Value.cfgvalue(...) or translate("Not set")
end

return m
