
local NXFS = require "nixio.fs"
local SYS  = require "luci.sys"
local HTTP = require "luci.http"
local DISP = require "luci.dispatcher"
local UTIL = require "luci.util"
local uci = require "luci.model.uci".cursor()

m = Map("openclash", translate("Global Settings(Will Modify The Config File Or Subscribe According To The Settings On This Page)"))
m.pageaction = false
s = m:section(TypedSection, "openclash")
s.anonymous = true

s:tab("settings", translate("General Settings"))
s:tab("dns", translate("DNS Setting"))
s:tab("rules", translate("Rules Setting"))
s:tab("dashboard", translate("Dashboard Settings"))
s:tab("config_update", translate("Config Update"))
s:tab("rules_update", translate("Rules Update"))
s:tab("geo_update", translate("GEOIP Update"))
s:tab("version_update", translate("Version Update"))

---- General Settings
local cpu_model=SYS.exec("opkg status libc 2>/dev/null |grep 'Architecture' |awk -F ': ' '{print $2}' 2>/dev/null")
o = s:taboption("settings", ListValue, "core_version", translate("Chose to Download"))
o.description = translate("CPU Model")..': '..cpu_model..', '..translate("Select Based On Your CPU Model For Core Update, Wrong Version Will Not Work")
o:value("linux-386")
o:value("linux-amd64", translate("linux-amd64(x86-64)"))
o:value("linux-armv5")
o:value("linux-armv6")
o:value("linux-armv7")
o:value("linux-armv8")
o:value("linux-mips-hardfloat")
o:value("linux-mips-softfloat")
o:value("linux-mips64")
o:value("linux-mips64le")
o:value("linux-mipsle-softfloat")
o:value("linux-mipsle-hardfloat")
o:value("0", translate("Not Set"))
o.default=0

o = s:taboption("settings", ListValue, "en_mode", translate("Select Mode"))
o.description = translate("Select Mode For OpenClash Work, Network Error Try Flush DNS Cache")
o:value("redir-host", translate("redir-host"))
o:value("fake-ip", translate("fake-ip"))
o.default = "redir-host"

o = s:taboption("settings", Value, "proxy_port")
o.title = translate("Redir Port")
o.default = 7892
o.datatype = "port"
o.rmempty = false
o.description = translate("Please Make Sure Ports Available")

o = s:taboption("settings", Value, "http_port")
o.title = translate("HTTP(S) Port")
o.default = 7890
o.datatype = "port"
o.rmempty = false
o.description = translate("Please Make Sure Ports Available")

o = s:taboption("settings", Value, "socks_port")
o.title = translate("SOCKS5 Port")
o.default = 7891
o.datatype = "port"
o.rmempty = false
o.description = translate("Please Make Sure Ports Available")

---- DNS Settings
o = s:taboption("dns", ListValue, "enable_redirect_dns", translate("Redirect Local DNS Setting"))
o.description = translate("Set Local DNS Redirect")
o:value("0", translate("Disable"))
o:value("1", translate("Enable"))
o.default = 1

o = s:taboption("dns", ListValue, "enable_custom_dns", translate("Custom DNS Setting"))
o.description = translate("Set OpenClash Upstream DNS Resolve Server")
o:value("0", translate("Disable"))
o:value("1", translate("Enable"))
o.default = 0

o = s:taboption("dns", ListValue, "ipv6_enable", translate("Enable ipv6 Resolve"))
o.description = translate("Force Enable to Resolve ipv6 DNS Requests")
o:value("0", translate("Disable"))
o:value("1", translate("Enable"))
o.default=0

o = s:taboption("dns", ListValue, "dns_advanced_setting", translate("Advanced Setting"))
o.description = translate("DNS Advanced Settings")
o:value("0", translate("Disable"))
o:value("1", translate("Enable"))
o.default=0

o = s:taboption("dns", Value, "direct_dns", translate("Specify DNS Server"))
o.description = translate("Specify DNS Server For List, Only One IP Server Address Support")
o.default="114.114.114.114"
o.placeholder = translate("114.114.114.114 or 127.0.0.1#5300")
o:depends("dns_advanced_setting", "1")

o = s:taboption("dns", Button, translate("Fake-IP Block List Update")) 
o.title = translate("Fake-IP Block List Update")
o:depends("dns_advanced_setting", "1")
o.inputtitle = translate("Check And Update")
o.inputstyle = "reload"
o.write = function()
  m.uci:set("openclash", "config", "enable", 1)
  m.uci:commit("openclash")
  SYS.call("sh /usr/share/openclash/openclash_fake_block.sh && /etc/init.d/openclash restart >/dev/null 2>&1 &")
  HTTP.redirect(DISP.build_url("admin", "services", "openclash"))
