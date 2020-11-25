--[[
    Copyright (C) 2011 Pau Escrich <pau@dabax.net>
    Contributors Jo-Philipp Wich <xm@subsignal.org>
                 Roger Pueyo Centelles <roger.pueyo@guifi.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

    The full GNU General Public License is included in this distribution in
    the file called "COPYING".
--]]

module("luci.controller.bmx7", package.seeall)

function index()
	local place = {}
	local ucim = require "luci.model.uci"
	local uci = ucim.cursor()

	-- checking if ignore is on
	if uci:get("luci-bmx7","luci","ignore") == "1" then
		return nil
	end

	-- getting value from uci database
	local uci_place = uci:get("luci-bmx7","luci","place")

	-- default values
	if uci_place == nil then
		place = {"bmx7"}
	else
		local util = require "luci.util"
		place = util.split(uci_place," ")
	end

	-- getting position of menu
	local uci_position = uci:get("luci-bmx7","luci","position")


	---------------------------
	-- Placing the pages in the menu
	---------------------------

	-- Status (default)
	entry(place,call("action_status_j"),place[#place],tonumber(uci_position))

	table.insert(place,"Status")
	entry(place,call("action_status_j"),"Status",0)
	table.remove(place)

	-- Topology
	table.insert(place,"Topology")
	entry(place,call("topology"),"Topology",1)
	table.remove(place)

	-- Nodes
	table.insert(place,"Nodes")
	entry(place,call("action_nodes_j"),"Nodes",2)
	table.remove(place)

	-- Tunnels
	table.insert(place,"Gateways")
	entry(place,call("action_tunnels_j"),"Gateways",3)
	table.remove(place)

	-- Integrate bmx7-mdns if present
	if nixio.fs.stat("/usr/lib/lua/luci/model/cbi/bmx7-mdns.lua","type") ~= nil then
		table.insert(place,"mDNS")
		entry(place, cbi("bmx7-mdns"), "mesh DNS", 1).dependent=false
		table.remove(place)
	end

end


function action_status_j()
	luci.template.render("bmx7/status_j", {})
end

function action_tunnels_j()
	luci.template.render("bmx7/tunnels_j", {})
end

function topology()
	luci.template.render("bmx7/topology", {})
end

function action_nodes_j()
	luci.template.render("bmx7/nodes_j", {})
end
