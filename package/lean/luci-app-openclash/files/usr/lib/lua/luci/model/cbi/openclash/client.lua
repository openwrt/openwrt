
local NXFS = require "nixio.fs"
local SYS  = require "luci.sys"
local HTTP = require "luci.http"
local DISP = require "luci.dispatcher"
local UTIL = require "luci.util"
local uci = require("luci.model.uci").cursor()

local t = {
    {enable, disable}
}

a = SimpleForm("apply")
a.reset = false
a.submit = false
s = a:section(Table, t)

o = s:option(Button, "enable") 
o.inputtitle = translate("Enable Clash")
o.inputstyle = "apply"
o.write = function()
  uci:set("openclash", "config", "enable", 1)
  uci:commit("openclash")
  SYS.call("/etc/init.d/openclash restart >/dev/null 2>&1 &")
end

o = s:option(Button, "disable")
o.inputtitle = translate("Disable Clash")
o.inputstyle = "reset"
o.write = function()
  uci:set("openclash", "config", "enable", 0)
  uci:commit("openclash")
  SYS.call("/etc/init.d/openclash stop >/dev/null 2>&1 &")
end

m = Map("openclash")
m.title = translate("OpenClash")
m.description = translate("A Clash Client For OpenWrt")
m.pageaction = false

m:section(SimpleSection).template  = "openclash/status"
m:section(SimpleSection).template  = "openclash/state"
m:section(SimpleSection).template  = "openclash/myip"

d = Map("openclash")
d.title = translate("Technical Support")
d.pageaction = false
d:section(SimpleSection).template  = "openclash/developer"

return m, a, d