end

custom_fake_black = s:taboption("dns", Value, "custom_fake_black")
custom_fake_black.template = "cbi/tvalue"
custom_fake_black.description = translate("Domain Names In The List Do Not Return Fake-IP, One rule per line")
custom_fake_black.rows = 20
custom_fake_black.wrap = "off"
custom_fake_black:depends("dns_advanced_setting", "1")

function custom_fake_black.cfgvalue(self, section)
	return NXFS.readfile("/etc/config/openclash_custom_fake_black.conf") or ""
end
function custom_fake_black.write(self, section, value)

	if value then
		value = value:gsub("\r\n?", "\n")
		NXFS.writefile("/etc/config/openclash_custom_fake_black.conf", value)
	end
end

---- Rules Settings
o = s:taboption("rules", ListValue, "enable_custom_clash_rules", translate("Custom Clash Rules"))
o.description = translate("Use Custom Rules")
o:value("0", translate("Disable"))
o:value("1", translate("Enable"))
o.default = 0

o = s:taboption("rules", ListValue, "rule_source", translate("Enable Other Rules"))
o.description = translate("Use Other Rules")
o:value("0", translate("Disable Other Rules"))
o:value("lhie1", translate("lhie1 Rules"))
o:value("ConnersHua", translate("ConnersHua Rules"))
o:value("ConnersHua_return", translate("ConnersHua Return Rules"))

SYS.call("/usr/share/openclash/yml_groups_name_get.sh 2>/dev/null")
file = io.open("/tmp/Proxy_Group", "r");

o = s:taboption("rules", ListValue, "GlobalTV", translate("GlobalTV"))
o:depends("rule_source", "lhie1")
o:depends("rule_source", "ConnersHua")
 for l in file:lines() do
   o:value(l)
   end
   file:seek("set")
o = s:taboption("rules", ListValue, "AsianTV", translate("AsianTV"))
o:depends("rule_source", "lhie1")
o:depends("rule_source", "ConnersHua")
 for l in file:lines() do
   o:value(l)
   end
   file:seek("set")
o = s:taboption("rules", ListValue, "Proxy", translate("Proxy"))
o:depends("rule_source", "lhie1")
o:depends("rule_source", "ConnersHua")
o:depends("rule_source", "ConnersHua_return")
 for l in file:lines() do
   o:value(l)
   end
   file:seek("set")
o = s:taboption("rules", ListValue, "Apple", translate("Apple"))
---- o:depends("rule_source", "lhie1")
o:depends("rule_source", "ConnersHua")
 for l in file:lines() do
   o:value(l)
   end
   file:seek("set")
o = s:taboption("rules", ListValue, "AdBlock", translate("AdBlock"))
---- o:depends("rule_source", "lhie1")
o:depends("rule_source", "ConnersHua")
 for l in file:lines() do
   o:value(l)
   end
   file:seek("set")
o = s:taboption("rules", ListValue, "Domestic", translate("Domestic"))
o:depends("rule_source", "lhie1")
o:depends("rule_source", "ConnersHua")
 for l in file:lines() do
   o:value(l)
   end
   file:seek("set")
o = s:taboption("rules", ListValue, "Others", translate("Others"))
o:depends("rule_source", "lhie1")
o:depends("rule_source", "ConnersHua")
o:depends("rule_source", "ConnersHua_return")
o.description = translate("Choose Proxy Group, Base On Your Servers Group in config.yaml")
 for l in file:lines() do
   o:value(l)
   end
   file:close()

---- update Settings
o = s:taboption("config_update", ListValue, "auto_update", translate("Auto Update"))
o.description = translate("Auto Update Server subscription")
o:value("0", translate("Disable"))
o:value("1", translate("Enable"))
o.default=0

o = s:taboption("config_update", ListValue, "config_update_week_time", translate("Update Time (Every Week)"))
o:value("*", translate("Every Day"))
o:value("1", translate("Every Monday"))
o:value("2", translate("Every Tuesday"))
o:value("3", translate("Every Wednesday"))
o:value("4", translate("Every Thursday"))
o:value("5", translate("Every Friday"))
o:value("6", translate("Every Saturday"))
o:value("0", translate("Every Sunday"))
o.default=1

o = s:taboption("config_update", ListValue, "auto_update_time", translate("Update time (every day)"))
for t = 0,23 do
o:value(t, t..":00")
end
o.default=0
o.rmempty = false

