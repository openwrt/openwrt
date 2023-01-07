local utl = require "luci.util"

local sys   = require "luci.sys"
local zones = require "luci.sys.zoneinfo"
local fs    = require "nixio.fs"
local conf  = require "luci.config"

m = Map("blockport", translate("Port Blocking"), translate("Block traffic using specific ports"))

m.on_after_save = function(self)
	luci.sys.call("/usr/lib/blockport/blockport.sh &")
end

s = m:section(TypedSection, "port", translate("Block Port List"))
s.anonymous = true
s.addremove = false

s:option(DynamicList, "block", translate("Ports"))


return m