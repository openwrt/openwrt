local utl = require "luci.util"
local uci = require "luci.model.uci".cursor()
local sys   = require "luci.sys"
local fs = require "nixio.fs" 

local maxmodem = luci.model.uci.cursor():get("modem", "general", "max")  
local profsave = luci.model.uci.cursor():get("custom", "profile", "save")  
if profsave == nil then
	profsave ="0"
end
local multilock = luci.model.uci.cursor():get("custom", "multiuser", "multi") or "0"
local rootlock = luci.model.uci.cursor():get("custom", "multiuser", "root") or "0"

m = Map("profile", translate("Modem Connection Profiles"),
	translate("Create Profiles used to provide information at connection time"))

m.on_after_commit = function(self)
	if profsave == "1" then
		luci.sys.call("/usr/lib/profile/restart.sh &")
	end
end

if profsave == "1" then
	m:section(SimpleSection).template = "rooter/profile"
	ds = m:section(TypedSection, "simpin", translate("Default SIM Pin"), translate("Used if no SIM Pin value in Profile"))
	ds.anonymous = true
	
	ms = ds:option(Value, "pin", translate("PIN :")); 
	ms.rmempty = true;
	ms.default = ""
end


-- 
-- Default profile
--

di = m:section(TypedSection, "default", translate("Default Profile"), translate("Used if no matching Custom Profile is found"))
di.anonymous = true
di:tab("default", translate("General"))
di:tab("advance", translate("Advanced"))
di:tab("connect", translate("Connection Monitoring"))
if (multilock == "0") or (multilock == "1" and rootlock == "1") then
	di:tab("bwidth", translate("Bandwidth Reporting"))
end

this_tab = "default"

ma = di:taboption(this_tab, Value, "apn", "APN :"); 
ma.rmempty = true;
ma.default = "broadband"

tt = di:taboption(this_tab, ListValue, "ttl", translate("Custom TTL Value :"))
tt:value("0", translate("Use Current Value"))
tt:value("1", translate("No TTL Value"))
tt:value("63", "TTL 63")
tt:value("64", "TTL 64")
tt:value("65", "TTL 65")
tt:value("66", "TTL 66")
tt:value("67", "TTL 67")
tt:value("117", "TTL 117")
tt:value("TTL-INC 1", "TTL-INC 1")
tt.default = "0"

tnl = di:taboption(this_tab, ListValue, "ttloption", translate("TTL Settings"));
tnl:value("0", translate("POSTROUTING and PREROUTING (Default)"))
tnl:value("1", translate("POSTROUTING only"))
tnl:value("2", translate("POSTROUTING with ICMP passthrough (May use minimal hotspot data)"))
tnl.default=0

ynl = di:taboption(this_tab, ListValue, "hostless", translate("Adjust TTL for Hostless Modem"));
ynl:value("0", "No")
ynl:value("1", translate("Yes"))
ynl.default=0

pt = di:taboption(this_tab, ListValue, "pdptype", translate("Protocol Type :"))
pt:value("IP", "IPv4")
pt:value("IPV6", "IPv6")
pt:value("IPV4V6", "IPv4+IPv6")
pt:value("0", "Default")
pt.default = "0"

cmcc = di:taboption(this_tab, Value, "context", translate("PDP Context for APN :"));
cmcc.optional=false; 
cmcc.rmempty = true;
cmcc.datatype = "and(uinteger,min(1),max(10))"
cmcc.default = "1"

mu = di:taboption(this_tab, Value, "user", translate("Connection User Name :")); 
mu.optional=false; 
mu.rmempty = true;

mp = di:taboption(this_tab, Value, "passw", translate("Connection Password :")); 
mp.optional=false; 
mp.rmempty = true;
mp.password = true

mpi = di:taboption(this_tab, Value, "pincode", translate("PIN :")); 
mpi.optional=false; 
mpi.rmempty = true;

mau = di:taboption(this_tab, ListValue, "auth", translate("Authentication Protocol :"))
mau:value("0", "None")
mau:value("1", "PAP")
mau:value("2", "CHAP")
mau.default = "0"

mtz = di:taboption(this_tab, ListValue, "tzone", translate("Auto Set Timezone"), translate("Set the Timezone automatically when modem connects"));
mtz:value("0", "No")
mtz:value("1", translate("Yes"))
mtz.default=1