o = s:taboption("config_update", ListValue, "config_update_url_type", translate("Update Url Type"))
o:value("clash", translate("Clash"))
o:value("v2ray", translate("V2ray"))
o:value("surge", translate("Surge"))
o.description = translate("Power By fndroid，Use Other Rules If V2ray Subcription")
o.default="clash"

o = s:taboption("config_update", Value, "subscribe_url")
o.title = translate("Subcription Url")
o.description = translate("Server Subscription Address")
o.rmempty = true

o = s:taboption("config_update", Button, translate("Config File Update")) 
o.title = translate("Update Subcription")
o.inputtitle = translate("Check And Update")
o.inputstyle = "reload"
o.write = function()
  m.uci:set("openclash", "config", "enable", 1)
  m.uci:commit("openclash")
  SYS.call("rm -rf /etc/openclash/config.bak 2>/dev/null")
  SYS.call("sh /usr/share/openclash/openclash.sh >/dev/null 2>&1 &")
  HTTP.redirect(DISP.build_url("admin", "services", "openclash"))
end

o = s:taboption("rules_update", ListValue, "other_rule_auto_update", translate("Auto Update"))
o.description = translate("Auto Update Other Rules")
o:value("0", translate("Disable"))
o:value("1", translate("Enable"))
o.default=0

o = s:taboption("rules_update", ListValue, "other_rule_update_week_time", translate("Update Time (Every Week)"))
o:value("*", translate("Every Day"))
o:value("1", translate("Every Monday"))
o:value("2", translate("Every Tuesday"))
o:value("3", translate("Every Wednesday"))
o:value("4", translate("Every Thursday"))
o:value("5", translate("Every Friday"))
o:value("6", translate("Every Saturday"))
o:value("0", translate("Every Sunday"))
o.default=1

o = s:taboption("rules_update", ListValue, "other_rule_update_day_time", translate("Update time (every day)"))
for t = 0,23 do
o:value(t, t..":00")
end
o.default=0

o = s:taboption("rules_update", Button, translate("Other Rules Update")) 
o.title = translate("Update Other Rules")
o.inputtitle = translate("Check And Update")
o.description = translate("Other Rules Update(Only in Use)")
o.inputstyle = "reload"
o.write = function()
  m.uci:set("openclash", "config", "enable", 1)
  m.uci:commit("openclash")
  SYS.call("sh /usr/share/openclash/openclash_rule.sh >/dev/null 2>&1 &")
  HTTP.redirect(DISP.build_url("admin", "services", "openclash"))
end

o = s:taboption("geo_update", ListValue, "geo_auto_update", translate("Auto Update"))
o.description = translate("Auto Update GEOIP Database")
o:value("0", translate("Disable"))
o:value("1", translate("Enable"))
o.default=0

o = s:taboption("geo_update", ListValue, "geo_update_week_time", translate("Update Time (Every Week)"))
o:value("*", translate("Every Day"))
o:value("1", translate("Every Monday"))
o:value("2", translate("Every Tuesday"))
o:value("3", translate("Every Wednesday"))
o:value("4", translate("Every Thursday"))
o:value("5", translate("Every Friday"))
o:value("6", translate("Every Saturday"))
o:value("0", translate("Every Sunday"))
o.default=1

o = s:taboption("geo_update", ListValue, "geo_update_day_time", translate("Update time (every day)"))
for t = 0,23 do
o:value(t, t..":00")
end
o.default=0

o = s:taboption("geo_update", Button, translate("GEOIP Update")) 
o.title = translate("Update GEOIP Database")
o.inputtitle = translate("Check And Update")
o.inputstyle = "reload"
o.write = function()
  m.uci:set("openclash", "config", "enable", 1)
  m.uci:commit("openclash")
  SYS.call("sh /usr/share/openclash/openclash_ipdb.sh >/dev/null 2>&1 &")
  HTTP.redirect(DISP.build_url("admin", "services", "openclash"))
end

---- Dashboard Settings
o = s:taboption("dashboard", Value, "cn_port")
o.title = translate("Dashboard Port")
o.default = 9090
o.datatype = "port"
o.rmempty = false
o.description = translate("Dashboard Address Example: 192.168.1.1/openclash、192.168.1.1:9090/ui")

o = s:taboption("dashboard", Value, "dashboard_password")
o.title = translate("Dashboard Secret")
o.rmempty = true
o.description = translate("Set Dashboard Secret")

---- version update
core_update = s:taboption("version_update", DummyValue, "", nil)
core_update.template = "openclash/update"

