local fs = require "nixio.fs"
local net = require"luci.model.network".init()
local ifaces = require"luci.sys".net:devices()

m = Map("passwall")

-- [[ Delay Settings ]]--
s = m:section(TypedSection, "global_delay", translate("Delay Settings"))
s.anonymous = true
s.addremove = false

---- Delay Start
o = s:option(Value, "start_delay", translate("Delay Start"),
             translate("Units:seconds"))
o.default = "0"
o.rmempty = true

---- Open and close Daemon
o = s:option(Flag, "start_daemon", translate("Open and close Daemon"))
o.default = 1
o.rmempty = false

---- Open and close automatically
o = s:option(Flag, "auto_on", translate("Open and close automatically"))
o.default = 0
o.rmempty = false

---- Automatically turn off time
o = s:option(ListValue, "time_off", translate("Automatically turn off time"))
o.default = nil
o:depends("auto_on", "1")
o:value(nil, translate("Disable"))
for e = 0, 23 do o:value(e, e .. translate("oclock")) end

---- Automatically turn on time
o = s:option(ListValue, "time_on", translate("Automatically turn on time"))
o.default = nil
o:depends("auto_on", "1")
o:value(nil, translate("Disable"))
for e = 0, 23 do o:value(e, e .. translate("oclock")) end

---- Automatically restart time
o = s:option(ListValue, "time_restart", translate("Automatically restart time"))
o.default = nil
o:depends("auto_on", "1")
o:value(nil, translate("Disable"))
for e = 0, 23 do o:value(e, e .. translate("oclock")) end

-- [[ DNS Settings ]]--
s = m:section(TypedSection, "global_dns", translate("DNS Settings"))
s.anonymous = true
s.addremove = false

---- DNS Forward Address
o = s:option(Value, "dns_forward", translate("DNS Forward Address"))
o.rmempty = false
o.default = "8.8.4.4:53"
o:value("8.8.4.4:53", "8.8.4.4:53(Google DNS1)")
o:value("8.8.8.8:53", "8.8.8.8:53(Google DNS2)")
o:value("208.67.222.222:53", "208.67.222.222:53(OpenDNS DNS1_53)")
o:value("208.67.222.222:5353", "208.67.222.222:5353(OpenDNS DNS1_5353)")
o:value("208.67.222.222:443", "208.67.222.222:443(OpenDNS DNS1_443)")
o:value("208.67.220.220:53", "208.67.220.220:53(OpenDNS DNS2_53)")
o:value("208.67.220.220:5353", "208.67.220.220:5353(OpenDNS DNS2_5353)")
o:value("208.67.220.220:443", "208.67.220.220:443(OpenDNS DNS2_443)")

---- Mainland DNS Sever 1
o = s:option(Value, "dns_1", translate("Mainland DNS Sever 1"))
o.rmempty = false
o.default = "dnsbyisp"
o:value("dnsbyisp", translate("dnsbyisp"))
o:value("223.5.5.5", "223.5.5.5(" .. translate("Ali") .. "DNS1)")
o:value("223.6.6.6", "223.6.6.6(" .. translate("Ali") .. "DNS2)")
o:value("114.114.114.114", "114.114.114.114(114DNS1)")
o:value("114.114.115.115", "114.114.115.115(114DNS2)")
o:value("119.29.29.29", "119.29.29.29(DNSPOD DNS1)")
o:value("182.254.116.116", "182.254.116.116(DNSPOD DNS2)")
o:value("1.2.4.8", "1.2.4.8(CNNIC DNS1)")
o:value("210.2.4.8", "210.2.4.8(CNNIC DNS2)")
o:value("180.76.76.76", "180.76.76.76(" .. translate("Baidu") .. "DNS)")

---- Mainland DNS Sever 2
o = s:option(Value, "dns_2", translate("Mainland DNS Sever 2"))
o.rmempty = false
o.default = "223.5.5.5"
o:value("dnsbyisp", translate("dnsbyisp"))
o:value("223.5.5.5", "223.5.5.5(" .. translate("Ali") .. "DNS1)")
o:value("223.6.6.6", "223.6.6.6(" .. translate("Ali") .. "DNS2)")
o:value("114.114.114.114", "114.114.114.114(114DNS1)")
o:value("114.114.115.115", "114.114.115.115(114DNS2)")
o:value("119.29.29.29", "119.29.29.29(DNSPOD DNS1)")
o:value("182.254.116.116", "182.254.116.116(DNSPOD DNS2)")
o:value("1.2.4.8", "1.2.4.8(CNNIC DNS1)")
o:value("210.2.4.8", "210.2.4.8(CNNIC DNS2)")
o:value("180.76.76.76", "180.76.76.76(" .. translate("Baidu") .. "DNS)")

