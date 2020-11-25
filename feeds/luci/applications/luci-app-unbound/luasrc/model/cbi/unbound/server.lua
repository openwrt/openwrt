-- Copyright 2016 Eric Luehrsen <ericluehrsen@gmail.com>
-- Licensed to the public under the Apache License 2.0.

local m3, s3, frm
local filename = "/etc/unbound/unbound_srv.conf"
local fs = require "nixio.fs"
local ut = require "luci.util"

m3 = SimpleForm("editing", nil)
m3:append(Template("unbound/css-editing"))
m3.submit = translate("Save")
m3.reset = false
s3 = m3:section(SimpleSection, "",
    translatef(
    "Edit 'server:' clause options for 'include: " .. filename .. "'"))

frm = s3:option(TextValue, "data")
frm.datatype = "string"
frm.rows = 20


function frm.cfgvalue()
    return fs.readfile(filename) or ""
end


function frm.write(self, section, data)
    return fs.writefile(filename, ut.trim(data:gsub("\r\n", "\n")))
end


return m3

