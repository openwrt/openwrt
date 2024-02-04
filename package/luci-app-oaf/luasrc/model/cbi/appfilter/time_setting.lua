local ds = require "luci.dispatcher"
local nxo = require "nixio"
local nfs = require "nixio.fs"
local ipc = require "luci.ip"
local sys = require "luci.sys"
local utl = require "luci.util"
local dsp = require "luci.dispatcher"
local uci = require "luci.model.uci"
local lng = require "luci.i18n"
local jsc = require "luci.jsonc"
local http = luci.http
local SYS = require "luci.sys"
local m, s

m = Map("appfilter", translate(""), translate(""))

s = m:section(TypedSection, "time", translate("Time Setting"),translate("The second time is optional, the end time must be greater than the start time"))
s.anonymous = true


o=s:option(ListValue, "time_mode", translate("Time Mode"),translate("")) 
o.default=0
o:value(0,translate("Blacklist mode"))
o:value(1,translate("Whitelist mode"))

days = s:option(MultiValue, "days", "", translate(""))
days.widget = "checkbox"
days.size = 10
days:value("0", translate("Sun"));
days:value("1", translate("Mon"));
days:value("2", translate("Tue"));
days:value("3", translate("Wed"));
days:value("4", translate("Thur"));
days:value("5", translate("Fri"));
days:value("6", translate("Sat"));

hv = s:option(Value, "start_time", translate("Start Time1"),translate("xx:xx"))
hv.optional = false
hv = s:option(Value, "end_time", translate("End Time1"))
hv.optional = false

hv = s:option(Value, "start_time2", translate("Start Time2"))
hv.optional = false
hv = s:option(Value, "end_time2", translate("End Time2"))
hv.optional = false



return m