if profsave == "1" then
	ml = di:taboption(this_tab, ListValue, "lock", translate("Allow Roaming :"));
	ml:value("0", translate("Yes"))
	ml:value("1", translate("No - Hard Lock"))
	ml:value("2", translate("Yes - Soft Lock"))
	ml.default=0
else
	ml = di:taboption(this_tab, ListValue, "lock", translate("Lock to Provider :"));
	ml:value("0", translate("No"))
	ml:value("1", translate("Hard"))
	ml:value("2", translate("Soft"))
	ml.default=0
end
mcc = di:taboption(this_tab, Value, "mcc", translate("Provider Country Code :"));
mcc.optional=false; 
mcc.rmempty = true;
mcc.datatype = "and(uinteger,min(1),max(999))"
mcc:depends("lock", "1")
mcc:depends("lock", "2")

mnc = di:taboption(this_tab, Value, "mnc", translate("Provider Network Code :"));
mnc.optional=false; 
mnc.rmempty = true;
mnc.datatype = "and(uinteger,min(1),max(999))"
mnc:depends("lock", "1")
mnc:depends("lock", "2")

this_taba = "advance"

mf = di:taboption(this_taba, ListValue, "ppp", translate("Force Modem to PPP Protocol :"));
mf:value("0", translate("No"))
mf:value("1", translate("Yes"))
mf.default=0

md = di:taboption(this_taba, Value, "delay", translate("Connection Delay in Seconds :")); 
md.optional=false; 
md.rmempty = false;
md.default = 5
md.datatype = "and(uinteger,min(5))"

nl = di:taboption(this_taba, ListValue, "nodhcp", translate("No DHCP for QMI Modems :"));
nl:value("0", translate("No"))
nl:value("1", translate("Yes"))
nl.default=0

mdns1 = di:taboption(this_taba, Value, "dns1", translate("Custom DNS Server1 :")); 
mdns1.rmempty = true;
mdns1.optional=false;
mdns1.datatype = "ipaddr"

mdns2 = di:taboption(this_taba, Value, "dns2", translate("Custom DNS Server2 :")); 
mdns2.rmempty = true;
mdns2.optional=false;
mdns2.datatype = "ipaddr"

mdns3 = di:taboption(this_taba, Value, "dns3", translate("Custom DNS Server3 :")); 
mdns3.rmempty = true;
mdns3.optional=false;
mdns3.datatype = "ipaddr"

mdns4 = di:taboption(this_taba, Value, "dns4", translate("Custom DNS Server4 :")); 
mdns4.rmempty = true;
mdns4.optional=false;
mdns4.datatype = "ipaddr"


mlog = di:taboption(this_taba, ListValue, "log", translate("Enable Connection Logging :"));
mlog:value("0", translate("No"))
mlog:value("1", translate("Yes"))
mlog.default=0

if nixio.fs.access("/etc/config/mwan3") then
	mlb = di:taboption(this_taba, ListValue, "lb", translate("Enable Load Balancing at Connection :"));
	mlb:value("0", translate("No"))
	mlb:value("1", translate("Yes"))
	mlb.default=1
end

mtu = di:taboption(this_taba, Value, "mtu", translate("Custom MTU :"),
		translate("Acceptable values: 1420-1500. Size for Custom MTU. This may have to be adjusted for certain ISPs"));
mtu.optional=true
mtu.rmempty = true
mtu.default = "1500"
mtu.datatype = "range(1420, 1500)"

mat = di:taboption(this_taba, ListValue, "at", translate("Enable Custom AT Startup Command at Connection :"));
mat:value("0", translate("No"))
mat:value("1", translate("Yes"))
mat.default=0

matc = di:taboption(this_taba, Value, "atc", translate("Custom AT Startup Command :"));
matc.optional=false;
matc.rmempty = true;

--
-- Default Connection Monitoring
--

this_tab = "connect"

alive = di:taboption(this_tab, ListValue, "alive", translate("Connection Monitoring Status :")); 
alive.rmempty = true;
alive:value("0", translate("Disabled"))
alive:value("2", translate("Enabled with Router Reboot"))
alive:value("3", translate("Enabled with Modem Restart"))
alive.default=0

