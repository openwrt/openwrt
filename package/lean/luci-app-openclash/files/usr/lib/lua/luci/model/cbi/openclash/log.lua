--
local NXFS = require "nixio.fs"
local SYS  = require "luci.sys"
local HTTP = require "luci.http"
local DISP = require "luci.dispatcher"
local UTIL = require "luci.util"


m = Map("openclash", translate("Server logs"))
s = m:section(TypedSection, "openclash")
m.pageaction = false
s.anonymous = true
s.addremove=false

local clog = "/tmp/openclash.log"
log = s:option(TextValue, "clog")
log.readonly=true
log.description = translate("")
log.rows = 29
log.wrap = "off"
log.cfgvalue = function(self, section)
	return NXFS.readfile(clog) or ""
end
log.write = function(self, section, value)
end

return m