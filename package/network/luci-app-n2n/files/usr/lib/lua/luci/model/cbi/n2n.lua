local cfgcmd = "var Macaddr=document.getElementById(this.parentNode.parentNode.parentNode.id.replace(\"cbi\", \"cbid\").replace(/-/g, \".\"));" ..
"function randomString(len,type){len=len||32;type=type||0;" ..
"var $chars=\"\";" ..
"switch(type){case 1:$chars=\"13579bdf\";break;" ..
"case 2:$chars=\"24680ace\";break;" ..
"default:$chars=\"0123456789abcdef\";break;}" ..
"var maxPos=$chars.length;var pwd=\"\";" ..
"for(i = 0; i < len; i++){pwd+=$chars.charAt(Math.floor(Math.random() * maxPos));}return pwd;};" ..
"Macaddr.value=randomString(1)+randomString(1,2)+\":\"+randomString(2)+\":\"+randomString(2)+\":\"+randomString(2)+\":\"+randomString(2)+\":\"+randomString(2)"

local cfgbtn = "&nbsp;<br><input type=\"button\" id=\"test1\" value=\" " .. translate("Generate Randomly") .. " \" onclick='" .. cfgcmd .. "'/>"

m = Map("n2n", translate("n2n vpn"),translate("A Layer Two Peer-to-Peer VPN."))

s = m:section(TypedSection, "edge", translate("Edgenode"))
s.addremove = true
s.anonymous = false
s:option(Flag, "enabled", translate("Enable"))
s:option(Value, "supernode", translate("Supernode")).rmempty = true
s:option(Value, "port", translate("Port")).rmempty = true
s:option(Value, "community", translate("Community")).rmempty = true
netmask=s:option(Value, "netmask", translate("Netmask"))
netmask.placeholder="255.255.255.0"
netmask.rmempty = true
macaddr=s:option(Value, "macaddr", translate("MAC Address"), cfgbtn)
macaddr.placeholder="01:02:03:04:05:06"
macaddr.rmempty = true
key=s:option(Value, "key", translate("Key"))
key.password = true
key.rmempty = true
extra=s:option(Value, "extra", translate("Extra options"))
extra.rmempty = true

s:option(Value, "ipaddr", translate("IP Address")).rmempty = true
s:option(Flag, "route", translate("Route")).rmempty = true


supernode = m:section(TypedSection, "supernode", translate("Supernode"))
supernode:option(Flag, "enabled", translate("Enable"))
supernode:option(Value, "port", translate("Port")).rmempty = true
supernode.addremove = true
supernode.anonymous = false

return m