reliability = di:taboption(this_tab, Value, "reliability", translate("Tracking reliability"),
		translate("Acceptable values: 1-100. This many Tracking IP addresses must respond for the link to be deemed up"))
reliability.datatype = "range(1, 100)"
reliability.default = "1"
reliability:depends("alive", "1")
reliability:depends("alive", "2")
reliability:depends("alive", "3")
reliability:depends("alive", "4")

count = di:taboption(this_tab, ListValue, "count", translate("Ping count"))
count.default = "1"
count:value("1")
count:value("2")
count:value("3")
count:value("4")
count:value("5")
count:depends("alive", "1")
count:depends("alive", "2")
count:depends("alive", "3")
count:depends("alive", "4")

interval = di:taboption(this_tab, ListValue, "pingtime", translate("Ping interval"),
		translate("Amount of time between tracking tests"))
interval.default = "10"
interval:value("5", translate("5 seconds"))
interval:value("10", translate("10 seconds"))
interval:value("20", translate("20 seconds"))
interval:value("30", translate("30 seconds"))
interval:value("60", translate("1 minute"))
interval:value("300", translate("5 minutes"))
interval:value("600", translate("10 minutes"))
interval:value("900", translate("15 minutes"))
interval:value("1800", translate("30 minutes"))
interval:value("3600", translate("1 hour"))
interval:depends("alive", "1")
interval:depends("alive", "2")
interval:depends("alive", "3")
interval:depends("alive", "4")

timeout = di:taboption(this_tab, ListValue, "pingwait", translate("Ping timeout"))
timeout.default = "2"
timeout:value("1", translate("1 second"))
timeout:value("2", translate("2 seconds"))
timeout:value("3", translate("3 seconds"))
timeout:value("4", translate("4 seconds"))
timeout:value("5", translate("5 seconds"))
timeout:value("6", translate("6 seconds"))
timeout:value("7", translate("7 seconds"))
timeout:value("8", translate("8 seconds"))
timeout:value("9", translate("9 seconds"))
timeout:value("10", translate("10 seconds"))
timeout:depends("alive", "1")
timeout:depends("alive", "2")
timeout:depends("alive", "3")
timeout:depends("alive", "4")

packetsize = di:taboption(this_tab, Value, "packetsize", translate("Ping packet size in bytes"),
		translate("Acceptable values: 4-56. Number of data bytes to send in ping packets. This may have to be adjusted for certain ISPs"))
	packetsize.datatype = "range(4, 56)"
	packetsize.default = "56"
	packetsize:depends("alive", "1")
	packetsize:depends("alive", "2")
	packetsize:depends("alive", "3")
	packetsize:depends("alive", "4")

down = di:taboption(this_tab, ListValue, "down", translate("Interface down"),
		translate("Interface will be deemed down after this many failed ping tests"))
down.default = "3"
down:value("1")
down:value("2")
down:value("3")
down:value("4")
down:value("5")
down:value("6")
down:value("7")
down:value("8")
down:value("9")
down:value("10")
down:depends("alive", "1")
down:depends("alive", "2")
down:depends("alive", "3")
down:depends("alive", "4")

up = di:taboption(this_tab, ListValue, "up", translate("Interface up"),
		translate("Downed interface will be deemed up after this many successful ping tests"))
up.default = "3"
up:value("1")
up:value("2")
up:value("3")
up:value("4")
up:value("5")
up:value("6")
up:value("7")
up:value("8")
up:value("9")
up:value("10")
up:depends("alive", "1")
up:depends("alive", "2")
up:depends("alive", "3")
up:depends("alive", "4")

cb2 = di:taboption(this_tab, DynamicList, "trackip", translate("Tracking IP"),
		translate("This IP address will be pinged to dermine if the link is up or down."))
cb2.datatype = "ipaddr"
cb2:depends("alive", "1")
cb2:depends("alive", "2")
cb2:depends("alive", "3")
cb2:depends("alive", "4")
cb2.optional=false;
cb2.default="8.8.8.8"

