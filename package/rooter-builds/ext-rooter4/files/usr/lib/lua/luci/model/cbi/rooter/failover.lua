local utl = require "luci.util"
local uci  = require "luci.model.uci".cursor()

function cbiAddWan(field)
	i = 0
	uci.cursor():foreach("failover", "member",
		function (section)
			line = section[".name"]
			s, e = line:find("Modem")
			if s == nil then
				field:value(section[".name"])
				if i == 0 then
					field.default = section[".name"]
				end
				i = 1
			end
		end
	)
end

function cbiAddModem(field)
	i = 0
	uci.cursor():foreach("failover", "member",
		function (section)
			line = section[".name"]
			s, e = line:find("Modem")
			if s ~= nil then
				field:value(section[".name"])
				if i == 0 then
					field.default = section[".name"]
				end
				i = 1
			end
		end
	)
end

m = Map("failover", translate("Internet Connection Failover"), translate("Enable a connection failover system between two Internet sources."))

m.on_after_commit = function(self)
	luci.sys.call("/usr/lib/rooter/luci/failchk.sh")
end

enabl = m:section(NamedSection, "enabled", "enabled", "")
	enabl.addremove = false
	enabl.dynamic = false

e = enabl:option(Flag, "enabled", translate("Failover Enabled"))
e.rmempty = false

policy = m:section(NamedSection, "faillist", "policy", "")
	policy.addremove = false
	policy.dynamic = false

use_wan = policy:option(ListValue, "use_wan", translate("Primary Internet Source"),
		translate("Select Primary Internet source."))
	cbiAddWan(use_wan)

use_modem = policy:option(ListValue, "use_modem", translate("Secondary Internet Source"),
		translate("Select Secondary (Backup) Internet source."))
	cbiAddModem(use_modem)

tracker = m:section(NamedSection, "failover", "tracker", "")
	tracker.addremove = false
	tracker.dynamic = false

reliability = tracker:option(Value, "reliability", translate("Tracking reliability"),
		translate("Acceptable values: 1-100. This many Tracking IP addresses must respond for the source to be deemed up"))
	reliability.datatype = "range(1, 100)"
	reliability.default = "1"

count = tracker:option(ListValue, "count", translate("Ping count"))
	count.default = "1"
	count:value("1")
	count:value("2")
	count:value("3")
	count:value("4")
	count:value("5")

interval = tracker:option(ListValue, "pingtime", translate("Ping interval"),
		translate("Amount of time between tracking tests"))
	interval.default = "5"
	interval:value("5", translate("5 seconds"))
	interval:value("10", translate("10 seconds"))
	interval:value("20", translate("20 seconds"))
	interval:value("30", translate("30 seconds"))
	interval:value("60", translate("1 minute"))
	interval:value("300", translate("5 minutes"))
	interval:value("600", translate("10 minutes"))
	interval:value("900", translate("15 minutes"))
	interval:value("1800", translate("30 minutes"))
	interval:value("3600", translate("1 hour"))

timeout = tracker:option(ListValue, "pingwait", translate("Ping timeout"))
	timeout.default = "2"
	timeout:value("1", translate("1 second"))
	timeout:value("2", translate("2 seconds"))
	timeout:value("3", translate("3 seconds"))
	timeout:value("4", translate("4 seconds"))
	timeout:value("5", translate("5 seconds"))
	timeout:value("6", translate("6 seconds"))
	timeout:value("7", translate("7 seconds"))
	timeout:value("8", translate("8 seconds"))
	timeout:value("9", translate("9 seconds"))
	timeout:value("10", translate("10 seconds"))

down = tracker:option(ListValue, "down", translate("Interface down"),
		translate("Interface will be deemed down after this many failed ping tests in a row"))
	down.default = "2"
	down:value("1")
	down:value("2")
	down:value("3")
	down:value("4")
	down:value("5")
	down:value("6")
	down:value("7")
	down:value("8")
	down:value("9")
	down:value("10")

up = tracker:option(ListValue, "up", translate("Interface up"),
		translate("Downed interface will be deemed up after this many successful ping tests in a row"))
	up.default = "2"
	up:value("1")
	up:value("2")
	up:value("3")
	up:value("4")
	up:value("5")
	up:value("6")
	up:value("7")
	up:value("8")
	up:value("9")
	up:value("10")

cb2 = tracker:option(DynamicList, "trackip", translate("Tracking IP"),
		translate("This IP address will be pinged to dermine if the link is up or down."))
	cb2.datatype = "ipaddr"

return m

