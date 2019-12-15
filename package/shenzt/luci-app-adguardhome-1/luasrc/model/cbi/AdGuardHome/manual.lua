local m, s, o
local NXFS = require "nixio.fs"
local uci=require"luci.model.uci".cursor()
local sys=require"luci.sys"
require("string")
require("io")
require("table")
function gen_template_config()
	local b
	local d=""
	for cnt in io.lines("/tmp/resolv.conf.auto") do
		b=string.match (cnt,"^[^#]*nameserver%s+([^%s]+)$")
		if (b~=nil) then
			d=d.."  - "..b.."\n"
		end
	end
	local f=io.open("/usr/share/AdGuardHome/AdGuardHome_template.yaml", "r+")
	local tbl = {}
	local a=""
	while (1) do
    	a=f:read("*l")
		if (a=="#bootstrap_dns") then
			a=d
		elseif (a=="#upstream_dns") then
			a=d
		elseif (a==nil) then
			break
		end
		table.insert(tbl, a)
	end
	f:close()
	return table.concat(tbl, "\n")
end
m = Map("AdGuardHome")
local escconf = uci:get("AdGuardHome","AdGuardHome","configpath")
local binpath = uci:get("AdGuardHome","AdGuardHome","binpath")
s = m:section(TypedSection, "AdGuardHome")
s.anonymous=true
s.addremove=false
--- config
o = s:option(TextValue, "escconf")
o.rows = 66
o.wrap = "off"
o.rmempty = true
o.cfgvalue = function(self, section)
	return  NXFS.readfile("/tmp/AdGuardHometmpconfig.yaml") or NXFS.readfile(escconf) or gen_template_config() or ""
end
o.validate=function(self, value)
    NXFS.writefile("/tmp/AdGuardHometmpconfig.yaml", value:gsub("\r\n", "\n"))
	if NXFS.access(binpath) then
		if (sys.call(binpath.." -c /tmp/AdGuardHometmpconfig.yaml --check-config 2> /tmp/AdGuardHometest.log")==0) then
			return value
		end
	else
		return value
	end
	luci.http.redirect(luci.dispatcher.build_url("admin","services","AdGuardHome","manual"))
	return nil
end
o.write = function(self, section, value)
	NXFS.move("/tmp/AdGuardHometmpconfig.yaml",escconf)
end
o.remove = function(self, section, value)
	NXFS.writefile(escconf, "")
end
o = s:option(DummyValue, "")
o.anonymous=true
o.template = "AdGuardHome/yamleditor"
if not NXFS.access(binpath) then
	o.description=translate("WARNING!!! no bin found apply config will not be test")
end
--- log
if (NXFS.access("/tmp/AdGuardHometmpconfig.yaml")) then
local c=NXFS.readfile("/tmp/AdGuardHometest.log")
if (c~="") then
o = s:option(TextValue, "")
o.readonly=true
o.rows = 5
o.rmempty = true
o.cfgvalue = function(self, section)
	return NXFS.readfile("/tmp/AdGuardHometest.log")
end
--- reload config
o=s:option(Button,"","")
o.inputtitle=translate("Reload Config")
o.write=function()
	NXFS.remove("/tmp/AdGuardHometmpconfig.yaml")
end
end
end
return m