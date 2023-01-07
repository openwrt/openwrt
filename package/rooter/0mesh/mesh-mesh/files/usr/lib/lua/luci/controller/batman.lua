--[[
LuCI - Lua Configuration Interface

Copyright 2012 Jo-Philipp Wich <xm@subsignal.org>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

$Id$
]]--

module("luci.controller.batman", package.seeall)

local function split(str, pat)
   local t = {}  -- NOTE: use {n = 0} in Lua-5.0
   local fpat = "(.-)" .. pat
   local last_end = 1
   local s, e, cap = str:find(fpat, 1)
   while s do
      if s ~= 1 or cap ~= "" then
         table.insert(t,cap)
      end
      last_end = e+1
      s, e, cap = str:find(fpat, last_end)
   end
   if last_end <= #str then
      cap = str:sub(last_end)
      table.insert(t, cap)
   end
   return t
end


function index()
	local page

	page = node("admin", "mesh", "batman")
	page.target = template("batman/batman")
	page.title  = _("Mesh Status")
	page.order  = 1

	node("batman")

	page = node("batman", "json")
	page.target = call("act_json")

	page = node("batman", "topo")
	page.target = call("act_topo")
	page.leaf   = true

	page = node("batman", "graph")
	page.target = template("batman_graph")
	page.leaf   = true
end

function act_topo(mode)
	if not mode or mode == "dot" or mode == "json" then
		local fd = io.popen("batadv-vis -f %s" %( mode or "dot" ))
		if fd then
			if mode == "json" then
				luci.http.prepare_content("application/json")
				luci.http.write("[")
				local ln
				repeat
					ln = fd:read("*l")
					if ln then
						luci.http.write(ln)
						luci.http.write(", ")
					end
				until not ln
				luci.http.write("{ } ]")
			else
				luci.http.prepare_content("text/vnd.graphviz")
				luci.http.header("Content-Disposition",
					"attachment; filename=topo-%s-%s.vd"
						%{ luci.sys.hostname(), os.date("%Y%m%d-%H%M%S") })
				luci.http.write(fd:read("*a"))
			end
			fd:close()
		else
			luci.http.status(500, "No data")
		end
	else
		luci.http.status(500, "Bad mode")
	end
end

function act_json()
	local v, l, fd
	local rv = {
		interfaces  = { },
		originators = { },
		gateways    = { }
	}

	--
	-- interfaces
	--
	fd = io.popen("batctl if")
	if fd then
		repeat
			l = fd:read("*l")
			v = l and l:match("^(.-):")
			if v then
				rv.interfaces[#rv.interfaces+1] = v
			end
		until not l
		fd:close()
	end

os.execute("cat /proc/net/arp > /tmp/arp")

iplist = {}
maclist = {}
index = 0
fd = io.open("/tmp/arp")
if fd then
	-- skip header line
	fd:read("*l")
	repeat
		l = fd:read("*l")
		if l then
			lan = l
			s1, e1 = lan:find("0x2")
			if s1 ~= nil then
				s, e = l:find(" ")
				if s ~= nil then
					ip = l:sub(0, e-1)
	
					fc = io.popen("batctl translate " .. ip)
					if fc then
						mac = fc:read("*l")
						fc:close()
						iplist[index] = ip
						maclist[index] = mac
						index = index + 1
					end
				end
			end
		end
	until not l	
	fd:close()
end

	--
	-- originators
	--
        local originators_command = (
        "batctl o -H 2>/dev/null ".. -- gets originators from batctl
        "| tr -d '[]()' ".. -- removes brackets and parenthesis
        "| sed 's/^  / -/g' ".. -- normalizes output, adding a minus when no asterisk is outputed in each line
        "| sed 's/^ //g' "..  -- removes the space from the beginning of the line
        "| sed -r 's/\\s+/,/g'".. -- replaces tabs for commas
        "| sed -r 's/s,/,/g'" -- removes the 's' from the last_seen field referencing seconds
        )
	fd = io.popen(originators_command)
	if fd then
		repeat
			l = fd:read()
			if l then
				local asterisk, originator_name, last_seen, link_quality, next_hop, outgoing_if
                                asterisk, originator_name, last_seen, link_quality, next_hop, outgoing_if = unpack(split(l, ","))
				if originator_name and last_seen and link_quality then
					if originator_name == next_hop then
						next_hop = "Here"
					end
					if index > 0 then
						for j=0,index-1 do
							if maclist[j] == originator_name then
								originator_name = iplist[j] .. " (" .. originator_name .. ")"
							end
							if maclist[j] == next_hop then
								next_hop = iplist[j]
							end
						end
					end
					rv.originators[#rv.originators+1] = {
						originator_name,
						tonumber(last_seen) * 1000,
						tonumber(link_quality),
						next_hop,
						outgoing_if
					}
				end
			end
		until not l
		fd:close()
        end

	--
	-- gateways
	--
	fd = io.popen("batctl gwl")
	if fd then
		-- skip header line
		fd:read("*l")
		fd:read("*l")

		repeat
			l = fd:read("*l")
			if l then
				local a, m, q, n, i, r = l:match("^(%S*) +([^ ]+) +%( *(%d+)%) +([^ ]+) +%[ *(%S+)%]: +(%S+)")
				if a and m and q and n and i and r then
					c="0"
					if index > 0 then
						for j=0,index-1 do
							if maclist[j] == m then
								m = iplist[j]
							end
							if maclist[j] == n then
								n = iplist[j]
							end
						end
					end
					rv.gateways[#rv.gateways+1] = {
						#a > 0,
						m,
						tonumber(q),
						n,
						i,
						tonumber(c),
						r
					}
				end
			end
		until not l
		fd:close()
	end
	
	-- local Gateway status
	
	rv.status = "Client"
	fd = io.popen("batctl gw")
	if fd then
		l = fd:read("*l")
		s, e = l:find("server")
		if s ~= nil then
			rv.status = "Server"
		end
		fd:close()
	end


	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end