if (multilock == "0") or (multilock == "1" and rootlock == "1") then
	this_tab = "bwidth"
	bwday = di:taboption(this_tab, ListValue, "bwday", translate("Day to Send Report"),
		translate("Day of month to send report for this Profile."))
	bwday.default = "0"
	bwday:value("0", translate("Disabled"))
	bwday:value("1", translate("1st"))
	bwday:value("2", translate("2nd"))
	bwday:value("3", translate("3rd"))
	bwday:value("4", translate("4th"))
	bwday:value("5", translate("5th"))
	bwday:value("6", translate("6th"))
	bwday:value("7", translate("7th"))
	bwday:value("8", translate("8th"))
	bwday:value("9", translate("9th"))
	bwday:value("10", translate("10th"))
	bwday:value("11", translate("11th"))
	bwday:value("12", translate("12th"))
	bwday:value("13", translate("13th"))
	bwday:value("14", translate("14th"))
	bwday:value("15", translate("15th"))
	bwday:value("16", translate("16th"))
	bwday:value("17", translate("17th"))
	bwday:value("18", translate("18th"))
	bwday:value("19", translate("19th"))
	bwday:value("20", translate("20th"))
	bwday:value("21", translate("21th"))
	bwday:value("22", translate("22th"))
	bwday:value("23", translate("23th"))
	bwday:value("24", translate("24th"))
	bwday:value("25", translate("25th"))
	bwday:value("26", translate("26th"))
	bwday:value("27", translate("27th"))
	bwday:value("28", translate("28th"))

	phone = di:taboption(this_tab, Value, "phone", translate("Phone Number"), translate("Phone Number to send Text Message"))
	phone.default = "12223334444"
	
	bwdelay = di:taboption(this_tab, ListValue, "bwdelay", translate("Delay before Sending"),
		translate("Hours after Midnight before sending text"))
	bwdelay:value("0", translate("No Delay"))
	bwdelay:value("1", translate("1 hour"))
	bwdelay:value("2", translate("2 hour"))
	bwdelay:value("3", translate("3 hour"))
	bwdelay:value("4", translate("4 hour"))
	bwdelay:value("5", translate("5 hour"))
	bwdelay:value("6", translate("6 hour"))
	bwdelay:value("7", translate("7 hour"))
	bwdelay:value("8", translate("8 hour"))
	bwdelay:value("9", translate("9 hour"))
	bwdelay:value("10", translate("10 hour"))
	bwdelay:value("11", translate("11 hour"))
	bwdelay:value("12", translate("12 hour"))
end

if fs.stat("/usr/lib/autoapn/apn.data") then
	dda = m:section(TypedSection, "disable", translate("Use Automatic APN"), translate("Enable the use of the Automatic APN selection. This disables Custom Profiles."))
	dda.anonymous = true
	aenabled = dda:option(Flag, "autoapn", translate("Enabled"))
	aenabled.default="0"
	aenabled.optional=false;
end

dd = m:section(TypedSection, "disable", translate("Disable Custom Profiles"), translate("Disable the use of Custom profiles. All modems will use the Default Profile"))
dd.anonymous = true
enabled = dd:option(Flag, "enabled", translate("Disabled"))
enabled.default="0"
enabled.optional=false;

--
-- Custom profile
--
if (multilock == "0") or (multilock == "1" and rootlock == "1") then
s = m:section(TypedSection, "custom", translate("Custom Profiles"), translate("Matches specific modem and SIM combination to a Profile"))
s.anonymous = true
s.addremove = true
s:tab("custom", translate("General"))
s:tab("cadvanced", translate("Advanced"))
s:tab("cconnect", translate("Connection Monitoring"))
if (multilock == "0") or (multilock == "1" and rootlock == "1") then
	s:tab("cbwidth", translate("Bandwidth Reporting"))
end

this_ctab = "custom"

name = s:taboption(this_ctab, Value, "name", translate("Profile Name"))

enabled = s:taboption(this_ctab, Flag, "enabled", translate("Enabled"))
enabled.default="1"
enabled.optional=false;

select = s:taboption(this_ctab, ListValue, "select", translate("Select Modem by :"));
select:value("0", translate("Modem ID"))
select:value("1", translate("Modem IMEI"))
select:value("2", translate("Model Name"))
select:value("3", translate("SIM IMSI"))
select:value("4", translate("SIM ICCID"))
select.default=0

