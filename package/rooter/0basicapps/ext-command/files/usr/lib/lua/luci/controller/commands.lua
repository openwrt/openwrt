--[[
LuCI - Lua Configuration Interface

Copyright 2012 Jo-Philipp Wich <jow@openwrt.org>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

]]--

module("luci.controller.commands", package.seeall)

I18N = require "luci.i18n"
translate = I18N.translate

function index()
	local multilock = luci.model.uci.cursor():get("custom", "multiuser", "multi") or "0"
	local rootlock = luci.model.uci.cursor():get("custom", "multiuser", "root") or "0"
	if (multilock == "0") or (multilock == "1" and rootlock == "1") then
		entry({"admin", "system", "commands"}, firstchild(), _(translate("Custom Commands")), 80)
		entry({"admin", "system", "commands", "dashboard"}, template("commands"), _(translate("Dashboard")), 1)
		entry({"admin", "system", "commands", "config"}, cbi("commands"), _(translate("Configure")), 2)
		entry({"admin", "system", "commands", "script"}, template("cmdedit"), _(translate("Scripts")), 3)
		entry({"admin", "system", "commands", "run"}, call("action_run"), nil, 3).leaf = true
		entry({"admin", "system", "commands", "download"}, call("action_download"), nil, 3).leaf = true
	end

	entry({"admin", "system", "load_script"}, call("action_load_script"))
	entry({"admin", "system", "save_script"}, call("action_save_script"))
	entry({"admin", "system", "del_script"}, call("action_del_script"))

	entry({"command"}, call("action_public"), nil, 1).leaf = true
end

