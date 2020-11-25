-- Copyright 2016 Eric Luehrsen <ericluehrsen@gmail.com>
-- Licensed to the public under the Apache License 2.0.

local m4, s4, frm
local filename = "/etc/unbound/unbound_ext.conf"
local fs = require "nixio.fs"
local ut = require "luci.util"

m4 = SimpleForm("editing", nil)
m4:append(Template("unbound/css-editing"))
m4.submit = translate("Save")
m4.reset = false
s4 = m4:section(SimpleSection, "",
    translatef(
    "Edit clauses such as 'forward-zone:' for 'include: " .. filename .. "'"))

frm = s4:option(TextValue, "data")
frm.datatype = "string"
frm.rows = 20


function frm.cfgvalue()
    return fs.readfile(filename) or ""
end


function frm.write(self, section, data)
    return fs.writefile(filename, ut.trim(data:gsub("\r\n", "\n")))
end


return m4

