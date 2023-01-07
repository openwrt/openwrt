--[[
LuCI - Lua Configuration Interface

Copyright 2012 Jo-Philipp Wich <jow@openwrt.org>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

]]--

local m, s

m = Map("luci", translate("Custom Commands"),
	translate("This page allows you to configure custom shell commands which can be easily invoked from the web interface."))

s = m:section(TypedSection, "command", "")
s.template = "cbi/tblsection"
s.anonymous = true
s.addremove = true


s:option(Value, "name", translate("Description"),
         translate("A short textual description of the configured command"))

s:option(Value, "command", translate("Command"),
         translate("Command line to execute"))

s:option(Flag, "param", translate("Custom arguments"),
         translate("Allow the user to provide additional command line arguments"))

s:option(Flag, "public", translate("Public access"),
         translate("Allow executing the command and downloading its output without prior authentication"))

return m