---- DNS Export Of Multi WAN
o = s:option(ListValue, "dns_port", translate("DNS Export Of Multi WAN"))
o.rmempty = false
o.default = 0
o:value(0, translate("Auto"))
for _, iface in ipairs(ifaces) do
    if (iface:match("^pppoe*")) then
        local nets = net:get_interface(iface)
        nets = nets and nets:get_networks() or {}
        for k, v in pairs(nets) do nets[k] = nets[k].sid end
        nets = table.concat(nets, ",")
        o:value(iface, ((#nets > 0) and "%s (%s)" % {iface, nets} or iface))
    end
end

---- Designated Export for Client
o = s:option(ListValue, "wan_port", translate("Designated Export for Client"))
o.default = 0
o.rmempty = false
o:value(0, translate("Auto"))
for _, iface in ipairs(ifaces) do
    if (iface:match("^pppoe*")) then
        local nets = net:get_interface(iface)
        nets = nets and nets:get_networks() or {}
        for k, v in pairs(nets) do nets[k] = nets[k].sid end
        nets = table.concat(nets, ",")
        o:value(iface, ((#nets > 0) and "%s (%s)" % {iface, nets} or iface))
    end
end

---- DNS Hijack
o = s:option(Flag, "dns_53", translate("DNS Hijack"))
o.default = 0
o.rmempty = false

-- [[ Forwarding Settings ]]--
s = m:section(TypedSection, "global_forwarding",
              translate("Forwarding Settings"))
s.anonymous = true
s.addremove = false

---- TCP Redir Ports
o = s:option(Value, "tcp_redir_ports", translate("TCP Redir Ports"))
o.default = "80,443"
o:value("1:65535", translate("All"))
o:value("80,443", "80,443")
o:value("80:", "80 " .. translate("or more"))
o:value(":443", "443 " .. translate("or less"))

---- UDP Redir Ports
o = s:option(Value, "udp_redir_ports", translate("UDP Redir Ports"))
o.default = "1:65535"
o:value("1:65535", translate("All"))
o:value("53", "53")

---- Multi SS/SSR Process Option
o = s:option(Value, "process", translate("Multi Process Option"),
             translate("you can start SS/SSR with multiple process"))
o.default = "0"
o.rmempty = false
o:value("0", translate("Auto"))
o:value("1", translate("1 Process"))
o:value("2", "2 " .. translate("Process"))
o:value("3", "3 " .. translate("Process"))
o:value("4", "4 " .. translate("Process"))

-- [[ Proxy Settings ]]--
s = m:section(TypedSection, "global_proxy", translate("Proxy Settings"))
s.anonymous = true
s.addremove = false

---- TCP Redir Port
o = s:option(Value, "tcp_redir_port", translate("TCP Redir Port"))
o.datatype = "port"
o.default = 1031
o.rmempty = true

---- UDP Redir Port
o = s:option(Value, "udp_redir_port", translate("UDP Redir Port"))
o.datatype = "port"
o.default = 1032
o.rmempty = true

---- Socks5 Proxy Port
o = s:option(Value, "socks5_port", translate("Socks5 Proxy Port"))
o.datatype = "port"
o.default = 1033

---- Kcptun Port
o = s:option(Value, "kcptun_port", translate("Kcptun Port"))
o.datatype = "port"
o.default = 11183
o.rmempty = true

---- Proxy IPv6
o = s:option(Flag, "proxy_ipv6", translate("Proxy IPv6"),
             translate("The IPv6 traffic can be proxyed when selected"))
o.default = 0

-- [[ Other Settings ]]--
s = m:section(TypedSection, "global_other", translate("Other Settings"))
s.anonymous = true
s.addremove = false

---- TCP Servers Number Option
o = s:option(ListValue, "tcp_redir_server_num",
             translate("TCP Redir Server Number"), translate(
                 "You can only set up a maximum of 3 servers for the time being"))
o.default = "1"
o.rmempty = false
o:value("1")
o:value("2")
o:value("3")

---- UDP Servers Number Option
o = s:option(ListValue, "udp_redir_server_num",
             translate("UDP Redir Server Number"), translate(
                 "You can only set up a maximum of 3 servers for the time being"))
o.default = "1"
o.rmempty = false
o:value("1")
o:value("2")
o:value("3")

---- Hide Menu
o = s:option(Button, "hide", translate("Hide Menu"), translate(
                 "After the hidden to the display, type in the address bar enter the admin/vpn/passwall/show, such as: http://192.168.1.1/cgi-bin/luci/admin/vpn/passwall/show"))
o.inputstyle = "remove"
function o.write(e, e)
    luci.http.redirect(luci.dispatcher.build_url("admin", "vpn", "passwall",
                                                 "hide"))
end

-- [[ Custom Dnsmasq Settings ]]--
--[[
s = m:section(TypedSection, "global", translate("Custom Dnsmasq"))
s.anonymous = true
local e = "/usr/share/passwall/dnsmasq.d/user.conf"
o = s:option(TextValue, "userconf")
o.description = translate("Setting a parameter error will cause dnsmasq fail to start.")
o.rows = 15
o.wrap = "off"
o.cfgvalue = function(a, a)
return fs.readfile(e)or""
end
o.write = function(o, o, a)
fs.writefile(e, a:gsub("\r\n", "\n"))
end
]] --

return m
