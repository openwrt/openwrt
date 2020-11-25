-- Copyright 2016 Eric Luehrsen <ericluehrsen@gmail.com>
-- Licensed to the public under the Apache License 2.0.

local m2, s2, frm
local filename = "/etc/unbound/unbound.conf"
local fs = require "nixio.fs"
local ut = require "luci.util"

m2 = SimpleForm("editing", nil)
m2:append(Template("unbound/css-editing"))
m2.submit = translate("Save")
m2.reset = false
s2 = m2:section(SimpleSection, "",
    translatef(
    "Edit '" .. filename .. "' when you do not use UCI."))

frm = s2:option(TextValue, "data")
frm.datatype = "string"
frm.rows = 20


function frm.cfgvalue()
    return fs.readfile(filename) or ""
end


function frm.write(self, section, data)
    return fs.writefile(filename, ut.trim(data:gsub("\r\n", "\n")))
end


return m2

