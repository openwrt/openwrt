-- Copyright 2009-2015 Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

local type, next, pairs, ipairs, loadfile, table, select
	= type, next, pairs, ipairs, loadfile, table, select

local tonumber, tostring, math = tonumber, tostring, math

local pcall, require, setmetatable = pcall, require, setmetatable

local nxo = require "nixio"
local nfs = require "nixio.fs"
local ipc = require "luci.ip"
local utl = require "luci.util"
local uci = require "luci.model.uci"
local lng = require "luci.i18n"
local jsc = require "luci.jsonc"

module "luci.model.network"


IFACE_PATTERNS_VIRTUAL  = { }
IFACE_PATTERNS_IGNORE   = { "^wmaster%d", "^wifi%d", "^hwsim%d", "^imq%d", "^ifb%d", "^mon%.wlan%d", "^sit%d", "^gre%d", "^gretap%d", "^ip6gre%d", "^ip6tnl%d", "^tunl%d", "^lo$" }
IFACE_PATTERNS_WIRELESS = { "^wlan%d", "^wl%d", "^ath%d", "^%w+%.network%d" }

IFACE_ERRORS = {
	CONNECT_FAILED			= lng.translate("Connection attempt failed"),
	INVALID_ADDRESS			= lng.translate("IP address in invalid"),
	INVALID_GATEWAY			= lng.translate("Gateway address is invalid"),
	INVALID_LOCAL_ADDRESS	= lng.translate("Local IP address is invalid"),
	MISSING_ADDRESS			= lng.translate("IP address is missing"),
	MISSING_PEER_ADDRESS	= lng.translate("Peer address is missing"),
	NO_DEVICE				= lng.translate("Network device is not present"),
	NO_IFACE				= lng.translate("Unable to determine device name"),
	NO_IFNAME				= lng.translate("Unable to determine device name"),
	NO_WAN_ADDRESS			= lng.translate("Unable to determine external IP address"),
	NO_WAN_LINK				= lng.translate("Unable to determine upstream interface"),
	PEER_RESOLVE_FAIL		= lng.translate("Unable to resolve peer host name"),
	PIN_FAILED				= lng.translate("PIN code rejected")
}


protocol = utl.class()

local _protocols = { }

local _interfaces, _bridge, _switch, _tunnel, _swtopo
local _ubusnetcache, _ubusdevcache, _ubuswificache
local _uci

