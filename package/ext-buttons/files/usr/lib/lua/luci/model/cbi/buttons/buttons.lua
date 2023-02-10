--[[
LuCI - Lua Configuration Interface

Copyright 2008 Steven Barth <steven@midlink.org>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

$Id$
]]--

m = Map("system", translate("Buttons"),
	translate("This page allows the configuration of custom button actions"))

s = m:section(TypedSection, "button", "")
s.anonymous = true
s.addremove = true

s:option(Value, "button", translate("Name"))

act = s:option(ListValue, "action",
	translate("Action"),
	translate("Specifies the button state to handle"))

act:value("released")
act:value("pressed")
act.default = "released"

s:option(Value, "handler",
	translate("Handler"),
	translate("Path to executable which handles the button event"))

min = s:option(Value, "min", translate("Minimum hold time"))
min.rmempty = true
min:depends("action", "released")

max = s:option(Value, "max", translate("Maximum hold time"))
max.rmempty = true
max:depends("action", "released")

return m