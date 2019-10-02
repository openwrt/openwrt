local d = require "luci.dispatcher"
local fs = require "nixio.fs"
local sys = require "luci.sys"
local uci = require "luci.model.uci".cursor()
local appname = "passwall"

m = Map(appname)
m:section(SimpleSection).template  = "passwall/global/status"
-- [[ Other Settings ]]--
s = m:section(TypedSection, "global_other")
s.anonymous = true



-- [[ Servers List ]]--
s = m:section(TypedSection, "servers")
s.anonymous = true
s.sortable = true
s.addremove = true
s.template = "cbi/tblsection"
s.extedit = d.build_url("admin", "vpn", "passwall", "serverconfig", "%s")
function s.create(e, t)
    local e = TypedSection.create(e, t)
    luci.http.redirect(
        d.build_url("admin", "vpn", "passwall", "serverconfig", e))
end

function s.remove(t, a)
    s.map.proceed = true
    s.map:del(a)
    luci.http.redirect(d.build_url("admin", "vpn", "passwall", "server_list"))
end

---- Node Remarks
o = s:option(DummyValue, "remarks", translate("Node Remarks"))
o.width = "20%"

---- Server Type
o = s:option(DummyValue, "server_type", translate("Server Type"))
o.width = "10%"

---- Server Address
o = s:option(DummyValue, "server", translate("Server Address"))
o.width = "15%"

---- Server Port
o = s:option(DummyValue, "server_port", translate("Server Port"))
o.width = "10%"

o = s:option(DummyValue, "encrypt_method", translate("Encrypt Method"))
o.width="15%"
o.cfgvalue=function(t, n)
local str="无"
local type = m.uci:get(appname, n, "server_type") or ""
if type == "SSR" then
	return m.uci:get(appname, n, "ssr_encrypt_method")
elseif type == "SS" then
	return m.uci:get(appname, n, "ss_encrypt_method")
elseif type == "V2ray" then
	return m.uci:get(appname, n, "v2ray_security")
end
return str
end
o = s:option(DummyValue, "protocol", translate("Protocol"))
o.width = "10%"
o = s:option(DummyValue, "obfs", translate("Obfs"))

o.width = "10%"

o = s:option(Flag,"use_kcp",translate("Kcptun Switch"))
o.width="10%"



---- Ping
o = s:option(DummyValue, "server", translate("Ping Latency"))
if uci:get(appname, "@global_other[0]", "auto_ping") == "0" then
    o.template = "passwall/server_list/ping"
else
    o.template = "passwall/server_list/auto_ping"
end
o.width = "10%"

---- Apply
o = s:option(DummyValue, "apply", translate("Apply"))
o.width = "15%"
o.template = "passwall/server_list/apply"

m:append(Template("passwall/server_list/server_list"))

if luci.http.formvalue("cbi.apply") then
    luci.http.redirect(d.build_url("admin", "vpn", "passwall", "server_list"))
end

return m
