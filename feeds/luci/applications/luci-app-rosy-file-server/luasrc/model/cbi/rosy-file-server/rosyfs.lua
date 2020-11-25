-- Copyright 2019 Rosy Song <rosysong@rosinson.com>
-- Licensed to the public under the Apache License 2.0.

local uci = require("luci.model.uci").cursor()
local dis = uci:get("rosyfs", "default", "disabled")
local tgt = uci:get("rosyfs", "default", "target")
local tlt = uci:get("rosyfs", "default", "title")

m = Map("rosyfs", translate("Rosy File Server Settings"))

s = m:section(TypedSection, "rosyfs", nil)
s.addremove = false
s.anonymous = true

e = s:option(Flag, "disabled", translate("Disable"), translate("Disable Rosy File Server"))
e.default = dis or e.disabled
e.rmempty = false

a = s:option(Value, "target", translate("Target"), translate("Specify path to be mapped"))
a.default = tgt or "/www"
a.datatype = "directory"

t = s:option(Value, "title", translate("Title"), translate("Title to be shown"))
t.default = tlt or "Rosy File Server"

return m
