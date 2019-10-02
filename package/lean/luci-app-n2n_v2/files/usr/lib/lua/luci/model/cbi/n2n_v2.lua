m = Map("n2n_v2")
m.title	= translate("N2N v2 VPN")
m.description = translate("n2n is a layer-two peer-to-peer virtual private network (VPN) which allows users to exploit features typical of P2P applications at network instead of application level.")
m:section(SimpleSection).template  = "n2n_v2/edge_status"
s=m:section(TypedSection,"edge",translate("N2N Edge Settings"))
s.anonymous=true
switch=s:option(Flag,"enabled",translate("Enable"))
switch.rmempty=false
tunname=s:option(Value,"tunname",translate("TUN desvice name"))
tunname.optional=false
mode=s:option(ListValue,"mode",translate("Interface mode"))
mode:value("dhcp")
mode:value("static")
ipaddr=s:option(Value,"ipaddr",translate("Interface IP address"))
ipaddr.optional=false
ipaddr.datatype = "ip4addr"
ipaddr:depends("mode","static")
ipaddr.rmempty=true
netmask=s:option(Value,"netmask",translate("Interface netmask"))
netmask.optional=false
netmask.datatype = "ip4addr"
netmask:depends("mode","static")
netmask.rmempty=true
supernode=s:option(Value,"supernode",translate("Supernode Address"),
translate("Support domain name"))
supernode.optional=false
supernode.password=true
supernode.datatype = "host"
supernode.rmempty=false
port=s:option(Value,"port",translate("Supernode Port"))
port.datatype="range(1,65535)"
port.optional=false
port.rmempty=false
community=s:option(Value,"community",translate("N2N Community name"))
community.datatype = "minlength(1)"
community.optional=false
community.rmempty=false
key=s:option(Value,"key",translate("Encryption key"))
key.password=true
key.rmempty=true
route=s:option(Flag,"route",translate("Enable packet forwarding"))
route.rmempty=true
ddns=s:option(Flag,"ddns",translate("Periodically resolve supernode IP"),
translate("Enable with dynamic domain name"))
ddns.rmempty=true
enable_localip = s:option(Flag, "enable_localip", translate("Set local IP"),
translate("Add local IP to bypass between same nat problem"))
localip=s:option(Value,"localip",translate("Local IP"))
localip.rmempty=true
localip.datatype = "ip4addr"
localip:depends("enable_localip", "1")
enable_interval = s:option(Flag, "enable_interval", translate("Set NAT hole-punch Interval"),
translate("Set the NAT hole-punch interval (If not open, default 20 seconds)"))
interval=s:option(Value,"interval",translate("Interval (Seconds)"))
interval.rmempty=true
interval.datatype = "range(1,600)"
interval:depends("enable_interval", "1")
enable_checkip = s:option(Flag, "enable_checkip", translate("Enable IP Check"),
translate("Open the IP detection function, restart N2N when it is not possible to ping the designated IP"))
enable_checkip.rmempty=true
checkip=s:option(Value,"checkip",translate("Designated IP"))
checkip.rmempty=true
checkip.datatype = "ip4addr"
checkip:depends("enable_checkip", "1")
time_checkip = s:option(ListValue, "time_checkip", translate("Check Interval (Minutes)"))
for s=1,60 do
time_checkip:value(s)
end
time_checkip.rmempty=true
time_checkip:depends("enable_checkip", "1")
m:section(SimpleSection).template  = "n2n_v2/supernode_status"
s2=m:section(TypedSection,"supernode",translate("N2N Supernode Settings"))
s2.anonymous=true
switch=s2:option(Flag,"enabled",translate("Enable"))
switch.rmempty=false
port=s2:option(Value,"port",translate("Port"))
port.datatype="range(1,65535)"
port.optional=false

return m
