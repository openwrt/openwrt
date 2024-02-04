local api = require "luci.passwall2.api"
local appname = api.appname

f = SimpleForm(appname)
f.reset = false
f.submit = false
f:append(Template(appname .. "/log/log"))
return f
