local fs  = require "nixio.fs"
local sys = require "luci.sys"
local uci = require "luci.model.uci".cursor()
local testfullps = sys.exec("ps --help 2>&1 | grep BusyBox") --check which ps do we have
local psstring = (string.len(testfullps)>0) and  "ps w" or  "ps axfw" --set command we use to get pid

local m = Map("wireguard", translate("Wireguard"), translate("Set up a Wireguard VPN Tunnel on your Router"))

local s = m:section( TypedSection, "wireguard", translate("Instances"), translate("Below is a list of configured Wireguard Instances and their current state") )
s.template = "cbi/tblsection"
s.template_addremove = "wireguard/cbi-select-input-add"
s.addremove = true
s.add_select_options = { }

local cfg = s:option(DummyValue, "config")
function cfg.cfgvalue(self, section)
	local file_cfg = self.map:get(section, "client")
	if file_cfg == "1" then
		s.extedit = luci.dispatcher.build_url("admin", "vpn", "wireguard", "client", "%s")
	else
		s.extedit = luci.dispatcher.build_url("admin", "vpn", "wireguard", "server", "%s")
	end
end

uci:load("wireguard_recipes")
uci:foreach( "wireguard_recipes", "wireguard_recipe",
	function(section)
		s.add_select_options[section['.name']] =
			section['_description'] or section['.name']
	end
)

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
	if #name > 3 and not name:match("[^a-zA-Z0-9_]") then
		local s = uci:section("wireguard", "wireguard", name)
		if s then
			local options = uci:get_all("wireguard_recipes", recipe)
			for k, v in pairs(options) do
				if k ~= "_role" and k ~= "_description" then
					if type(v) == "boolean" then
						v = v and "1" or "0"
					end
					uci:set("wireguard", name, k, v)
				end
			end
			uci:save("wireguard")
			uci:commit("wireguard")
			if extedit then
				luci.http.redirect( self.extedit:format(name) )
			end
		end
	elseif #name > 0 then
		self.invalid_cts = true
	end
	return 0
end

function s.remove(self, name)
	local cfg_file  = "/etc/openvpn/" ..name.. ".conf"
	local auth_file = "/etc/openvpn/" ..name.. ".auth"
	if fs.access(cfg_file) then
		fs.unlink(cfg_file)
	end
	if fs.access(auth_file) then
		fs.unlink(auth_file)
	end
	uci:delete("wireguard", name)
	uci:save("wireguard")
	uci:commit("wireguard")
end

local port = s:option( DummyValue, "client", translate("Type") )
function port.cfgvalue(self, section)
	local val = AbstractValue.cfgvalue(self, section)
	if val == nil then
		val = 0
	end
	if val == "1" then
		return "Client"
	else
		return "Server"
	end
end

local addr = s:option( DummyValue, "addresses", translate("IP Addresses") )
function addr.cfgvalue(self, section)
	local val = AbstractValue.cfgvalue(self, section)
	return val or "----"
end

local auto = s:option( DummyValue, "udptunnel", translate("UDP over TCP") )
function auto.cfgvalue(self, section)
	local val = AbstractValue.cfgvalue(self, section)
	if val == nil then
		val = 0
	end
	if val == "1" then
		return "Yes"
	else
		return "No"
	end
end

local auto = s:option( DummyValue, "auto", translate("Start on Boot") )
function auto.cfgvalue(self, section)
	local val = AbstractValue.cfgvalue(self, section)
	if val == nil then
		val = 0
	end
	if val == "1" then
		return "Yes"
	else
		return "No"
	end
end

local active = s:option( DummyValue, "active", translate("Started") )
function active.cfgvalue(self, section)
	local val = AbstractValue.cfgvalue(self, section)
	if val == nil then
		val = 0
	end
	if val == "1" then
		return "Yes"
	else
		return "No"
	end
end

local updown = s:option( Button, "_updown", translate("Start/Stop") )
updown._state = false
updown.redirect = luci.dispatcher.build_url(
	"admin", "vpn", "wireguard"
)
function updown.cbid(self, section)
	local file_cfg = self.map:get(section, "active")
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
		sys.call("/usr/lib/wireguard/stopvpn.sh %s" % section)
	else
		sys.call("/usr/lib/wireguard/startvpn.sh %s" % section)
	end
	luci.http.redirect( self.redirect )
end

m:section(SimpleSection).template = "wireguard/wireguard"

return m