require("luci.sys")
require("luci.util")
local fs=require"nixio.fs"
local uci=require"luci.model.uci".cursor()
require("io")
local configpath=uci:get("AdGuardHome","AdGuardHome","configpath")
if (configpath==nil) then
configpath="/etc/AdGuardHome.yaml"
end
local binpath=uci:get("AdGuardHome","AdGuardHome","binpath")
if (binpath==nil) then
binpath="/usr/bin/AdGuardHome/AdGuardHome"
end
local httpport=luci.sys.exec("awk '/bind_port:/{printf($2);exit;}' "..configpath.." 2>/dev/null")
if (httpport=="") then
httpport=uci:get("AdGuardHome","AdGuardHome","httpport") or "3000"
end
mp = Map("AdGuardHome", "AdGuard Home")
mp.description = translate("Free and open source, powerful network-wide ads & trackers blocking DNS server.")
mp:section(SimpleSection).template  = "AdGuardHome/AdGuardHome_status"

s = mp:section(TypedSection, "AdGuardHome")
s.anonymous=true
s.addremove=false
---- enable
o = s:option(Flag, "enabled", translate("Enable"))
o.default = 0
o.rmempty = false
---- httpport
o =s:option(Value,"httpport",translate("Browser management port"))
o.placeholder=3000
o.default=3000
o.datatype="port"
o.rmempty=false
o.description = translate("<input type=\"button\" style=\"width:210px;border-color:Teal; text-align:center;font-weight:bold;color:Green;\" value=\"AdGuardHome Web:"..httpport.."\" onclick=\"window.open('http://'+window.location.hostname+':"..httpport.."/')\"/>")
---- update warning not safe
version=uci:get("AdGuardHome","AdGuardHome","version")
e=""
if not fs.access(configpath) then
	e=e.." no config"
end
if not fs.access(binpath) then
	e=e.." no bin"
else
	if (version ~= nil) then
		e=version..e
	else
		e="unknown"..e
	end
end
o=s:option(Button,"restart",translate("Update"))
o.inputtitle=translate("Update core version")
o.template = "AdGuardHome/AdGuardHome_check"
o.description=string.format(translate("core version got last time:").."<strong><font id=\"updateversion\" color=\"green\">%s </font></strong>",e)

---- port warning not safe
local port=luci.sys.exec("awk '/  port:/{printf($2);exit;}' "..configpath.." 2>nul")
if (port=="") then
port="?"
end
---- Redirect
o = s:option(ListValue, "redirect", port..translate("Redirect"), translate("AdGuardHome redirect mode"))
o.placeholder = "none"
o:value("none", translate("none"))
o:value("dnsmasq-upstream", translate("Run as dnsmasq upstream server"))
o:value("redirect", translate("Redirect 53 port to AdGuardHome"))
o:value("exchange", translate("Use port 53 replace dnsmasq"))
o.default     = "none"

---- bin path
o = s:option(Value, "binpath", translate("Bin Path"), translate("AdGuardHome Bin path if no bin will auto download"))
o.default     = "/usr/bin/AdGuardHome/AdGuardHome"
o.datatype    = "string"
o.validate=function(self, value)
if fs.stat(value,"type")=="dir" then
	mp.message ="error!bin path is a dir"
	return nil
end 
return value
end
--- upx
o = s:option(ListValue, "upxflag", translate("use upx to compress bin after download"))
o:value("", translate("none"))
o:value("-1", translate("compress faster"))
o:value("-9", translate("compress better"))
o:value("--best", translate("compress best(can be slow for big files)"))
o:value("--brute", translate("try all available compression methods & filters [slow]"))
o:value("--ultra-brute", translate("try even more compression variants [very slow]"))
o.default     = ""
o.description=translate("bin use less space,but may have compatibility issues")
---- config path
o = s:option(Value, "configpath", translate("Config Path"), translate("AdGuardHome config path"))
o.default     = "/etc/AdGuardHome.yaml"
o.datatype    = "string"
o.validate=function(self, value)
if fs.stat(value,"type")=="dir" then
	mp.message ="error!config path is a dir"
	return nil
end 
return value
end
---- work dir
o = s:option(Value, "workdir", translate("Work dir"), translate("AdGuardHome work dir include rules,audit log and database"))
o.default     = "/usr/bin/AdGuardHome"
o.datatype    = "string"
o.validate=function(self, value)
if fs.stat(value,"type")=="reg" then
	mp.message ="error!work dir is a file"
	return nil
end 
return value
end
---- log file
o = s:option(Value, "logfile", translate("Runtime log file"), translate("AdGuardHome runtime Log file if 'syslog': write to system log;if empty no log"))
o.default     = ""
o.datatype    = "string"
o.validate=function(self, value)
if fs.stat(value,"type")=="dir" then
	mp.message ="error!log file is a dir"
	return nil
end 
return value
end
---- debug
o = s:option(Flag, "verbose", translate("Verbose log"))
o.default = 0
---- gfwlist 
local a=luci.sys.call("grep -m 1 -q programadd "..configpath)
if (a==0) then
a="Added"
else
a="Not added"
end
o=s:option(Button,"gfwadd",translate("Add gfwlist"),translate(a))
o.inputtitle=translate("Add")
o.write=function()
luci.sys.exec("sh /usr/share/AdGuardHome/gfw2adg.sh 2>&1")
luci.http.redirect(luci.dispatcher.build_url("admin","services","AdGuardHome"))
end
o = s:option(Value, "gfwupstream", translate("Gfwlist upstream dns server"), translate("Gfwlist domain upstream dns service"))
o.default     = "tcp://208.67.220.220:5353"
o.datatype    = "string"
---- chpass

o = s:option(Value, "hashpass", translate("Change browser management password"), translate("Press load culculate model and culculate finally save/apply"))
o.default     = ""
o.datatype    = "string"
o.template = "AdGuardHome/AdGuardHome_chpass"
---- database protect
o = s:option(Flag, "keepdb", translate("Keep database when system upgrade"))
o.default = 0
---- wait net on boot
o = s:option(Flag, "waitonboot", translate("Boot delay until network ok"))
o.default = 1
---- backup workdir on shutdown
o = s:option(Flag, "backupwd", translate("Backup workdir when shutdown"))
o.default = 0
o.description=translate("Will be restore when workdir/data is empty")
----backup workdir path
o = s:option(Value, "backupwdpath", translate("Backup workdir path"))
o.default     = "/usr/bin/AdGuardHome"
o.datatype    = "string"
o.validate=function(self, value)
if fs.stat(value,"type")=="reg" then
	mp.message ="error!backup dir is a file"
	return nil
end 
return value
end
nixio.fs.writefile("/var/run/lucilogpos","0")
return mp