idV = s:taboption(this_ctab, Value, "vid", translate("Switched Vendor ID :")); 
idV.optional=false;
idV:depends("select", "0")
idV.default="xxxx"

idP = s:taboption(this_ctab, Value, "pid", translate("Switched Product ID :")); 
idP.optional=false;
idP:depends("select", "0") 
idP.default="xxxx"

imei = s:taboption(this_ctab, Value, "imei", translate("Modem IMEI Number :")); 
imei.optional=false;
imei:depends("select", "1")
imei.datatype = "uinteger"
imei.default="1234567"

model = s:taboption(this_ctab, Value, "model", translate("Modem Model Name contains :")); 
model.optional=false;
model:depends("select", "2")
model.default="xxxx"

imsi = s:taboption(this_ctab, Value, "imsi", translate("SIM IMSI Number :")); 
imsi.optional=false;
imsi:depends("select", "3")
imsi.datatype = "uinteger"
imsi.default="1234567"

iccid = s:taboption(this_ctab, Value, "iccid", translate("SIM ICCID Number :")); 
iccid.optional=false;
iccid:depends("select", "4")
iccid.datatype = "uinteger"
iccid.default="1234567"

select1 = s:taboption(this_ctab, ListValue, "select1", translate("Optional Select by :"));
select1:value("0", "Modem ID")
select1:value("1", "Modem IMEI")
select1:value("2", "Model Name")
select1:value("3", "SIM IMSI")
select1:value("4", "SIM ICCID")
select1:value("10", "None")
select1.default=10

idV1 = s:taboption(this_ctab, Value, "vid1", translate("Switched Vendor ID :")); 
idV1.optional=false;
idV1:depends("select1", "0")
idV1.default="xxxx"

idP1 = s:taboption(this_ctab, Value, "pid1", translate("Switched Product ID :")); 
idP1.optional=false;
idP1:depends("select1", "0") 
idP1.default="xxxx"

imei1 = s:taboption(this_ctab, Value, "imei1", translate("Modem IMEI Number :")); 
imei1.optional=false;
imei1:depends("select1", "1")
imei1.datatype = "uinteger"
imei1.default="1234567"

model1 = s:taboption(this_ctab, Value, "model1", translate("Modem Model Name contains :")); 
model1.optional=false;
model1:depends("select1", "2")
model1.default="xxxx"

imsi1 = s:taboption(this_ctab, Value, "imsi1", translate("SIM IMSI Number :")); 
imsi1.optional=false;
imsi1:depends("select1", "3")
imsi1.datatype = "uinteger"
imsi1.default="1234567"

iccid1 = s:taboption(this_ctab, Value, "iccid1", translate("SIM ICCID Number :")); 
iccid1.optional=false;
iccid1:depends("select1", "4")
iccid1.datatype = "uinteger"
iccid1.default="1234567"

cma = s:taboption(this_ctab, Value, "apn", "APN :"); 
cma.rmempty = true;

tt = s:taboption(this_ctab, ListValue, "ttl", translate("Custom TTL Value :"))
tt:value("0", translate("Use Current Value"))
tt:value("1", translate("No TTL Value"))
tt:value("63", "TTL 63")
tt:value("64", "TTL 64")
tt:value("65", "TTL 65")
tt:value("66", "TTL 66")
tt:value("67", "TTL 67")
tt:value("117", "TTL 117")
tt:value("TTL-INC 1", "TTL-INC 1")
tt.default = "0"

ttnl = s:taboption(this_ctab, ListValue, "ttloption", translate("TTL Settings"));
ttnl:value("0", translate("POSTROUTING and PREROUTING (Default)"))
ttnl:value("1", translate("POSTROUTING only"))
ttnl:value("2", translate("POSTROUTING with ICMP passthrough (May use minimal hotspot data)"))
ttnl.default=0

nl = s:taboption(this_ctab, ListValue, "hostless", translate("Adjust TTL for Hostless Modem"));
nl:value("0", translate("No"))
nl:value("1", translate("Yes"))
nl.default=0

pt = s:taboption(this_ctab, ListValue, "pdptype", translate("Protocol Type :"))
pt:value("IP", "IPv4")
pt:value("IPv6", "IPv6")
pt:value("IPV4V6", "IPv4+IPv6")
pt:value("0", "Default")
pt.default = "0"

