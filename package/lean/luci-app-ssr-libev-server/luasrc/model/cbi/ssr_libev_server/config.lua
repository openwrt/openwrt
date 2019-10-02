local i = "ssr_libev_server"
local d = require "luci.dispatcher"
local a, t, e

local encrypt_method = {
    "table", "rc4", "rc4-md5", "aes-128-cfb", "aes-192-cfb", "aes-256-cfb",
    "aes-128-ctr", "aes-192-ctr", "aes-256-ctr", "bf-cfb", "camellia-128-cfb",
    "camellia-192-cfb", "camellia-256-cfb", "cast5-cfb", "des-cfb", "idea-cfb",
    "rc2-cfb", "seed-cfb", "salsa20", "chacha20", "chacha20-ietf"
}

local protocol = {"origin"}
local obfs = {"plain", "http_simple", "http_post"}

a = Map(i, "ShadowsocksR Libev " .. translate("Server Config"))
a.redirect = d.build_url("admin", "vpn", "ssr_libev_server")

t = a:section(NamedSection, arg[1], "user", "")
t.addremove = false
t.dynamic = false

e = t:option(Flag, "enable", translate("Enable"))
e.default = "1"
e.rmempty = false

e = t:option(Value, "remarks", translate("Remarks"))
e.default = translate("Remarks")
e.rmempty = false

e = t:option(Value, "port", translate("Port"))
e.datatype = "port"
e.rmempty = false

e = t:option(Value, "password", translate("Password"))
e.password = true
e.rmempty = false

e = t:option(ListValue, "method", translate("Encrypt Method"))
for a, t in ipairs(encrypt_method) do e:value(t) end

e = t:option(ListValue, "protocol", translate("Protocol"))
for a, t in ipairs(protocol) do e:value(t) end

e = t:option(Value, "protocol_param", translate("Protocol Param"))

e = t:option(ListValue, "obfs", translate("Obfs"))
for a, t in ipairs(obfs) do e:value(t) end

e = t:option(Value, "obfs_param", translate("Obfs Param"))

e = t:option(Value, "timeout", translate("Connection Timeout"))
e.datatype = "uinteger"
e.default = 300
e.rmempty = false

e = t:option(ListValue, "fast_open", translate("Fast Open"))
e:value("false")
e:value("true")
e.rmempty = false

e = t:option(Flag, "udp_forward", translate("UDP Forward"))
e.default = "1"
e.rmempty = false

return a
