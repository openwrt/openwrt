-- Copyright 2019 Xingwang Liao <kuoruan@gmail.com>
-- Licensed to the public under the MIT License.

local http = require "luci.http"
local uci = require "luci.model.uci".cursor()
local sys = require "luci.sys"

module("luci.controller.frpc", package.seeall)

function index()
	if not nixio.fs.access("/etc/config/frpc") then
		return
	end

	entry({"admin", "services", "frpc"},
		firstchild(), _("Frpc")).dependent = false

	entry({"admin", "services", "frpc", "common"},
		cbi("frpc/common"), _("Settings"), 1)

	entry({"admin", "services", "frpc", "rules"},
		arcombine(cbi("frpc/rules"), cbi("frpc/rule-detail")),
		_("Rules"), 2).leaf = true

	entry({"admin", "services", "frpc", "servers"},
		arcombine(cbi("frpc/servers"), cbi("frpc/server-detail")),
		_("Servers"), 3).leaf = true

	entry({"admin", "services", "frpc", "status"}, call("action_status"))
end


function action_status()
	local running = false

	local client = uci:get("frpc", "main", "client_file")
	if client and client ~= "" then
		local file_name = client:match(".*/([^/]+)$") or ""
		if file_name ~= "" then
			running = sys.call("pidof %s >/dev/null" % file_name) == 0
		end
	end

	http.prepare_content("application/json")
	http.write_json({
		running = running
	})
end
