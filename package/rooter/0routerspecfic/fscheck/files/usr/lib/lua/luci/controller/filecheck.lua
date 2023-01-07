module("luci.controller.filecheck", package.seeall)

function index()
        local page
        page = entry({"admin", "system", "filecheck"}, template("admin_system/filecheck"), _("File System Check"), 93)
        entry({"admin", "system", "do_filecheck"}, call("action_filecheck"))
		entry({"admin", "system", "do_reboot"}, call("action_rebt"))
        page.dependent = true
end

function action_filecheck()
    local rv ={}
	os.execute("/usr/lib/rooter/filecheck.sh")
	result = "/tmp/fsresult"
	local file = io.open(result, "r")
	if file ~= nil then
		rv["result"] = file:read("*all")
		file:close()
		os.execute("/usr/lib/rooter/luci/luaops.sh delete /tmp/fsresult")
	else
		rv["result"] = "No response"
	end
	file = io.open("/tmp/fsro", "r")
	if file ~= nil then
		rv["fsro"] = 1
		file:close()
	else
		rv["fsro"] = 0
	end
	
	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_rebt()
	os.execute("reboot &")
end