cmcc = s:taboption(this_ctab, Value, "context", translate("PDP Context for APN :"));
cmcc.optional=false; 
cmcc.rmempty = true;
cmcc.datatype = "and(uinteger,min(1),max(10))"
cmcc.default = "1"

cmu = s:taboption(this_ctab, Value, "user", translate("Connection User Name :")); 
cmu.optional=false; 
cmu.rmempty = true;

cmp = s:taboption(this_ctab, Value, "passw", translate("Connection Password :")); 
cmp.optional=false; 
cmp.rmempty = true;
cmp.password = true

cmpi = s:taboption(this_ctab, Value, "pincode", "PIN :"); 
cmpi.optional=false; 
cmpi.rmempty = true;

cmau = s:taboption(this_ctab, ListValue, "auth", translate("Authentication Protocol :"))
cmau:value("0", "None")
cmau:value("1", "PAP")
cmau:value("2", "CHAP")
cmau.default = "0"

cmtz = s:taboption(this_ctab, ListValue, "tzone", translate("Set Timezone"), translate("Set the Timezone automatically when modem connects"));
cmtz:value("0", translate("No"))
cmtz:value("1", translate("Yes"))
cmtz.default=1

if profsave == "1" then
	cml = s:taboption(this_ctab, ListValue, "lock", translate("Allow Roaming :"));
	cml:value("0", translate("Yes"))
	cml:value("1", translate("No - Hard Lock"))
	cml:value("2", translate("No - Soft Lock"))
	cml.default=0
else
	cml = s:taboption(this_ctab, ListValue, "lock", translate("Lock to Provider :"));
	cml:value("0", translate("No"))
	cml:value("1", translate("Hard"))
	cml:value("2", translate("Soft"))
	cml.default=0
end

cmcc = s:taboption(this_ctab, Value, "mcc", translate("Provider Country Code :"));
cmcc.optional=false; 
cmcc.rmempty = true;
cmcc.datatype = "and(uinteger,min(1),max(999))"
cmcc:depends("lock", "1")
cmcc:depends("lock", "2")

cmnc = s:taboption(this_ctab, Value, "mnc", translate("Provider Network Code :"));
cmnc.optional=false; 
cmnc.rmempty = true;
cmnc.datatype = "and(uinteger,min(1),max(999))"
cmnc:depends("lock", "1")
cmnc:depends("lock", "2")

this_ctaba = "cadvanced"

cmf = s:taboption(this_ctaba, ListValue, "ppp", translate("Force Modem to PPP Protocol :"));
cmf:value("0", translate("No"))
cmf:value("1", translate("Yes"))
cmf.default=0

cmw = s:taboption(this_ctaba, ListValue, "inter", translate("Modem Interface Selection :"));
cmw:value("0", "Auto")
cmw:value("1", "WAN1")
cmw:value("2", "WAN2")
cmw:value("3", "OFF")
cmw.default=0

cmd = s:taboption(this_ctaba, Value, "delay", translate("Connection Delay in Seconds :")); 
cmd.optional=false; 
cmd.rmempty = false;
cmd.default = 5
cmd.datatype = "and(uinteger,min(5))"

cnl = s:taboption(this_ctaba, ListValue, "nodhcp", translate("No DHCP for QMI Modems :"));
cnl:value("0", translate("No"))
cnl:value("1", translate("Yes"))
cnl.default=0

cmdns1 = s:taboption(this_ctaba, Value, "dns1", translate("Custom DNS Server1 :")); 
cmdns1.rmempty = true;
cmdns1.optional=false;
cmdns1.datatype = "ipaddr"

cmdns2 = s:taboption(this_ctaba, Value, "dns2", translate("Custom DNS Server2 :")); 
cmdns2.rmempty = true;
cmdns2.optional=false;
cmdns2.datatype = "ipaddr"

cmdns3 = s:taboption(this_ctaba, Value, "dns3", translate("Custom DNS Server3 :")); 
cmdns3.rmempty = true;
cmdns3.optional=false;
cmdns3.datatype = "ipaddr"

