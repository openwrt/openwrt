-- Copyright 2018 Jo-Philipp Wich <jo@mein.io>
-- Licensed to the public under the Apache License 2.0.

module("luci.controller.opkg", package.seeall)

function index()
	entry({"admin", "system", "opkg"}, template("opkg"), _("Software"), 30)
	entry({"admin", "system", "opkg", "list"}, call("action_list")).leaf = true
	entry({"admin", "system", "opkg", "exec"}, post("action_exec")).leaf = true
	entry({"admin", "system", "opkg", "statvfs"}, call("action_statvfs")).leaf = true
	entry({"admin", "system", "opkg", "config"}, post_on({ data = true }, "action_config")).leaf = true
end

function action_list(mode)
	local util = require "luci.util"
	local cmd

	if mode == "installed" then
		cmd = { "/bin/cat", "/usr/lib/opkg/status" }
	else
		local lists_dir = nil

		local fd = io.popen([[sed -rne 's#^lists_dir \S+ (\S+)#\1#p' /etc/opkg.conf /etc/opkg/*.conf 2>/dev/null]], "r")
		if fd then
			lists_dir = fd:read("*l")
			fd:close()
		end

		if not lists_dir or #lists_dir == "" then
			lists_dir = "/tmp/opkg-lists"
		end

		cmd = { "/bin/sh", "-c", [[find %s -type f '!' -name '*.sig' | xargs -r gzip -cd]] % util.shellquote(lists_dir) }
	end

	luci.http.prepare_content("text/plain; charset=utf-8")
	luci.sys.process.exec(cmd, luci.http.write)
end

function action_exec(command, package)
	local sys = require "luci.sys"
	local cmd = { "/bin/opkg", "--force-removal-of-dependent-packages" }
	local pkg = luci.http.formvalue("package")

	if luci.http.formvalue("autoremove") == "true" then
		cmd[#cmd + 1] = "--autoremove"
	end

	if luci.http.formvalue("overwrite") == "true" then
		cmd[#cmd + 1] = "--force-overwrite"
	end

	cmd[#cmd + 1] = command

	if pkg then
		cmd[#cmd + 1] = pkg
	end

	luci.http.prepare_content("application/json")
	luci.http.write_json(sys.process.exec(cmd, true, true))
end

function action_statvfs()
	local fs = require "nixio.fs"

	luci.http.prepare_content("application/json")
	luci.http.write_json(fs.statvfs("/") or {})
end

function action_config()
	local fs = require "nixio.fs"
	local js = require "luci.jsonc"
	local data = luci.http.formvalue("data")

	if data then
		data = js.parse(data)

		if not data then
			luci.http.status(400, "Bad Request")
			return
		end

		local file, content
		for file, content in pairs(data) do
			if type(content) ~= "string" or
			   (file ~= "opkg.conf" and not file:match("^opkg/[^/]+%.conf$"))
			then
				luci.http.status(400, "Bad Request")
				return
			end

			local path = "/etc/%s" % file
			if not fs.access(path, "w") then
				luci.http.status(403, "Permission denied")
				return
			end

			fs.writefile(path, content:gsub("\r\n", "\n"))
		end

		luci.http.status(204, "Saved")
	else
		local rv = { ["opkg.conf"] = fs.readfile("/etc/opkg.conf") }
		local entries = fs.dir("/etc/opkg")
		if entries then
			local entry
			for entry in entries do
				if entry:match("%.conf$") then
					rv["opkg/%s" % entry] = fs.readfile("/etc/opkg/%s" % entry)
				end
			end
		end

		luci.http.prepare_content("application/json")
		luci.http.write_json(rv)
	end
end
