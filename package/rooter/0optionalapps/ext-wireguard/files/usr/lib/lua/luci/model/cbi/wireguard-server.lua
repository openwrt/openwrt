require("luci.ip")
require("luci.model.uci")

--luci.sys.call("/usr/lib/wireguard/keygen.sh " .. arg[1])

local m = Map("wireguard", translate("Wireguard Server"), translate("Set up a Wireguard Server"))

e = m:section(NamedSection, "settings", "")

m.on_init = function(self)
	luci.sys.call("/usr/lib/wireguard/keygen.sh " .. arg[1])
end

m.on_after_save = function(self)
	luci.sys.call("/usr/lib/wireguard/keygen.sh " .. arg[1] ..  "&")
end

btn = e:option(Button, "_btn", translate(" "))
btn.inputtitle = translate("Back to Main Page")
btn.inputstyle = "apply"
btn.redirect = luci.dispatcher.build_url(
	"admin", "vpn", "wireguard"
)
function btn.write(self, section, value)
	luci.http.redirect( self.redirect )
end


local s = m:section( NamedSection, arg[1], "wireguard", translate("Server") )

ip = s:option(Value, "addresses", translate("Internal IP Address :")); 
ip.rmempty = true;
ip.optional=false;
ip.default="10.14.0.1/32";
ip.datatype = "ipaddr"

host = s:option(Value, "endpoint_host", translate("Server Address :"), translate("URL or IP Address of Server")); 
host.rmempty = true;
host.optional=false;
host.default="example.wireguard.org";

port = s:option(Value, "port", translate("Port :"), translate("Server Listen Port. Default is 51280")); 
port.rmempty = true;
port.optional=false;
port.default="51280";

ul = s:option(ListValue, "udptunnel", "Enable UDP over TCP :");
ul:value("0", translate("No"))
ul:value("1", translate("Yes"))
ul.default=0

uport = s:option(Value, "udpport", translate("UDP over TCP Port :"), translate("Server Local TCP Port. Default is 54321")); 
uport.rmempty = true;
uport.optional=false;
uport.default="54321";
uport:depends("udptunnel", "1")

pkey = s:option(DummyValue, "privatekey", translate("Private Key :")); 
pkey.optional=false;

pukey = s:option(DummyValue, "publickey", translate("Public Key :"), translate("Server Public key sent to Clients")); 
pukey.optional=false;

pl = s:option(ListValue, "usepre", "Use PreSharedKey :");
pl:value("0", translate("No"))
pl:value("1", translate("Yes"))
pl.default=0

prkey = s:option(DummyValue, "presharedkey", translate("PreShared Key :"), translate("PreShared Key sent to Client")); 
prkey.optional=false;
prkey:depends("usepre", "1")

bl = s:option(ListValue, "auto", translate("Start on Boot :"));
bl:value("0", translate("No"))
bl:value("1", translate("Yes"))
bl.default="0"

xbl = s:option(ListValue, "forward", translate("All Traffic Through Tunnel :"));
xbl:value("0", translate("No"))
xbl:value("1", translate("Yes"))
xbl.default="1"


b3 = s:option(DummyValue, "blank", " ");

sx = s:option(Value, "_dmy1", translate(" "))
sx.template = "wireguard/conf"

ss = m:section(TypedSection, "custom" .. arg[1], translate("Clients"), translate("Clients of this server"))
ss.anonymous = true
ss.addremove = true

name = ss:option(Value, "name", translate("Client Name"))
name.optional=false;

cport = ss:option(Value, "endpoint_port", translate("Listen Port :"), translate("Port sent to Client. Default is 51280")); 
cport.rmempty = true;
cport.optional=false;
cport.default="";

aip = ss:option(Value, "address", translate("Assigned IP Address :"), translate("IP Address assigned to Client")); 
aip.rmempty = true;
aip.optional=false;
aip.default="10.14.0.2/32";

dns = ss:option(Value, "dns", translate("DNS Servers :"), translate("Comma separated list of DNS Servers sent to Client")); 
dns.rmempty = true;
dns.optional=false;
dns.default="";

mtu = ss:option(Value, "mtu", translate("MTU :"), translate("Maximum MTU")); 
mtu.rmempty = true;
mtu.optional=false;
mtu.datatype = 'range(1280,1420)';
mtu.default="1280";

aip = ss:option(Value, "allowed_ips", translate("Allowed IP Address :"), translate("Comma separated list of IP Addresses allowed from Client")); 
aip.rmempty = true;
aip.optional=false;
aip.default="0.0.0.0/0,::/0";

pukey = ss:option(DummyValue, "publickey", translate("Public Key :"), translate("Client Public Key")); 
pukey.optional=false;

pikey = ss:option(DummyValue, "privatekey", translate("Private Key :"), translate("Private Key sent to Client")); 
pikey.optional=false;

b3 = ss:option(DummyValue, "blank", " ");

return m