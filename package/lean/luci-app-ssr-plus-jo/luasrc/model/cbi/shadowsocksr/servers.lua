-- Licensed to the public under the GNU General Public License v3.
local d = require "luci.dispatcher"
local fs = require "nixio.fs"
local sys = require "luci.sys"
local uci = require "luci.model.uci".cursor()
local m, s, o
local shadowsocksr = "shadowsocksr"

local uci = luci.model.uci.cursor()
local server_count = 0
uci:foreach("shadowsocksr", "servers", function(s)
  server_count = server_count + 1
end)

m = Map(shadowsocksr)


m:section(SimpleSection).template  = "shadowsocksr/status"
-- [[ Servers Manage ]]--
s = m:section(TypedSection, "servers")
s.anonymous = true
s.addremove = true
s.sortable = true
s.template = "cbi/tblsection"
s.description = string.format(translate("Server Count") ..  ": %d", server_count)
s.extedit = d.build_url("admin", "services", "shadowsocksr", "servers", "%s")

function s.create(e, t)
    local e = TypedSection.create(e, t)
    luci.http.redirect(
        d.build_url("admin", "services", "shadowsocksr", "servers", e))
end

function s.remove(t, a)
    s.map.proceed = true
    s.map:del(a)
    luci.http.redirect(d.build_url("admin", "services", "shadowsocksr", "servers"))
end
o = s:option(DummyValue, "type", translate("Type"))
function o.cfgvalue(...)
	return Value.cfgvalue(...) or translate("")
end

o = s:option(DummyValue, "alias", translate("Alias"))
function o.cfgvalue(...)
	return Value.cfgvalue(...) or translate("None")
end

o = s:option(DummyValue, "server", translate("Server Address"))
function o.cfgvalue(...)
	return Value.cfgvalue(...) or "?"
end

o = s:option(DummyValue, "encrypt_method", translate("Encrypt Method"))
o.width="10%"


o = s:option(DummyValue, "server_port", translate("Server Port"))
function o.cfgvalue(...)
	return Value.cfgvalue(...) or "?"
end

if nixio.fs.access("/usr/bin/kcptun-client") then

o = s:option(Flag, "kcp_enable", translate("KcpTun"))
function o.cfgvalue(...)
	return Value.cfgvalue(...) or "?"
end

end

o = s:option(DummyValue, "switch_enable", translate("Auto Switch"))
function o.cfgvalue(...)
	return Value.cfgvalue(...) or "0"
end


o = s:option(DummyValue,"server",translate("Ping Latency"))
o.template="shadowsocksr/ping"
o.width="10%"


m:append(Template("shadowsocksr/server_list"))
return m
