local d = require "luci.dispatcher"
local fs = require "nixio.fs"
local sys = require "luci.sys"
local uci = require "luci.model.uci".cursor()
local appname = "passwall"

m = Map(appname)
m:section(SimpleSection).template  = "passwall/global/status1"
return m