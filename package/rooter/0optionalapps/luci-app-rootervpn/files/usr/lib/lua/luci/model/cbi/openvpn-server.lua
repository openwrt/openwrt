-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Licensed to the public under the Apache License 2.0.

local fs  = require "nixio.fs"
local sys = require "luci.sys"
local uci = require "luci.model.uci".cursor()

local m = Map("openvpn", translate("OpenVPN Extras"), translate("Extra Settings for an OpenVPN Client or Server"))

m.on_after_save = function(self)
	luci.sys.call("/usr/lib/easyrsa/firewall.sh &")
	luci.sys.call("/usr/lib/easyrsa/dns.sh &")
end

gw = m:section(TypedSection, "settings", translate("Advanced Extras"))
gw.anonymous = true
gw:tab("default", translate("Custom Firewall Settings"))
gw:tab("dns", translate("Custom DNS Settings"))
gw:tab("key", translate("Key and Certificate Generation"))

this_tab = "default"

gw:taboption(this_tab, Flag, "vpn2lan", translate("Forward Client VPN to LAN"), translate("(Client) Allow clients behind the VPN server to connect to computers within your LAN") )
gw:taboption(this_tab, Flag, "vpns2lan", translate("Forward Server VPN to LAN"), translate("(Server) Allow clients behind the VPN server to connect to computers within your LAN") )
gw:taboption(this_tab, Flag, "vpn2wan", translate("Forward Server VPN to WAN"), translate("(Server) Allow clients to connect to the internet (WAN) through the tunnel") )

this_tab = "dns"

gw:taboption(this_tab, Flag, "lanopendns", translate("LAN DNS using OpenDNS"), translate("Fixed DNS on LAN interface using OpenDNS") )
gw:taboption(this_tab, Flag, "langoogle", translate("LAN DNS using Google"), translate("Fixed DNS on LAN interface using Google") )
gw:taboption(this_tab, Flag, "wanopendns", translate("WAN DNS using OpenDNS"), translate("Fixed DNS on WAN interface using OpenDNS") )
gw:taboption(this_tab, Flag, "wangoogle", translate("WAN DNS using Google"), translate("Fixed DNS on WAN interface using Google") )

this_tab = "key"

country = gw:taboption(this_tab, Value, "country", translate("Country Name :"), translate("2 letter country abbreviation")); 
country.optional=false; 
country.rmempty = true;
country.default="CA"
country.datatype = "rangelength(2, 2)"

city = gw:taboption(this_tab, Value, "city", translate("City Name :")); 
city.optional=false; 
city.rmempty = true;
city.default="Abbotsford"
city.datatype = "minlength(2)"

organ = gw:taboption(this_tab, Value, "organ", translate("Organization Name :"), translate("name will appear on certs and keys")); 
organ.optional=false; 
organ.rmempty = true;
organ.default="ROOter"
organ.datatype = "minlength(2)"

comm = gw:taboption(this_tab, Value, "comm", translate("Common Name :"), translate("(Optional) Common Name of Organization")); 
comm.optional=true; 
comm.rmempty = true;

unit = gw:taboption(this_tab, Value, "unit", translate("Section Name :"), translate("(Optional) Name of Section")); 
unit.optional=true; 
unit.rmempty = true;

unstruc = gw:taboption(this_tab, Value, "unstruc", translate("Optional Organization Name :"), translate("(Optional) Another Name for Organization")); 
unstruc.optional=true; 
unstruc.rmempty = true;

email = gw:taboption(this_tab, Value, "email", translate("Email Address :"), translate("(Optional) Email Address")); 
unit.optional=true; 
unit.rmempty = true;

days = gw:taboption(this_tab, Value, "days", translate("Days to certify for :"), translate("number of days certs and keys are valid")); 
days.optional=false; 
days.rmempty = true;
days.default="3650"
days.datatype = "min(1)"

clnt = gw:taboption(this_tab, Value, "nclient", translate("Number of Clients :"), translate("number of clients to generate certs and keys for")); 
clnt.optional=false; 
clnt.rmempty = true;
clnt.default="1"
clnt.datatype = "min(1)"

sx = gw:taboption(this_tab, Value, "_dmy1", translate(" "))
sx.template = "easyrsa/easyrsa"


return m
