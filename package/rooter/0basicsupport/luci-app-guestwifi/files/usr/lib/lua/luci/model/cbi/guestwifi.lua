local fs  = require "nixio.fs"
local sys = require "luci.sys"
local uci = require "luci.model.uci".cursor()
local testfullps = sys.exec("ps --help 2>&1 | grep BusyBox") --check which ps do we have
local psstring = (string.len(testfullps)>0) and  "ps w" or  "ps axfw" --set command we use to get pid

local m = Map("guestwifi", translate("Guest Wifi"), translate("Set up a Guest Wifi on your Router"))

local s = m:section( TypedSection, "guestwifi", translate("Instances"), translate("Below is a list of Guest Wifi Instances and their current state") )
s.template = "cbi/tblsection"
s.template_addremove = "guestwifi/cbi-select-input-add"
s.addremove = true
s.add_select_options = { }

s.extedit = luci.dispatcher.build_url("admin", "network", "guestwifi", "edit", "%s")

function s.parse(self, section)
	local recipe = luci.http.formvalue(
		luci.cbi.CREATE_PREFIX .. self.config .. "." ..
		self.sectiontype .. ".select"
	)

	if recipe and not s.add_select_options[recipe] then
		self.invalid_cts = true
	else
		TypedSection.parse( self, section )
	end
end

function s.create(self, name)
	local recipe = luci.http.formvalue(
		luci.cbi.CREATE_PREFIX .. self.config .. "." ..
		self.sectiontype .. ".select"
	)
	local name = luci.http.formvalue(
		luci.cbi.CREATE_PREFIX .. self.config .. "." ..
		self.sectiontype .. ".text"
	)
	return 0
end

function s.remove(self, name)
	sys.call("/usr/lib/guestwifi/stop.sh %s" % name)
	uci:delete("guestwifi", name)
	uci:save("guestwifi")
	uci:commit("guestwifi")
end



local updown = s:option( Button, "_updown", translate("Start/Stop") )
updown._state = false
updown.redirect = luci.dispatcher.build_url(
	"admin", "network", "guestwifi"
)
function updown.cbid(self, section)
	local file_cfg = self.map:get(section, "enabled")
	if file_cfg == "1" then
		pid = 1
	else
		pid = nil
	end
	self._state = pid ~= nil
	self.option = self._state and "stop" or "start"
	return AbstractValue.cbid(self, section)
end
function updown.cfgvalue(self, section)
	self.title = self._state and "stop" or "start"
	self.inputstyle = self._state and "reset" or "reload"
end
function updown.write(self, section, value)
	if self.option == "stop" then
		sys.call("/usr/lib/guestwifi/stop.sh %s" % section)
	else
		sys.call("/usr/lib/guestwifi/start.sh %s" % section)
	end
	luci.http.redirect( self.redirect )
end

local port = s:option( DummyValue, "ssid", translate("SSID") )

local freq = s:option( DummyValue, "freq", translate("Frequency") )
function freq.cfgvalue(self, section)
	local val = AbstractValue.cfgvalue(self, section)
	if val == nil then
		val = "0"
	end
	if val == "1" then
		return "5.0Ghz"
	else
		return "2.4Ghz"
	end
end

local auto = s:option( DummyValue, "encrypted", translate("Encryption") )
function auto.cfgvalue(self, section)
	local val = AbstractValue.cfgvalue(self, section)
	if val == nil then
		val = 0
	end
	if val == "1" then
		return translate("WPA-PSK (Medium Security)")
	else
		if val == "2" then
			return translate("WPA2-PSK (Strong Security)")
		else
			return translate("None")
		end
	end
end

local qos = s:option( DummyValue, "qos", translate("Bandwidth Limiting") )
function qos.cfgvalue(self, section)
	local val = AbstractValue.cfgvalue(self, section)
	if val == nil then
		val = 0
	end
	if val == "1" then
		dl_cfg = self.map:get(section, "dl")
		ul_cfg = self.map:get(section, "ul")
		return translate("Download : ") .. dl_cfg .. translate(" Mbit/s / Upload : ") .. ul_cfg .. translate(" Mbit/s")
	else
		return translate("Disabled")
	end
end


return m