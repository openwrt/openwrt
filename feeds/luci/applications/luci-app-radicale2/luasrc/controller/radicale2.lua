-- Licensed to the public under the Apache License 2.0.

module("luci.controller.radicale2", package.seeall)

function index()
	local page

        -- no config create an empty one
        if not nixio.fs.access("/etc/config/radicale2") then
                nxfs.writefile("/etc/config/radicale2", "")
        end

	page = entry({"admin", "services", "radicale2"}, alias("admin", "services", "radicale2", "server"), _("Radicale 2.x"))
	page.leaf = false

	page = entry({"admin", "services", "radicale2", "server"}, cbi("radicale2/server"), _("Server Settings"))
	page.leaf = true
	page.order = 10

	page = entry({"admin", "services", "radicale2", "auth"}, cbi("radicale2/auth"), _("Authentication / Users"))
	page.leaf = true
	page.order = 20

	page = entry({"admin", "services", "radicale2", "storage"}, cbi("radicale2/storage"), _("Storage"))
	page.leaf = true
	page.order = 30

	page = entry({"admin", "services", "radicale2", "logging"}, cbi("radicale2/logging"), _("Logging"))
	page.leaf = true
	page.order = 40
end

function pymodexists(module)
   retfun = luci.util.execi('python3 -c \'import importlib.util as util;found_module = util.find_spec("' .. module .. '");print(found_module is not None);print("\\n")\'')
   retval = retfun() == "True"
   while retfun() do end
   return retval
end
