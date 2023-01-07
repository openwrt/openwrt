module("luci.controller.mesh", package.seeall)
function index()
	local page
	entry({"admin", "mesh"}, firstchild(), "Mesh", 71).dependent=false
	page = entry({"admin", "mesh", "mesh"}, template("mesh/mesh-setup"), "Mesh Configuration", 71)
	page.dependent = true
	
	entry({"admin", "mesh", "getstate"}, call("action_getstate"))
	entry({"admin", "mesh", "sendmeshstate"}, call("action_sendmeshstate"))
	entry({"admin", "mesh", "meshcfg"}, call("action_meshcfg"))
	entry({"admin", "mesh", "meshstartstop"}, call("action_meshstartstop"))
end

function action_getstate()
	local rv = {}
	local radiolist = {}
	
	file = io.open("/etc/meshrun", "r")
	if file == nil then
		rv["state"] = "0"
	else
		rv["state"] = "1"
		file:close()
	end
	os.execute("/usr/lib/mesh/radio.sh ")
	file = io.open("/tmp/radiolist", "r")
	if file ~= nil then
		j = file:read("*line")
		rv['radio'] = j
		if j ~=0 then
			for i=0, j-1 do
				radiolist[i] = file:read("*line")
			end
			rv['radiolist'] = radiolist
		end
		rv['radionumber'] = file:read("*line")
		rv['channelindex'] = file:read("*line")
		rv['channellist'] = file:read("*line")
		rv['channelwidth'] = file:read("*line")
		rv['usedfs'] = file:read("*line")
		rv['dedicated'] = file:read("*line")
		
		rv['networkid'] = file:read("*line")
		rv['netencrypted'] = file:read("*line")
		rv['netpassword'] = file:read("*line") 
		
		rv['signalenable'] = file:read("*line")
		rv['signalid'] = file:read("*line")
		
		file:close()
	else
		rv["radio"] = "0"
	end
	
	
	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_sendmeshstate()
	local set = luci.http.formvalue("set")
	
	os.execute('/usr/lib/mesh/save.sh "' .. set .. '"')
end

function action_meshcfg()
	local set = luci.http.formvalue("set")
	
	os.execute('/usr/lib/mesh/savecfg.sh "' .. set .. '"')
end

function action_meshstartstop()
	os.execute('/usr/lib/mesh/startstop.sh')
	os.execute("reboot &")
end