-- [[ Edit Server ]] --
s = m:section(TypedSection, "dns_servers", translate("Add Custom DNS Servers"))
s.anonymous = true
s.addremove = true
s.sortable = false
s.template = "cbi/tblsection"
s.rmempty = false

---- enable flag
o = s:option(Flag, "enabled", translate("Enable"), translate("(Enable or Disable)"))
o.rmempty     = false
o.default     = o.enabled
o.cfgvalue    = function(...)
    return Flag.cfgvalue(...) or "1"
end

---- group
o = s:option(ListValue, "group", translate("DNS Server Group"))
o.description = translate("(NameServer Group Must Be Set)")
o:value("nameserver", translate("NameServer"))
o:value("fallback", translate("FallBack"))
o.default     = "nameserver"
o.rempty      = false

---- IP address
o = s:option(Value, "ip", translate("DNS Server Address"))
o.description = translate("(Do Not Add Type Ahead)")
o.placeholder = translate("Not Null")
o.datatype = "or(host, string)"
o.rmempty = true

---- port
o = s:option(Value, "port", translate("DNS Server Port"))
o.description = translate("(Require When Use Non-Standard Port)")
o.datatype    = "port"
o.rempty      = true

---- type
o = s:option(ListValue, "type", translate("DNS Server Type"))
o.description = translate("(Communication protocol)")
o:value("udp", translate("UDP"))
o:value("tcp", translate("TCP"))
o:value("tls", translate("TLS"))
o:value("https", translate("HTTPS"))
o.default     = "udp"
o.rempty      = false

-- [[ Edit Authentication ]] --
s = m:section(TypedSection, "authentication", translate("Set Authentication of SOCKS5/HTTP(S)"))
s.anonymous = true
s.addremove = true
s.sortable = false
s.template = "cbi/tblsection"
s.rmempty = false

---- enable flag
o = s:option(Flag, "enabled", translate("Enable"))
o.rmempty     = false
o.default     = o.enabled
o.cfgvalue    = function(...)
    return Flag.cfgvalue(...) or "1"
end

---- username
o = s:option(Value, "username", translate("Username"))
o.placeholder = translate("Not Null")
o.rempty      = true

---- password
o = s:option(Value, "password", translate("Password"))
o.placeholder = translate("Not Null")
o.rmempty = true

s = m:section(TypedSection, "openclash", translate("Set Custom Rules, Will Add When Flag Turn on"))
s.anonymous = true

custom_rules = s:option(Value, "custom_rules")
custom_rules.template = "cbi/tvalue"
custom_rules.description = translate("Custom Rules Here, For More Go Github:https://github.com/Dreamacro/clash/blob/master/README.md, IP To CIDR: http://ip2cidr.com")
custom_rules.rows = 20
custom_rules.wrap = "off"

function custom_rules.cfgvalue(self, section)
	return NXFS.readfile("/etc/config/openclash_custom_rules.list") or ""
end
function custom_rules.write(self, section, value)

	if value then
		value = value:gsub("\r\n?", "\n")
		NXFS.writefile("/etc/config/openclash_custom_rules.list", value)
	end
end

s = m:section(TypedSection, "openclash", translate("Set Custom Hosts, Only Work with Redir-Host Mode"))
s.anonymous = true

custom_hosts = s:option(Value, "custom_hosts")
custom_hosts.template = "cbi/tvalue"
custom_hosts.description = translate("Custom Hosts Here, For More Go Github:https://github.com/Dreamacro/clash/blob/master/README.md")
custom_hosts.rows = 20
custom_hosts.wrap = "off"

function custom_hosts.cfgvalue(self, section)
	return NXFS.readfile("/etc/config/openclash_custom_hosts.list") or ""
end
function custom_hosts.write(self, section, value)
	if value then
		value = value:gsub("\r\n?", "\n")
		NXFS.writefile("/etc/config/openclash_custom_hosts.list", value)
	end
end

local t = {
    {Commit, Apply}
}

a = m:section(Table, t)

o = a:option(Button, "Commit") 
o.inputtitle = translate("Commit Configurations")
o.inputstyle = "apply"
o.write = function()
  m.uci:commit("openclash")
end

o = a:option(Button, "Apply")
o.inputtitle = translate("Apply Configurations")
o.inputstyle = "apply"
o.write = function()
  m.uci:set("openclash", "config", "enable", 1)
  m.uci:commit("openclash")
  SYS.call("/etc/init.d/openclash restart >/dev/null 2>&1 &")
  HTTP.redirect(DISP.build_url("admin", "services", "openclash"))
end
return m


