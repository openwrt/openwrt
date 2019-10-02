local e = require "nixio.fs"
local e = require "luci.sys"
local net = require "luci.model.network".init()
local uci = require "luci.model.uci".cursor()
local ifaces = e.net:devices()
local appname = "passwall"
local servers_name = {}
local servers_port = {}

uci:foreach(appname, "servers", function(e)
    if e.server and e.server_port and e.server ~= "127.0.0.1" then
        servers_name[e[".name"]] = "%s" % {e.server}
        servers_port[e[".name"]] = "%s" % {e.server_port}
    end
end)

m = Map("passwall")

-- [[ Haproxy Settings ]]--
s = m:section(TypedSection, "global_haproxy", translate("Admin Status"),
              translate(
                  "In the browser input routing IP plus port access, such as:192.168.1.1:1188") ..
                  "<br><input type='button' class='cbi-button cbi-input-reload' value='" ..
                  translate("Click here to setting your Load Balancing") ..
                  "' onclick=javascript:window.open('http://koolshare.cn/thread-65561-1-1.html','target'); />")
s.anonymous = true

---- Admin Enable
o = s:option(Flag, "admin_enable", translate("Enable Admin Status"))
o.rmempty = false
o.default = false

---- Admin Port
o = s:option(Value, "admin_port", translate("Admin Status port setting"))
o.default = "1188"
o:depends("admin_enable", 1)

---- Admin User
o = s:option(Value, "admin_user", translate("Admin Status User"))
o.default = "admin"
o:depends("admin_enable", 1)

---- Admin Password
o = s:option(Value, "admin_password", translate("Admin Status Password"))
o.password = true
o.default = "admin"
o:depends("admin_enable", 1)

---- Balancing Enable
o = s:option(Flag, "balancing_enable",
             translate("Enable or Disable Load Balancing"))
o.rmempty = false
o.default = false

---- Haproxy Port
o = s:option(Value, "haproxy_port", translate("Haproxy port setting"))
o.default = "1181"
o:depends("balancing_enable", 1)

-- [[ Balancing Settings ]]--
s = m:section(TypedSection, "balancing",
              translate("Load Balancing Server Setting"), translate(
                  "Add a load balancing server, note reading above requirements."))
s.template = "cbi/tblsection"
s.sortable = true
s.anonymous = true
s.addremove = true

---- Server Address
o = s:option(Value, "lbss", translate("Server Address"))
for m, s in pairs(servers_name) do o:value(s) end
o.rmempty = false

---- Server Port
o = s:option(Value, "lbort", translate("Server Port"))
for m, s in pairs(servers_port) do o:value(s) end
o.rmempty = false

---- Server weight
o = s:option(Value, "lbweight", translate("Server weight"))
o.default = "5"
o.rmempty = false

---- Export
o = s:option(ListValue, "export", translate("Export Of Multi WAN"))
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
o.default = 0
o.rmempty = false

---- Backup
o = s:option(ListValue, "backup", translate("Server Mode"))
o:value(0, translate("Primary Server"))
o:value(1, translate("Standby Server"))
o.rmempty = false

return m