function _filter(c, s, o, r)
	local val = _uci:get(c, s, o)
	if val then
		local l = { }
		if type(val) == "string" then
			for val in val:gmatch("%S+") do
				if val ~= r then
					l[#l+1] = val
				end
			end
			if #l > 0 then
				_uci:set(c, s, o, table.concat(l, " "))
			else
				_uci:delete(c, s, o)
			end
		elseif type(val) == "table" then
			for _, val in ipairs(val) do
				if val ~= r then
					l[#l+1] = val
				end
			end
			if #l > 0 then
				_uci:set(c, s, o, l)
			else
				_uci:delete(c, s, o)
			end
		end
	end
end

function _append(c, s, o, a)
	local val = _uci:get(c, s, o) or ""
	if type(val) == "string" then
		local l = { }
		for val in val:gmatch("%S+") do
			if val ~= a then
				l[#l+1] = val
			end
		end
		l[#l+1] = a
		_uci:set(c, s, o, table.concat(l, " "))
	elseif type(val) == "table" then
		local l = { }
		for _, val in ipairs(val) do
			if val ~= a then
				l[#l+1] = val
			end
		end
		l[#l+1] = a
		_uci:set(c, s, o, l)
	end
end

function _stror(s1, s2)
	if not s1 or #s1 == 0 then
		return s2 and #s2 > 0 and s2
	else
		return s1
	end
end

function _get(c, s, o)
	return _uci:get(c, s, o)
end

function _set(c, s, o, v)
	if v ~= nil then
		if type(v) == "boolean" then v = v and "1" or "0" end
		return _uci:set(c, s, o, v)
	else
		return _uci:delete(c, s, o)
	end
end

local function _wifi_state()
	if not next(_ubuswificache) then
		_ubuswificache = utl.ubus("network.wireless", "status", {}) or {}
	end
	return _ubuswificache
end

local function _wifi_state_by_sid(sid)
	local t1, n1 = _uci:get("wireless", sid)
	if t1 == "wifi-iface" and n1 ~= nil then
		local radioname, radiostate
		for radioname, radiostate in pairs(_wifi_state()) do
			if type(radiostate) == "table" and
			   type(radiostate.interfaces) == "table"
			then
				local netidx, netstate
				for netidx, netstate in ipairs(radiostate.interfaces) do
					if type(netstate) == "table" and
					   type(netstate.section) == "string"
					then
						local t2, n2 = _uci:get("wireless", netstate.section)
						if t1 == t2 and n1 == n2 then
							return radioname, radiostate, netstate
						end
					end
				end
			end
		end
	end
end

local function _wifi_state_by_ifname(ifname)
	if type(ifname) == "string" then
		local radioname, radiostate
		for radioname, radiostate in pairs(_wifi_state()) do
			if type(radiostate) == "table" and
			   type(radiostate.interfaces) == "table"
			then
				local netidx, netstate
				for netidx, netstate in ipairs(radiostate.interfaces) do
					if type(netstate) == "table" and
					   type(netstate.ifname) == "string" and
					   netstate.ifname == ifname
					then
						return radioname, radiostate, netstate
					end
				end
			end
		end
	end
end

function _wifi_iface(x)
	local _, p
	for _, p in ipairs(IFACE_PATTERNS_WIRELESS) do
		if x:match(p) then
			return true
		end
	end
	return (nfs.access("/sys/class/net/%s/phy80211" % x) == true)
end

local function _wifi_iwinfo_by_ifname(ifname, force_phy_only)
	local stat, iwinfo = pcall(require, "iwinfo")
	local iwtype = stat and type(ifname) == "string" and iwinfo.type(ifname)
	local is_nonphy_op = {
		bitrate     = true,
		quality     = true,
		quality_max = true,
		mode        = true,
		ssid        = true,
		bssid       = true,
		assoclist   = true,
		encryption  = true
	}

	if iwtype then
		-- if we got a type but no real netdev, we're referring to a phy
		local phy_only = force_phy_only or (ipc.link(ifname).type ~= 1)

		return setmetatable({}, {
			__index = function(t, k)
				if k == "ifname" then
					return ifname
				elseif phy_only and is_nonphy_op[k] then
					return nil
				elseif iwinfo[iwtype][k] then
					return iwinfo[iwtype][k](ifname)
				end
			end
		})
	end
end

local function _wifi_sid_by_netid(netid)
	if type(netid) == "string" then
		local radioname, netidx = netid:match("^(%w+)%.network(%d+)$")
		if radioname and netidx then
			local i, n = 0, nil

			netidx = tonumber(netidx)
			_uci:foreach("wireless", "wifi-iface",
				function(s)
					if s.device == radioname then
						i = i + 1
						if i == netidx then
							n = s[".name"]
							return false
						end
					end
				end)

			return n
		end
	end
end

function _wifi_sid_by_ifname(ifn)
	local sid = _wifi_sid_by_netid(ifn)
	if sid then
		return sid
	end

	local _, _, netstate = _wifi_state_by_ifname(ifn)
	if netstate and type(netstate.section) == "string" then
		return netstate.section
	end
end

local function _wifi_netid_by_sid(sid)
	local t, n = _uci:get("wireless", sid)
	if t == "wifi-iface" and n ~= nil then
		local radioname = _uci:get("wireless", n, "device")
		if type(radioname) == "string" then
			local i, netid = 0, nil

			_uci:foreach("wireless", "wifi-iface",
				function(s)
					if s.device == radioname then
						i = i + 1
						if s[".name"] == n then
							netid = "%s.network%d" %{ radioname, i }
							return false
						end
					end
				end)

			return netid, radioname
		end
	end
end

local function _wifi_netid_by_netname(name)
	local netid = nil

	_uci:foreach("wireless", "wifi-iface",
		function(s)
			local net
			for net in utl.imatch(s.network) do
				if net == name then
					netid = _wifi_netid_by_sid(s[".name"])
					return false
				end
			end
		end)

	return netid
end

function _iface_virtual(x)
	local _, p
	for _, p in ipairs(IFACE_PATTERNS_VIRTUAL) do
		if x:match(p) then
			return true
		end
	end
	return false
end

function _iface_ignore(x)
	local _, p
	for _, p in ipairs(IFACE_PATTERNS_IGNORE) do
		if x:match(p) then
			return true
		end
	end
	return false
end

function init(cursor)
	_uci = cursor or _uci or uci.cursor()

	_interfaces = { }
	_bridge     = { }
	_switch     = { }
	_tunnel     = { }
	_swtopo     = { }

	_ubusnetcache  = { }
	_ubusdevcache  = { }
	_ubuswificache = { }

	-- read interface information
	local n, i
	for n, i in ipairs(nxo.getifaddrs()) do
		local name = i.name:match("[^:]+")

		if _iface_virtual(name) then
			_tunnel[name] = true
		end

		if _tunnel[name] or not (_iface_ignore(name) or _iface_virtual(name)) then
			_interfaces[name] = _interfaces[name] or {
				idx      = i.ifindex or n,
				name     = name,
				rawname  = i.name,
				flags    = { },
				ipaddrs  = { },
				ip6addrs = { }
			}

			if i.family == "packet" then
				_interfaces[name].flags   = i.flags
				_interfaces[name].stats   = i.data
				_interfaces[name].macaddr = ipc.checkmac(i.addr)
			elseif i.family == "inet" then
				_interfaces[name].ipaddrs[#_interfaces[name].ipaddrs+1] = ipc.IPv4(i.addr, i.netmask)
			elseif i.family == "inet6" then
				_interfaces[name].ip6addrs[#_interfaces[name].ip6addrs+1] = ipc.IPv6(i.addr, i.netmask)
			end
		end
	end

	-- read bridge informaton
	local b, l
	for l in utl.execi("brctl show") do
		if not l:match("STP") then
			local r = utl.split(l, "%s+", nil, true)
			if #r == 4 then
				b = {
					name    = r[1],
					id      = r[2],
					stp     = r[3] == "yes",
					ifnames = { _interfaces[r[4]] }
				}
				if b.ifnames[1] then
					b.ifnames[1].bridge = b
				end
				_bridge[r[1]] = b
			elseif b then
				b.ifnames[#b.ifnames+1] = _interfaces[r[2]]
				b.ifnames[#b.ifnames].bridge = b
			end
		end
	end

	-- read switch topology
	local boardinfo = jsc.parse(nfs.readfile("/etc/board.json") or "")
	if type(boardinfo) == "table" and type(boardinfo.switch) == "table" then
		local switch, layout
		for switch, layout in pairs(boardinfo.switch) do
			if type(layout) == "table" and type(layout.ports) == "table" then
				local _, port
				local ports = { }
				local nports = { }
				local netdevs = { }

				for _, port in ipairs(layout.ports) do
					if type(port) == "table" and
					   type(port.num) == "number" and
					   (type(port.role) == "string" or
					    type(port.device) == "string")
					then
						local spec = {
							num    = port.num,
							role   = port.role or "cpu",
							index  = port.index or port.num
						}

						if port.device then
							spec.device = port.device
							spec.tagged = port.need_tag
							netdevs[tostring(port.num)] = port.device
						end

						ports[#ports+1] = spec

						if port.role then
							nports[port.role] = (nports[port.role] or 0) + 1
						end
					end
				end

				table.sort(ports, function(a, b)
					if a.role ~= b.role then
						return (a.role < b.role)
					end

					return (a.index < b.index)
				end)

				local pnum, role
				for _, port in ipairs(ports) do
					if port.role ~= role then
						role = port.role
						pnum = 1
					end

					if role == "cpu" then
						port.label = "CPU (%s)" % port.device
					elseif nports[role] > 1 then
						port.label = "%s %d" %{ role:upper(), pnum }
						pnum = pnum + 1
					else
						port.label = role:upper()
					end

					port.role = nil
					port.index = nil
				end

				_swtopo[switch] = {
					ports = ports,
					netdevs = netdevs
				}
			end
		end
	end

	return _M
end

function save(self, ...)
	_uci:save(...)
	_uci:load(...)
end

function commit(self, ...)
	_uci:commit(...)
	_uci:load(...)
end

function ifnameof(self, x)
	if utl.instanceof(x, interface) then
		return x:name()
	elseif utl.instanceof(x, protocol) then
		return x:ifname()
	elseif type(x) == "string" then
		return x:match("^[^:]+")
	end
end

function get_protocol(self, protoname, netname)
	local v = _protocols[protoname]
	if v then
		return v(netname or "__dummy__")
	end
end

function get_protocols(self)
	local p = { }
	local _, v
	for _, v in ipairs(_protocols) do
		p[#p+1] = v("__dummy__")
	end
	return p
end

function register_protocol(self, protoname)
	local proto = utl.class(protocol)

	function proto.__init__(self, name)
		self.sid = name
	end

	function proto.proto(self)
		return protoname
	end

	_protocols[#_protocols+1] = proto
	_protocols[protoname]     = proto

	return proto
end

function register_pattern_virtual(self, pat)
	IFACE_PATTERNS_VIRTUAL[#IFACE_PATTERNS_VIRTUAL+1] = pat
end

function register_error_code(self, code, message)
	if type(code) == "string" and
	   type(message) == "string" and
	   not IFACE_ERRORS[code]
	then
		IFACE_ERRORS[code] = message
		return true
	end

	return false
end

function has_ipv6(self)
	return nfs.access("/proc/net/ipv6_route")
end

function add_network(self, n, options)
	local oldnet = self:get_network(n)
	if n and #n > 0 and n:match("^[a-zA-Z0-9_]+$") and not oldnet then
		if _uci:section("network", "interface", n, options) then
			return network(n)
		end
	elseif oldnet and oldnet:is_empty() then
		if options then
			local k, v
			for k, v in pairs(options) do
				oldnet:set(k, v)
			end
		end
		return oldnet
	end
end

function get_network(self, n)
	if n and _uci:get("network", n) == "interface" then
		return network(n)
	elseif n then
		local stat = utl.ubus("network.interface", "status", { interface = n })
		if type(stat) == "table" and
		   type(stat.proto) == "string"
		then
			return network(n, stat.proto)
		end
	end
end

function get_networks(self)
	local nets = { }
	local nls = { }

	_uci:foreach("network", "interface",
		function(s)
			nls[s['.name']] = network(s['.name'])
		end)

	local dump = utl.ubus("network.interface", "dump", { })
	if type(dump) == "table" and
	   type(dump.interface) == "table"
	then
		local _, net
		for _, net in ipairs(dump.interface) do
			if type(net) == "table" and
			   type(net.proto) == "string" and
			   type(net.interface) == "string"
			then
				if not nls[net.interface] then
					nls[net.interface] = network(net.interface, net.proto)
				end
			end
		end
	end

	local n
	for n in utl.kspairs(nls) do
		nets[#nets+1] = nls[n]
	end

	return nets
end

function del_network(self, n)
	local r = _uci:delete("network", n)
	if r then
		_uci:delete_all("luci", "ifstate",
			function(s) return (s.interface == n) end)

		_uci:delete_all("network", "alias",
			function(s) return (s.interface == n) end)

		_uci:delete_all("network", "route",
			function(s) return (s.interface == n) end)

		_uci:delete_all("network", "route6",
			function(s) return (s.interface == n) end)

		_uci:foreach("wireless", "wifi-iface",
			function(s)
				local net
				local rest = { }
				for net in utl.imatch(s.network) do
					if net ~= n then
						rest[#rest+1] = net
					end
				end
				if #rest > 0 then
					_uci:set("wireless", s['.name'], "network",
					              table.concat(rest, " "))
				else
					_uci:delete("wireless", s['.name'], "network")
				end
			end)

		local ok, fw = pcall(require, "luci.model.firewall")
		if ok then
			fw.init()
			fw:del_network(n)
		end
	end
	return r
end

function rename_network(self, old, new)
	local r
	if new and #new > 0 and new:match("^[a-zA-Z0-9_]+$") and not self:get_network(new) then
		r = _uci:section("network", "interface", new, _uci:get_all("network", old))

		if r then
			_uci:foreach("network", "alias",
				function(s)
					if s.interface == old then
						_uci:set("network", s['.name'], "interface", new)
					end
				end)

			_uci:foreach("network", "route",
				function(s)
					if s.interface == old then
						_uci:set("network", s['.name'], "interface", new)
					end
				end)

			_uci:foreach("network", "route6",
				function(s)
					if s.interface == old then
						_uci:set("network", s['.name'], "interface", new)
					end
				end)

			_uci:foreach("wireless", "wifi-iface",
				function(s)
					local net
					local list = { }
					for net in utl.imatch(s.network) do
						if net == old then
							list[#list+1] = new
						else
							list[#list+1] = net
						end
					end
					if #list > 0 then
						_uci:set("wireless", s['.name'], "network",
						              table.concat(list, " "))
					end
				end)

			_uci:delete("network", old)
		end
	end
	return r or false
end

function get_interface(self, i)
	if _interfaces[i] or _wifi_iface(i) then
		return interface(i)
	else
		local netid = _wifi_netid_by_sid(i)
		return netid and interface(netid)
	end
end

function get_interfaces(self)
	local iface
	local ifaces = { }
	local nfs = { }

	-- find normal interfaces
	_uci:foreach("network", "interface",
		function(s)
			for iface in utl.imatch(s.ifname) do
				if not _iface_ignore(iface) and not _iface_virtual(iface) and not _wifi_iface(iface) then
					nfs[iface] = interface(iface)
				end
			end
		end)

	for iface in utl.kspairs(_interfaces) do
		if not (nfs[iface] or _iface_ignore(iface) or _iface_virtual(iface) or _wifi_iface(iface)) then
			nfs[iface] = interface(iface)
		end
	end

	-- find vlan interfaces
	_uci:foreach("network", "switch_vlan",
		function(s)
			if type(s.ports) ~= "string" or
			   type(s.device) ~= "string" or
			   type(_swtopo[s.device]) ~= "table"
			then
				return
			end

			local pnum, ptag
			for pnum, ptag in s.ports:gmatch("(%d+)([tu]?)") do
				local netdev = _swtopo[s.device].netdevs[pnum]
				if netdev then
					if not nfs[netdev] then
						nfs[netdev] = interface(netdev)
					end
					_switch[netdev] = true

					if ptag == "t" then
						local vid = tonumber(s.vid or s.vlan)
						if vid ~= nil and vid >= 0 and vid <= 4095 then
							local iface = "%s.%d" %{ netdev, vid }
							if not nfs[iface] then
								nfs[iface] = interface(iface)
							end
							_switch[iface] = true
						end
					end
				end
			end
		end)

	for iface in utl.kspairs(nfs) do
		ifaces[#ifaces+1] = nfs[iface]
	end

	-- find wifi interfaces
	local num = { }
	local wfs = { }
	_uci:foreach("wireless", "wifi-iface",
		function(s)
			if s.device then
				num[s.device] = num[s.device] and num[s.device] + 1 or 1
				local i = "%s.network%d" %{ s.device, num[s.device] }
				wfs[i] = interface(i)
			end
		end)

	for iface in utl.kspairs(wfs) do
		ifaces[#ifaces+1] = wfs[iface]
	end

	return ifaces
end

function ignore_interface(self, x)
	return _iface_ignore(x)
end

function get_wifidev(self, dev)
	if _uci:get("wireless", dev) == "wifi-device" then
		return wifidev(dev)
	end
end

function get_wifidevs(self)
	local devs = { }
	local wfd  = { }

	_uci:foreach("wireless", "wifi-device",
		function(s) wfd[#wfd+1] = s['.name'] end)

	local dev
	for _, dev in utl.vspairs(wfd) do
		devs[#devs+1] = wifidev(dev)
	end

	return devs
end

function get_wifinet(self, net)
	local wnet = _wifi_sid_by_ifname(net)
	if wnet then
		return wifinet(wnet)
	end
end

function add_wifinet(self, net, options)
	if type(options) == "table" and options.device and
		_uci:get("wireless", options.device) == "wifi-device"
	then
		local wnet = _uci:section("wireless", "wifi-iface", nil, options)
		return wifinet(wnet)
	end
end

function del_wifinet(self, net)
	local wnet = _wifi_sid_by_ifname(net)
	if wnet then
		_uci:delete("wireless", wnet)
		return true
	end
	return false
end

function get_status_by_route(self, addr, mask)
	local route_statuses = { }
	local _, object
	for _, object in ipairs(utl.ubus()) do
		local net = object:match("^network%.interface%.(.+)")
		if net then
			local s = utl.ubus(object, "status", {})
			if s and s.route then
				local rt
				for _, rt in ipairs(s.route) do
					if not rt.table and rt.target == addr and rt.mask == mask then
						route_statuses[net] = s
					end
				end
			end
		end
	end

	return route_statuses
end

function get_status_by_address(self, addr)
	local _, object
	for _, object in ipairs(utl.ubus()) do
		local net = object:match("^network%.interface%.(.+)")
		if net then
			local s = utl.ubus(object, "status", {})
			if s and s['ipv4-address'] then
				local a
				for _, a in ipairs(s['ipv4-address']) do
					if a.address == addr then
						return net, s
					end
				end
			end
			if s and s['ipv6-address'] then
				local a
				for _, a in ipairs(s['ipv6-address']) do
					if a.address == addr then
						return net, s
					end
				end
			end
			if s and s['ipv6-prefix-assignment'] then
				local a
				for _, a in ipairs(s['ipv6-prefix-assignment']) do
					if a and a['local-address'] and a['local-address'].address == addr then
						return net, s
					end
				end
			end
		end
	end
end

function get_wan_networks(self)
	local k, v
	local wan_nets = { }
	local route_statuses = self:get_status_by_route("0.0.0.0", 0)

	for k, v in pairs(route_statuses) do
		wan_nets[#wan_nets+1] = network(k, v.proto)
	end

	return wan_nets
end

function get_wan6_networks(self)
	local k, v
	local wan6_nets = { }
	local route_statuses = self:get_status_by_route("::", 0)

	for k, v in pairs(route_statuses) do
		wan6_nets[#wan6_nets+1] = network(k, v.proto)
	end

	return wan6_nets
end

function get_switch_topologies(self)
	return _swtopo
end


function network(name, proto)
	if name then
		local p = proto or _uci:get("network", name, "proto")
		local c = p and _protocols[p] or protocol
		return c(name)
	end
end

function protocol.__init__(self, name)
	self.sid = name
end

function protocol._get(self, opt)
	local v = _uci:get("network", self.sid, opt)
	if type(v) == "table" then
		return table.concat(v, " ")
	end
	return v or ""
end

function protocol._ubus(self, field)
	if not _ubusnetcache[self.sid] then
		_ubusnetcache[self.sid] = utl.ubus("network.interface.%s" % self.sid,
		                                   "status", { })
	end
	if _ubusnetcache[self.sid] and field then
		return _ubusnetcache[self.sid][field]
	end
	return _ubusnetcache[self.sid]
end

function protocol.get(self, opt)
	return _get("network", self.sid, opt)
end

function protocol.set(self, opt, val)
	return _set("network", self.sid, opt, val)
end

function protocol.ifname(self)
	local ifname
	if self:is_floating() then
		ifname = self:_ubus("l3_device")
	else
		ifname = self:_ubus("device")
	end
	if not ifname then
		ifname = _wifi_netid_by_netname(self.sid)
	end
	return ifname
end

function protocol.proto(self)
	return "none"
end

function protocol.get_i18n(self)
	local p = self:proto()
	if p == "none" then
		return lng.translate("Unmanaged")
	elseif p == "static" then
		return lng.translate("Static address")
	elseif p == "dhcp" then
		return lng.translate("DHCP client")
	else
		return lng.translate("Unknown")
	end
end

function protocol.type(self)
	return self:_get("type")
end

function protocol.name(self)
	return self.sid
end

function protocol.uptime(self)
	return self:_ubus("uptime") or 0
end

function protocol.expires(self)
	local u = self:_ubus("uptime")
	local d = self:_ubus("data")

	if type(u) == "number" and type(d) == "table" and
	   type(d.leasetime) == "number"
	then
		local r = (d.leasetime - (u % d.leasetime))
		return r > 0 and r or 0
	end

	return -1
end

function protocol.metric(self)
	return self:_ubus("metric") or 0
end

function protocol.zonename(self)
	local d = self:_ubus("data")

	if type(d) == "table" and type(d.zone) == "string" then
		return d.zone
	end

	return nil
end

function protocol.ipaddr(self)
	local addrs = self:_ubus("ipv4-address")
	return addrs and #addrs > 0 and addrs[1].address
end

function protocol.ipaddrs(self)
	local addrs = self:_ubus("ipv4-address")
	local rv = { }

	if type(addrs) == "table" then
		local n, addr
		for n, addr in ipairs(addrs) do
			rv[#rv+1] = "%s/%d" %{ addr.address, addr.mask }
		end
	end

	return rv
end

function protocol.netmask(self)
	local addrs = self:_ubus("ipv4-address")
	return addrs and #addrs > 0 and
		ipc.IPv4("0.0.0.0/%d" % addrs[1].mask):mask():string()
end

function protocol.gwaddr(self)
	local _, route
	for _, route in ipairs(self:_ubus("route") or { }) do
		if route.target == "0.0.0.0" and route.mask == 0 then
			return route.nexthop
		end
	end
end

function protocol.dnsaddrs(self)
	local dns = { }
	local _, addr
	for _, addr in ipairs(self:_ubus("dns-server") or { }) do
		if not addr:match(":") then
			dns[#dns+1] = addr
		end
	end
	return dns
end

function protocol.ip6addr(self)
	local addrs = self:_ubus("ipv6-address")
	if addrs and #addrs > 0 then
		return "%s/%d" %{ addrs[1].address, addrs[1].mask }
	else
		addrs = self:_ubus("ipv6-prefix-assignment")
		if addrs and #addrs > 0 then
			return "%s/%d" %{ addrs[1].address, addrs[1].mask }
		end
	end
end

function protocol.ip6addrs(self)
	local addrs = self:_ubus("ipv6-address")
	local rv = { }
	local n, addr

	if type(addrs) == "table" then
		for n, addr in ipairs(addrs) do
			rv[#rv+1] = "%s/%d" %{ addr.address, addr.mask }
		end
	end

	addrs = self:_ubus("ipv6-prefix-assignment")

	if type(addrs) == "table" then
		for n, addr in ipairs(addrs) do
			if type(addr["local-address"]) == "table" and
			   type(addr["local-address"].mask) == "number" and
			   type(addr["local-address"].address) == "string"
			then
				rv[#rv+1] = "%s/%d" %{
					addr["local-address"].address,
					addr["local-address"].mask
				}
			end
		end
	end

	return rv
end

function protocol.gw6addr(self)
	local _, route
	for _, route in ipairs(self:_ubus("route") or { }) do
		if route.target == "::" and route.mask == 0 then
			return ipc.IPv6(route.nexthop):string()
		end
	end
end

function protocol.dns6addrs(self)
	local dns = { }
	local _, addr
	for _, addr in ipairs(self:_ubus("dns-server") or { }) do
		if addr:match(":") then
			dns[#dns+1] = addr
		end
	end
	return dns
end

function protocol.ip6prefix(self)
	local prefix = self:_ubus("ipv6-prefix")
	if prefix and #prefix > 0 then
		return "%s/%d" %{ prefix[1].address, prefix[1].mask }
	end
end

function protocol.errors(self)
	local _, err, rv
	local errors = self:_ubus("errors")
	if type(errors) == "table" then
		for _, err in ipairs(errors) do
			if type(err) == "table" and
			   type(err.code) == "string"
			then
				rv = rv or { }
				rv[#rv+1] = IFACE_ERRORS[err.code] or lng.translatef("Unknown error (%s)", err.code)
			end
		end
	end
	return rv
end

function protocol.is_bridge(self)
	return (not self:is_virtual() and self:type() == "bridge")
end

function protocol.opkg_package(self)
	return nil
end

function protocol.is_installed(self)
	return true
end

function protocol.is_virtual(self)
	return false
end

function protocol.is_floating(self)
	return false
end

function protocol.is_dynamic(self)
	return (self:_ubus("dynamic") == true)
end

function protocol.is_auto(self)
	return (self:_get("auto") ~= "0")
end

function protocol.is_alias(self)
	local ifn, parent = nil, nil

	for ifn in utl.imatch(_uci:get("network", self.sid, "ifname")) do
		if #ifn > 1 and ifn:byte(1) == 64 then
			parent = ifn:sub(2)
		elseif parent ~= nil then
			parent = nil
		end
	end

	return parent
end

function protocol.is_empty(self)
	if self:is_floating() then
		return false
	else
		local empty = true

		if (self:_get("ifname") or ""):match("%S+") then
			empty = false
		end

		if empty and _wifi_netid_by_netname(self.sid) then
			empty = false
		end

		return empty
	end
end

function protocol.is_up(self)
	return (self:_ubus("up") == true)
end

function protocol.add_interface(self, ifname)
	ifname = _M:ifnameof(ifname)
	if ifname and not self:is_floating() then
		-- if its a wifi interface, change its network option
		local wif = _wifi_sid_by_ifname(ifname)
		if wif then
			_append("wireless", wif, "network", self.sid)

		-- add iface to our iface list
		else
			_append("network", self.sid, "ifname", ifname)
		end
	end
end

function protocol.del_interface(self, ifname)
	ifname = _M:ifnameof(ifname)
	if ifname and not self:is_floating() then
		-- if its a wireless interface, clear its network option
		local wif = _wifi_sid_by_ifname(ifname)
		if wif then _filter("wireless", wif, "network", self.sid) end

		-- remove the interface
		_filter("network", self.sid, "ifname", ifname)
	end
end

function protocol.get_interface(self)
	if self:is_virtual() then
		_tunnel[self:proto() .. "-" .. self.sid] = true
		return interface(self:proto() .. "-" .. self.sid, self)
	elseif self:is_bridge() then
		_bridge["br-" .. self.sid] = true
		return interface("br-" .. self.sid, self)
	else
		local ifn = self:_ubus("l3_device") or self:_ubus("device")
		if ifn then
			return interface(ifn, self)
		end

		for ifn in utl.imatch(_uci:get("network", self.sid, "ifname")) do
			ifn = ifn:match("^[^:/]+")
			return ifn and interface(ifn, self)
		end

		ifn = _wifi_netid_by_netname(self.sid)
		return ifn and interface(ifn, self)
	end
end

function protocol.get_interfaces(self, ignore_bridge_state)
	if ignore_bridge_state or self:is_bridge() or (self:is_virtual() and not self:is_floating()) then
		local ifaces = { }

		local ifn
		local nfs = { }
		for ifn in utl.imatch(self:get("ifname")) do
			ifn = ifn:match("^[^:/]+")
			nfs[ifn] = interface(ifn, self)
		end

		for ifn in utl.kspairs(nfs) do
			ifaces[#ifaces+1] = nfs[ifn]
		end

		local wfs = { }
		_uci:foreach("wireless", "wifi-iface",
			function(s)
				if s.device then
					local net
					for net in utl.imatch(s.network) do
						if net == self.sid then
							ifn = _wifi_netid_by_sid(s[".name"])
							if ifn then
								wfs[ifn] = interface(ifn, self)
							end
						end
					end
				end
			end)

		for ifn in utl.kspairs(wfs) do
			ifaces[#ifaces+1] = wfs[ifn]
		end

		return ifaces
	end
end

function protocol.contains_interface(self, ifname)
	ifname = _M:ifnameof(ifname)
	if not ifname then
		return false
	elseif self:is_virtual() and self:proto() .. "-" .. self.sid == ifname then
		return true
	elseif self:is_bridge() and "br-" .. self.sid == ifname then
		return true
	else
		local ifn
		for ifn in utl.imatch(self:get("ifname")) do
			ifn = ifn:match("[^:]+")
			if ifn == ifname then
				return true
			end
		end

		local wif = _wifi_sid_by_ifname(ifname)
		if wif then
			local n
			for n in utl.imatch(_uci:get("wireless", wif, "network")) do
				if n == self.sid then
					return true
				end
			end
		end
	end

	return false
end

function protocol.adminlink(self)
	local stat, dsp = pcall(require, "luci.dispatcher")
	return stat and dsp.build_url("admin", "network", "network", self.sid)
end


interface = utl.class()

function interface.__init__(self, ifname, network)
	local wif = _wifi_sid_by_ifname(ifname)
	if wif then
		self.wif    = wifinet(wif)
		self.ifname = self.wif:ifname()
	end

	self.ifname  = self.ifname or ifname
	self.dev     = _interfaces[self.ifname]
	self.network = network
end

function interface._ubus(self, field)
	if not _ubusdevcache[self.ifname] then
		_ubusdevcache[self.ifname] = utl.ubus("network.device", "status",
		                                      { name = self.ifname })
	end
	if _ubusdevcache[self.ifname] and field then
		return _ubusdevcache[self.ifname][field]
	end
	return _ubusdevcache[self.ifname]
end

function interface.name(self)
	return self.wif and self.wif:ifname() or self.ifname
end

function interface.mac(self)
	return ipc.checkmac(self:_ubus("macaddr"))
end

function interface.ipaddrs(self)
	return self.dev and self.dev.ipaddrs or { }
end

function interface.ip6addrs(self)
	return self.dev and self.dev.ip6addrs or { }
end

function interface.type(self)
	if self.ifname and self.ifname:byte(1) == 64 then
		return "alias"
	elseif self.wif or _wifi_iface(self.ifname) then
		return "wifi"
	elseif _bridge[self.ifname] then
		return "bridge"
	elseif _tunnel[self.ifname] then
		return "tunnel"
	elseif self.ifname:match("%.") then
		return "vlan"
	elseif _switch[self.ifname] then
		return "switch"
	else
		return "ethernet"
	end
end

function interface.shortname(self)
	if self.wif then
		return self.wif:shortname()
	else
		return self.ifname
	end
end

function interface.get_i18n(self)
	if self.wif then
		return "%s: %s %q" %{
			lng.translate("Wireless Network"),
			self.wif:active_mode(),
			self.wif:active_ssid() or self.wif:active_bssid() or self.wif:id() or "?"
		}
	else
		return "%s: %q" %{ self:get_type_i18n(), self:name() }
	end
end

function interface.get_type_i18n(self)
	local x = self:type()
	if x == "alias" then
		return lng.translate("Alias Interface")
	elseif x == "wifi" then
		return lng.translate("Wireless Adapter")
	elseif x == "bridge" then
		return lng.translate("Bridge")
	elseif x == "switch" then
		return lng.translate("Ethernet Switch")
	elseif x == "vlan" then
		if _switch[self.ifname] then
			return lng.translate("Switch VLAN")
		else
			return lng.translate("Software VLAN")
		end
	elseif x == "tunnel" then
		return lng.translate("Tunnel Interface")
	else
		return lng.translate("Ethernet Adapter")
	end
end

function interface.adminlink(self)
	if self.wif then
		return self.wif:adminlink()
	end
end

function interface.ports(self)
	local members = self:_ubus("bridge-members")
	if members then
		local _, iface
		local ifaces = { }
		for _, iface in ipairs(members) do
			ifaces[#ifaces+1] = interface(iface)
		end
	end
end

function interface.bridge_id(self)
	if self.br then
		return self.br.id
	else
		return nil
	end
end

function interface.bridge_stp(self)
	if self.br then
		return self.br.stp
	else
		return false
	end
end

function interface.is_up(self)
	local up = self:_ubus("up")
	if up == nil then
		up = (self:type() == "alias")
	end
	return up or false
end

function interface.is_bridge(self)
	return (self:type() == "bridge")
end

function interface.is_bridgeport(self)
	return self.dev and self.dev.bridge and true or false
end

function interface.tx_bytes(self)
	local stat = self:_ubus("statistics")
	return stat and stat.tx_bytes or 0
end

function interface.rx_bytes(self)
	local stat = self:_ubus("statistics")
	return stat and stat.rx_bytes or 0
end

function interface.tx_packets(self)
	local stat = self:_ubus("statistics")
	return stat and stat.tx_packets or 0
end

function interface.rx_packets(self)
	local stat = self:_ubus("statistics")
	return stat and stat.rx_packets or 0
end

function interface.get_network(self)
	return self:get_networks()[1]
end

function interface.get_networks(self)
	if not self.networks then
		local nets = { }
		local _, net
		for _, net in ipairs(_M:get_networks()) do
			if net:contains_interface(self.ifname) or
			   net:ifname() == self.ifname
			then
				nets[#nets+1] = net
			end
		end
		table.sort(nets, function(a, b) return a.sid < b.sid end)
		self.networks = nets
		return nets
	else
		return self.networks
	end
end

function interface.get_wifinet(self)
	return self.wif
end


wifidev = utl.class()

function wifidev.__init__(self, name)
	local t, n = _uci:get("wireless", name)
	if t == "wifi-device" and n ~= nil then
		self.sid    = n
		self.iwinfo = _wifi_iwinfo_by_ifname(self.sid, true)
	end
	self.sid    = self.sid    or name
	self.iwinfo = self.iwinfo or { ifname = self.sid }
end

function wifidev.get(self, opt)
	return _get("wireless", self.sid, opt)
end

function wifidev.set(self, opt, val)
	return _set("wireless", self.sid, opt, val)
end

function wifidev.name(self)
	return self.sid
end

function wifidev.hwmodes(self)
	local l = self.iwinfo.hwmodelist
	if l and next(l) then
		return l
	else
		return { b = true, g = true }
	end
end

function wifidev.get_i18n(self)
	local t = self.iwinfo.hardware_name or "Generic"
	if self.iwinfo.type == "wl" then
		t = "Broadcom"
	end

	local m = ""
	local l = self:hwmodes()
	if l.a then m = m .. "a" end
	if l.b then m = m .. "b" end
	if l.g then m = m .. "g" end
	if l.n then m = m .. "n" end
	if l.ac then m = "ac" end

	return "%s 802.11%s Wireless Controller (%s)" %{ t, m, self:name() }
end

function wifidev.is_up(self)
	if _ubuswificache[self.sid] then
		return (_ubuswificache[self.sid].up == true)
	end

	return false
end

function wifidev.get_wifinet(self, net)
	if _uci:get("wireless", net) == "wifi-iface" then
		return wifinet(net)
	else
		local wnet = _wifi_sid_by_ifname(net)
		if wnet then
			return wifinet(wnet)
		end
	end
end

function wifidev.get_wifinets(self)
	local nets = { }

	_uci:foreach("wireless", "wifi-iface",
		function(s)
			if s.device == self.sid then
				nets[#nets+1] = wifinet(s['.name'])
			end
		end)

	return nets
end

function wifidev.add_wifinet(self, options)
	options = options or { }
	options.device = self.sid

	local wnet = _uci:section("wireless", "wifi-iface", nil, options)
	if wnet then
		return wifinet(wnet, options)
	end
end

function wifidev.del_wifinet(self, net)
	if utl.instanceof(net, wifinet) then
		net = net.sid
	elseif _uci:get("wireless", net) ~= "wifi-iface" then
		net = _wifi_sid_by_ifname(net)
	end

	if net and _uci:get("wireless", net, "device") == self.sid then
		_uci:delete("wireless", net)
		return true
	end

	return false
end


wifinet = utl.class()

function wifinet.__init__(self, name, data)
	local sid, netid, radioname, radiostate, netstate

	-- lookup state by radio#.network# notation
	sid = _wifi_sid_by_netid(name)
	if sid then
		netid = name
		radioname, radiostate, netstate = _wifi_state_by_sid(sid)
	else
		-- lookup state by ifname (e.g. wlan0)
		radioname, radiostate, netstate = _wifi_state_by_ifname(name)
		if radioname and radiostate and netstate then
			sid = netstate.section
			netid = _wifi_netid_by_sid(sid)
		else
			-- lookup state by uci section id (e.g. cfg053579)
			radioname, radiostate, netstate = _wifi_state_by_sid(name)
			if radioname and radiostate and netstate then
				sid = name
				netid = _wifi_netid_by_sid(sid)
			else
				-- no state available, try to resolve from uci
				netid, radioname = _wifi_netid_by_sid(name)
				if netid and radioname then
					sid = name
				end
			end
		end
	end

	local iwinfo =
		(netstate and _wifi_iwinfo_by_ifname(netstate.ifname)) or
		(radioname and _wifi_iwinfo_by_ifname(radioname)) or
		{ ifname = (netid or sid or name) }

	self.sid       = sid or name
	self.wdev      = iwinfo.ifname
	self.iwinfo    = iwinfo
	self.netid     = netid
	self._ubusdata = {
		radio = radioname,
		dev   = radiostate,
		net   = netstate
	}
end

function wifinet.ubus(self, ...)
	local n, v = self._ubusdata
	for n = 1, select('#', ...) do
		if type(v) == "table" then
			v = v[select(n, ...)]
		else
			return nil
		end
	end
	return v
end

function wifinet.get(self, opt)
	return _get("wireless", self.sid, opt)
end

function wifinet.set(self, opt, val)
	return _set("wireless", self.sid, opt, val)
end

function wifinet.mode(self)
	return self:ubus("net", "config", "mode") or self:get("mode") or "ap"
end

function wifinet.ssid(self)
	return self:ubus("net", "config", "ssid") or self:get("ssid")
end

function wifinet.bssid(self)
	return self:ubus("net", "config", "bssid") or self:get("bssid")
end

function wifinet.network(self)
	local net, networks = nil, { }
	for net in utl.imatch(self:ubus("net", "config", "network") or self:get("network")) do
		networks[#networks+1] = net
	end
	return networks
end

function wifinet.id(self)
	return self.netid
end

function wifinet.name(self)
	return self.sid
end

function wifinet.ifname(self)
	local ifname = self:ubus("net", "ifname") or self.iwinfo.ifname
	if not ifname or ifname:match("^wifi%d") or ifname:match("^radio%d") then
		ifname = self.netid
	end
	return ifname
end

function wifinet.get_device(self)
	local dev = self:ubus("radio") or self:get("device")
	return dev and wifidev(dev) or nil
end

function wifinet.is_up(self)
	local ifc = self:get_interface()
	return (ifc and ifc:is_up() or false)
end

function wifinet.active_mode(self)
	local m = self.iwinfo.mode or self:ubus("net", "config", "mode") or self:get("mode") or "ap"

	if     m == "ap"      then m = "Master"
	elseif m == "sta"     then m = "Client"
	elseif m == "adhoc"   then m = "Ad-Hoc"
	elseif m == "mesh"    then m = "Mesh"
	elseif m == "monitor" then m = "Monitor"
	end

	return m
end

function wifinet.active_mode_i18n(self)
	return lng.translate(self:active_mode())
end

function wifinet.active_ssid(self)
	return self.iwinfo.ssid or self:ubus("net", "config", "ssid") or self:get("ssid")
end

function wifinet.active_bssid(self)
	return self.iwinfo.bssid or self:ubus("net", "config", "bssid") or self:get("bssid")
end

function wifinet.active_encryption(self)
	local enc = self.iwinfo and self.iwinfo.encryption
	return enc and enc.description or "-"
end

function wifinet.assoclist(self)
	return self.iwinfo.assoclist or { }
end

function wifinet.frequency(self)
	local freq = self.iwinfo.frequency
	if freq and freq > 0 then
		return "%.03f" % (freq / 1000)
	end
end

function wifinet.bitrate(self)
	local rate = self.iwinfo.bitrate
	if rate and rate > 0 then
		return (rate / 1000)
	end
end

function wifinet.channel(self)
	return self.iwinfo.channel or self:ubus("dev", "config", "channel") or
		tonumber(self:get("channel"))
end

function wifinet.signal(self)
	return self.iwinfo.signal or 0
end

function wifinet.noise(self)
	return self.iwinfo.noise or 0
end

function wifinet.country(self)
	return self.iwinfo.country or self:ubus("dev", "config", "country") or "00"
end

function wifinet.txpower(self)
	local pwr = (self.iwinfo.txpower or 0)
	return pwr + self:txpower_offset()
end

function wifinet.txpower_offset(self)
	return self.iwinfo.txpower_offset or 0
end

function wifinet.signal_level(self, s, n)
	if self:active_bssid() ~= "00:00:00:00:00:00" then
		local signal = s or self:signal()
		local noise  = n or self:noise()

		if signal < 0 and noise < 0 then
			local snr = -1 * (noise - signal)
			return math.floor(snr / 5)
		else
			return 0
		end
	else
		return -1
	end
end

function wifinet.signal_percent(self)
	local qc = self.iwinfo.quality or 0
	local qm = self.iwinfo.quality_max or 0

	if qc > 0 and qm > 0 then
		return math.floor((100 / qm) * qc)
	else
		return 0
	end
end

function wifinet.shortname(self)
	return "%s %q" %{
		lng.translate(self:active_mode()),
		self:active_ssid() or self:active_bssid() or self:id()
	}
end

function wifinet.get_i18n(self)
	return "%s: %s %q (%s)" %{
		lng.translate("Wireless Network"),
		lng.translate(self:active_mode()),
		self:active_ssid() or self:active_bssid() or self:id(),
		self:ifname()
	}
end

function wifinet.adminlink(self)
	local stat, dsp = pcall(require, "luci.dispatcher")
	return dsp and dsp.build_url("admin", "network", "wireless", self.netid)
end

function wifinet.get_network(self)
	return self:get_networks()[1]
end

function wifinet.get_networks(self)
	local nets = { }
	local net
	for net in utl.imatch(self:ubus("net", "config", "network") or self:get("network")) do
		if _uci:get("network", net) == "interface" then
			nets[#nets+1] = network(net)
		end
	end
	table.sort(nets, function(a, b) return a.sid < b.sid end)
	return nets
end

function wifinet.get_interface(self)
	return interface(self:ifname())
end


-- setup base protocols
_M:register_protocol("static")
_M:register_protocol("dhcp")
_M:register_protocol("none")

-- load protocol extensions
local exts = nfs.dir(utl.libpath() .. "/model/network")
if exts then
	local ext
	for ext in exts do
		if ext:match("%.lua$") then
			require("luci.model.network." .. ext:gsub("%.lua$", ""))
		end
	end
end
