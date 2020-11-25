-- Copyright 2016 Eric Luehrsen <ericluehrsen@gmail.com>
-- Licensed to the public under the Apache License 2.0.

local m6, s6, frm
local filename = "/etc/config/unbound"
local fs = require "nixio.fs"
local ut = require "luci.util"

m6 = SimpleForm("editing", nil)
m6:append(Template("unbound/css-editing"))
m6.submit = translate("Save")
m6.reset = false
s6 = m6:section(SimpleSection, "",
    translatef("Edit '" .. filename .. "' "
    .. "and recipes can be found in OpenWrt "
    .. "<a href=\"%s\" target=\"_blank\">Guides</a> "
    .. "and <a href=\"%s\" target=\"_blank\">Github</a>.",
    "https://openwrt.org/docs/guide-user/services/dns/unbound",
    "https://github.com/openwrt/packages/blob/master/net/unbound/files/README.md"))

frm = s6:option(TextValue, "data")
frm.datatype = "string"
frm.rows = 20


function frm.cfgvalue()
    return fs.readfile(filename) or ""
end


function frm.write(self, section, data)
    return fs.writefile(filename, ut.trim(data:gsub("\r\n", "\n")))
end


return m6

