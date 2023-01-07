require("luci.ip")
require("luci.model.uci")

--luci.sys.call("/usr/lib/wireguard/keygen.sh " .. arg[1])

local m = Map("wireguard", translate("Wireguard Client"), translate("Set up a Wireguard Client"))

e = m:section(NamedSection, "settings", "")

m.on_init = function(self)
	--luci.sys.call("/usr/lib/wireguard/keygen.sh " .. arg[1])
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


local s = m:section( NamedSection, arg[1], "wireguard", translate("Client") )

ip = s:option(Value, "addresses", translate("IP Addresses :"), translate("Comma separated list of IP Addresses that server will accept from this client")); 
ip.rmempty = true;
ip.optional=false;
ip.default="10.14.0.2/24";

port = s:option(Value, "port", translate("Listen Port :"), translate("Client Listen Port")); 
port.rmempty = true;
port.optional=false;
port.default="51820";

ul = s:option(ListValue, "udptunnel", translate("Enable UDP over TCP :"));
ul:value("0", translate("No"))
ul:value("1", translate("Yes"))
ul.default=0

dns = s:option(Value, "dns", translate("DNS Servers :"), translate("Comma separated list of DNS Servers.")); 
dns.rmempty = true;
dns.optional=false;

mtu = s:option(Value, "mtu", translate("MTU :"), translate("Maximum MTU")); 
mtu.rmempty = true;
mtu.optional=false;
mtu.datatype = 'range(1280,1420)';
mtu.default="1280";

pka = s:option(Value, "persistent_keepalive", translate("Persistent Keep Alive :"), translate("Seconds between keep alive messages")); 
pka.rmempty = true;
pka.optional=false;
pka.datatype = 'range(1,100)';
pka.default="25";

pkey = s:option(Value, "privatekey", translate("Private Key :"), translate("Private Key supplied by the Server")); 
pkey.rmempty = true;
pkey.optional=false;

il = s:option(ListValue, "wginter", translate("Interface to Use :"));
il:value("0", translate("WG0"))
il:value("1", translate("WG1"))
il.default="0"

bl = s:option(ListValue, "auto", translate("Start on Boot :"));
bl:value("0", translate("No"))
bl:value("1", translate("Yes"))
bl.default="0"

xbl = s:option(ListValue, "forward", translate("All Traffic Through Tunnel :"));
xbl:value("0", translate("No"))
xbl:value("1", translate("Yes"))
xbl.default="1"

s = m:section( NamedSection, arg[1], "wireguard", translate("Server") )

name = s:option( Value, "name", translate("Server Name :"), translate("Optional Server name"))

pukey = s:option(Value, "publickey", translate("Public Key :"), translate("Public Key of the Server")); 
pukey.rmempty = true;
pukey.optional=false;

prkey = s:option(Value, "presharedkey", translate("Presharedkey :"), translate("PreShared Key from the Server")); 
prkey.rmempty = true;
prkey.optional=false;

host = s:option(Value, "endpoint_host", translate("Server Address :"), translate("URL or IP Address of Server")); 
host.rmempty = true;
host.optional=false;
host.default="";

sport = s:option(Value, "sport", translate("Listen Port :"), translate("Server Listen Port")); 
sport.rmempty = true;
sport.optional=false;
sport.default="51820";

sip = s:option(Value, "ips", translate("Allowed IP Addresses :"), translate("Comma separated list of IP Addresses that server will accept")); 
sip.rmempty = true;
sip.optional=false;
sip.default="10.14.0.0/24";

return m