--[[
LuCI - Lua Configuration Interface for mosquitto

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

]]--

module("luci.controller.mosquitto", package.seeall)

function index()
    entry({"admin", "services", "mosquitto"}, cbi("mosquitto"), _("Mosquitto"))
end
