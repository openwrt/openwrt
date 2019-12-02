local m, s, o
local NXFS = require "nixio.fs"
local uci=require"luci.model.uci".cursor()
local sys=require"luci.sys"
m = Map("AdGuardHome")

local escconf = uci:get("AdGuardHome","AdGuardHome","configpath")
local binpath = uci:get("AdGuardHome","AdGuardHome","binpath")
s = m:section(TypedSection, "AdGuardHome")
s.anonymous=true
s.addremove=false
o = s:option(TextValue, "escconf")
o.rows = 33
o.wrap = "off"
o.rmempty = true
o.cfgvalue = function(self, section)
	return NXFS.readfile("/tmp/AdGuardHometmpconfig.yaml") or NXFS.readfile(escconf)
end
o.validate=function(self, value)
    NXFS.writefile("/tmp/AdGuardHometmpconfig.yaml", value:gsub("\r\n", "\n"))
	if (sys.call(binpath.." -c /tmp/AdGuardHometmpconfig.yaml --check-config 2> /tmp/AdGuardHometest.log")==0) then
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
o=s:option(Button,"","")
o.inputtitle=translate("Reload Config")
o.write=function()
NXFS.remove("/tmp/AdGuardHometmpconfig.yaml")
luci.http.redirect(luci.dispatcher.build_url("admin","services","AdGuardHome","manual"))
end
end
end
local apply = luci.http.formvalue("cbi.apply")
 if apply then
     io.popen("/etc/init.d/AdGuardHome reload &")
end

return m