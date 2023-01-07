-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2008-2013 Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

local fs = require "nixio.fs"
local cronfile = "/etc/cronuser"

f = SimpleForm("crontab", translate("Scheduled Tasks"), translate("This is the system crontab in which scheduled tasks can be defined."))

t = f:field(TextValue, "crons")
t.rmempty = true
t.rows = 10
function t.cfgvalue()
        return fs.readfile(cronfile) or ""
end

function f.handle(self, state, data)
        if state == FORM_VALID then
                if data.crons then
                        fs.writefile(cronfile, data.crons:gsub("\r\n", "\n"))
                else
                        fs.writefile(cronfile, "")
                end
                luci.sys.call("/usr/lib/rooter/luci/croncat.sh")
        end
        return true
end

return f
