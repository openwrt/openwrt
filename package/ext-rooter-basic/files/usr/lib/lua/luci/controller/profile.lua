module("luci.controller.profile", package.seeall)
function index()
	entry({"admin", "profile", "savecfg"}, call("action_savecfg"))
	entry({"admin", "profile", "loadcfg"}, call("action_loadcfg"))
	entry({"admin", "profile", "loadcfg1"}, call("action_loadcfg1"))
	entry({"admin", "profile", "loadcfg2"}, call("action_loadcfg2"))
end

function action_savecfg()
	os.execute('/usr/lib/profile/savecfg.sh')
end

function action_loadcfg()
	local set = luci.http.formvalue("set")
	local set1 = luci.http.formvalue("set1")
	local tfile = io.open("/tmp/profilename", "w")
	tfile:write(set, "\n")
	if set1 ~= "~~" then
		tfile:write(set1, "\n")
	end
	tfile:close()
	os.execute("/usr/lib/profile/loadcfg.sh")
end

function action_loadcfg1()
	local set = luci.http.formvalue("set")
	local tfile = io.open("/tmp/profilename", "w")
	tfile:write(set)
	tfile:close()
end

function action_loadcfg2()
	local set1 = luci.http.formvalue("set1")
	if set1 ~= "~~" then
		local tfile = io.open("/tmp/profilename", "a")
		tfile:write(set1)
		tfile:close()
	end
	os.execute("/usr/lib/profile/loadcfg.sh")
end