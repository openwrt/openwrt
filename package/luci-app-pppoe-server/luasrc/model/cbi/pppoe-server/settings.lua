local sys = require "luci.sys"
local ifaces = sys.net:devices()
local net = require "luci.model.network".init()

m = Map("pppoe-server", translate("PPPoE Server"))
m.description = translate("The PPPoE server is a broadband access authentication server that prevents ARP spoofing.")
m.template = "pppoe-server/index"

s = m:section(TypedSection, "service")
s.anonymous = true

o = s:option(DummyValue, "_status", translate("Current Condition"))
o.template = "pppoe-server/status"
o.value = translate("Collecting data...")

o = s:option(Flag, "enabled", translate("Enable"))
o.rmempty = false

o = s:option(ListValue, "client_interface", translate("Client Interface"), translate("Listen in this interface"))
for _, iface in ipairs(ifaces) do
    if (iface:match("^br") or iface:match("^eth*") or iface:match("^pppoe*") or iface:match("wlan*")) then
        local nets = net:get_interface(iface)
        nets = nets and nets:get_networks() or {}
        for k, v in pairs(nets) do nets[k] = nets[k].sid end
        nets = table.concat(nets, ",")
        o:value(iface, ((#nets > 0) and "%s (%s)" % {iface, nets} or iface))
    end
end

o = s:option(Value, "localip", translate("Server IP"), translate("PPPoE Server IP address.default: 10.0.1.1"))
o.datatype = "ipaddr"
o.placeholder = "10.0.1.1"
o.default = o.placeholder

o = s:option(Value, "remoteip", translate("Client IP"), translate("PPPoE Client IP address.default: 10.0.1.100-254"))
o.placeholder = "10.0.1.100-254"
o.default = o.placeholder

o = s:option(Value, "count", translate("Client upper limit"), translate("Specify how many clients can connect to the server simultaneously."))
o.placeholder = "50"
o.default = o.placeholder

o = s:option(Value, "dns1", translate("Preferred DNS server"))
o.placeholder = "10.0.1.1"
o.datatype = "ipaddr"

o = s:option(Value, "dns2", translate("Alternate DNS server"))
o.datatype = "ipaddr"
o.placeholder = "119.29.29.29"

return m
