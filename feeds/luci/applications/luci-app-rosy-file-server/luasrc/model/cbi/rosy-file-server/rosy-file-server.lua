-- Copyright 2018 Rosy Song <rosysong@rosinson.com>
-- Licensed to the public under the Apache License 2.0.

local uci = require "luci.model.uci".cursor()
local dis = uci:get("rosyfs", "default", "disabled")

local targets = {}
local server_root = luci.http.formvalue("server_root") or "/www/rosyfs-share/"
local buffer = io.popen("/bin/busybox ls -ahLlp %s" % server_root)
if dis ~= '1' and buffer then
	for l in buffer:lines() do
		local _p, i, u, g, sz, mm, dd, tt, nm = l:match(
			"^([^%s]+)%s+([^%s]+)%s+([^%s]+)%s+([^%s]+)%s+([^%s]+)%s+([^%s]+)%s+([^%s]+)%s+([^%s]+)%s+(.+)"
		)
		local p = _p and string.sub(_p, 1, 1) or nil
		if p and (p == '-' or p == 'd') and nm and (nm ~= "./") and
			not (server_root == "/www/rosyfs-share/" and nm == "../") and
			not (server_root == "/www/rosyfs-share/" and nm == "rosyfs-share/") then
			targets[nm] = {
				['type'] = p,
				['size'] = sz,
				['last'] = "%s %s %s" % { mm, dd, tt },
				['name'] = nm
			}
		end
	end
end

local title = uci:get("rosyfs", "default", "title") or nil

m = SimpleForm("rosyfs", title or translate("Rosy File Server"), translate("This is rosy file server for luci.<br /><strong>Note: targets will be mapped at /www/rosyfs-share !</strong>"))
m.reset = false
m.submit = false


s = m:section(Table, targets)

t = s:option(DummyValue, "type", translate("Type"))

n = s:option(DummyValue, "name", translate("Name"))
n.rawhtml = true

function n.cfgvalue(self, section)
	local v = DummyValue.cfgvalue(self, section)
	local hv = (v == "../") and "Parent Directory" or v
	local t = targets[v]['type']

	if t and t ~='d' then
		-- File
		return translatef("<a href='%s%s'>%s</a>",
			string.sub(server_root, 5, #server_root), hv, hv);
	elseif t then
		-- Directory
		if v == "../" then
			local dir = luci.util.trim(luci.util.exec("dirname " .. server_root))

			if dir ~= "/" then dir = dir .. "/" end

			return translatef("<a href='%s?server_root=%s'>%s</a>",
				luci.dispatcher.build_url("httpfs/rosy-file-server"), dir, hv)
		else
			return translatef("<a href='%s?server_root=%s%s'>%s</a>",
				luci.dispatcher.build_url("httpfs/rosy-file-server"),
				server_root, hv, hv)
		end
	end
end

l = s:option(DummyValue, "last", translate("Last Modified"))
sz = s:option(DummyValue, "size", translate("Size"))

return m
