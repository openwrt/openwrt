local o = require "luci.dispatcher"
local fs = require "nixio.fs"
local sys = require "luci.sys"
local ipkg = require("luci.model.ipkg")
local uci = require"luci.model.uci".cursor()
local appname = "passwall"

local function is_installed(e) return ipkg.installed(e) end

local function is_finded(e)
    return
        sys.exec("find /usr/*bin -iname " .. e .. " -type f") ~= "" and true or
            false
end

local function has_udp_relay()
    return sys.call("lsmod | grep TPROXY >/dev/null") == 0
end

local n = {}
uci:foreach(appname, "servers", function(e)
    if e.server_type and e.server and e.remarks then
        if e.use_kcp and e.use_kcp == "1" then
            n[e[".name"]] = "%s+%s：[%s] %s" %
                                {e.server_type, "Kcptun", e.remarks, e.server}
        else
            n[e[".name"]] = "%s：[%s] %s" %
                                {e.server_type, e.remarks, e.server}
        end
    end
end)

local key_table = {}
for key, _ in pairs(n) do table.insert(key_table, key) end
table.sort(key_table)

m = Map(appname)
m:append(Template("passwall/global/status"))

-- [[ Global Settings ]]--
s = m:section(TypedSection, "global", translate("Global Settings"))
s.anonymous = true
s.addremove = false

---- TCP Redir Server
o = s:option(ListValue, "tcp_redir_server", translate("TCP Redir Server"),
             translate("For used to surf the Internet."))
o:value("nil", translate("Close"))
for _, key in pairs(key_table) do o:value(key, n[key]) end

local tcp_redir_server_num = uci:get(appname, "@global_other[0]",
                                     "tcp_redir_server_num")
if tcp_redir_server_num and tonumber(tcp_redir_server_num) >= 2 then
    for i = 2, tcp_redir_server_num, 1 do
        o = s:option(ListValue, "tcp_redir_server" .. i,
                     translate("TCP Redir Server") .. " " .. i)
        o:value("nil", translate("Close"))
        for _, key in pairs(key_table) do o:value(key, n[key]) end
    end
end

---- UDP Redir Server
if has_udp_relay() then
    o = s:option(ListValue, "udp_redir_server", translate("UDP Redir Server"),
                 translate("For Game Mode or DNS resolution and more.") ..
                     translate("The selected server will not use Kcptun."))
    o:value("nil", translate("Close"))
    o:value("default", translate("Same as the tcp redir server"))
    for _, key in pairs(key_table) do o:value(key, n[key]) end

    local udp_redir_server_num = uci:get(appname, "@global_other[0]",
                                         "udp_redir_server_num")
    if udp_redir_server_num and tonumber(udp_redir_server_num) >= 2 then
        for i = 2, udp_redir_server_num, 1 do
            o = s:option(ListValue, "udp_redir_server" .. i,
                         translate("UDP Redir Server") .. " " .. i)
            o:value("nil", translate("Close"))
            for _, key in pairs(key_table) do o:value(key, n[key]) end
        end
    end
end

---- Socks5 Proxy Server
o = s:option(ListValue, "socks5_proxy_server", translate("Socks5 Proxy Server"),
             translate("The client can use the router's Socks5 proxy"))
o:value("nil", translate("Close"))
for _, key in pairs(key_table) do o:value(key, n[key]) end

---- DNS Forward Mode
o = s:option(ListValue, "dns_mode", translate("DNS Forward Mode"))
o.rmempty = false
o:reset_values()
if is_installed("ChinaDNS") or is_finded("chinadns") then
    o:value("chinadns", "ChinaDNS")
end
if (is_installed("dns2socks") or is_finded("dns2socks")) and
    (is_finded("ss-local") or is_finded("ssr-local")) then
    o:value("dns2socks",
            "dns2socks " .. translate("Only SS/R servers are supported"))
end
if is_installed("pcap-dnsproxy") or is_finded("Pcap_DNSProxy") then
    o:value("Pcap_DNSProxy", "Pcap_DNSProxy")
end
if is_installed("pdnsd") or is_installed("pdnsd-alt") or is_finded("pdnsd") then
    o:value("pdnsd", "pdnsd")
end
o:value("local_7913", translate("Use local port 7913 as DNS"))

---- upstreamm DNS Server for ChinaDNS
o = s:option(ListValue, "up_chinadns_mode",
             translate("upstreamm DNS Server for ChinaDNS"), translate(
                 "Domestic DNS server 1 in advanced Settings is used as domestic DNS by default"))
o.default = "OpenDNS_1"
o:depends("dns_mode", "chinadns")
if is_installed("dnsproxy") or is_finded("dnsproxy") then
    o:value("dnsproxy", "dnsproxy")
end
if is_installed("dns-forwarder") or is_finded("dns-forwarder") then
    o:value("dns-forwarder", "dns-forwarder")
end
o:value("OpenDNS_1", "OpenDNS_1")
o:value("OpenDNS_2", "OpenDNS_2")
o:value("custom", translate("custom"))

---- upstreamm DNS Server
o = s:option(Value, "up_chinadns_custom", translate("DNS Server"), translate(
                 "example: 114.114.114.114,208.67.222.222:443,8.8.8.8<br>Need at least one,Other DNS services can be used as upstream, such as smartdns."))
o.default = "114.114.114.114,208.67.222.222:5353"
o:depends("up_chinadns_mode", "custom")

---- Default Proxy Mode
o = s:option(ListValue, "proxy_mode",
             translate("Default") .. translate("Proxy Mode"))
o.default = "gfwlist"
o.rmempty = false
o:value("disable", translate("No Proxy"))
o:value("global", translate("Global Proxy"))
o:value("gfwlist", translate("GFW List"))
o:value("chnroute", translate("China WhiteList"))
o:value("gamemode", translate("Game Mode"))
o:value("returnhome", translate("Return Home"))

---- Localhost Proxy Mode
o = s:option(ListValue, "localhost_proxy_mode",
             translate("Localhost") .. translate("Proxy Mode"), translate(
                 "The server client can also use this rule to scientifically surf the Internet"))
o:value("default", translate("Default"))
-- o:value("global", translate("Global Proxy").."（"..translate("Danger").."）")
o:value("gfwlist", translate("GFW List"))
-- o:value("chnroute", translate("China WhiteList"))
o.default = "default"
o.rmempty = false
m:section(SimpleSection).template  = "passwall/server_list/checkport"
--[[
local apply = luci.http.formvalue("cbi.apply")
if apply then
os.execute("/etc/init.d/passwall restart")
end
--]]

return m
