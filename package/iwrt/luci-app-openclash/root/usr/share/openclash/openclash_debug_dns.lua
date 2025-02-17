#!/usr/bin/lua

require "nixio"
require "luci.util"
require "luci.sys"
local uci = require("luci.model.uci").cursor()
local json = require "luci.jsonc"
local datatype = require "luci.cbi.datatypes"
local fs = require "luci.openclash"

local addr = arg[1]
local resolve = arg[2]

local function debug_dns()
	local info, ip, host
	ip = fs.lanip()
	local port = uci:get("openclash", "config", "cn_port")
	local passwd = uci:get("openclash", "config", "dashboard_password") or ""

	if datatype.hostname(addr) and ip and port then
		info_v4 = luci.sys.exec(string.format('curl -sL -m 3 -H "Content-Type: application/json" -H "Authorization: Bearer %s" -XGET http://"%s":"%s"/dns/query?name="%s"', passwd, ip, port, addr))
		info_v6 = luci.sys.exec(string.format('curl -sL -m 3 -H "Content-Type: application/json" -H "Authorization: Bearer %s" -XGET \"http://"%s":"%s"/dns/query?name="%s"&type=AAAA\"', passwd, ip, port, addr))
		if info_v4 then
			info_v4 = json.parse(info_v4)
		end
		if info_v6 then
			info_v6 = json.parse(info_v6)
		end
		for _, info in pairs({info_v4, info_v6}) do
			if info and not resolve then
				print("Status: "..(info.Status))
				print("TC: "..tostring(info.TC))
				print("RD: "..tostring(info.RD))
				print("RA: "..tostring(info.RA))
				print("AD: "..tostring(info.AD))
				print("CD: "..tostring(info.CD))
				print("")
				print("Question: ")
				for _, v in pairs(info.Question) do
					print("  Name: "..(v.Name))
					print("  Qtype: "..(v.Qtype))
					print("  Qclass: "..(v.Qclass))
					print("")
				end
				if info.Answer then
					print("Answer: ")
					for _, v in pairs(info.Answer) do
						print("  TTL: "..(v.TTL))
						print("  data: "..(v.data:gsub("\n?", "")))
						print("  name: "..(v.name))
						print("  type: "..(v.type))
						print("")
					end
				end
				if info.Additional then
					print("Additional: ")
					for _, v in pairs(info.Additional) do
						print("  TTL: "..(v.TTL))
						print("  data: "..(v.data:gsub("\n?", "")))
						print("  name: "..(v.name))
						print("  type: "..(v.type))
						print("")
					end
				end
				if info.Authority then
					print("Authority: ")
					for _, v in pairs(info.Authority) do
						print("  TTL: "..(v.TTL))
						print("  data: "..(v.data:gsub("\n?", "")))
						print("  name: "..(v.name))
						print("  type: "..(v.type))
						print("")
					end
				end
			end
			if info and resolve then
				if info.Answer then
					for _, v in pairs(info.Answer) do
						if v.type == 1 or v.type == 28 then
							print(v.data)
						end
					end
				end
			end
		end
	end
	os.exit(0)
end

debug_dns()
