module("luci.controller.failover", package.seeall)

function index()
	local page

	if not nixio.fs.access("/etc/config/failover") then
		return
	end

	page = entry({"admin", "network", "failover"}, cbi("rooter/failover"), "Internet Failover", 600)
	page.dependent = true

	entry({"admin", "network", "get_fstatus"}, call("action_get_fstatus"))
end

function action_get_fstatus()
       local file
	mArray = {}
	file = io.open("/tmp/wanstatus", "r")
	if file == nil then
		mArray["wan"] = " "
	else
		mArray["wan"] = file:read("*line")
		mArray["winter"] = file:read("*line")
		mArray["wstatus"] = file:read("*line")
		file:close()
	end
	file = io.open("/tmp/modemstatus", "r")
	if file == nil then
		mArray["modem"] = " "
	else
		mArray["modem"] = file:read("*line")
		mArray["minter"] = file:read("*line")
		mArray["mstatus"] = file:read("*line")
		file:close()
	end
	luci.http.prepare_content("application/json")
	luci.http.write_json(mArray)
end