cmdns4 = s:taboption(this_ctaba, Value, "dns4", translate("Custom DNS Server4 :")); 
cmdns4.rmempty = true;
cmdns4.optional=false;
cmdns4.datatype = "ipaddr"

cmlog = s:taboption(this_ctaba, ListValue, "log", translate("Enable Connection Logging :"));
cmlog:value("0", translate("No"))
cmlog:value("1", translate("Yes"))
cmlog.default=0

if nixio.fs.access("/etc/config/mwan3") then
	cmlb = s:taboption(this_ctaba, ListValue, "lb", translate("Enable Load Balancing at Connection :"));
	cmlb:value("0", translate("No"))
	cmlb:value("1", translate("Yes"))
	cmlb.default=1
end

mtu = s:taboption(this_ctaba, Value, "mtu", translate("Custom MTU :"),
		translate("Acceptable values: 1420-1500. Size for Custom MTU. This may have to be adjusted for certain ISPs"));
mtu.optional=true
mtu.rmempty = true
mtu.default = "1500"
mtu.datatype = "range(1420, 1500)"

cmat = s:taboption(this_ctaba, ListValue, "at", translate("Enable Custom AT Startup Command at Connection :"));
cmat:value("0", translate("No"))
cmat:value("1", translate("Yes"))
cmat.default=0

cmatc = s:taboption(this_ctaba, Value, "atc", translate("Custom AT Startup Command :"));
cmatc.optional=false;
cmatc.rmempty = true;

--
-- Custom Connection Monitoring
--

this_ctab = "cconnect"

calive = s:taboption(this_ctab, ListValue, "alive", translate("Connection Monitoring Status :")); 
calive.rmempty = true;
calive:value("0", translate("Disabled"))
calive:value("2", translate("Enabled with Router Reboot"))
calive:value("3", translate("Enabled with Modem Restart"))
calive.default=0

reliability = s:taboption(this_ctab, Value, "reliability", translate("Tracking reliability"),
		translate("Acceptable values: 1-100. This many Tracking IP addresses must respond for the link to be deemed up"))
reliability.datatype = "range(1, 100)"
reliability.default = "1"
reliability:depends("alive", "1")
reliability:depends("alive", "2")
reliability:depends("alive", "3")
reliability:depends("alive", "4")

count = s:taboption(this_ctab, ListValue, "count", translate("Ping count"))
count.default = "1"
count:value("1")
count:value("2")
count:value("3")
count:value("4")
count:value("5")
count:depends("alive", "1")
count:depends("alive", "2")
count:depends("alive", "3")
count:depends("alive", "4")

interval = s:taboption(this_ctab, ListValue, "pingtime", translate("Ping interval"),
		translate("Amount of time between tracking tests"))
interval.default = "10"
interval:value("5", translate("5 seconds"))
interval:value("10", translate("10 seconds"))
interval:value("20", translate("20 seconds"))
interval:value("30", translate("30 seconds"))
interval:value("60", translate("1 minute"))
interval:value("300", translate("5 minutes"))
interval:value("600", translate("10 minutes"))
interval:value("900", translate("15 minutes"))
interval:value("1800", translate("30 minutes"))
interval:value("3600", translate("1 hour"))
interval:depends("alive", "1")
interval:depends("alive", "2")
interval:depends("alive", "3")
interval:depends("alive", "4")

timeout = s:taboption(this_ctab, ListValue, "pingwait", translate("Ping timeout"))
timeout.default = "2"
timeout:value("1", translate("1 second"))
timeout:value("2", translate("2 seconds"))
timeout:value("3", translate("3 seconds"))
timeout:value("4", translate("4 seconds"))
timeout:value("5", translate("5 seconds"))
timeout:value("6", translate("6 seconds"))
timeout:value("7", translate("7 seconds"))
timeout:value("8", translate("8 seconds"))
timeout:value("9", translate("9 seconds"))
timeout:value("10", translate("10 seconds"))
timeout:depends("alive", "1")
timeout:depends("alive", "2")
timeout:depends("alive", "3")
timeout:depends("alive", "4")

