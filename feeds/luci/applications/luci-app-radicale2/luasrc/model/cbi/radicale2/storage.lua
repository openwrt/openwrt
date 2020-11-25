-- Licensed to the public under the Apache License 2.0.

local rad2 = luci.controller.radicale2
local fs = require("nixio.fs")

local m = Map("radicale2", translate("Radicale 2.x"),
	      translate("A lightweight CalDAV/CardDAV server"))

local s = m:section(NamedSection, "storage", "section", translate("Storage"))
s.addremove = true
s.anonymous = false

o = s:option(ListValue, "type", translate("Storage Type"))
o:value("", translate("Default (multifilesystem)"))
o:value("multifilesystem", translate("Multiple files on filesystem"))
o.default = ""
o.rmempty = true

o = s:option(Value, "filesystem_folder", translate("Folder"), translate("Folder in which to store collections"))
o:depends("type", "")
o:depends("type", "multifilesystem")
o.rmempty = true
o.placeholder = "/srv/radicale2/data"

o = s:option(Flag, "filesystem_locking", translate("Use File Locks"), translate("Prevent other instances or processes from modifying collections while in use"))
o:depends("type", "")
o:depends("type", "multifilesystem")
o.rmempty = true
o.default = o.enabled

o = s:option(Value, "max_sync_token_age", translate("Max Sync Token Age"), translate("Delete sync token that are older (seconds)"))
o:depends("type", "")
o:depends("type", "multifilesystem")
o.rmempty = true
o.placeholder = 2592000
o.datatype = "uinteger"

o = s:option(Flag, "filesystem_close_lock_file", translate("Close Lock File"), translate("Close the lock file when no more clients are waiting"))
o:depends("type", "")
o:depends("type", "multifilesystem")
o.rmempty = true
o.default = o.disabled

o = s:option(Value, "hook", translate("Hook"), translate("Command that is run after changes to storage"))
o:depends("type", "")
o:depends("type", "multifilesystem")
o.rmempty = true
o.placeholder = ("Example: ([ -d .git ] || git init) && git add -A && (git diff --cached --quiet || git commit -m \"Changes by \"%(user)s")

return m