--- Decode a given string into arguments following shell quoting rules
--- [[abc \def "foo\"bar" abc'def']] -> [[abc def]] [[foo"bar]] [[abcdef]]
local function parse_args(str)
	local args = { }

	local function isspace(c)
		if c == 9 or c == 10 or c == 11 or c == 12 or c == 13 or c == 32 then
			return c
		end
	end

	local function isquote(c)
		if c == 34 or c == 39 or c == 96 then
			return c
		end
	end

	local function isescape(c)
		if c == 92 then
			return c
		end
	end

	local function ismeta(c)
		if c == 36 or c == 92 or c == 96 then
			return c
		end
	end

	--- Convert given table of byte values into a Lua string and append it to
	--- the "args" table. Segment byte value sequence into chunks of 256 values
	--- to not trip over the parameter limit for string.char()
	local function putstr(bytes)
		local chunks = { }
		local csz = 256
		local upk = unpack
		local chr = string.char
		local min = math.min
		local len = #bytes
		local off

		for off = 1, len, csz do
			chunks[#chunks+1] = chr(upk(bytes, off, min(off + csz - 1, len)))
		end

		args[#args+1] = table.concat(chunks)
	end

	--- Scan substring defined by the indexes [s, e] of the string "str",
	--- perform unquoting and de-escaping on the fly and store the result in
	--- a table of byte values which is passed to putstr()
	local function unquote(s, e)
		local off, esc, quote
		local res = { }

		for off = s, e do
			local byte = str:byte(off)
			local q = isquote(byte)
			local e = isescape(byte)
			local m = ismeta(byte)

			if e then
				esc = true
			elseif esc then
				if m then res[#res+1] = 92 end
				res[#res+1] = byte
				esc = false
			elseif q and quote and q == quote then
				quote = nil
			elseif q and not quote then
				quote = q
			else
				if m then res[#res+1] = 92 end
				res[#res+1] = byte
			end
		end

		putstr(res)
	end

	--- Find substring boundaries in "str". Ignore escaped or quoted
	--- whitespace, pass found start- and end-index for each substring
	--- to unquote()
	local off, esc, start, quote
	for off = 1, #str + 1 do
		local byte = str:byte(off)
		local q = isquote(byte)
		local s = isspace(byte) or (off > #str)
		local e = isescape(byte)

		if esc then
			esc = false
		elseif e then
			esc = true
		elseif q and quote and q == quote then
			quote = nil
		elseif q and not quote then
			start = start or off
			quote = q
		elseif s and not quote then
			if start then
				unquote(start, off - 1)
				start = nil
			end
		else
			start = start or off
		end
	end

	--- If the "quote" is still set we encountered an unfinished string
	if quote then
		unquote(start, #str)
	end

	return args
end

local function parse_cmdline(cmdid, args)
	local uci = require "luci.model.uci".cursor()
	if uci:get("luci", cmdid) == "command" then
		local cmd = uci:get_all("luci", cmdid)
		local argv = parse_args(cmd.command)
		local i, v

		if cmd.param == "1" and args then
			for i, v in ipairs(parse_args(luci.http.urldecode(args))) do
				argv[#argv+1] = v
			end
		end

		for i, v in ipairs(argv) do
			if v:match("[^%w%.%-i/]") then
				argv[i] = '"%s"' % v:gsub('"', '\\"')
			end
		end

		return argv
	end
end

function action_run(...)
	local fs   = require "nixio.fs"
	local argv = parse_cmdline(...)
	if argv then
		local outfile = os.tmpname()
		local errfile = os.tmpname()

		local rv = os.execute(table.concat(argv, " ") .. " >%s 2>%s" %{ outfile, errfile })
		local stdout = fs.readfile(outfile, 1024 * 512) or ""
		local stderr = fs.readfile(errfile, 1024 * 512) or ""

		fs.unlink(outfile)
		fs.unlink(errfile)

		local binary = not not (stdout:match("[%z\1-\8\14-\31]"))

		luci.http.prepare_content("application/json")
		luci.http.write_json({
			command  = table.concat(argv, " "),
			stdout   = not binary and stdout,
			stderr   = stderr,
			exitcode = rv,
			binary   = binary
		})
	else
		luci.http.status(404, translate("No such command"))
	end
end

function action_download(...)
	local fs   = require "nixio.fs"
	local argv = parse_cmdline(...)
	if argv then
		local fd = io.popen(table.concat(argv, " ") .. " 2>/dev/null")
		if fd then
			local chunk = fd:read(4096) or ""
			local name
			if chunk:match("[%z\1-\8\14-\31]") then
				luci.http.header("Content-Disposition", "attachment; filename=%s"
				                 % fs.basename(argv[1]):gsub("%W+", ".") .. ".bin")
				luci.http.prepare_content("application/octet-stream")
			else
				luci.http.header("Content-Disposition", "attachment; filename=%s"
				                 % fs.basename(argv[1]):gsub("%W+", ".") .. ".txt")
				luci.http.prepare_content("text/plain")
			end

			while chunk do
				luci.http.write(chunk)
				chunk = fd:read(4096)
			end

			fd:close()
		else
			luci.http.status(500, translate("Failed to execute command"))
		end
	else
		luci.http.status(404, "No such command")
	end
end

function action_public(cmdid, args)
	local uci = require "luci.model.uci".cursor()
	if cmdid and
	   uci:get("luci", cmdid) == "command" and
	   uci:get("luci", cmdid, "public") == "1"
	then
		action_download(cmdid, args)
	else
		luci.http.status(403, translate("Access to command denied"))
	end
end

function action_load_script()
	local set = luci.http.formvalue("set")
	local rv ={}
	local file

	file = io.open(set, "r")
	if file ~= nil then
		local tmp = file:read("*all")
		rv["text"] = tmp
		file:close()
	else
		rv["text"] = translate("No file found")
	end

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_save_script()
	local line = luci.http.formvalue("set")
	local rv ={}
	local file

	s, e = line:find("|")
	name = line:sub(1,s-1)
	text = line:sub(e+1)

	file = io.open(name, "w")
	file:write(text)
	file:close()
	os.execute("chmod 777 " .. name)

	rv["name"] = name
	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_del_script()
	local name = luci.http.formvalue("set")
	local rv ={}
	os.remove(name)

	rv["name"] = name
	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end