packetsize = s:taboption(this_ctab, Value, "packetsize", translate("Ping packet size in bytes"),
		translate("Acceptable values: 4-56. Number of data bytes to send in ping packets. This may have to be adjusted for certain ISPs"))
	packetsize.datatype = "range(4, 56)"
	packetsize.default = "56"
	packetsize:depends("alive", "1")
	packetsize:depends("alive", "2")
	packetsize:depends("alive", "3")
	packetsize:depends("alive", "4")

down = s:taboption(this_ctab, ListValue, "down", translate("Interface down"),
		translate("Interface will be deemed down after this many failed ping tests"))
down.default = "3"
down:value("1")
down:value("2")
down:value("3")
down:value("4")
down:value("5")
down:value("6")
down:value("7")
down:value("8")
down:value("9")
down:value("10")
down:depends("alive", "1")
down:depends("alive", "2")
down:depends("alive", "3")
down:depends("alive", "4")

up = s:taboption(this_ctab, ListValue, "up", translate("Interface up"),
		translate("Downed interface will be deemed up after this many successful ping tests"))
up.default = "3"
up:value("1")
up:value("2")
up:value("3")
up:value("4")
up:value("5")
up:value("6")
up:value("7")
up:value("8")
up:value("9")
up:value("10")
up:depends("alive", "1")
up:depends("alive", "2")
up:depends("alive", "3")
up:depends("alive", "4")

cb2 = s:taboption(this_ctab, DynamicList, "trackip", translate("Tracking IP"),
		translate("This IP address will be pinged to dermine if the link is up or down."))
cb2.datatype = "ipaddr"
cb2:depends("alive", "1")
cb2:depends("alive", "2")
cb2:depends("alive", "3")
cb2:depends("alive", "4")
cb2.optional=false;
cb2.default="8.8.8.8"

if (multilock == "0") or (multilock == "1" and rootlock == "1") then
	this_ctab = "cbwidth"
	bwday = s:taboption(this_ctab, ListValue, "bwday", translate("Day to Send Report"),
		translate("Day of month to send report for this Profile."))
	bwday.default = "0"
	bwday:value("0", translate("Disabled"))
	bwday:value("1", translate("1st"))
	bwday:value("2", translate("2nd"))
	bwday:value("3", translate("3rd"))
	bwday:value("4", translate("4th"))
	bwday:value("5", translate("5th"))
	bwday:value("6", translate("6th"))
	bwday:value("7", translate("7th"))
	bwday:value("8", translate("8th"))
	bwday:value("9", translate("9th"))
	bwday:value("10", translate("10th"))
	bwday:value("11", translate("11th"))
	bwday:value("12", translate("12th"))
	bwday:value("13", translate("13th"))
	bwday:value("14", translate("14th"))
	bwday:value("15", translate("15th"))
	bwday:value("16", translate("16th"))
	bwday:value("17", translate("17th"))
	bwday:value("18", translate("18th"))
	bwday:value("19", translate("19th"))
	bwday:value("20", translate("20th"))
	bwday:value("21", translate("21th"))
	bwday:value("22", translate("22th"))
	bwday:value("23", translate("23th"))
	bwday:value("24", translate("24th"))
	bwday:value("25", translate("25th"))
	bwday:value("26", translate("26th"))
	bwday:value("27", translate("27th"))
	bwday:value("28", translate("28th"))

	phone = s:taboption(this_ctab, Value, "phone", translate("Phone Number"), translate("Phone Number to send Text Message"))
	phone.default = "12223334444"
	
	bwdelay = s:taboption(this_ctab, ListValue, "bwdelay", translate("Delay before Sending"),
		translate("Hours after Midnight before sending text"))
	bwdelay:value("0", translate("No Delay"))
	bwdelay:value("1", translate("1 hour"))
	bwdelay:value("2", translate("2 hour"))
	bwdelay:value("3", translate("3 hour"))
	bwdelay:value("4", translate("4 hour"))
	bwdelay:value("5", translate("5 hour"))
	bwdelay:value("6", translate("6 hour"))
	bwdelay:value("7", translate("7 hour"))
	bwdelay:value("8", translate("8 hour"))
	bwdelay:value("9", translate("9 hour"))
	bwdelay:value("10", translate("10 hour"))
	bwdelay:value("11", translate("11 hour"))
	bwdelay:value("12", translate("12 hour"))
end

end

return m

