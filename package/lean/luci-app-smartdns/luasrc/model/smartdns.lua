-- Copyright 2018 Nick Peng (pymumu@gmail.com)

require ("nixio.fs")
require ("luci.http")
require ("luci.dispatcher")
require ("nixio.fs")

local uci = require "luci.model.uci".cursor()

module("luci.model.smartdns", package.seeall)

function get_config_option(module, section, option, default)
	return uci:get_first(module, section, option) or default
end

return m

