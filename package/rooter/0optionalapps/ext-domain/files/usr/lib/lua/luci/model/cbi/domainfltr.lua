local utl = require "luci.util"

local sys   = require "luci.sys"
local zones = require "luci.sys.zoneinfo"
local fs    = require "nixio.fs"
local conf  = require "luci.config"

m = Map("filter", translate("Domain Filter"), translate("Block traffic to specified URLs"))

m.on_after_save = function(self)
	luci.sys.call("/usr/lib/domain/filter.sh &")
end

s = m:section(TypedSection, "filter", translate("Block List"))
s.anonymous = true
s.addremove = false

s:option(DynamicList, "ipset", translate("Domain URLs"))


return m