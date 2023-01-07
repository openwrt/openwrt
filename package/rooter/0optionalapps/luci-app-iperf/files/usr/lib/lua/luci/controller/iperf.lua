local uci = luci.model.uci.cursor()
local ut = require "luci.util"

module("luci.controller.iperf", package.seeall)

function index()
	--entry({"admin", "openmptcprouter", "iperf"}, cbi("iperf"), _("iperf"))
	entry({"admin", "services", "iperf"}, alias("admin", "services", "iperf", "test"), _("iPerf Speed Test"),72)
	entry({"admin", "services", "iperf", "test"}, template("iperf/test"), nil,1)
	entry({"admin", "services", "iperf", "run_test"}, post("run_test")).leaf = true
end

function run_test(server,proto,mode,updown,omit,parallel,transmit,bitrate)
	luci.http.prepare_content("text/plain")
	local iperf
	local addr = uci:get("iperf",server,"host")
	local ports = uci:get("iperf",server,"ports")
	local user = uci:get("iperf",server,"user") or ""
	local password = uci:get("iperf",server,"password") or ""
	local key = uci:get("iperf",server,"key") or ""
	local options = ""
	if user ~= "" and password ~= "" and key ~= "" then
		luci.sys.call("echo " .. key .. " | base64 -d > /tmp/iperf.pem")
		options = options .. " --username " .. user .. " --rsa-public-key-path /tmp/iperf.pem"
	end
	if mode == "udp" then
		options = options .. " -u -b " .. bitrate
	end
	if updown ~= "upload" then
		options = options .. " -R"
	end
	local ipv = "4"
	if proto == "ipv6" then
		local ipv = "6"
	end
	
	local t={}
	for pt in ports:gmatch("([^,%s]+)") do
		table.insert(t,pt)
	end
	local port = t[ math.random( #t ) ]
	if password ~= "" then
		iperf = io.popen("omr-iperf -P %s -%s -O %s -t %s -J -Z %s" % {parallel,ipv,omit,transmit,options})
	else
		iperf = io.popen("iperf3 -c %s -P %s -%s -p %s -O %s -t %s -J -Z %s" % {ut.shellquote(addr),parallel,ipv,port,omit,transmit,options})
	end
	if iperf then
		while true do
			local ln = iperf:read("*l")
			if not ln then break end
			luci.http.write(ln)
			luci.http.write("\n")
		end
	end
	return
end