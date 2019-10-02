local sys = require "luci.sys"
local webadmin = require "luci.tools.webadmin"
local uci = require"luci.model.uci".cursor()
local appname = "passwall"

local n = {}
uci:foreach(appname, "servers", function(e)
    if e.server_type and e.server and e.remarks then
        if e.use_kcp and e.use_kcp == "1" then
            n[e[".name"]] = "%s+%s：[%s]" %
                                {e.server_type, "Kcptun", e.remarks}
        else
            n[e[".name"]] = "%s：[%s]" % {e.server_type, e.remarks}
        end
    end
end)

local key_table = {}
for key, _ in pairs(n) do table.insert(key_table, key) end
table.sort(key_table)

m = Map("passwall")

-- [[ ACLs Settings ]]--
s = m:section(TypedSection, "acl_rule", translate("ACLs"), translate(
                  "ACLs is a tools which used to designate specific IP proxy mode"))
s.template = "cbi/tblsection"
s.sortable = true
s.anonymous = true
s.addremove = true

---- Enable
o = s:option(Flag, "enabled", translate("Enable"))
o.rmempty = false

---- ACL Remarks
o = s:option(Value, "aclremarks", translate("ACL Remarks"))
o.rmempty = true

---- IP Address
o = s:option(Value, "ipaddr", translate("IP Address"))
o.datatype = "ip4addr"
o.rmempty = true

local temp = {}
for index, n in ipairs(luci.ip.neighbors({family = 4})) do
    if n.dest then temp[index] = n.dest:string() end
end
local ips = {}
for _, key in pairs(temp) do table.insert(ips, key) end
table.sort(ips)

for index, key in pairs(ips) do o:value(key, temp[key]) end
-- webadmin.cbi_add_knownips(o)

---- MAC Address
o = s:option(Value, "macaddr", translate("MAC Address"))
o.rmempty = true
sys.net.mac_hints(function(e, t) o:value(e, "%s " % {e}) end)

---- TCP Redir Server
local tcp_redir_server_num = uci:get(appname, "@global_other[0]",
                                     "tcp_redir_server_num")
o = s:option(ListValue, "tcp_redir_server", translate("TCP Server"))
o:value("1",translate("TCP Redir Server").." 1")
if tcp_redir_server_num and tonumber(tcp_redir_server_num) >= 2 then
    for i = 2, tcp_redir_server_num, 1 do o:value(i,translate("TCP Redir Server").." "..i) end
end

---- UDP Redir Server
local udp_redir_server_num = uci:get(appname, "@global_other[0]",
                                     "udp_redir_server_num")
o = s:option(ListValue, "udp_redir_server", translate("UDP Server"))
o:value("1",translate("UDP Redir Server").." 1")
if udp_redir_server_num and tonumber(udp_redir_server_num) >= 2 then
    for i = 2, udp_redir_server_num, 1 do o:value(i,translate("UDP Redir Server").." "..i) end
end

---- Proxy Mode
o = s:option(ListValue, "proxy_mode", translate("Proxy Mode"))
o.default = "default"
o.rmempty = false
o:value("default", translate("Default"))
o:value("disable", translate("No Proxy"))
o:value("global", translate("Global Proxy"))
o:value("gfwlist", translate("GFW List"))
o:value("chnroute", translate("China WhiteList"))
o:value("gamemode", translate("Game Mode"))
o:value("returnhome", translate("Return Home"))

---- TCP Redir Ports
o = s:option(Value, "tcp_redir_ports", translate("TCP Redir Ports"))
o.default = "default"
o:value("default", translate("Default"))
o:value("1:65535", translate("All"))
o:value("80,443", "80,443")
o:value("80:", "80 " .. translate("or more"))
o:value(":443", "443 " .. translate("or less"))

---- UDP Redir Ports
o = s:option(Value, "udp_redir_ports", translate("UDP Redir Ports"))
o.default = "default"
o:value("default", translate("Default"))
o:value("1:65535", translate("All"))
o:value("53", "53")

return m
