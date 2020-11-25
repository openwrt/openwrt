-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Licensed to the public under the Apache License 2.0.

local fs  = require "nixio.fs"
local sys = require "luci.sys"
local uci = require "luci.model.uci".cursor()
local testfullps = sys.exec("ps --help 2>&1 | grep BusyBox") --check which ps do we have
local psstring = (string.len(testfullps)>0) and  "ps w" or  "ps axfw" --set command we use to get pid

local m = Map("openvpn", translate("OpenVPN"))
local s = m:section( TypedSection, "openvpn", translate("OpenVPN instances"), translate("Below is a list of configured OpenVPN instances and their current state") )
s.template = "cbi/tblsection"
s.template_addremove = "openvpn/cbi-select-input-add"
s.addremove = true
s.add_select_options = { }

local cfg = s:option(DummyValue, "config")
function cfg.cfgvalue(self, section)
	local file_cfg = self.map:get(section, "config")
	if file_cfg then
		s.extedit = luci.dispatcher.build_url("admin", "vpn", "openvpn", "file", "%s")
	else
		s.extedit = luci.dispatcher.build_url("admin", "vpn", "openvpn", "basic", "%s")
	end
end

uci:load("openvpn_recipes")
uci:foreach( "openvpn_recipes", "openvpn_recipe",
	function(section)
		s.add_select_options[section['.name']] =
			section['_description'] or section['.name']
	end
)

function s.getPID(section) -- Universal function which returns valid pid # or nil
	local pid = sys.exec("%s | grep -w '[o]penvpn(%s)'" % { psstring, section })
	if pid and #pid > 0 then
		return tonumber(pid:match("^%s*(%d+)"))
	else
		return nil
	end
end

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
		local s = uci:section("openvpn", "openvpn", name)
		if s then
			local options = uci:get_all("openvpn_recipes", recipe)
			for k, v in pairs(options) do
				if k ~= "_role" and k ~= "_description" then
					if type(v) == "boolean" then
						v = v and "1" or "0"
					end
					uci:set("openvpn", name, k, v)
				end
			end
			uci:save("openvpn")
			uci:commit("openvpn")
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
	local cfg_file  = "/etc/openvpn/" ..name.. ".ovpn"
	local auth_file = "/etc/openvpn/" ..name.. ".auth"
	if fs.access(cfg_file) then
		fs.unlink(cfg_file)
	end
	if fs.access(auth_file) then
		fs.unlink(auth_file)
	end
	uci:delete("openvpn", name)
	uci:save("openvpn")
	uci:commit("openvpn")
end

s:option( Flag, "enabled", translate("Enabled") )

local active = s:option( DummyValue, "_active", translate("Started") )
function active.cfgvalue(self, section)
	local pid = s.getPID(section)
	if pid ~= nil then
		return (sys.process.signal(pid, 0))
			and translatef("yes (%i)", pid)
			or  translate("no")
	end
	return translate("no")
end

local updown = s:option( Button, "_updown", translate("Start/Stop") )
updown._state = false
updown.redirect = luci.dispatcher.build_url(
	"admin", "vpn", "openvpn"
)
function updown.cbid(self, section)
	local pid = s.getPID(section)
	self._state = pid ~= nil and sys.process.signal(pid, 0)
	self.option = self._state and "stop" or "start"
	return AbstractValue.cbid(self, section)
end
function updown.cfgvalue(self, section)
	self.title = self._state and "stop" or "start"
	self.inputstyle = self._state and "reset" or "reload"
end
function updown.write(self, section, value)
	if self.option == "stop" then
		sys.call("/etc/init.d/openvpn stop %s" % section)
	else
		sys.call("/etc/init.d/openvpn start %s" % section)
	end
	luci.http.redirect( self.redirect )
end

local port = s:option( DummyValue, "port", translate("Port") )
function port.cfgvalue(self, section)
	local val = AbstractValue.cfgvalue(self, section)
	if not val then
		local file_cfg = self.map:get(section, "config")
		if file_cfg  and fs.access(file_cfg) then
			val = sys.exec("awk '{if(match(tolower($1),/^port$/)&&match($2,/[0-9]+/)){cnt++;printf $2;exit}}END{if(cnt==0)printf \"-\"}' " ..file_cfg)
			if val == "-" then
				val = sys.exec("awk '{if(match(tolower($1),/^remote$/)&&match($3,/[0-9]+/)){cnt++;printf $3;exit}}END{if(cnt==0)printf \"-\"}' " ..file_cfg)
			end
		end
	end
	return val or "-"
end

local proto = s:option( DummyValue, "proto", translate("Protocol") )
function proto.cfgvalue(self, section)
	local val = AbstractValue.cfgvalue(self, section)
	if not val then
		local file_cfg = self.map:get(section, "config")
		if file_cfg and fs.access(file_cfg) then
			val = sys.exec("awk '{if(match(tolower($1),/^proto$/)&&match(tolower($2),/^udp[46]*$|^tcp[46]*-server$|^tcp[46]*-client$/)){cnt++;printf tolower($2);exit}}END{if(cnt==0)printf \"-\"}' " ..file_cfg)
			if val == "-" then
				val = sys.exec("awk '{if(match(tolower($1),/^remote$/)&&match(tolower($4),/^udp[46]*$|^tcp[46]*-server$|^tcp[46]*-client$/)){cnt++;printf $4;exit}}END{if(cnt==0)printf \"-\"}' " ..file_cfg)
			end
		end
	end
	return val or "-"
end

function m.on_after_apply(self,map)
	sys.call('/etc/init.d/openvpn reload')
end

return m
